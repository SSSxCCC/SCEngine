#include "editor/SubWindow.h"
#include "imgui_backends/imgui_impl_vulkan.h"

SubWindow::SubWindow(const std::string& title, VulkanManager* vulkanManager) :
		mTitle(title), mVulkanManager(vulkanManager), mFocus(false),
		mWidth(0), mHeight(0) {  // mWidth and mHeight will be set to ImGui window size
	mVulkanManager->createCommandBuffers(mCommandBuffers);
}

SubWindow::~SubWindow() {
	cleanupRenderObjects();
}

void SubWindow::createRenderObjects() {
	mResovleImages.resize(mVulkanManager->MAX_FRAMES_IN_FLIGHT);
	mResovleImageMemories.resize(mVulkanManager->MAX_FRAMES_IN_FLIGHT);
	mResovleImageViews.resize(mVulkanManager->MAX_FRAMES_IN_FLIGHT);
	mResovleImageSamplers.resize(mVulkanManager->MAX_FRAMES_IN_FLIGHT);
	mResolveDescriptorSet.resize(mVulkanManager->MAX_FRAMES_IN_FLIGHT);
	for (size_t i = 0; i < mVulkanManager->MAX_FRAMES_IN_FLIGHT; i++) {
        mVulkanManager->createImage(mWidth, mHeight, 1, mVulkanManager->mMsaaSamples, mVulkanManager->mSwapChainImageFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mResovleImages[i], mResovleImageMemories[i]);
        mResovleImageViews[i] = mVulkanManager->createImageView(mResovleImages[i], mVulkanManager->mSwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
        mResovleImageSamplers[i] = mVulkanManager->createSampler();
        mResolveDescriptorSet[i] = ImGui_ImplVulkan_AddTexture(mResovleImageSamplers[i], mResovleImageViews[i], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
    mVulkanManager->createColorResources(mWidth, mHeight, mColorImage, mColorImageMemory, mColorImageView);
    mVulkanManager->createDepthResources(mWidth, mHeight, mDepthImage, mDepthImageMemory, mDepthImageView);
    mVulkanManager->createFramebuffers(mWidth, mHeight, mColorImageView, mDepthImageView, mResovleImageViews, mFramebuffers);
}

void SubWindow::cleanupRenderObjects() {
	if (mResovleImages.size() == 0) {
		return;  // we have not created render objects yet
	}

    vkDestroyImageView(mVulkanManager->mDevice, mColorImageView, nullptr);
    vkDestroyImage(mVulkanManager->mDevice, mColorImage, nullptr);
    vkFreeMemory(mVulkanManager->mDevice, mColorImageMemory, nullptr);

    vkDestroyImageView(mVulkanManager->mDevice, mDepthImageView, nullptr);
    vkDestroyImage(mVulkanManager->mDevice, mDepthImage, nullptr);
    vkFreeMemory(mVulkanManager->mDevice, mDepthImageMemory, nullptr);

    for (size_t i = 0; i < mFramebuffers.size(); i++) {
        vkDestroyFramebuffer(mVulkanManager->mDevice, mFramebuffers[i], nullptr);
    }

    for (size_t i = 0; i < mResovleImages.size(); i++) {
        vkDestroySampler(mVulkanManager->mDevice, mResovleImageSamplers[i], nullptr);
        vkDestroyImageView(mVulkanManager->mDevice, mResovleImageViews[i], nullptr);
        vkDestroyImage(mVulkanManager->mDevice, mResovleImages[i], nullptr);
        vkFreeMemory(mVulkanManager->mDevice, mResovleImageMemories[i], nullptr);
        ImGui_ImplVulkan_RemoveTexture(mResolveDescriptorSet[i]);
    }
}

VkCommandBuffer SubWindow::preDrawFrame() {
	ImGui::Begin(mTitle.c_str());
	mFocus = ImGui::IsWindowFocused();
	mCursorScreenPos = ImGui::GetCursorScreenPos();
	ImVec2 windowSize = ImGui::GetWindowSize();
	windowSize.x = std::max(windowSize.x, 1.0f);
	windowSize.y = std::max(windowSize.y - 50.0f, 1.0f);
	if (mWidth != windowSize.x || mHeight != windowSize.y) {
		mWidth = windowSize.x;
		mHeight = windowSize.y;
		cleanupRenderObjects();
		createRenderObjects();
	}

	mVulkanManager->beginRender(mCommandBuffers[mVulkanManager->mCurrentFrame], mFramebuffers[mVulkanManager->mCurrentFrame], { mWidth, mHeight });
	return mCommandBuffers[mVulkanManager->mCurrentFrame];
}

void SubWindow::postDrawFrame() {
	std::vector<VkSemaphore> waitSemaphores;
    std::vector<VkPipelineStageFlags> waitStages;
    std::vector<VkSemaphore> signalSemaphores;
    mVulkanManager->endRender(mCommandBuffers[mVulkanManager->mCurrentFrame], waitSemaphores, waitStages, signalSemaphores, VK_NULL_HANDLE);

    ImGui::Image((ImTextureID)mResolveDescriptorSet[mVulkanManager->mCurrentFrame], ImVec2(mWidth, mHeight));
    ImGui::End();
}
