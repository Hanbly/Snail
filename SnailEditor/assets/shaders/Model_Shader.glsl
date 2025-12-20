#type vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 TextureCoords;
// mat4 自动占用 location 3, 4, 5, 6
layout(location = 3) in mat4 a_Model; 
// 法线矩阵 (loc 7,8,9) - 直接从 CPU 传
layout(location = 7) in mat3 a_NormalMatrix;

out vec2 v_TextureCoords;
out vec3 v_Normal;
out vec3 v_FragPos;

// uniform mat4 u_Model;
uniform mat4 u_ViewProjection;
uniform mat3 u_NormalMatrix;

void main()
{
    // ------------ 临时 ---------------- TODO: 把法线矩阵计算移回 cpu
    vec4 worldPos = a_Model * vec4(position, 1.0);

    v_Normal = normalize(a_NormalMatrix * a_Normal);
    v_FragPos = vec3(worldPos);
    gl_Position = u_ViewProjection * worldPos;
    // ---------------------------------

    //gl_Position = u_ViewProjection * u_Model * vec4(position, 1.0);
    v_TextureCoords = TextureCoords;
    //v_Normal = normalize(u_NormalMatrix * a_Normal);
    //v_FragPos = vec3(u_Model * vec4(position, 1.0));
}

#type fragment
#version 330 core

out vec4 FinalColor;

in vec2 v_TextureCoords;
in vec3 v_Normal;
in vec3 v_FragPos;

uniform vec4 u_LightColor;
uniform sampler2D u_Diffuse1;
uniform sampler2D u_Specular1;
uniform bool u_UseTexture;
uniform vec3 u_LightPosition;
uniform vec3 u_ViewPosition;

uniform float u_AmbientStrength;     // 环境光照系数
uniform float u_DiffuseStrength;     // 漫反射系数
uniform float u_SpecularStrength;    // 镜面反射系数

// MTL 的颜色变量
uniform vec3 u_ColorDiffuse;  // Kd
uniform vec3 u_ColorSpecular; // Ks
uniform vec3 u_ColorAmbient;  // Ka
uniform float u_Shininess;    // Ns

void main()
{
    // 1. 获取基础颜色 (有纹理用纹理，没纹理用 Kd)
    vec4 objectColor;
    if(u_UseTexture)
        objectColor = texture(u_Diffuse1, v_TextureCoords);
    else
        objectColor = vec4(u_ColorDiffuse, 1.0);

    if(objectColor.a < 0.1)
        discard;




    // 2. 环境光 (Ambient = Ka * LightColor)
    vec3 ambient = u_ColorAmbient * u_LightColor.rgb * vec3(u_AmbientStrength);

    // 3. 漫反射 (Diffuse = Diff * LightColor)
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(u_LightPosition - v_FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_LightColor.rgb * vec3(u_DiffuseStrength);

    // 4. 镜面反射 (Specular = Spec * Ks * LightColor)
    vec3 viewDir = normalize(u_ViewPosition - v_FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shininess);
    
    float specMask = 1.0; // 默认全反光
    if (u_UseTexture) {
        // 从纹理采样。因为高光图通常是黑白的，取红色通道 r 即可
        // 这决定了“哪里该亮，哪里不该亮”
        specMask = texture(u_Specular1, v_TextureCoords).r;
    }
    // 原始高光 * 材质高光色(Ks) * 光源颜色 * 全局强度系数 * 贴图遮罩(specMask)
    vec3 specular = spec * u_ColorSpecular * u_LightColor.rgb * vec3(u_SpecularStrength) * specMask;




    // 5. 组合结果
    // (环境光 + 漫反射) * 物体颜色 + 镜面高光
    vec3 result = (ambient + diffuse + specular) * objectColor.rgb;

    FinalColor = vec4(result, 1.0);
}