#ifndef _SCEngine_H_
#define _SCEngine_H_

#include "common/CallbackPointer.h"
#include "data/SceneData.h"
#include "common/Platform.h"
#include "graphics/VulkanManager.h"
#include "asset/AssetManager.h"
#include "core/Scene.h"
#include <vulkan/vulkan.h>
#include <filesystem>
namespace fs = std::filesystem;

namespace sc {

extern "C" __declspec(dllexport) void init(Platform* platform, VulkanManager* vulkanManager, CallbackPointer& callbackPointer, const fs::path& assetDir);

extern "C" __declspec(dllexport) SceneData& update(bool editorMode);

extern "C" __declspec(dllexport) void draw(bool focus, uint32_t width, uint32_t height, float cursorOffsetX, float cursorOffsetY, VkCommandBuffer commandBuffer, bool forEditor);

extern "C" __declspec(dllexport) void runGame();

extern "C" __declspec(dllexport) void stopGame();

extern "C" __declspec(dllexport) nlohmann::json save();

extern "C" __declspec(dllexport) void load(const nlohmann::json& j);

extern "C" __declspec(dllexport) void close();

class SCEngine {
public:
    Platform* const mPlatform;
    VulkanManager* const mVulkanManager;
    AssetManager* const mAssetManager;
private:
    std::shared_ptr<Scene> mScene;
    SceneData mSceneData, mTempSceneData;
    std::chrono::steady_clock::time_point mStartTime;

    CallbackPointer& mCallbackPointer;

    void createEmptyGame();
    void reloadGame();

    SCEngine(Platform* platform, VulkanManager* vulkanManager, CallbackPointer& callbackPointer, const fs::path& assetDir);
    ~SCEngine();
    SceneData& update(bool editorMode);
    void draw(bool focus, uint32_t width, uint32_t height, float cursorOffsetX, float cursorOffsetY, VkCommandBuffer commandBuffer, bool forEdtor);
    void runGame();
    void stopGame();
    nlohmann::json save();
    void load(const nlohmann::json& j);

friend void init(Platform*, VulkanManager*, CallbackPointer&, const fs::path&);
friend SceneData& update(bool);
friend void draw(bool, uint32_t, uint32_t, float, float, VkCommandBuffer, bool);
friend void runGame();
friend void stopGame();
friend nlohmann::json save();
friend void load(const nlohmann::json& j);
friend void close();
};

} // namespace sc

#endif // _SCEngine_H_
