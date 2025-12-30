#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TextureCoords;

#ifdef INSTANCING
    // 实例化模式：矩阵来自顶点属性 (VBO)
    layout(location = 3) in mat4 a_Model; 
    layout(location = 7) in mat3 a_NormalMatrix;
    layout(location = 10) in int a_EntityID;
#else
    // 普通模式：矩阵来自 Uniform
    uniform mat4 u_Model;
    uniform mat3 u_NormalMatrix;
    uniform int u_EntityID;
#endif

out vec2 v_TextureCoords;
out vec3 v_Normal;
out vec3 v_FragPos;
flat out int v_EntityID;

uniform mat4 u_ViewProjection;

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

    // 计算世界坐标
    vec4 worldPos = modelMatrix * vec4(a_Position, 1.0);

    // 计算法线 (已在CPU端计算好 NormalMatrix)
    v_Normal = normalize(normalMatrix * a_Normal);    
    
    // --- 传递世界坐标给片元着色器用于光照计算 ---
    v_FragPos = vec3(worldPos);
    
    v_TextureCoords = a_TextureCoords;
    v_EntityID = entityID;

    
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

// 材质
uniform sampler2D u_Diffuse1;
uniform sampler2D u_Specular1;
uniform sampler2D u_Normal1;
uniform bool u_UseTexture;
uniform bool u_UseTextureNormal;

uniform vec3 u_ColorDiffuse;
uniform vec3 u_ColorSpecular;
uniform vec3 u_ColorAmbient;
uniform float u_Shininess; // 建议值: 32.0 ~ 256.0

uniform vec3 u_ViewPosition;

struct DirLight {
    vec3 direction;
    vec3 color; // rgb
    float ambient;
    float diffuse;
    float specular;
};

struct PointLight {
    vec3 position;
    vec3 color; // rgb
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

// 无需预计算切线，利用偏导数构建 TBN 并计算扰动后的法线
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
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffColor, float specMask);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffColor, float specMask);

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
        result += CalcDirLight(u_DirLights[i], norm, viewDir, diffMapColor, specMask);
    }

    // 4. 计算点光源
    int pointCount = min(u_PointLightCount, MAX_POINT_LIGHTS);
    for(int i = 0; i < pointCount; i++)
    {
        result += CalcPointLight(u_PointLights[i], norm, v_FragPos, viewDir, diffMapColor, specMask);
    }

    // 简单的色调映射或钳制，防止过曝成纯白
    result = result / (result + vec3(1.0)); // Reinhard tone mapping
    
    FinalColor = vec4(result, 1.0);
    EntityIDBuffer = v_EntityID;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffColor, float specMask)
{
    // 这里的 direction 是光线传播方向，所以取反指向光源
    vec3 lightDir = normalize(-light.direction);
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shininess);
    
    // 如果漫反射为0（光源在背面），强制取消高光
    if(diff <= 0.0) spec = 0.0;

    // 合并
    vec3 ambient  = light.ambient  * light.color * diffColor; // 注意：多光源时环境光会累加变白，调低 light.ambient
    vec3 diffuse  = light.diffuse  * diff * light.color * diffColor;
    vec3 specular = light.specular * spec * light.color * u_ColorSpecular * specMask;
    
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffColor, float specMask)
{
    vec3 lightDir = normalize(light.position - fragPos);
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shininess);
    
    // 背面剔除高光
    if(diff <= 0.0) spec = 0.0;

    // 衰减计算
    float distance = length(light.position - fragPos);
    // 衰减公式: 1 / (Kc + Kl*d + Kq*d^2)
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    // 组合
    vec3 ambient  = light.ambient  * light.color * diffColor;
    vec3 diffuse  = light.diffuse  * diff * light.color * diffColor;
    vec3 specular = light.specular * spec * light.color * u_ColorSpecular * specMask;
    
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    
    return (ambient + diffuse + specular);
}