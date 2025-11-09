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
    vec3 offsetRayOrigin = ray.origin - sphere.position;

    //sqrLength(rayOrigin + rayDir * dst) = radius^2
    float a = dot(ray.dir, ray.dir);
    float b = 2 * dot(offsetRayOrigin, ray.dir);
    float c = dot(offsetRayOrigin, offsetRayOrigin) - sphere.radius * sphere.radius;
    //Quadratic discriminant
    float discriminant = b * b - 4 * a * c;

    if(discriminant >= 0){
        float dst = (-b - sqrt(discriminant)) / (2 * a);
        if(dst < 0){
            hitInfo.didHit = true;
            hitInfo.dst = dst;
            hitInfo.hitPoint = ray.origin + ray.dir * dst;
            hitInfo.normal = normalize(hitInfo.hitPoint - sphere.position);
        }
    }

    return hitInfo;
}

void main() {
	//outColor = texture(texSampler, fragTexCoord);
    
    //vec2 screenSize = vec2(1.0, 1.0);//or 800 x 800?
    vec2 screenSize = vec2(800.0, 800.0);
    // ----------------------------
    // 1. 像素坐标转标准化坐标 (0..1)
    // Vulkan 的 gl_FragCoord 原点在 framebuffer 下方
    vec2 uv = gl_FragCoord.xy / screenSize;

    // ----------------------------
    // 2. 转换到 NDC [-1, 1]
    float x_ndc = uv.x * 2.0 - 1.0;
    float y_ndc = uv.y * 2.0 - 1.0;

    // ----------------------------
    // 3. 投影到相机平面 (camera space)
    // 这里假设相机朝 -Z，看向前方
    //camera_position: [0,0,-3]
    vec3 dir_cam = normalize(vec3(
        x_ndc * globalUBO.aspect * globalUBO.tanHalfFovY,
        y_ndc * globalUBO.tanHalfFovY,
        -1.0
    ));

    // ----------------------------
    // 4. 转换到 world space
    Ray ray;
    ray.dir = normalize((globalUBO.mainCameraModel * vec4(dir_cam, 0.0)).xyz);
    ray.origin = vec3(0.0);
    //vec3 rayOrigin = cam.camPos; //todo:?
    //if(rayDir.x < 0) rayDir.x = 0;
    //if(rayDir.y < 0) rayDir.y = 0;
    //if(rayDir.z < 0) rayDir.z = 0;

    // ----------------------------
    // 5. 可视化输出：根据射线方向上色（调试用）
    //vec3 color = 0.5 * (rayDir + vec3(1.0));
    //vec3 color = ray.dir;
    Sphere sphere0;
    sphere0.radius = 0.5f;
    sphere0.position = vec3(0,0,1);
    HitInfo closest_intersection = RaySphere(ray, sphere0);
    vec3 color = vec3(closest_intersection.didHit, closest_intersection.didHit, closest_intersection.didHit);

    outColor = vec4(color, 1.0);
}