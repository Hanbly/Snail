#type vertex
#version 330 core
layout (location = 0) in vec2 a_Pos;
layout (location = 1) in vec2 a_TexCoords;
out vec2 v_TexCoords;
void main() {
    v_TexCoords = a_TexCoords;
    gl_Position = vec4(a_Pos.x, a_Pos.y, 0.0, 1.0); 
}

#type fragment
#version 330 core

out vec4 FragColor;
in vec2 v_TexCoords;

uniform isampler2D u_MaskTexture;
uniform sampler2D u_DepthTexture;
uniform vec3 u_OutlineColor;
uniform int u_OutlineWidth;
uniform float u_Width;
uniform float u_Height;

// 必须与 C++ 相机设置完全一致
uniform float u_Near;
uniform float u_Far;

// 将深度值转换为线性距离 (View Space Z)
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * u_Near * u_Far) / (u_Far + u_Near - z * (u_Far - u_Near));	
}

void main()
{
    int centerMask = texture(u_MaskTexture, v_TexCoords).r;
    if (centerMask == 1) discard;

    // 获取线性深度 (单位：米/Unit)
    float rawCenterDepth = texture(u_DepthTexture, v_TexCoords).r;
    float linearCenterDepth = LinearizeDepth(rawCenterDepth);

    vec2 texelSize = vec2(1.0 / u_Width, 1.0 / u_Height);
    bool isEdge = false;

    for (int x = -u_OutlineWidth; x <= u_OutlineWidth; ++x) 
    {
        for (int y = -u_OutlineWidth; y <= u_OutlineWidth; ++y) 
        {
            if (x==0 && y==0) continue;

            vec2 uv = v_TexCoords + vec2(x, y) * texelSize;
            int neighborMask = texture(u_MaskTexture, uv).r;

            if (neighborMask == 1) 
            {
                float rawNeighborDepth = texture(u_DepthTexture, uv).r;
                float linearNeighborDepth = LinearizeDepth(rawNeighborDepth);

                // linearCenterDepth: 当前片段离相机的距离
                // linearNeighborDepth: 选中物体片段距离相机的距离
                
                // 情况 1: Center是前面的遮挡物片段
                // linearCenterDepth (10米) < linearNeighborDepth (20米)
                // -> 不画线
            
                // 情况 2: Center是后面的墙/背景片段
                // linearCenterDepth (50米) > linearNeighborDepth (20米)
                // -> 画线
            
                // 情况 3: Center是物体脚下的地面 (深度非常接近)片段
                // -> 画线
                
                // 所以：只有当 center >= neighbor (减去一个线性容差) 时才画
                // 这里的 0.1 代表 0.1 个单位距离，比 raw depth 的 0.0001 靠谱得多
                if (linearCenterDepth >= (linearNeighborDepth - 0.1)) {
                    isEdge = true;
                    break;
                }
            }
        }
        if (isEdge) break;
    }

    if (isEdge) FragColor = vec4(u_OutlineColor, 1.0);
    else discard;
}