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

uniform isampler2D u_MaskTexture; // Mask ID
uniform sampler2D u_DepthTexture; // 深度图
uniform vec3 u_OutlineColor;
uniform int u_OutlineWidth;
uniform float u_Width;
uniform float u_Height;

void main()
{
    // 1. 如果自己就是选中物体，不绘制 (内部抠空)
    int centerID = texture(u_MaskTexture, v_TexCoords).r;
    if (centerID == 1) {
        discard;
    }

    vec2 texelSize = vec2(1.0 / u_Width, 1.0 / u_Height);
    
    // 2. 获取当前像素的深度
    float centerDepth = texture(u_DepthTexture, v_TexCoords).r;

    bool isEdge = false;

    // 3. 搜索周边
    for (int x = -u_OutlineWidth; x <= u_OutlineWidth; ++x) 
    {
        for (int y = -u_OutlineWidth; y <= u_OutlineWidth; ++y) 
        {
            if (x == 0 && y == 0) continue;

            vec2 sampleUV = v_TexCoords + vec2(x, y) * texelSize;
            int neighborID = texture(u_MaskTexture, sampleUV).r;
            
            if (neighborID == 1) 
            {
                // 读取该邻居(选中物体)的深度
                float neighborDepth = texture(u_DepthTexture, sampleUV).r;

                // 只有当 [当前像素深度] >= [物体深度] 时才绘制描边
                // 也就是说：只有当我是背景，或者我在物体后面时，才显示描边。
                // 如果我比物体深度小(比物体近)，说明我挡住了物体，就不画线。
                // 加上一个小偏移量(epsilon)防止Z-fighting闪烁
                if (centerDepth >= neighborDepth - 0.0001) {
                    isEdge = true;
                    break; 
                }
            }
        }
        if (isEdge) break;
    }

    if (isEdge) {
        FragColor = vec4(u_OutlineColor, 1.0);
    } else {
        discard;
    }
}