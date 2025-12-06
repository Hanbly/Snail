#type vertex
#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;

void main()
{
    gl_Position = u_ViewProjection * u_Model * vec4(position, 1.0);
}

#type fragment
#version 330 core

out vec4 FinalColor; // 最终输出到屏幕

uniform vec4 u_LightColor;

void main()
{
    FinalColor = u_LightColor;
}