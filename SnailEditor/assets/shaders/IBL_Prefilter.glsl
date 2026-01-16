#type vertex
#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 v_WorldPos;

uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
    v_WorldPos = aPos;
    gl_Position =  u_Projection * u_View * vec4(v_WorldPos, 1.0);
}

#type fragment
#version 330 core
out vec4 FragColor;
in vec3 v_WorldPos;

uniform samplerCube u_EnvironmentMap;
uniform float u_Roughness; // C++ 循环里每次传入不同的粗糙度 (0.0 到 1.0)
uniform float u_Resolution; // 贴图分辨率

const float PI = 3.14159265359;

// --- 低差异序列生成函数 (Hammersley) ---
float RadicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}

// --- 重要性采样 GGX ---
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness*roughness;
    
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
    
    // 球坐标 -> 笛卡尔
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
    
    // 切线空间 -> 世界空间
    vec3 up          = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
    
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

void main()
{		
    vec3 N = normalize(v_WorldPos);    
    vec3 R = N;
    vec3 V = R;

    const uint SAMPLE_COUNT = 1024u; // 采样次数，越多噪点越少但越慢
    float totalWeight = 0.0;   
    vec3 prefilteredColor = vec3(0.0);     

    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H  = ImportanceSampleGGX(Xi, N, u_Roughness);
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        if(NdotL > 0.0)
        {
            // 这里的 mipLevel 计算是为了减少采样时的噪点
            float D   = (u_Roughness * u_Roughness * u_Roughness * u_Roughness) / (PI * pow(dot(N, H) * dot(N, H) * (u_Roughness * u_Roughness - 1.0) + 1.0, 2.0));
            float pdf = D * max(dot(N, H), 0.0) / (4.0 * max(dot(H, V), 0.0)) + 0.0001; 
            float saTexel  = 4.0 * PI / (6.0 * u_Resolution * u_Resolution);
            float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);
            float mipLevel = u_Roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel); 
            
            prefilteredColor += textureLod(u_EnvironmentMap, L, mipLevel).rgb * NdotL;
            totalWeight      += NdotL;
        }
    }
    prefilteredColor = prefilteredColor / totalWeight;

    FragColor = vec4(prefilteredColor, 1.0);
}