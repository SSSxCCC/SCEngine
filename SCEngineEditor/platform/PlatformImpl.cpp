#include "PlatformImpl.h"
#include <fstream>
#include <iostream>

namespace sc {

#if defined(WINDOWS)
Platform* PlatformImpl::build(GLFWwindow* window) {
    PlatformImpl* impl = new PlatformImpl(window);
#elif defined(ANDROID)
Platform* PlatformImpl::build(android_app* app) {
    PlatformImpl* impl = new PlatformImpl(app);
#endif

    Platform* platform = new Platform(impl, impl->mOut);
    platform->createSurface = [=](VkInstance& instance, VkSurfaceKHR& surface) { impl->createSurface(instance, surface); };
    platform->getSurfaceSize = [=](int& width, int& height) { impl->getSurfaceSize(width, height); };
    platform->waitSurfaceSize = [=]() { impl->waitSurfaceSize(); };
    platform->readFile = [=](const fs::path& filePath) { return impl->readFile(filePath); };
    platform->getExtensions = [=]() { return impl->getExtensions(); };
    platform->getCompositeAlpha = [=]() { return impl->getCompositeAlpha(); };
    return platform;
}

void PlatformImpl::createSurface(VkInstance& instance, VkSurfaceKHR& surface) {
#if defined(WINDOWS)
    if (glfwCreateWindowSurface(instance, mWindow, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
#elif defined(ANDROID)
    VkAndroidSurfaceCreateInfoKHR createInfo { VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR };
    createInfo.window = mApp->window;
    if (vkCreateAndroidSurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create android surface!");
    }
#endif
}

void PlatformImpl::getSurfaceSize(int& width, int& height) {
#if defined(WINDOWS)
    glfwGetFramebufferSize(mWindow, &width, &height);
#elif defined(ANDROID)
    width = ANativeWindow_getWidth(mApp->window);
    height = ANativeWindow_getHeight(mApp->window);
#endif
}

void PlatformImpl::waitSurfaceSize() {
#if defined(WINDOWS)
    int width = 0, height = 0;
    getSurfaceSize(width, height);
    while (width == 0 || height == 0) {
        getSurfaceSize(width, height);
        glfwWaitEvents();
    }
#elif defined(ANDROID)
#endif
}

std::vector<char> PlatformImpl::readFile(const fs::path& filePath) {
#if defined(WINDOWS)
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
#elif defined(ANDROID)
    AAsset* file = AAssetManager_open(mApp->activity->assetManager, filePath.c_str(), AASSET_MODE_BUFFER);

    if (!file) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = AAsset_getLength(file);
    std::vector<char> buffer(fileSize);
    AAsset_read(file, buffer.data(), fileSize);
    AAsset_close(file);

    return buffer;
#endif
}

std::vector<const char*> PlatformImpl::getExtensions() {
#if defined(WINDOWS)
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> extensions;
    for (int i = 0; i < glfwExtensionCount; i++) {
        extensions.push_back(glfwExtensions[i]);
    }
    return extensions;
#elif defined(ANDROID)
    std::vector<const char*> extensions;
    extensions.push_back("VK_KHR_surface");
    extensions.push_back("VK_KHR_android_surface");
    return extensions;
#endif
}

VkCompositeAlphaFlagBitsKHR PlatformImpl::getCompositeAlpha() {
#if defined(WINDOWS)
    return VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
#elif defined(ANDROID)
    return VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
#endif
}

} // namespace sc
