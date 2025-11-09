#ifndef GLOBALUBO_GLSL
#define GLOBALUBO_GLSL

layout(set = 0, binding = UNIFORM_GLOBAL_BINDING) uniform GlobalBufferObject {
    mat4 mainCameraView;
    mat4 mainCameraViewInverse;
    mat4 mainCameraProj;
    //mat4 mainCameraProjInverse; 
    vec3 mainCameraPos;
    float tanHalfFovY;
    float aspect;
    //float padding[58];
    float padding[14];
} globalUBO;

#endif