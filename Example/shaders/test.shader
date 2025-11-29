#SHADER vertex
#version 330 core

// 只有位置输入了
layout(location = 0) in vec3 position;

// 输出位置给片段着色器，用来计算颜色
out vec3 v_Pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
    v_Pos = position; // 把原始坐标传给片段着色器
}

#SHADER fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_Pos;

void main()
{
    // 将坐标 (-0.5 ~ 0.5) 映射到颜色范围 (0.0 ~ 1.0)
    // 这样立方体就会变成彩色的：左边黑一点，右边红一点，上面绿一点
    color = vec4(v_Pos + 0.5, 1.0);
}