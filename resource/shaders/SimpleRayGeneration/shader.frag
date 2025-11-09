#version 450
#include "../Common/constants.glsl"
#include "../Common/globalUBO.glsl"

layout(set = 1, binding = UNIFORM_TEXTURE_SAMPLER_BINDING) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

struct Ray{
    vec3 origin;
    vec3 dir;
};

struct Sphere{
    float radius;
    vec3 position;
};

struct HitInfo{
    bool didHit;
    float dst;
    vec3 hitPoint;
    vec3 normal;
};

HitInfo RaySphere(Ray ray, Sphere sphere){
    HitInfo hitInfo;
    hitInfo.didHit = false;
    hitInfo.dst = -1.0;

    vec3 oc = ray.origin - sphere.position;
    float a = dot(ray.dir, ray.dir);
    float b = 2.0 * dot(oc, ray.dir);
    float c = dot(oc, oc) - sphere.radius * sphere.radius;
    float discriminant = b * b - 4.0 * a * c;
    
    if (discriminant >= 0.0) {
        float sqrtDisc = sqrt(discriminant);
        float t1 = (-b - sqrtDisc) / (2.0 * a);
        float t2 = (-b + sqrtDisc) / (2.0 * a);
        
        // 修复：选择最小的正数解
        if (t1 > 0.0 && t2 > 0.0) {
            hitInfo.didHit = true;
            hitInfo.dst = min(t1, t2);
        } else if (t1 > 0.0) {
            hitInfo.didHit = true;
            hitInfo.dst = t1;
        } else if (t2 > 0.0) {
            hitInfo.didHit = true;
            hitInfo.dst = t2;
        }
        
        if (hitInfo.didHit) {
            hitInfo.hitPoint = ray.origin + ray.dir * hitInfo.dst;
            hitInfo.normal = normalize(hitInfo.hitPoint - sphere.position);
        }
    }


    // vec3 offsetRayOrigin = ray.origin - sphere.position;

    // //sqrLength(rayOrigin + rayDir * dst) = radius^2
    // float a = dot(ray.dir, ray.dir);
    // float b = 2 * dot(offsetRayOrigin, ray.dir);
    // float c = dot(offsetRayOrigin, offsetRayOrigin) - sphere.radius * sphere.radius;
    // //Quadratic discriminant
    // float discriminant = b * b - 4 * a * c;

    // if(discriminant >= 0){
    //     float dst = (-b - sqrt(discriminant)) / (2 * a);
    //     if(dst < 0){
    //         hitInfo.didHit = true;
    //         hitInfo.dst = dst;
    //         hitInfo.hitPoint = ray.origin + ray.dir * dst;
    //         hitInfo.normal = normalize(hitInfo.hitPoint - sphere.position);
    //     }
    // }

    return hitInfo;
}

void main() {
	//outColor = texture(texSampler, fragTexCoord);
    
    vec2 screenSize = vec2(800.0, 800.0);
    // ----------------------------
    // 1. Pixel cord conver to NDC(0..1)
    vec2 uv = gl_FragCoord.xy / screenSize;

    // ----------------------------
    // 2. Convert to NDC [-1, 1]
    float x_ndc = uv.x * 2.0 - 1.0;
    float y_ndc = uv.y * 2.0 - 1.0;

    // ----------------------------
    // 3. Project to camera space
    vec3 dir_cam = normalize(vec3(
        x_ndc * globalUBO.aspect * globalUBO.tanHalfFovY,
        y_ndc * globalUBO.tanHalfFovY,
        1.0 //!not -1, because we want camera to look positive z direction
    ));

    // ----------------------------
    // 4. Convert to world space
    Ray ray;

    //wrong
    //ray.dir = normalize((globalUBO.mainCameraModel * vec4(dir_cam, 0.0)).xyz);

    //假设 mainCameraModel 只包含旋转，但实际上它可能包含非均匀缩放或其他变换，导致方向失真。
    //mat3 cameraRotation = mat3(globalUBO.mainCameraModel);
    //ray.dir = normalize(cameraRotation * dir_cam);

    //问题：这只在视图矩阵是纯旋转矩阵时才正确，但实际上视图矩阵包含从世界空间到相机空间的完整变换。
    //mat3 viewRotation = mat3(globalUBO.mainCameraView);
    //ray.dir = normalize((viewRotation) * dir_cam);

    //可以运行的版本
    vec4 dir_clip = vec4(x_ndc, y_ndc, 0.0, 1.0); // 在裁剪空间
    // 逆投影变换到视图空间: 屏幕像素 → 裁剪空间 → 视图空间 → 世界空间
    // 步骤1：裁剪空间 → 视图空间（逆投影）
    vec4 dir_view = inverse(globalUBO.mainCameraProj) * dir_clip;
    dir_view = vec4(dir_view.xy, -1.0, 0.0); // 看向-Z
    // 变换到世界空间
    // 步骤2：视图空间 → 世界空间（逆视图变换） 
    //vec4 dir_world = inverse(globalUBO.mainCameraView) * dir_view;
    vec4 dir_world = globalUBO.mainCameraViewInverse * dir_view;
    ray.dir = normalize(dir_world.xyz);

    //优化版本：在host端求inverse view，以避免在shader中计算
    //vec4 dir_world = globalUBO.mainCameraViewInverse * vec4(x_ndc, y_ndc, 0.0, 1.0);
    //ray.dir = normalize(dir_world.xyz / dir_world.w - globalUBO.mainCameraPos);
    
    ray.origin = globalUBO.mainCameraPos;
    //ray.origin = vec3(globalUBO.mainCameraModel[3]); //an alternative to get camera pos

    // ----------------------------
    // 5. Output
    //vec3 color = 0.5 * (rayDir + vec3(1.0));
    //vec3 color = ray.dir;
    Sphere sphere0;
    sphere0.radius = 0.5f;
    sphere0.position = vec3(0,0,0);
    HitInfo hitInfo = RaySphere(ray, sphere0);
    //vec3 color = vec3(hitInfo.didHit, hitInfo.didHit, hitInfo.didHit);

    vec3 color;
    if (hitInfo.didHit) color = 0.5 * (hitInfo.normal + vec3(1.0));
    else color = vec3(0.7, 0.7, 0.7); // background
    
    outColor = vec4(color, 1.0);
}