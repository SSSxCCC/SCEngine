#ifndef _DrawData_H_
#define _DrawData_H_

#include <vulkan/vulkan.h>

namespace sc {

struct DrawData {
    bool forEditor;
    VkCommandBuffer commandBuffer;
    glm::mat4 projectionViewMatrix;
    VkExtent2D extent;
};

} // namespace sc

#endif // _DrawData_H_
