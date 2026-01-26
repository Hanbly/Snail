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
uniform float u_Threshold; // 0.8 - 1.2
// 控制过渡区的柔软度。值越大，渐变越丰富，光球感越弱。
uniform float u_SoftKnee; 

void main() {
    vec3 color = texture(u_SceneTexture, v_TexCoords).rgb;
    
    // 计算亮度 (人眼感知亮度公式)
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));

    // --- Soft Knee 阈值计算 ---
    
    // 1. 计算一个基于阈值和柔软度的“膝盖”曲线
    // 我们希望在 (u_Threshold - u_SoftKnee) 开始有微光，
    // 在 (u_Threshold + u_SoftKnee) 达到全光。
    float knee = u_Threshold * u_SoftKnee; // 计算过渡区的一半宽度
    float soft = brightness - u_Threshold + knee;
    soft = clamp(soft, 0.0, 2.0 * knee);
    soft = soft * soft / (4.0 * knee + 0.00001); // 二次曲线过渡，避免除零

    // 2. 结合硬阈值和软阈值
    float contribution = max(soft, brightness - u_Threshold);
    // 归一化一下，防止过曝
    contribution /= max(brightness, 0.00001); 

    // 3. 应用贡献度
    // 最终得到的 brightColor 是只有高光部分，且边缘带有柔和过渡的颜色
    vec3 brightColor = color * contribution;

    FragColor = vec4(brightColor, 1.0);
}