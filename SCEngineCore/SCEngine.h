#ifndef _SCEngine_H_
#define _SCEngine_H_

#include "sc/common/CallbackPointer.h"
#include "sc/common/Platform.h"
#include "sc/common/IEngine.h"
#include "sc/graphics/VulkanManager.h"
#include <filesystem>
namespace fs = std::filesystem;

namespace sc {

// Only Windows has dllexport, so we should not use dllexport in Android
// TODO: Only use dllexport in editor build
#if defined(WINDOWS)
#define DLLEXPORT __declspec(dllexport)
#elif defined(ANDROID)
#define DLLEXPORT
#endif

extern "C" DLLEXPORT IEngine* scCreate(Platform* platform, VulkanManager* vulkanManager, CallbackPointer& callbackPointer, const fs::path& assetDir);

} // namespace sc

#endif // _SCEngine_H_
