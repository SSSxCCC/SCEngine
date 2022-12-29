#ifndef _PlatformImpl_H_
#define _PlatformImpl_H_

#include "common/Platform.h"

#if defined(WINDOWS)
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#elif defined(ANDROID)
#define VK_USE_PLATFORM_ANDROID_KHR
#include <android/log.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>
/*!
 * Use this class to create an output stream that writes to logcat.
 */
class AndroidOut : public std::stringbuf {
public:
    /*!
     * Creates a new output stream for logcat
     * @param kLogTag the log tag to output
     */
    inline AndroidOut(const char* kLogTag) : logTag_(kLogTag) {}
protected:
    virtual int sync() override {
        __android_log_print(ANDROID_LOG_DEBUG, logTag_, "%s", str().c_str());
        str("");
        return 0;
    }
private:
    const char* logTag_;
};
#endif

class PlatformImpl {
public:
#if defined(WINDOWS)
    PlatformImpl(GLFWwindow* window) : mWindow(window){};
    static Platform* build(GLFWwindow* window);
    std::ostream& mOut = std::cout;
#elif defined(ANDROID)
    PlatformImpl(android_app* app) : mApp(app), mAndroidOut("AO"), mOut(&mAndroidOut){};
    static Platform* build(android_app* app);
    std::ostream mOut;
#endif
    void createSurface(VkInstance& instance, VkSurfaceKHR& surface);
    void getSurfaceSize(int& width, int& height);
    void waitSurfaceSize();
    std::vector<char> readFile(const fs::path& filePath);
    std::vector<const char*> getExtensions();
    VkCompositeAlphaFlagBitsKHR getCompositeAlpha();
private:
#if defined(WINDOWS)
    GLFWwindow* mWindow;
#elif defined(ANDROID)
    android_app* mApp;
    AndroidOut mAndroidOut;
#endif
};

#endif // _PlatformImpl_H_
