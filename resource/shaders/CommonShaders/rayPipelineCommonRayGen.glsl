#ifndef RAY_COMMONRAYGEN_GLSL
#define RAY_COMMONRAYGEN_GLSL

layout(location = 0) rayPayloadEXT PrimaryPayloadStruct primaryPayload;

layout(set = 0, binding = 1) uniform accelerationStructureEXT topLevelAS;

layout(set = 0, binding = 4) uniform GlobalUniformInfo {
    mat4 mainCameraView;
    mat4 mainCameraViewInverse;
    mat4 mainCameraProj;
    mat4 mainCameraProjInverse; 
    vec3 mainCameraPos;
    float tanHalfFovY;
    float aspect;
    float padding[58];
} globalUBO;

const uint MAX_PATHS = 8u;//最大支持的path。不是实际使用的path
vec3 TracePixelRadiance(ivec2 size, ivec2 pixel){
    vec3 accumulatedRadiance = vec3(0.0);

    for (int s = 0; s < configUBO.spp; ++s){
        vec2 jitter = rnd2(pixel, s, customUBO.frameCount);
        vec2 uv = (vec2(pixel) + jitter) / vec2(size);
        vec2 ndc = uv * 2.0 - 1.0;
        // ndc.y = -ndc.y;// 如有需要再开

        vec4 viewDirH = globalUBO.mainCameraProjInverse * vec4(ndc, 1.0, 1.0);
        vec3 rayDir = normalize((globalUBO.mainCameraViewInverse * vec4(normalize(viewDirH.xyz), 0.0)).xyz);
        vec3 rayOrigin = globalUBO.mainCameraPos;

        vec3 lightColor = vec3(0.0);

        PathStateStruct pathStack[MAX_PATHS]; //最大路径数
        int stackSize = 0;

        pathStack[stackSize++] = PathStateStruct(
            rayOrigin,
            rayDir,
            vec3(1.0),
            1.0, //ior
            vec3(0.0),   // mediumEntryPos
            0u,          // insideMedium=0，假设一开始在空气里
            0u //depth
        );

        primaryPayload.sampleIndex = s;

        while(stackSize > 0){
            PathStateStruct path = pathStack[--stackSize];

            primaryPayload.radiance = vec3(0.0);
            primaryPayload.throughput = path.throughput;
            primaryPayload.spawnRayCount = 0u;
            primaryPayload.done = 0u;
            primaryPayload.currentIOR = path.currentIOR;
            primaryPayload.insideMedium = path.insideMedium;
            primaryPayload.mediumEntryPos = path.mediumEntryPos;
            primaryPayload.depth = path.depth;

            traceRayEXT(
                topLevelAS,
                0, //gl_RayFlagsOpaqueEXT,
                0xFF,
                0, 1, 0,
                path.origin,
                EPSILON,
                path.direction,
                1e32,
                0
            );
            //path.throughput = vec3(1.0);//test
            accumulatedRadiance += path.throughput * primaryPayload.radiance;

            if(path.depth + 1u >= configUBO.maxBounce) continue;

            for(uint i = 0u; i < primaryPayload.spawnRayCount; ++i){
                PathStateStruct nextPath;

                nextPath.origin         = primaryPayload.nextRay[i].origin;
                nextPath.direction      = primaryPayload.nextRay[i].dir;
                nextPath.throughput     = path.throughput * primaryPayload.nextRay[i].throughputMul;
                nextPath.currentIOR     = primaryPayload.nextRay[i].currentIOR;
                nextPath.insideMedium   = primaryPayload.nextRay[i].insideMedium;
                nextPath.mediumEntryPos = primaryPayload.nextRay[i].mediumEntryPos;

                nextPath.depth = path.depth + 1u;
                pathStack[stackSize++] = nextPath;
                if(stackSize >= configUBO.maxPath) break;
            }
        }//stack
        
    }//spp

    return accumulatedRadiance / float(configUBO.spp);
}

#endif