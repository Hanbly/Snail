#type vertex
#version 450 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

out vec2 v_TexCoord;

void main()
{
    v_TexCoord = a_TexCoord;
    gl_Position = vec4(a_Position, 1.0);
}

#type fragment
#version 450 core
layout(location = 0) out vec4 color;
in vec2 v_TexCoord;

uniform sampler2D u_ScreenTexture;
uniform float u_Gamma; // 通常为 2.2
uniform float u_Exposure; // 曝光度，通常为 1.0

// ACES Tone Mapping 算法
vec3 ace_tone_mapping(vec3 x) {
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main()
{
    vec3 hdrColor = texture(u_ScreenTexture, v_TexCoord).rgb;

    // 1. 曝光调整 (可选)
    hdrColor *= u_Exposure;

    // 2. Tone Mapping (HDR -> LDR)
    // 这里使用 ACES 电影级色调映射，能很好处理 EXR 的高光
    vec3 mapped = ace_tone_mapping(hdrColor);
    
    // 如果你不喜欢 ACES，也可以用简单的 Reinhard:
    // vec3 mapped = hdrColor / (hdrColor + vec3(1.0));

    // 3. Gamma Correction (Linear -> sRGB)
    // 这一步解决了"暗部细节不足"的问题
    mapped = pow(mapped, vec3(1.0 / u_Gamma));

    color = vec4(mapped, 1.0);
}