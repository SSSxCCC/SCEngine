#ifndef _SCEngine_H_
#define _SCEngine_H_

#include "sc/common/CallbackPointer.h"
#include "sc/common/Platform.h"
#include "sc/common/IEngine.h"
#include "sc/graphics/VulkanManager.h"
#include <filesystem>
namespace fs = std::filesystem;

namespace sc {

extern "C" __declspec(dllexport) IEngine* scCreate(Platform* platform, VulkanManager* vulkanManager, CallbackPointer& callbackPointer, const fs::path& assetDir);

} // namespace sc

#endif // _SCEngine_H_
