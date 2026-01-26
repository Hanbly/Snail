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
uniform sampler2D u_BloomBlurTexture; // 模糊后的光晕 (Bloom)
uniform float u_BloomIntensity; // 泛光强度
uniform bool u_BloomEnabled;

uniform float u_Gamma; // 通常为 2.2
uniform float u_Exposure; // 曝光度，通常为 1.0

// ACES Tone Mapping 算法
// float 版本的 ACES
float aces_approx_scalar(float v)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((v*(a*v+b))/(v*(c*v+d)+e), 0.0f, 1.0f);
}

void main()
{
    vec3 hdrColor = texture(u_ScreenTexture, v_TexCoord).rgb;
    vec3 bloomColor = texture(u_BloomBlurTexture, v_TexCoord).rgb;

    // 曝光调整
    hdrColor *= u_Exposure;

    // 混合泛光
    if(u_BloomEnabled) {
        hdrColor += bloomColor * u_BloomIntensity; 
    }

    // 2. 计算当前颜色的亮度 (Luminance)
    // 使用 Rec.709 系数，或者更简单的 dot(color, vec3(0.299, 0.587, 0.114))
    float luminance = dot(hdrColor, vec3(0.2126, 0.7152, 0.0722));
    
    // 防止除以 0
    luminance = max(luminance, 0.0001);

    // 3. 对亮度进行 Tone Mapping，而不是对颜色通道
    float mappedLuminance = aces_approx_scalar(luminance);

    // 4. 计算缩放比例：将 HDR 亮度缩放到 LDR 范围
    vec3 mapped = hdrColor * (mappedLuminance / luminance);

    // 5. [关键步骤] 高光平滑去饱和 (Highlight Desaturation)
    // 如果不加这一步，极亮的红色会变成 (1.0, 0.0, 0.0) 而不是白色，看起来很假且刺眼。
    // 我们根据“原始亮度”来决定混合白色的程度。
    
    // 阈值控制：当亮度超过 2.0 时开始变白，超过 10.0 时完全变白 (参数可调)
    float desaturationFactor = smoothstep(10.0, 15000.0, luminance);
    
    // 混合目标：纯白色的亮度应该是 mappedLuminance
    vec3 whiteScale = vec3(mappedLuminance);
    
    // 执行混合：保留色相 -> 平滑过渡 -> 纯白
    mapped = mix(mapped, whiteScale, desaturationFactor);
    
    //vec3 mapped = ace_tone_mapping(hdrColor);

    // Gamma Correction (Linear -> sRGB)
    mapped = pow(mapped, vec3(1.0 / u_Gamma));

    color = vec4(mapped, 1.0);
}