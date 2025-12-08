#type vertex
#version 330 core

// 只有位置输入了
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 TextureCoords;

out vec2 v_TextureCoords;
out vec3 v_Normal;
out vec3 v_FragPos;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;
uniform mat3 u_NormalMatrix;

void main()
{
    gl_Position = u_ViewProjection * u_Model * vec4(position, 1.0);
    v_TextureCoords = TextureCoords;

    v_Normal = normalize(u_NormalMatrix * a_Normal);

    v_FragPos = vec3(u_Model * vec4(position, 1.0));
}

#type fragment
#version 330 core

out vec4 FinalColor; // 最终输出到屏幕

in vec2 v_TextureCoords;
in vec3 v_Normal;
in vec3 v_FragPos;

uniform vec4 u_LightColor;      // 光照色调
uniform sampler2D u_Diffuse1;   // 纹理 1
uniform sampler2D u_Diffuse2;   // 纹理 2
uniform float u_MixValue;       // 混合比例 (0.0 = 全显示纹理1， 1.0 = 全显示纹理2)
uniform vec3 u_LightPosition;   // 光源位置
uniform vec3 u_ViewPosition;    // 相机的观察位置

uniform float u_AmbientStrength;     // 环境光照系数
uniform float u_DiffuseStrength;     // 漫反射系数
uniform float u_SpecularStrength;    // 镜面反射系数
uniform float u_Shininess;           // 反光度

void main()
{
    // 1. 分别采样纹理12
    vec4 tex1 = texture(u_Diffuse1, v_TextureCoords);
    vec4 tex2 = texture(u_Diffuse2, v_TextureCoords);

    // 2. 混合纹理 (两种常见方式)

    // 方式 A: 线性插值 (Linear Interpolation)
    // 效果：像两张幻灯片叠在一起，u_MixValue 决定透明度
    vec4 mixedTexture = mix(tex1, tex2, u_MixValue);

    // 方式 B: 乘法 (Multiplication)
    // 效果：像是在纹理1上画了纹理2（通常用于光照贴图或细节叠加）
    // vec4 mixedTexture = tex1 * tex2;



    // 3. 光照计算

    // 环境光照
    vec4 ambient = vec4(vec3(u_AmbientStrength), 1.0) * u_LightColor;
    // 漫反射
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(u_LightPosition - v_FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = vec4(vec3(u_DiffuseStrength * diff), 1.0) * u_LightColor;
    // 镜面反射
    vec3 viewDir = normalize(u_ViewPosition - v_FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shininess);
    vec4 specular = vec4(vec3(u_SpecularStrength * spec), 1.0) * u_LightColor;

    FinalColor = mixedTexture * (ambient + diffuse + specular);
}