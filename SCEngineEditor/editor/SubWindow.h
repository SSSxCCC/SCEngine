#ifndef _SubWindow_H_
#define _SubWindow_H_

#include <string>
#include "imgui/imgui.h"
#include "graphics/VulkanManager.h"

namespace sc {

class SubWindow {
public:
	SubWindow(const std::string& title, VulkanManager* vulkanManager);
	~SubWindow();
	VkCommandBuffer preDrawFrame();
	void postDrawFrame();
	bool isFocus() { return mFocus; }
	int getWidth() { return mWidth; }
	int getHeight() { return mHeight; }
	ImVec2 getCursorScreenPos() { return mCursorScreenPos; }
private:
	std::string mTitle;
	VulkanManager* mVulkanManager;
	bool mFocus;
	uint32_t mWidth;
	uint32_t mHeight;
	ImVec2 mCursorScreenPos;

	VkImage mDepthImage;
    VkDeviceMemory mDepthImageMemory;
    VkImageView mDepthImageView;
    VkImage mColorImage;
    VkDeviceMemory mColorImageMemory;
    VkImageView mColorImageView;
    std::vector<VkImage> mResovleImages;  // size is MAX_FRAMES_IN_FLIGHT
    std::vector<VkDeviceMemory> mResovleImageMemories;  // size is MAX_FRAMES_IN_FLIGHT
    std::vector<VkImageView> mResovleImageViews;  // size is MAX_FRAMES_IN_FLIGHT
    std::vector<VkSampler> mResovleImageSamplers;  // size is MAX_FRAMES_IN_FLIGHT
    std::vector<VkDescriptorSet> mResolveDescriptorSet;  // size is MAX_FRAMES_IN_FLIGHT
    std::vector<VkFramebuffer> mFramebuffers;  // size is MAX_FRAMES_IN_FLIGHT
    std::vector<VkCommandBuffer> mCommandBuffers;  // size is MAX_FRAMES_IN_FLIGHT
    std::vector<VkFence> mInFlightFences;  // size is MAX_FRAMES_IN_FLIGHT

    void createRenderObjects();
    void cleanupRenderObjects();
};

} // namespace sc

#endif // _SubWindow_H_
