#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>

//这个结构是装从glb读进来的数据
enum class AlphaMode : int {
    Opaque = 0,
    Mask   = 1,
    Blend  = 2,
};

struct GLBMaterial{
    glm::vec4 baseColorFactor{1.0f};

    float metallicFactor = 1.0f;
    float roughnessFactor = 1.0f;

    glm::vec3 emissiveFactor{0.0f};

    int baseColorTextureIndex = -1;
    int metallicRoughnessTextureIndex = -1;
    int normalTextureIndex = -1;
    int occlusionTextureIndex = -1;
    int emissiveTextureIndex = -1;

    int baseColorTexCoord = 0;
    int metallicRoughnessTexCoord = 0;
    int normalTexCoord = 0;
    int occlusionTexCoord = 0;
    int emissiveTexCoord = 0;

    float normalScale = 1.0f;
    float occlusionStrength = 1.0f;

    float alphaCutoff = 0.5f;
    int doubleSided = 0;
    // tinygltf 中通常是 "OPAQUE"、"MASK"、"BLEND"
    //OPAQUE	完全不透明，忽略 Alpha	墙、地板、金属、普通物体
    //MASK	二值透明：要么完全显示，要么完全丢弃	树叶、铁丝网、镂空纹理
    //  Alpha >= alphaCutoff  → 显示
    //  Alpha <  alphaCutoff  → 丢弃
    //  为什么需要 MASK:最经典就是树叶。树叶 texture 可能是一张矩形：实际上只有叶子的部分有 Alpha：MASK 可以把叶子外面的矩形区域直接 discard。
    //BLEND	真正的半透明，可以 0~100%	玻璃、烟雾、塑料薄膜
    //std::string alphaMode = "OPAQUE";
    int alphaMode = static_cast<int>(AlphaMode::Opaque); //0-"OPAQUE", 1-"MASK", 2-"BLEND"

    std::string name;
};
