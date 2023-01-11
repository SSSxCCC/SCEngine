#ifndef _AssetManager_H_
#define _AssetManager_H_

#include "sc/common/Platform.h"
#include <filesystem>
namespace fs = std::filesystem;

namespace sc {

class AssetManager {
public:
    AssetManager(Platform* platform, const fs::path& assetDir) : mPlatform(platform), mAssetDir(assetDir) { }
    std::vector<char> readFile(const fs::path& filePath) { return mPlatform->readFile(mAssetDir / filePath); }
private:
    Platform* mPlatform;
    fs::path mAssetDir;
};

} // namespace sc

#endif // _AssetManager_H_
