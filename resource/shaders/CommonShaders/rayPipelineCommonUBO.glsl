#ifndef RAY_COMMONUBO_GLSL
#define RAY_COMMONUBO_GLSL

layout(set = 0, binding = 5) uniform CustomBufferObject {
    int frameCount;
    uint cameraInMotion;
    uint renderMode;      // 0 = Whitted, 1 = Path Tracing, 2 = ReSTIR(未实现), 3 = Bidirectional(未实现)
} customObject;

layout(set = 0, binding = 8) uniform ConfigUniformBufferStruct {
    uint lightCount;
    uint materialCount;

    uint spp;             // Samples Per Pixel
    uint maxBounce;       // 最大反弹次数
    uint maxPath;         // 最大路径数
    uint accumulate;      // 0 = 不积累, 1 = 帧间积累
    uint randomSeed;      // 可选，每次运行不同

    float rrProbability;   //RR（俄罗斯轮盘）
    uint enableNEE;
    uint useSky;
    float maxRadiance;
    uint debugMode;

    uint softShadowEnable; //for whitted style only
    uint softShadowSampleNumber; //for whitted style only

    uint maxReflectionDepth;
    uint maxRefractionDepth;
} configObject;

#endif