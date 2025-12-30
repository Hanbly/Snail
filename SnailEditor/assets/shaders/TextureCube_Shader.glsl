#type vertex
#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 v_TexCoords;

uniform mat4 u_ViewProjection;

void main()
{
    v_TexCoords = aPos;

    vec4 pos = u_ViewProjection * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}

#type fragment
#version 330 core

out vec4 FinalColor; // 最终输出到屏幕

in vec3 v_TexCoords;

uniform samplerCube u_Cubemap1;
uniform bool u_UseTexture;

void main()
{
    if(u_UseTexture){
        FinalColor = texture(u_Cubemap1, v_TexCoords);
    }
    else{
        FinalColor = vec4(0.1, 0.1, 0.1, 0.1);
    }
}