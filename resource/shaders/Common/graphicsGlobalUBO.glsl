#ifndef GRAPHICSGLOBALUBO_GLSL
#define GRAPHICSGLOBALUBO_GLSL

layout(set = 0, binding = UNIFORM_GLOBAL_BINDING) uniform GraphicsGlobalBufferObject {
    mat4 mainCameraView;
    mat4 mainCameraViewInverse;
    mat4 mainCameraProj;
    mat4 mainCameraProjInverse; 
    vec3 mainCameraPos;
    float tanHalfFovY;
    float aspect;
    float padding[58];
    //float padding[14];
} graphicsGlobalUBO;

#endif