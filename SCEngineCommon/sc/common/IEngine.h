#ifndef _IEngine_H_
#define _IEngine_H_

#include "sc/data/SceneData.h"
#include <vulkan/vulkan.h>

namespace sc {

// The engie interface. Every client (Editor, Windows and Android) need use a instance of this interface.
class IEngine {
protected:
    virtual ~IEngine() { };

private:
    virtual SceneData& update(bool editorMode) = 0;
    virtual void draw(bool focus, uint32_t width, uint32_t height, float cursorOffsetX, float cursorOffsetY, VkCommandBuffer commandBuffer, bool forEdtor) = 0;
    virtual void runGame() = 0;
    virtual void stopGame() = 0;
    virtual nlohmann::json save() = 0;
    virtual void load(const nlohmann::json& j) = 0;

friend class SCEngineEditor;
friend class SCEngineClient;
};

} // namespace sc

#endif // _IEngine_H_
