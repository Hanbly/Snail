#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TextureCoords;

#ifdef INSTANCING
    layout(location = 3) in mat4 a_Model; 
    layout(location = 7) in mat3 a_NormalMatrix;
    layout(location = 10) in int a_EntityID;
#else
    uniform mat4 u_Model;
    uniform mat3 u_NormalMatrix;
    uniform int u_EntityID;
#endif

uniform mat4 u_ViewProjection;
// 接收光照空间矩阵
uniform mat4 u_LightSpaceMatrix;

out vec2 v_TextureCoords;
out vec3 v_Normal;
out vec3 v_FragPos;
flat out int v_EntityID;
// 输出光照空间坐标
out vec4 v_FragPosLightSpace;

void main()
{
    mat4 modelMatrix;
    mat3 normalMatrix;
    int entityID;

#ifdef INSTANCING
    modelMatrix = a_Model;
    normalMatrix = a_NormalMatrix;
    entityID = a_EntityID;
#else
    modelMatrix = u_Model;
    normalMatrix = u_NormalMatrix;
    entityID = u_EntityID;
#endif

    vec4 worldPos = modelMatrix * vec4(a_Position, 1.0);

    v_Normal = normalize(normalMatrix * a_Normal);    
    v_FragPos = vec3(worldPos);
    v_TextureCoords = a_TextureCoords;
    v_EntityID = entityID;

    // --- 计算光照空间的 片段坐标 ---
    v_FragPosLightSpace = u_LightSpaceMatrix * worldPos;
    
    gl_Position = u_ViewProjection * worldPos;
}

#type fragment
#version 330 core

// 最大光源数量定义
#define MAX_DIR_LIGHTS 4
#define MAX_POINT_LIGHTS 16
const float PI = 3.14159265359;

// 输出
layout(location = 0) out vec4 FinalColor;
layout(location = 1) out int EntityIDBuffer;

// 输入 (来自顶点着色器)
in vec2 v_TextureCoords;
in vec3 v_Normal;
in vec3 v_FragPos;
flat in int v_EntityID;
in vec4 v_FragPosLightSpace;

// --- 材质 Uniforms (PBR 工作流) ---
uniform bool u_UseTexture;

// 1. 反照率 (Albedo/Base Color)
uniform sampler2D u_AlbedoMap;
uniform bool u_UseAlbedoMap;
uniform vec3 u_AlbedoColor; // 备用纯色

// 2. 法线贴图 (Normal)
uniform sampler2D u_NormalMap;
uniform bool u_UseNormalMap;

// 3. 金属度 (Metallic)
uniform sampler2D u_MetallicMap;
uniform bool u_UseMetallicMap;
uniform float u_MetallicVal; // 备用数值 (0.0 = 非金属, 1.0 = 金属)

// 4. 粗糙度 (Roughness)
uniform sampler2D u_RoughnessMap;
uniform bool u_UseRoughnessMap;
uniform float u_RoughnessVal; // 备用数值

// 5. 环境光遮蔽 (AO)
uniform sampler2D u_AOMap;
uniform bool u_UseAOMap;
uniform float u_AOVal; // 备用数值 (通常为 1.0)

// 阴影贴图
uniform sampler2D u_ShadowMap; 

// 摄像机位置
uniform vec3 u_ViewPosition;

// --- 光源结构体定义 ---
struct DirLight {
    vec3 direction;
    vec3 color;     // PBR中这里代表辐射率(Radiance)颜色
    float intensity;  // 光的强度
};

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;  // 光的强度
};

uniform DirLight u_DirLights[MAX_DIR_LIGHTS];
uniform int u_DirLightCount;

uniform PointLight u_PointLights[MAX_POINT_LIGHTS];
uniform int u_PointLightCount;

// --- 函数声明 ---

// PBR 核心函数 (Cook-Torrance BRDF)
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);

// 工具函数
vec3 getNormalFromMap();
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);

void main()
{
    // 1. 采样材质属性
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;

    if (u_UseTexture && u_UseAlbedoMap) {
        vec4 albedoTex = texture(u_AlbedoMap, v_TextureCoords);
        if(albedoTex.a < 0.1) discard; // 透明度测试
        albedo = albedoTex.rgb;
    } else {
        albedo = u_AlbedoColor;
    }

    if (u_UseTexture && u_UseMetallicMap) {
        metallic = texture(u_MetallicMap, v_TextureCoords).r;
    } else {
        metallic = u_MetallicVal;
    }

    if (u_UseTexture && u_UseRoughnessMap) {
        roughness = texture(u_RoughnessMap, v_TextureCoords).r;
    } else {
        roughness = u_RoughnessVal;
    }

    if (u_UseTexture && u_UseAOMap) {
        ao = texture(u_AOMap, v_TextureCoords).r;
    } else {
        ao = u_AOVal;
    }

    // 2. 准备向量
    vec3 N = (u_UseTexture && u_UseNormalMap) ? getNormalFromMap() : normalize(v_Normal);
    vec3 V = normalize(u_ViewPosition - v_FragPos);

    // 3. 计算 F0 (基础反射率)
    // 非金属 F0 约为 0.04，金属 F0 为其 Albedo 颜色
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // 反射率方程结果 (Reflectance Equation)
    vec3 Lo = vec3(0.0);

    // --- 计算平行光 (Directional Lights) ---
    int dirCount = min(u_DirLightCount, MAX_DIR_LIGHTS);
    for(int i = 0; i < dirCount; i++)
    {
        vec3 L = normalize(-u_DirLights[i].direction);
        vec3 H = normalize(V + L);
        
        // 对于平行光，没有距离衰减，直接使用光颜色作为辐射率
        vec3 radiance = u_DirLights[i].color * u_DirLights[i].intensity; 
        
        // 计算阴影 (仅对第一个光源计算)
        float shadow = 0.0;
        if(i == 0) {
            shadow = ShadowCalculation(v_FragPosLightSpace, N, L);
        }

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);       
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
            
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // +0.0001 防止除零
        vec3 specular = numerator / denominator;
        
        // kS 是菲涅尔项 F
        vec3 kS = F;
        // kD 是漫反射项，能量守恒：Diffuse + Specular = 1.0
        vec3 kD = vec3(1.0) - kS;
        // 金属没有漫反射成分
        kD *= 1.0 - metallic;      

        float NdotL = max(dot(N, L), 0.0);

        // 累加光照 (应用阴影遮蔽)
        Lo += (kD * albedo / PI + specular) * radiance * NdotL * (1.0 - shadow); 
    }

    // --- 计算点光源 (Point Lights) ---
    int pointCount = min(u_PointLightCount, MAX_POINT_LIGHTS);
    for(int i = 0; i < pointCount; i++)
    {
        vec3 L = normalize(u_PointLights[i].position - v_FragPos);
        vec3 H = normalize(V + L);
        
        float distance = length(u_PointLights[i].position - v_FragPos);
        // PBR 物理衰减 (Inverse Square Law)
        float attenuation = 1.0 / (distance * distance); 
        
        vec3 radiance = u_PointLights[i].color * u_PointLights[i].intensity * attenuation;

        // Cook-Torrance BRDF (逻辑同上)
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);       
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
        
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; 
        vec3 specular = numerator / denominator;
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;      

        float NdotL = max(dot(N, L), 0.0);

        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    // --- 环境光 (Ambient) ---
    // TODO [PBR进阶]: 这里目前使用简单的常数环境光。
    // 完整的PBR需要使用 IBL (Image Based Lighting) 替换此处。
    // 即: Irradiance Map (漫反射) + Prefiltered Map (镜面反射) + BRDF LUT
    vec3 ambient = vec3(0.03) * albedo * ao;
    
    vec3 color = ambient + Lo;

    FinalColor = vec4(color, 1.0);
    EntityIDBuffer = v_EntityID;
}

// ----------------------------------------------------------------------------
// PBR 数学函数实现
// ----------------------------------------------------------------------------

// 1. 正态分布函数 (D) - Trowbridge-Reitz GGX
// 估算微表面法线与中间向量 H 对齐的比例
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / max(denom, 0.0000001); // 避免除零
}

// 2. 几何遮蔽函数 (G) - Schlick-GGX
// 估算微表面自遮挡的比例
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0; // 直接光照使用此 k 值

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// 3. 菲涅尔方程 (F) - Fresnel-Schlick
// 估算不同观察角度下的表面反射率
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// ----------------------------------------------------------------------------
// 辅助函数
// ----------------------------------------------------------------------------

// 法线贴图 TBN 计算 (无需预计算切线)
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(u_NormalMap, v_TextureCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(v_FragPos);
    vec3 Q2  = dFdy(v_FragPos);
    vec2 st1 = dFdx(v_TextureCoords);
    vec2 st2 = dFdy(v_TextureCoords);

    vec3 N   = normalize(v_Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

// 阴影计算 ( PCF 逻辑)
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if(projCoords.z > 1.0)
        return 0.0;
        
    // 偏差值计算，防止阴影波纹 (Shadow Acne)
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
    
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(u_ShadowMap, 0));
    
    // 3x3 PCF 采样
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += (projCoords.z - bias) > pcfDepth ? 1.0 : 0.0;
        }    
    }
    shadow /= 9.0;
    
    return shadow;
}