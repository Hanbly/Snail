#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TextureCoords;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_Bitangent;

#ifdef INSTANCING
    layout(location = 5) in mat4 a_Model; 
    layout(location = 9) in mat3 a_NormalMatrix;
    layout(location = 12) in int a_EntityID;
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
// 输出TBN
out mat3 v_TBN;

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

    v_FragPos = vec3(worldPos);
    v_TextureCoords = a_TextureCoords;
    v_EntityID = entityID;

    // --- 计算光照空间的 片段坐标 ---
    v_FragPosLightSpace = u_LightSpaceMatrix * worldPos;

    // --- TBN 计算核心 ---
    // 1. 将法线和切线变换到世界空间
    vec3 T = normalize(normalMatrix * a_Tangent);
    vec3 N = normalize(normalMatrix * a_Normal);
    
    // 2. Gram-Schmidt 正交化 (重新调整 T，使其垂直于 N)
    // 这一步能消除由于插值造成的 TBN 不垂直问题
    T = normalize(T - dot(T, N) * N);
    
    // 3. 计算副切线 B
    // 直接使用 CPU 传来的 Bitangent (处理镜像纹理更稳健)
    vec3 B = normalize(normalMatrix * a_Bitangent);
    
    // 4. 构建矩阵
    v_TBN = mat3(T, B, N);

    v_Normal = N;
    
    gl_Position = u_ViewProjection * worldPos;
}

#type fragment
#version 330 core

#define MAX_DIR_LIGHTS 4
#define MAX_POINT_LIGHTS 16

// 输出
layout(location = 0) out vec4 FinalColor;
layout(location = 1) out int EntityIDBuffer;

// 输入
in vec2 v_TextureCoords;
in vec3 v_Normal;
in vec3 v_FragPos;
flat in int v_EntityID;
in vec4 v_FragPosLightSpace;
in mat3 v_TBN;

// --- 材质 Uniforms (Phong 工作流) ---
uniform sampler2D u_Diffuse1;
uniform sampler2D u_Specular1;
uniform sampler2D u_NormalMap;
uniform bool u_UseTexture;
uniform bool u_UseDiffuseMap;
uniform bool u_UseSpecularMap;
uniform bool u_UseNormalMap;
uniform sampler2D u_ShadowMap; 

uniform vec3 u_ColorDiffuse;
uniform vec3 u_ColorSpecular;
uniform vec3 u_ColorAmbient;
uniform float u_Shininess; 

uniform vec3 u_ViewPosition;

struct DirLight {
    vec3 direction;
    vec3 color;
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

// TBN 计算
vec3 getNormalFromMap()
{
    // 1. 采样法线贴图 [0, 1]
    vec3 tangentNormal = texture(u_NormalMap, v_TextureCoords).xyz;
    
    // 2. 映射到 [-1, 1]
    tangentNormal = tangentNormal * 2.0 - 1.0;
    
    // 3. 将法线从切线空间变换到世界空间
    // 直接乘以上一步传来的 TBN 矩阵
    vec3 worldNormal = normalize(v_TBN * tangentNormal);
    
    return worldNormal;
}

// 函数声明
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffColor, float specMask, float shadow);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffColor, float specMask);
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);

void main()
{
    // 1. 纹理/材质颜色采样
    vec3 diffMapColor;
    if(u_UseTexture && u_UseDiffuseMap) {
        vec4 texColor = texture(u_Diffuse1, v_TextureCoords);
        if(texColor.a < 0.1) discard;
        diffMapColor = texColor.rgb;
    } else {
        diffMapColor = u_ColorDiffuse;
    }

    // 2. 高光遮罩采样
    float specMask = 1.0;
    if (u_UseTexture && u_UseSpecularMap) {
        specMask = texture(u_Specular1, v_TextureCoords).r;
    }
    
    // --- 法线计算 ---
    vec3 norm;
    if (u_UseTexture && u_UseNormalMap) {
        norm = getNormalFromMap();
    } else {
        norm = normalize(v_Normal);
    }

    vec3 viewDir = normalize(u_ViewPosition - v_FragPos);
    
    vec3 result = vec3(0.0);
    // 3. 计算平行光
    int dirCount = min(u_DirLightCount, MAX_DIR_LIGHTS);
    for(int i = 0; i < dirCount; i++)
    {
        vec3 lightDir = normalize(-u_DirLights[i].direction);
        
        float shadow = 0.0;
        
        if(i == 0) {
            shadow = ShadowCalculation(v_FragPosLightSpace, norm, lightDir);
        }
        
        result += CalcDirLight(u_DirLights[i], norm, viewDir, diffMapColor, specMask, shadow);
    }

    // 4. 计算点光源
    int pointCount = min(u_PointLightCount, MAX_POINT_LIGHTS);
    for(int i = 0; i < pointCount; i++)
    {
        result += CalcPointLight(u_PointLights[i], norm, v_FragPos, viewDir, diffMapColor, specMask);
    }

    // Tone Mapping
    // result = result / (result + vec3(1.0));
    
    FinalColor = vec4(result, 1.0);
    EntityIDBuffer = v_EntityID;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffColor, float specMask, float shadow)
{
    vec3 lightDir = normalize(-light.direction);
    vec3 lightColor = light.color * light.intensity;
    
    float diff = max(dot(normal, lightDir), 0.0);
    
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shininess);
    
    if(diff <= 0.0) spec = 0.0;

    // 不再使用 light.ambient/diffuse/specular
    // 传统模型里，漫反射和高光通常都反射光的颜色
    vec3 ambient  = vec3(0.1) * lightColor * diffColor;
    vec3 diffuse  = diff * lightColor * diffColor;
    vec3 specular = spec * lightColor * u_ColorSpecular * specMask;
    // vec3 ambient  = light.ambient  * light.color * diffColor;
    // vec3 diffuse  = light.diffuse  * diff * light.color * diffColor;
    // vec3 specular = light.specular * spec * light.color * u_ColorSpecular * specMask;
    
    // 应用阴影：环境光不受影响，漫反射和高光受阴影遮蔽
    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffColor, float specMask)
{
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 lightColor = light.color * light.intensity;
    
    float diff = max(dot(normal, lightDir), 0.0);
    
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shininess);
    
    if(diff <= 0.0) spec = 0.0;

    float distance = length(light.position - fragPos);
    // 加一个很小的数 0.0001 避免除以零
    float attenuation = 1.0 / (distance * distance + 0.0001);  
    
    vec3 ambient  = vec3(0.1) * lightColor * diffColor;
    vec3 diffuse  = diff * lightColor * diffColor;
    vec3 specular = spec * lightColor * u_ColorSpecular * specMask;
    
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    
    return (ambient + diffuse + specular);
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if(projCoords.z > 1.0)
        return 0.0;
        
    // 保留一个较小的值，因为绘制阴影贴图时使用了正面剔除
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
    // float bias =0.0f;
    
    float shadow = 0.0;
    
    vec2 texelSize = 1.0 / vec2(textureSize(u_ShadowMap, 0));
    
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