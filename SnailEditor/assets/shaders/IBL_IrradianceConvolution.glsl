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

uniform samplerCube u_EnvironmentMap; // 转换好的 Cubemap

const float PI = 3.14159265359;

void main()
{		
    // 这里的 v_WorldPos 就是这一方向的法线
    vec3 N = normalize(v_WorldPos);

    vec3 irradiance = vec3(0.0);   
    
    // 我们需要沿着半球表面进行卷积积分
    vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, N));
    up         = normalize(cross(N, right));

    float sampleDelta = 0.025; // 采样步长，越小越精确但越慢
    float nrSamples = 0.0; 
    
    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            // 1. 球坐标 -> 笛卡尔坐标 (切线空间)
            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            
            // 2. 切线空间 -> 世界空间
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N; 

            // 3. 采样环境图，并应用余弦加权 (cos(theta)) 和 面积加权 (sin(theta))
            irradiance += texture(u_EnvironmentMap, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(nrSamples));
    
    FragColor = vec4(irradiance, 1.0);
}