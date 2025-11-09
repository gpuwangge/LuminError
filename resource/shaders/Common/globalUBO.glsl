#ifndef GLOBALUBO_GLSL
#define GLOBALUBO_GLSL

layout(set = 0, binding = UNIFORM_GLOBAL_BINDING) uniform GlobalBufferObject {
	mat4 mainCameraModel;
    mat4 mainCameraView;
    mat4 mainCameraProj;
    float tanHalfFovY;
    float aspect;
    float padding[14];
} globalUBO;

#endif