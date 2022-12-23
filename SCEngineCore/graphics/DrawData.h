#ifndef _DrawData_H_
#define _DrawData_H_

#include <vulkan/vulkan.h>

struct DrawData {
    bool forEditor;
    VkCommandBuffer commandBuffer;
};

#endif // _DrawData_H_
