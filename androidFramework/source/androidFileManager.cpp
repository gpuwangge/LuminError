#include "../include/androidFileManager.h"

CAndroidFileManager::CAndroidFileManager(){}
CAndroidFileManager::~CAndroidFileManager(){}

bool CAndroidFileManager::AssetReadFile(const char *assetName, std::vector<uint8_t>& buf) {
    //if (!assetName.length()) return false;
    AAsset* file = AAssetManager_open(assetManager, assetName, AASSET_MODE_BUFFER);
    //ASSERT(assetDescriptor, "%s does not exist in %s", assetName.c_str(), __FUNCTION__);
    size_t fileLength = AAsset_getLength(file);

    buf.resize(fileLength);
    int64_t readSize = AAsset_read(file, buf.data(), buf.size());

    AAsset_close(file);
    return (readSize == buf.size());
}