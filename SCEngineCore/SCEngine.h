#ifndef _SCEngine_H_
#define _SCEngine_H_

#include "common/CallbackPointer.h"
#include "data/GameWorldData.h"
#include <vulkan/vulkan.h>
#include <filesystem>
namespace fs = std::filesystem;

extern "C" __declspec(dllexport) void init(CallbackPointer& callbackPointer, const fs::path& assetDir);

extern "C" __declspec(dllexport) GameWorldData& doFrame(bool editorMode);

extern "C" __declspec(dllexport) void doEditorFrame(bool focus, int with, int height, float cursorOffsetX, float cursorOffsetY, VkCommandBuffer commandBuffer);

extern "C" __declspec(dllexport) void doGameFrame(bool focus, int with, int height, float cursorOffsetX, float cursorOffsetY, VkCommandBuffer commandBuffer);

extern "C" __declspec(dllexport) void runGame();

extern "C" __declspec(dllexport) void stopGame();

extern "C" __declspec(dllexport) nlohmann::json save();

extern "C" __declspec(dllexport) void load(const nlohmann::json& j);

extern "C" __declspec(dllexport) void close();

#endif // _SCEngine_H_
