#ifndef _DrawData_H_
#define _DrawData_H_

#include <vulkan/vulkan.h>

struct DrawData {
    bool forEditor;
    VkCommandBuffer commandBuffer;
    glm::mat4 projectionViewMatrix;
    VkExtent2D extent;
};

#endif // _DrawData_H_
