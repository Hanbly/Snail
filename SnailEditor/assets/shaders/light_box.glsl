#type vertex
#version 330 core

layout(location = 0) in vec3 position;
// mat4 自动占用 location 1, 2, 3, 4
layout(location = 3) in mat4 a_Model; 

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;

void main()
{
    vec4 worldPos = a_Model * vec4(position, 1.0);
    gl_Position = u_ViewProjection * worldPos;
}

#type fragment
#version 330 core

out vec4 FinalColor; // 最终输出到屏幕

uniform vec4 u_LightColor;

void main()
{
    FinalColor = u_LightColor;
}