#type vertex
#version 330 core

layout(location = 0) in vec3 position;
// mat4 自动占用 location 1, 2, 3, 4
layout(location = 3) in mat4 a_Model; 
// 选中状态 0未选中 1选中
layout(location = 10) in int a_EntityID;

flat out int v_EntityID;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;

void main()
{
    
    v_EntityID = a_EntityID;

    vec4 worldPos = a_Model * vec4(position, 1.0);
    gl_Position = u_ViewProjection * worldPos;
}

#type fragment
#version 330 core

layout(location = 0) out vec4 FinalColor;      // 输出到 GL_COLOR_ATTACHMENT0
layout(location = 1) out int EntityIDBuffer;   // 输出到 GL_COLOR_ATTACHMENT1 (GL_R8/GL_RED_INTEGER)

flat in int v_EntityID;

uniform vec4 u_LightColor;

void main()
{
    FinalColor = u_LightColor;
    EntityIDBuffer = v_EntityID; 
}