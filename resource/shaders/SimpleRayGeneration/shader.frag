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
    // 1. 像素坐标转标准化坐标 (0..1)
    // Vulkan 的 gl_FragCoord 原点在 framebuffer 下方
    vec2 uv = gl_FragCoord.xy / screenSize;

    // ----------------------------
    // 2. 转换到 NDC [-1, 1]
    float x_ndc = uv.x * 2.0 - 1.0;
    float y_ndc = uv.y * 2.0 - 1.0;

    // ----------------------------
    // 3. 投影到相机平面 (camera space)
    // 这里假设相机朝 Z正方向
    vec3 dir_cam = normalize(vec3(
        x_ndc * globalUBO.aspect * globalUBO.tanHalfFovY,
        y_ndc * globalUBO.tanHalfFovY,
        1.0 //!not -1
    ));

    // ----------------------------
    // 4. 转换到 world space
    Ray ray;
    ray.dir = normalize((globalUBO.mainCameraModel * vec4(dir_cam, 0.0)).xyz);
    ray.origin = vec3(globalUBO.mainCameraModel[3]);

    // ----------------------------
    // 5. 可视化输出：根据射线方向上色（调试用）
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

    //test1
    // 直接在屏幕空间画圆，不依赖任何坐标系
    // vec2 pixelCoord = gl_FragCoord.xy;
    // vec2 screenCenter = vec2(400.0, 400.0); // 假设800x800屏幕
    
    // float dst = length(pixelCoord - screenCenter);
    
    // if (dst < 200.0) { // 半径为200像素的圆
    //     outColor = vec4(1.0, 0.0, 0.0, 1.0); // 红色圆
    // } else {
    //     outColor = vec4(0.0, 0.0, 1.0, 1.0); // 蓝色背景
    // }

    //test2
    // vec2 pixelCoord = gl_FragCoord.xy;
    
    // // 将屏幕分成4个区域，分别显示不同颜色
    // if (pixelCoord.x < 400.0 && pixelCoord.y < 400.0) {
    //     outColor = vec4(1.0, 0.0, 0.0, 1.0); // 左下：红
    // } else if (pixelCoord.x >= 400.0 && pixelCoord.y < 400.0) {
    //     outColor = vec4(0.0, 1.0, 0.0, 1.0); // 右下：绿
    // } else if (pixelCoord.x < 400.0 && pixelCoord.y >= 400.0) {
    //     outColor = vec4(0.0, 0.0, 1.0, 1.0); // 左上：蓝
    // } else {
    //     outColor = vec4(1.0, 1.0, 0.0, 1.0); // 右上：黄
    // }

    //test3
    // uv = gl_FragCoord.xy / vec2(800.0, 800.0);
    // vec2 ndc = (uv * 2.0 - 1.0);
    
    // // 只测试屏幕中心的一条光线
    // if (length(ndc) < 0.1) { // 屏幕中心小区域
    //     //Ray ray;
    //     ray.origin = vec3(0.0, 0.0, 0.0);
    //     ray.dir = vec3(0.0, 0.0, 1.0); // 直接看向正前方
        
    //     Sphere sphere;
    //     sphere.radius = 0.5;
    //     sphere.position = vec3(0.0, 0.0, 2.0);
        
    //     HitInfo hit = RaySphere(ray, sphere);
        
    //     if (hit.didHit) {
    //         outColor = vec4(1.0, 0.0, 0.0, 1.0); // 中心命中：红色
    //     } else {
    //         outColor = vec4(0.0, 1.0, 0.0, 1.0); // 中心未命中：绿色
    //     }
    // } else {
    //     outColor = vec4(0.0, 0.0, 1.0, 1.0); // 其他区域：蓝色
    // }

    //test4
    // uv = gl_FragCoord.xy / vec2(800.0, 800.0);
    // vec2 ndc = (uv * 2.0 - 1.0);
    
    // //ray.origin = vec3(0.0, 0.0, 0.0);
    // //ray.dir = normalize(vec3(ndc.x, ndc.y, 1.0));
    // ray.origin = vec3(0, 0, -3.0);
    // ray.dir = normalize((globalUBO.mainCameraModel * vec4(dir_cam, 0.0)).xyz);
    
    // Sphere sphere;
    // sphere.radius = 0.5;
    // sphere.position = vec3(0.0, 0.0, 2.0); // 球在Z=2的位置
    
    // HitInfo hit = RaySphere(ray, sphere);
    
    // if (hit.didHit) {
    //     // 命中：用法向量着色
    //     outColor = vec4(0.5 * (hit.normal + vec3(1.0)), 1.0);
    // } else {
    //     outColor = vec4(0.0, 0.0, 0.3, 1.0); // 未命中：深蓝
    // }

    //test5
    //  uv = gl_FragCoord.xy / vec2(800.0, 800.0);
    // vec2 ndc = (uv * 2.0 - 1.0);
    
    //  ray;
    // ray.origin = vec3(0.0, 0.0, 0.0);
    // ray.dir = normalize(vec3(ndc.x, ndc.y, 1.0));
    
    // Sphere sphere;
    // sphere.radius = 0.5;
    // sphere.position = vec3(0.0, 0.0, 2.0);
    
    // // 直接计算，不调用函数
    // vec3 oc = ray.origin - sphere.position;
    // float a = dot(ray.dir, ray.dir);
    // float b = 2.0 * dot(oc, ray.dir);
    // float c = dot(oc, oc) - sphere.radius * sphere.radius;
    // float discriminant = b * b - 4.0 * a * c;
    
    // // 调试输出
    // if (discriminant > 0.0) {
    //     outColor = vec4(1.0, 0.0, 0.0, 1.0); // 有解：红色
    // } else {
    //     outColor = vec4(0.0, 1.0, 0.0, 1.0); // 无解：绿色
    // }
}