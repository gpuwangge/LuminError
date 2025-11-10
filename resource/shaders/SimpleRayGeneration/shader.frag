#version 450
#include "../Common/constants.glsl"
#include "../Common/globalUBO.glsl"

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

struct Ray{
    vec3 origin;
    vec3 dir;
};

struct Material{
    vec3 color;
    float emissionStrength;
    vec3 emissionColor;
};

struct Sphere{
    float radius;
    vec3 position;
    Material material;
};

const int BOUNCE_LIMIT = 5;
const int RAY_PER_PIXEL = 50;

const int NUM_SPHERES = 4;
Sphere spheres[NUM_SPHERES];

struct HitInfo{
    bool didHit;
    float dst;
    vec3 hitPoint;
    vec3 normal;
    Material material;
};

// float Rand(inout uint state){
//     state = state * 1664525u + 1013904223u;
//     return float(state & 0x00FFFFFFu) / float(0x01000000u);
// }

// 改进的随机数生成器
uint WangHash(inout uint seed) {
    seed = uint(seed ^ uint(61)) ^ uint(seed >> uint(16));
    seed *= uint(9);
    seed = seed ^ (seed >> 4);
    seed *= uint(0x27d4eb2d);
    seed = seed ^ (seed >> 15);
    return seed;
}

float Rand(inout uint state) {
    return float(WangHash(state)) / 4294967296.0;
}

vec3 RandomDirectionInHemisphere(vec3 normal, inout uint state){
    float u1 = Rand(state);
    float u2 = Rand(state);

    float r = sqrt(u1);
    float theta = 2.0 * 3.14159265359 * u2;
    float x = r * cos(theta);
    float y = r * sin(theta);
    float z = sqrt(1.0 - u1);

    vec3 tangent = normalize(abs(normal.x) > 0.1 ? cross(normal, vec3(0,1,0)) : cross(normal, vec3(1,0,0)));
    vec3 bitangent = cross(tangent, normal);

    vec3 direction = x * tangent + y * bitangent + z * normal;
    return normalize(direction);
}

HitInfo RaySphereHitCheck(Ray ray, Sphere sphere){
    HitInfo hitInfo;
    hitInfo.didHit = false;
    hitInfo.dst = -1.0;
    hitInfo.material.color = vec3(0.0);
    hitInfo.material.emissionColor = vec3(0.0);
    hitInfo.material.emissionStrength = 0;

    vec3 oc = ray.origin - sphere.position;
    float a = dot(ray.dir, ray.dir);
    float b = 2.0 * dot(oc, ray.dir);
    float c = dot(oc, oc) - sphere.radius * sphere.radius;
    float discriminant = b * b - 4.0 * a * c;

    if (discriminant < 0.0) {
        return hitInfo;
    }

    float sqrtDisc = sqrt(discriminant);
    float t1 = (-b - sqrtDisc) / (2.0 * a);
    float t2 = (-b + sqrtDisc) / (2.0 * a);
    // 选择最小的正数解，并添加容差
    float t = min(t1, t2);
    if (t < 0.001) { // 避免自相交
        t = max(t1, t2);
    }
    
    if (t >= 0.001) {
        // float sqrtDisc = sqrt(discriminant);
        // float t1 = (-b - sqrtDisc) / (2.0 * a);
        // float t2 = (-b + sqrtDisc) / (2.0 * a);
        
        // // 修复：选择最小的正数解
        // if (t1 > 0.0 && t2 > 0.0) {
        //     hitInfo.didHit = true;
        //     hitInfo.dst = min(t1, t2);
        // } else if (t1 > 0.0) {
        //     hitInfo.didHit = true;
        //     hitInfo.dst = t1;
        // } else if (t2 > 0.0) {
        //     hitInfo.didHit = true;
        //     hitInfo.dst = t2;
        // }
        
        hitInfo.didHit = true;
        hitInfo.dst = t;
        hitInfo.hitPoint = ray.origin + ray.dir * t;
        hitInfo.normal = normalize(hitInfo.hitPoint - sphere.position);
        hitInfo.material = sphere.material;

        //if (hitInfo.didHit) {
        //    hitInfo.hitPoint = ray.origin + ray.dir * hitInfo.dst;
        //    hitInfo.normal = normalize(hitInfo.hitPoint - sphere.position);
        //    hitInfo.material = sphere.material;
        //}
    }

    return hitInfo;
}

HitInfo RaySpheresHitCheck(Ray ray){
    HitInfo hitInfo_closest;
    hitInfo_closest.didHit = false;
    hitInfo_closest.dst = -1.0;
    hitInfo_closest.material.color = vec3(0.7, 0.7, 0.7);
    hitInfo_closest.material.emissionStrength = 0;
    hitInfo_closest.material.emissionColor = vec3(0.0);
    for(int i = 0; i < NUM_SPHERES; i++){
        HitInfo hitInfo_current = RaySphereHitCheck(ray, spheres[i]);
        if(hitInfo_current.didHit && hitInfo_closest.didHit){ //current hit and previous hit
            if(hitInfo_current.dst < hitInfo_closest.dst) hitInfo_closest = hitInfo_current;
        }else if(hitInfo_current.didHit){ //current hit and no previous hit
            hitInfo_closest = hitInfo_current;
        }
    }
    return hitInfo_closest;
}

vec3 TraceRay(Ray ray, inout uint state){
    vec3 throughput = vec3(1.0);
    vec3 result_brightness_score = vec3(0.0);

    for(int i = 0; i < BOUNCE_LIMIT; i++ ){
        HitInfo hitInfo = RaySpheresHitCheck(ray);
        if(!hitInfo.didHit){
            result_brightness_score += vec3(0.005) * throughput;
            break;
        }

        vec3 emittedLight = hitInfo.material.emissionColor * hitInfo.material.emissionStrength;
        result_brightness_score += emittedLight * throughput;

        //if(hitInfo.material.emissionStrength > 0) break;
        
        ray.origin = hitInfo.hitPoint + hitInfo.normal * 0.001; //add a small value to avoid self-collision
        ray.dir = RandomDirectionInHemisphere(hitInfo.normal, state);
            
        throughput *= hitInfo.material.color;
        
        // 俄罗斯轮盘赌（从第3次反弹开始）
        if(i > 2) {
            float p = max(throughput.r, max(throughput.g, throughput.b));
            if(p < 0.001) break; // 吞吐量太小，直接终止
            
            if(Rand(state) > p) break;
            throughput /= p;
        }
    }

    return result_brightness_score;
}

void main() {
    spheres[0].radius = 0.75f;
    spheres[0].position = vec3(0, 0, -1);
    spheres[0].material.color = vec3(1, 1, 1);
    spheres[0].material.emissionStrength = 1.0f;
    spheres[0].material.emissionColor = vec3(0.0, 0.0, 1);

    spheres[1].radius = 8.0f;
    spheres[1].position = vec3(0, -8, 0);
    spheres[1].material.color = vec3(1, 1, 1);
    spheres[1].material.emissionStrength = 0.0f;
    spheres[1].material.emissionColor = vec3(0.2, 0.2, 0.2);

    spheres[2].radius = 0.35f;
    spheres[2].position = vec3(-0.95, 0.2, -1.75);
    spheres[2].material.color = vec3(1, 1, 1);
    spheres[2].material.emissionStrength = 1.0f;
    spheres[2].material.emissionColor = vec3(1, 0.0, 0.0);

    spheres[3].radius = 0.35f;
    spheres[3].position = vec3(0.5, 0.6, -1.8);
    spheres[3].material.color = vec3(1, 1, 1);
    spheres[3].material.emissionStrength = 1.0f;
    spheres[3].material.emissionColor = vec3(0.0, 1, 0.0);
    
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
    // vec4 dir_clip = vec4(x_ndc, y_ndc, 0.0, 1.0); // 在裁剪空间
    // // 逆投影变换到视图空间: 屏幕像素 → 裁剪空间 → 视图空间 → 世界空间
    // // 步骤1：裁剪空间 → 视图空间（逆投影）
    // vec4 dir_view = globalUBO.mainCameraProjInverse * dir_clip;
    // dir_view = vec4(dir_view.xy, -1.0, 0.0); // 看向-Z
    // // 变换到世界空间
    // // 步骤2：视图空间 → 世界空间（逆视图变换） 
    // vec4 dir_world = globalUBO.mainCameraViewInverse * dir_view;
    // ray.dir = normalize(dir_world.xyz);

    // 简洁但正确的版本
    vec4 clip_coord = vec4(x_ndc, y_ndc, -1.0, 1.0);  // 看向远平面
    vec4 view_coord = globalUBO.mainCameraProjInverse * clip_coord;
    view_coord = vec4(view_coord.xy, -1.0, 0.0);      // 方向向量
    vec4 world_coord = globalUBO.mainCameraViewInverse * view_coord;
    ray.dir = normalize(world_coord.xyz);

    
    ray.origin = globalUBO.mainCameraPos;
    //ray.origin = vec3(globalUBO.mainCameraModel[3]); //an alternative to get camera pos

    // ----------------------------
    // 5. Output
    uint state = 12345u;
    vec3 birghtness_score_sum = vec3(0.0);
    for(int i = 0; i < RAY_PER_PIXEL; i++)
        birghtness_score_sum += TraceRay(ray, state);

    vec3 finalColor = birghtness_score_sum / RAY_PER_PIXEL;

    //vec3 color;
    //if (hitInfo_closest.didHit) color = 0.5 * (hitInfo_closest.normal + vec3(1.0));
    //else color = vec3(0.7, 0.7, 0.7); // background
    
    outColor = vec4(finalColor, 1.0);
}