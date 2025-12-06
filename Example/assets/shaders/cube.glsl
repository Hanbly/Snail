#type vertex
#version 330 core

// 只有位置输入了
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texture1_coords;
layout(location = 2) in vec3 a_Normal;

out vec2 v_texture1_coords;
out vec3 v_Normal;
out vec3 v_FragPos;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;

void main()
{
    gl_Position = u_ViewProjection * u_Model * vec4(position, 1.0);
    v_texture1_coords = texture1_coords;

    // [重要修复]：计算法线矩阵并变换法线到世界空间
    // 注意：在实际引擎中，这个计算通常在 CPU 端完成并作为一个 mat3 uniform 传入，
    // 因为在顶点着色器中对每个顶点都做逆矩阵计算开销很大。
    // 但为了演示原理，这里先写在 Shader 里。
    mat3 normalMatrix = transpose(inverse(mat3(u_Model)));
    v_Normal = normalize(normalMatrix * a_Normal);

    v_FragPos = vec3(u_Model * vec4(position, 1.0));
}

#type fragment
#version 330 core

out vec4 FinalColor; // 最终输出到屏幕

in vec2 v_texture1_coords;
in vec3 v_Normal;
in vec3 v_FragPos;

uniform vec4 u_LightColor;      // 光照色调
uniform sampler2D u_Texture1;   // 纹理 1
uniform sampler2D u_Texture2;   // 纹理 2
uniform float u_MixValue;       // 混合比例 (0.0 = 全显示纹理1， 1.0 = 全显示纹理2)
uniform vec3 u_LightPosition;   // 光源位置
uniform vec3 u_ViewPosition;    // 相机的观察位置

void main()
{
    // 1. 分别采样纹理12
    vec4 tex1 = texture(u_Texture1, v_texture1_coords);
    vec4 tex2 = texture(u_Texture2, v_texture1_coords);

    // 2. 混合纹理 (两种常见方式)

    // 方式 A: 线性插值 (Linear Interpolation)
    // 效果：像两张幻灯片叠在一起，u_MixValue 决定透明度
    vec4 mixedTexture = mix(tex1, tex2, u_MixValue);

    // 方式 B: 乘法 (Multiplication)
    // 效果：像是在纹理1上画了纹理2（通常用于光照贴图或细节叠加）
    // vec4 mixedTexture = tex1 * tex2;



    // 3. 光照计算

    // 环境光照
    float ambientStrength = 0.1; // 环境光照系数
    vec4 ambient = vec4(vec3(ambientStrength), 1.0) * u_LightColor;
    // 漫反射
    float diffuseStrength = 0.5; // 环境光照系数
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(u_LightPosition - v_FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = vec4(vec3(diffuseStrength * diff), 1.0) * u_LightColor;
    // 镜面反射
    float specularStrength = 0.5; // 镜面反射系数
    vec3 viewDir = normalize(u_ViewPosition - v_FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
    vec4 specular = vec4(vec3(specularStrength * spec), 1.0) * u_LightColor;

    FinalColor = mixedTexture * (ambient + diffuse + specular);
}