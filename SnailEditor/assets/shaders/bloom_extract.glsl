#type vertex
#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;
out vec2 v_TexCoords;
void main() {
    v_TexCoords = a_TexCoords;
    gl_Position = vec4(a_Position, 1.0);
}

#type fragment
#version 330 core
out vec4 FragColor;
in vec2 v_TexCoords;

uniform sampler2D u_SceneTexture;
uniform float u_Threshold; // 比如 1.0

void main() {
    vec3 color = texture(u_SceneTexture, v_TexCoords).rgb;
    
    // 计算亮度 (人眼感知亮度公式)
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    
    if(brightness > u_Threshold)
        FragColor = vec4(color, 1.0);
    else
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}