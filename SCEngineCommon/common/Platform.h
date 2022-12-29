#ifndef _Platform_H_
#define _Platform_H_

#if defined(_WIN32)
#define WINDOWS
#elif defined(__ANDROID__)
#define ANDROID
#endif

#include <sstream>
#include <vulkan/vulkan.h>
#include <vector>
#include <iostream>
#include <functional>
#include <filesystem>
namespace fs = std::filesystem;

class Platform {
public:
    Platform(void* impl, std::ostream& out) : mImpl(impl), mOut(out) { };
    ~Platform() { delete mImpl; }
    std::ostream& mOut;

    std::function<void(VkInstance& instance, VkSurfaceKHR& surface)> createSurface;
    std::function<void(int& width, int& height)> getSurfaceSize;
    std::function<void()> waitSurfaceSize;
    std::function<std::vector<char>(const fs::path& filePath)> readFile;
    std::function<std::vector<const char*>()> getExtensions;
    std::function<VkCompositeAlphaFlagBitsKHR()> getCompositeAlpha;
private:
    void* mImpl;
};

#endif // _Platform_H_
