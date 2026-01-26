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

// 扩散因子。
// 1.0 为默认间距。设置到 1.5 或 2.0 可以显著增加模糊半径，
// 但太大会导致出现条纹状伪影 (Banding artifacts)。
uniform float u_Spread;

void main() {
    vec2 tex_offset = 1.0 / textureSize(u_Image, 0); // 单个像素大小
    
    // 中心像素贡献
    vec3 result = texture(u_Image, v_TexCoords).rgb * u_Weight[0]; 
    
    // 计算采样方向和间距
    vec2 stride = (u_Horizontal ? vec2(tex_offset.x, 0.0) : vec2(0.0, tex_offset.y)) * u_Spread;

    for(int i = 1; i < 5; ++i) {
        // 使用乘以了 u_Spread 的 stride 进行采样
        vec2 offset = stride * float(i);
        result += texture(u_Image, v_TexCoords + offset).rgb * u_Weight[i];
        result += texture(u_Image, v_TexCoords - offset).rgb * u_Weight[i];
    }
    
    FragColor = vec4(result, 1.0);
}