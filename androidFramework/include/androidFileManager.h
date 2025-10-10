#ifndef H_ANDROIDFILEMANAGER
#define H_ANDROIDFILEMANAGER

#include "..\\..\\framework\\include\\common.h"
#include <android/asset_manager.h>

class CAndroidFileManager final{
public:
    CAndroidFileManager();
    ~CAndroidFileManager();

    AAssetManager *assetManager;
    bool AssetReadFile(const char *assetName, std::vector<uint8_t>& buf); //std::string& assetName
};

#endif
