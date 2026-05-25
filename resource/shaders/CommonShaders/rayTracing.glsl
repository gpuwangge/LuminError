#ifndef RAYTRACING_GLSL
#define RAYTRACING_GLSL

/***************************
* Ray Tracing Data Structure
***************************/
struct Ray{
    vec3 origin;
    vec3 dir;
};

struct Triangle{
    vec3 a;
    vec3 b;
    vec3 c;
    vec3 normal_a;
    vec3 normal_b;
    vec3 normal_c;
    int material_id;
};

struct HitInfo{
    bool didHit;
    float dst;
    vec3 hitPoint;
    vec3 normal;
    int material_id;
    //vec3 debugColor;
};

/***************************
* Utility Functions
***************************/
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

/***************************
* Ray Tracing Functions (Sphere)
***************************/
HitInfo RaySphereHitCheck(Ray ray, int sphere_id){
    HitInfo hitInfo;
    hitInfo.didHit = false;
    hitInfo.dst = -1.0;

    vec3 oc = ray.origin - sboSphere.spheres[sphere_id].position;
    float a = dot(ray.dir, ray.dir);
    float b = 2.0 * dot(oc, ray.dir);
    float c = dot(oc, oc) - sboSphere.spheres[sphere_id].radius * sboSphere.spheres[sphere_id].radius;
    float discriminant = b * b - 4.0 * a * c;

    if (discriminant < 0.0) {
        return hitInfo;
    }

    float sqrtDisc = sqrt(discriminant);
    float t1 = (-b - sqrtDisc) / (2.0 * a);
    float t2 = (-b + sqrtDisc) / (2.0 * a);
    float t = min(t1, t2);
    if (t < 0.001) t = max(t1, t2);
    
    
    if (t >= 0.001) {
        hitInfo.didHit = true;
        hitInfo.dst = t;
        hitInfo.hitPoint = ray.origin + ray.dir * t;
        hitInfo.normal = normalize(hitInfo.hitPoint - sboSphere.spheres[sphere_id].position);
        hitInfo.material_id = sboSphere.spheres[sphere_id].material_id;
    }

    return hitInfo;
}

/***************************
* Ray Tracing Functions (Triangle)
***************************/
HitInfo RayTriangleHitCheck(Ray ray, Triangle tri){
    HitInfo hitInfo;
    hitInfo.didHit = false;
    hitInfo.dst = -1.0;

    vec3 e1 = tri.b - tri.a;
    vec3 e2 = tri.c - tri.a;
    vec3 triNormal = cross(e1, e2); //should not use nagative here; otherise indexbuffer winding order need to be changed too

    //no phong shading, the normals are the same at the vertex as the triangle plane
    tri.normal_a = triNormal;
    tri.normal_b = triNormal;
    tri.normal_c = triNormal;

    float determinant = -dot(ray.dir, triNormal);
    float invdet = 1.0 / determinant;
    vec3 ao = ray.origin - tri.a; //vector from ray origin to the point a
    vec3 dao = cross(ao, ray.dir);

    float t = dot(ao, triNormal) * invdet;
    float u = dot(e2, dao) * invdet;
    float v = -dot(e1, dao) * invdet;
    float w = 1 - u - v;

    if(abs(determinant) >= 1E-6 && t >= 0 && u >= 0 && v >= 0 && w >= 0){
        hitInfo.didHit = true;
        hitInfo.dst = t;
        hitInfo.hitPoint = ray.origin + ray.dir * t;
        //hitInfo.normal = normalize(tri.normal_a * w + tri.normal_b * u + tri.normal_c * v);
        if(determinant > 0) {
            // 从正面命中
            hitInfo.normal = normalize(triNormal);
        } else {
            // 从背面命中
            hitInfo.normal = normalize(-triNormal);
        }
        hitInfo.material_id = tri.material_id;

        //hitInfo.debugColor = vec3(t*0.1);
    }else{
        //hitInfo.debugColor = vec3(1, 0, 0);
    }

    return hitInfo;
}
#include "rayBVH.glsl"

/***************************
* Ray Tracing Functions (Core)
***************************/
HitInfo RayHitCheck(Ray ray){
    HitInfo hitInfo_closest;
    hitInfo_closest.didHit = false;
    hitInfo_closest.dst = 1e30;

    //these code draw the simple sphere okay, no bvh involved
    for(int i = 0; i < NUM_SPHERES; i++){
        HitInfo hitInfo_current = RaySphereHitCheck(ray, i);
        if(hitInfo_current.didHit && hitInfo_closest.didHit){ //current hit and previous hit
            if(hitInfo_current.dst < hitInfo_closest.dst) hitInfo_closest = hitInfo_current;
        }else if(hitInfo_current.didHit){ //current hit and no previous hit
            hitInfo_closest = hitInfo_current;
        }
    }

    if(!ENABLE_BVH){ //30fps(5080 Laptop)
        int totalNumTriangles = 240; //228 for low tree model (228, 68, 20, 4, 2)
        for(int i = 0; i < totalNumTriangles; i++){
            Triangle tri;
            tri.a = sboTriangleVertex.vertices[sboTriangleIndex.indices[i * 3 + 0]].position;
            tri.b = sboTriangleVertex.vertices[sboTriangleIndex.indices[i * 3 + 1]].position;
            tri.c = sboTriangleVertex.vertices[sboTriangleIndex.indices[i * 3 + 2]].position;
            //tri.normal_a = sboTriangleVertex.vertices[sboTriangleIndex.indices[i * 3 + 0]].normal;
            //tri.normal_b = sboTriangleVertex.vertices[sboTriangleIndex.indices[i * 3 + 1]].normal;
            //tri.normal_c = sboTriangleVertex.vertices[sboTriangleIndex.indices[i * 3 + 2]].normal;
            tri.material_id = sboTriangleVertex.vertices[sboTriangleIndex.indices[i * 3 + 0]].material_id;

            HitInfo hitInfo_current = RayTriangleHitCheck(ray, tri);
            if(hitInfo_current.didHit && hitInfo_closest.didHit){ //current hit and previous hit
                if(hitInfo_current.dst < hitInfo_closest.dst) hitInfo_closest = hitInfo_current;
            }else if(hitInfo_current.didHit){ //current hit and no previous hit
                hitInfo_closest = hitInfo_current;
            }
        }
    }else RayTraceWithBVH(ray, hitInfo_closest); //500fps(5080 Laptop)

    return hitInfo_closest;
}

vec3 TraceRay(Ray ray, inout uint state){
    vec3 throughput = vec3(1.0);
    vec3 result_brightness_score = vec3(0.0);

    for(int i = 0; i < BOUNCE_LIMIT; i++) {
        HitInfo hitInfo = RayHitCheck(ray);

        if(!hitInfo.didHit) {
            result_brightness_score += vec3(0.2, 0.3, 0.4) * throughput;
            //result_brightness_score = hitInfo.debugColor;
            break;
        }

        Material mat = sboMaterial.materials[hitInfo.material_id];
        
        // 添加自发光贡献
        vec3 emittedLight = mat.emissionColor * mat.emissionStrength;
        result_brightness_score += emittedLight * throughput;

        // 俄罗斯轮盘赌（从第3次反弹开始）
        if(i > 2) {
            float p = max(throughput.r, max(throughput.g, throughput.b));
            if(p < 0.001) break;
            if(Rand(state) > p) break;
            throughput /= p;
        }

        // 计算基础反射率 F0
        vec3 F0 = mix(vec3(mat.reflectance), mat.albedo, mat.metallic);
        
        // 计算菲涅尔项
        float cosTheta = abs(dot(hitInfo.normal, -ray.dir));
        vec3 F = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
        
        // 处理折射/透射材质（玻璃、水等）
        if (mat.transmission > 0.0 && mat.alpha < 0.5) {
            float refractionRatio = mat.ior;
            bool entering = dot(hitInfo.normal, -ray.dir) > 0.0;
            
            if (!entering) {
                hitInfo.normal = -hitInfo.normal;
                refractionRatio = 1.0 / mat.ior;
            }
            
            vec3 refractedDir = refract(ray.dir, hitInfo.normal, refractionRatio);
            
            // 增加基于粗糙度的法线扰动
            vec3 perturbedNormal = hitInfo.normal;
            if (mat.roughness > 0.0) {
                vec3 randomJitter = RandomDirectionInHemisphere(hitInfo.normal, state) - hitInfo.normal;
                perturbedNormal = normalize(hitInfo.normal + randomJitter * mat.roughness * 0.3);
            }
            
            // 使用扰动后的法线重新计算
            refractedDir = refract(ray.dir, perturbedNormal, refractionRatio);
            float cosTheta = abs(dot(perturbedNormal, -ray.dir));
            vec3 F = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
            
            float reflectionProbability = (F.r + F.g + F.b) / 3.0;
            
            if (length(refractedDir) < 0.001 || Rand(state) < reflectionProbability * 0.8) {
                ray.dir = reflect(ray.dir, perturbedNormal);
                if (mat.roughness > 0.0) {
                    ray.dir = normalize(mix(ray.dir, RandomDirectionInHemisphere(perturbedNormal, state), mat.roughness));
                }
                throughput *= F / max(reflectionProbability, 0.1);
            } else {
                ray.dir = refractedDir;
                vec3 glassColor = mat.transmissionColor * mat.albedo;
                throughput *= glassColor * (1.0 - F) / max(1.0 - reflectionProbability, 0.1);
            }
        }
        // 处理金属材质（高反射）
        else if (mat.metallic > 0.8) {
            // 金属材质主要进行镜面反射
            vec3 reflectedDir = reflect(ray.dir, hitInfo.normal);
            
            // 根据粗糙度添加随机性
            if (mat.roughness > 0.0) {
                reflectedDir = normalize(mix(reflectedDir, RandomDirectionInHemisphere(hitInfo.normal, state), mat.roughness));
            }
            
            ray.dir = reflectedDir;
            throughput *= F * mat.albedo;
        }
        // 处理电介质材质（混合漫反射和镜面反射）
        else {
            // 根据菲涅尔项决定反射和漫反射的比例
            float reflectionProbability = (F.r + F.g + F.b) / 3.0;
            
            if (Rand(state) < reflectionProbability) {
                // 镜面反射
                vec3 reflectedDir = reflect(ray.dir, hitInfo.normal);
                
                // 根据粗糙度添加随机性
                if (mat.roughness > 0.0) {
                    reflectedDir = normalize(mix(reflectedDir, RandomDirectionInHemisphere(hitInfo.normal, state), mat.roughness));
                }
                
                ray.dir = reflectedDir;
                throughput *= F / reflectionProbability;
            } else {
                // 漫反射
                ray.dir = RandomDirectionInHemisphere(hitInfo.normal, state);
                
                // 能量守恒：漫反射部分 = (1 - F) * 漫反射颜色
                vec3 kD = (1.0 - F) * (1.0 - mat.metallic);
                throughput *= kD * mat.albedo / (1.0 - reflectionProbability);
            }
        }

        ray.origin = hitInfo.hitPoint + hitInfo.normal * 0.001;
    }

    return result_brightness_score;
}

#endif