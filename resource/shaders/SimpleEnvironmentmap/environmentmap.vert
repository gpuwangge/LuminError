#version 450
#include "../Common/constants.glsl"
#include "../Common/objectUBO.glsl"
#include "../Common/graphicsGlobalUBO.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;   //no use
layout(location = 2) in vec2 inTexCoord; //no use
layout(location = 3) in vec3 inNormal; 

//layout(location = 0) out vec3 fragColor;
//layout(location = 1) out vec2 fragTexCoord;
layout(location = 0) out vec3 pos;
layout(location = 1) out vec3 normal;

void main() {
    gl_Position = graphicsGlobalUBO.mainCameraProj * graphicsGlobalUBO.mainCameraView * objectUBO.model * vec4(inPosition, 1.0);
    //fragColor = inColor;
    //fragTexCoord = inTexCoord;
    pos = vec3(objectUBO.model * vec4(inPosition, 1.0f));
    normal = mat3(transpose(inverse(objectUBO.model))) * inNormal;
}