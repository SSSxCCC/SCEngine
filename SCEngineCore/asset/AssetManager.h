#ifndef _AssetManager_H_
#define _AssetManager_H_

#include "common/Platform.h"
#include <filesystem>
namespace fs = std::filesystem;

class AssetManager {
public:
    AssetManager(Platform* platform, const fs::path& assetDir) : mPlatform(platform), mAssetDir(assetDir) { }
    std::vector<char> readFile(const fs::path& filePath) { mPlatform->readFile(mAssetDir / filePath); }
private:
    Platform* mPlatform;
    fs::path mAssetDir;
};

#endif // _AssetManager_H_
