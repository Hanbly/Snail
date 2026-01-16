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

uniform sampler2D u_Image;
uniform bool u_Horizontal; // true=水平模糊, false=垂直模糊
uniform float u_Weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main() {
    vec2 tex_offset = 1.0 / textureSize(u_Image, 0); // 单个像素大小
    vec3 result = texture(u_Image, v_TexCoords).rgb * u_Weight[0]; // 当前像素贡献
    
    if(u_Horizontal) {
        for(int i = 1; i < 5; ++i) {
            result += texture(u_Image, v_TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * u_Weight[i];
            result += texture(u_Image, v_TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * u_Weight[i];
        }
    } else {
        for(int i = 1; i < 5; ++i) {
            result += texture(u_Image, v_TexCoords + vec2(0.0, tex_offset.y * i)).rgb * u_Weight[i];
            result += texture(u_Image, v_TexCoords - vec2(0.0, tex_offset.y * i)).rgb * u_Weight[i];
        }
    }
    FragColor = vec4(result, 1.0);
}