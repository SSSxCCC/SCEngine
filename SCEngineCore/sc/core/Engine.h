#ifndef _Engine_H_
#define _Engine_H_

#include "sc/common/IEngine.h"
#include "sc/common/Platform.h"
#include "sc/common/CallbackPointer.h"
#include "sc/graphics/VulkanManager.h"
#include "sc/asset/AssetManager.h"
#include "sc/data/SceneData.h"
#include "sc/core/Scene.h"
#include "nlohmann/json.hpp"
#include <memory>
#include <chrono>
#include <filesystem>
namespace fs = std::filesystem;

namespace sc {

// Engine stores many global variables and pointers that a game needs, such as AssetManager, SceneManager and so on.
class Engine : public IEngine {
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

    Engine(Platform* platform, VulkanManager* vulkanManager, CallbackPointer& callbackPointer, const fs::path& assetDir);
    ~Engine() override;
    SceneData& update(bool editorMode) override;
    void draw(bool focus, uint32_t width, uint32_t height, float cursorOffsetX, float cursorOffsetY, VkCommandBuffer commandBuffer, bool forEdtor) override;
    void runGame() override;
    void stopGame() override;
    nlohmann::json save() override;
    void load(const nlohmann::json& j) override;

friend IEngine* scCreate(Platform*, VulkanManager*, CallbackPointer&, const fs::path&);
};

} // namespace sc

#endif // _Engine_H_
