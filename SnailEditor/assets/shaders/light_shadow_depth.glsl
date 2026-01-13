#type vertex
#version 330 core

layout (location = 0) in vec3 a_Position; 

#ifdef INSTANCING
    layout(location = 5) in mat4 a_Model;
#else
    uniform mat4 u_Model;
#endif

uniform mat4 u_LightSpaceMatrix;

void main()
{
    mat4 modelMatrix;
    
#ifdef INSTANCING
    modelMatrix = a_Model;
#else
    modelMatrix = u_Model;
#endif

    gl_Position = u_LightSpaceMatrix * modelMatrix * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

void main()
{
    // gl_FragDepth = gl_FragCoord.z;
}