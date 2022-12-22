#ifndef _SubWindow_H_
#define _SubWindow_H_

#include <string>
#include "imgui/imgui.h"

class SubWindow {
public:
	SubWindow(const std::string& title);
	~SubWindow();
	void update();
	void bind();
	void unbind();
	bool isFocus() { return mFocus; }
	int getWidth() { return mWidth; }
	int getHeight() { return mHeight; }
	ImVec2 getCursorScreenPos() { return mCursorScreenPos; }
	unsigned int getTexture() { return mTexture; }
private:
	std::string mTitle;
	bool mFocus;
	int mWidth;
	int mHeight;
	ImVec2 mCursorScreenPos;

	VkImage mDepthImage;
    VkDeviceMemory mDepthImageMemory;
    VkImageView mDepthImageView;
    VkImage mColorImage;
    VkDeviceMemory mColorImageMemory;
    VkImageView mColorImageView;
    std::vector<VkImage> mResovleImages;
    std::vector<VkDeviceMemory> mResovleImageMemories;
    std::vector<VkImageView> mResovleImageViews;
    std::vector<VkFramebuffer> mFramebuffers;
    std::vector<VkCommandBuffer> mCommandBuffers;
    std::vector<VkSemaphore> mImageAvailableSemaphores;
    std::vector<VkSemaphore> mRenderFinishedSemaphores;
    std::vector<VkFence> mInFlightFences;
    uint32_t mCurrentFrame = 0;
	std::vector<VkDescriptorSet> mResolveDescriptorSet;

	void updateFrameBuffer();
};

#endif // _SubWindow_H_
