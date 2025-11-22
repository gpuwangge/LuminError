#ifndef OBJECTUBO_GLSL
#define OBJECTUBO_GLSL

layout(set = 0, binding = UNIFORM_OBJECT_BINDING) uniform UniformBufferObject {
    mat4 model;
    bool identityCameraProj;
    bool identityCameraView;
    bool padding_bool[2];
    vec4 padding[11];
} objectUBO;

#endif