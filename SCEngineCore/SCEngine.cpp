#include "SCEngine.h"
#include "sc/core/Engine.h"

namespace sc {

IEngine* scCreate(Platform* platform, VulkanManager* vulkanManager, CallbackPointer& callbackPointer, const fs::path& assetDir) {
    return new Engine(platform, vulkanManager, callbackPointer, assetDir);
}

} // namespace sc
