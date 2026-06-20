#version 460
#extension GL_EXT_ray_tracing : require

layout(location = 0) rayPayloadInEXT vec3 payload;
hitAttributeEXT vec2 bary;

void main() {
    vec3 bc = vec3(1.0 - bary.x - bary.y, bary.x, bary.y);

    vec3 baseColor = vec3(bc.x, bc.y, bc.z);

    vec3 N = vec3(0.0, 0.0, 1.0);                // 三角形法线，先写死
    vec3 L = normalize(vec3(-0.5, 0.8, -0.6));   // 一个最小灯光方向
    float diff = max(dot(N, L), 0.0);

    payload = baseColor * (0.2 + 0.8 * diff);
}