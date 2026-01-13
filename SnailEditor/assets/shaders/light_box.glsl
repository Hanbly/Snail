#type vertex
#version 330 core


layout(location = 0) in vec3 position;
#ifdef INSTANCING
    // 实例化模式：矩阵来自顶点属性 (VBO)
    layout(location = 5) in mat4 a_Model; 
    layout(location = 12) in int a_EntityID;
#else
    // 普通模式：矩阵来自 Uniform
    uniform mat4 u_Model;
    uniform int u_EntityID;
#endif

flat out int v_EntityID;

uniform mat4 u_ViewProjection;

void main()
{
    mat4 modelMatrix;
    int entityID;

#ifdef INSTANCING
    modelMatrix = a_Model;
    entityID = a_EntityID;
#else
    modelMatrix = u_Model;
    entityID = u_EntityID;
#endif

    v_EntityID = entityID;

    vec4 worldPos = modelMatrix * vec4(position, 1.0);
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