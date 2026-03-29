#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal; // 传入模型法线

#ifdef INSTANCING
    // 实例化模式：矩阵来自顶点属性 (VBO)
    layout(location = 5) in mat4 a_Model; 
    layout(location = 12) in int a_EntityID;
#else
    // 普通模式：矩阵来自 Uniform
    uniform mat4 u_Model;
    uniform int u_EntityID;
#endif

flat out int v_EntityID;
out vec3 v_WorldPos; // 传递给片元着色器计算视角
out vec3 v_Normal;   // 传递给片元着色器计算光照

uniform mat4 u_ViewProjection;

void main()
{
    mat4 modelMatrix;
    int entityID;

#ifdef INSTANCING
    modelMatrix = a_Model;
    entityID = a_EntityID;
#else
    modelMatrix = u_Model;
    entityID = u_EntityID;
#endif

    v_EntityID = entityID;

    vec4 worldPos = modelMatrix * vec4(a_Position, 1.0);
    v_WorldPos = worldPos.xyz;
    
    // 提取旋转矩阵变换法线 (假设没有非均匀缩放)
    v_Normal = mat3(modelMatrix) * a_Normal; 

    gl_Position = u_ViewProjection * worldPos;
}

#type fragment
#version 330 core

layout(location = 0) out vec4 FinalColor;      // 输出到 GL_COLOR_ATTACHMENT0
layout(location = 1) out int EntityIDBuffer;   // 输出到 GL_COLOR_ATTACHMENT1 (GL_R8/GL_RED_INTEGER)

flat in int v_EntityID;
in vec3 v_WorldPos;
in vec3 v_Normal;

uniform vec4 u_LightColor;
uniform vec3 u_ViewPosition; // 传入相机世界坐标

void main()
{
    vec3 N = normalize(v_Normal);
    vec3 V = normalize(u_ViewPosition - v_WorldPos);

    // 计算视线与法线的点乘 (0.0 到 1.0，正中心为1，边缘为0)
    float NdotV = max(dot(N, V), 0.0);

    // 1. 基础底色：稍微压暗，给高光留出对比度空间
    vec3 baseColor = u_LightColor.rgb * 0.7;

    // 2. 中心高光 (Core Glow)：让球体中心发白/发亮，看起来像高能量发光核
    // pow的指数越大，白光核心越聚拢
    float coreIntensity = pow(NdotV, 3.0); 
    vec3 coreColor = mix(u_LightColor.rgb, vec3(1.0), 0.8) * coreIntensity;

    // 3. 边缘菲涅尔/轮廓光 (Rim Light)：让边缘有轻微的渐变和光晕感
    float rimIntensity = pow(1.0 - NdotV, 2.0);
    vec3 rimColor = u_LightColor.rgb * rimIntensity * 0.6;

    // 叠加组合颜色
    vec3 resultColor = baseColor + coreColor + rimColor;

    FinalColor = vec4(resultColor, u_LightColor.a);
    EntityIDBuffer = v_EntityID; 
}