#SHADER vertex
#version 330 core

// 只有位置输入了
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texture1_coords;

out vec3 v_Pos; // 输出位置给片段着色器，用来计算颜色
out vec2 v_texture1_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
    v_Pos = position; // 把原始坐标传给片段着色器
    v_texture1_coords = texture1_coords;
}

#SHADER fragment
#version 330 core

layout(location = 0) out vec4 FinalColor; // 最终输出到屏幕

in vec3 v_Pos;
in vec2 v_texture1_coords;

uniform vec4 u_Color;           // 整体色调
uniform sampler2D u_Texture1;   // 纹理 1 (比如箱子)
uniform sampler2D u_Texture2;   // 纹理 2 (比如笑脸)
uniform float u_MixValue;       // 混合比例 (0.0 = 全显示纹理1， 1.0 = 全显示纹理2)

void main()
{
    // 1. 分别采样
    vec4 tex1 = texture(u_Texture1, v_texture1_coords);
    vec4 tex2 = texture(u_Texture2, v_texture1_coords);

    // 2. 混合 (两种常见方式)

    // 方式 A: 线性插值 (Linear Interpolation)
    // 效果：像两张幻灯片叠在一起，u_MixValue 决定透明度
    vec4 mixedTexture = mix(tex1, tex2, u_MixValue); 

    // 方式 B: 乘法 (Multiplication)
    // 效果：像是在纹理1上画了纹理2（通常用于光照贴图或细节叠加）
    // vec4 mixedTexture = tex1 * tex2;

    // 3. 最终叠加 u_Color 并输出
    FinalColor = mixedTexture * u_Color;
}