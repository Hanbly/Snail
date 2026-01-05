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
// 输出光照空间矩阵
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

#define MAX_DIR_LIGHTS 4
#define MAX_POINT_LIGHTS 16

layout(location = 0) out vec4 FinalColor;
layout(location = 1) out int EntityIDBuffer;

in vec2 v_TextureCoords;
in vec3 v_Normal;
in vec3 v_FragPos;
flat in int v_EntityID;
in vec4 v_FragPosLightSpace;

uniform sampler2D u_Diffuse1;
uniform sampler2D u_Specular1;
uniform sampler2D u_Normal1;
uniform bool u_UseTexture;
uniform bool u_UseTextureNormal;
uniform sampler2D u_ShadowMap; 

uniform vec3 u_ColorDiffuse;
uniform vec3 u_ColorSpecular;
uniform vec3 u_ColorAmbient;
uniform float u_Shininess; 

uniform vec3 u_ViewPosition;

struct DirLight {
    vec3 direction;
    vec3 color;
    float ambient;
    float diffuse;
    float specular;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
    float ambient;
    float diffuse;
    float specular;
};

uniform DirLight u_DirLights[MAX_DIR_LIGHTS];
uniform int u_DirLightCount;

uniform PointLight u_PointLights[MAX_POINT_LIGHTS];
uniform int u_PointLightCount;

// TBN 计算
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(u_Normal1, v_TextureCoords).xyz * 2.0 - 1.0;

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

// 函数声明
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffColor, float specMask, float shadow);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffColor, float specMask);
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);

void main()
{
    // 1. 纹理/材质颜色采样
    vec4 texColor = texture(u_Diffuse1, v_TextureCoords);
    vec3 diffMapColor;
    if(u_UseTexture) {
        if(texColor.a < 0.1) discard;
        diffMapColor = texColor.rgb;
    } else {
        diffMapColor = u_ColorDiffuse;
    }

    // 2. 高光遮罩采样
    float specMask = 1.0;
    if (u_UseTexture) {
        specMask = texture(u_Specular1, v_TextureCoords).r;
    }
    
    // --- 法线计算 ---
    vec3 norm;
    if (u_UseTexture && u_UseTextureNormal) {
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
    result = result / (result + vec3(1.0));
    
    FinalColor = vec4(result, 1.0);
    EntityIDBuffer = v_EntityID;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffColor, float specMask, float shadow)
{
    vec3 lightDir = normalize(-light.direction);
    
    float diff = max(dot(normal, lightDir), 0.0);
    
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shininess);
    
    if(diff <= 0.0) spec = 0.0;

    vec3 ambient  = light.ambient  * light.color * diffColor;
    vec3 diffuse  = light.diffuse  * diff * light.color * diffColor;
    vec3 specular = light.specular * spec * light.color * u_ColorSpecular * specMask;
    
    // 应用阴影：环境光不受影响，漫反射和高光受阴影遮蔽
    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffColor, float specMask)
{
    vec3 lightDir = normalize(light.position - fragPos);
    
    float diff = max(dot(normal, lightDir), 0.0);
    
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shininess);
    
    if(diff <= 0.0) spec = 0.0;

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    vec3 ambient  = light.ambient  * light.color * diffColor;
    vec3 diffuse  = light.diffuse  * diff * light.color * diffColor;
    vec3 specular = light.specular * spec * light.color * u_ColorSpecular * specMask;
    
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
    // float bias = max(0.001 * (1.0 - dot(normal, lightDir)), 0.0001);
    float bias =0.0f;
    
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