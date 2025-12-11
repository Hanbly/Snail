#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;
uniform float u_Edge;

void main()
{
    vec3 pos = a_Position + u_Edge * normalize(a_Normal);
    gl_Position = u_ViewProjection * u_Model * vec4(pos, 1.0);
}

#type fragment
#version 330 core

out vec4 FinalColor; // 最终输出到屏幕

uniform vec4 u_Color;

void main()
{
    FinalColor = u_Color;
}