#ifndef _SCEngineClient_H_
#define _SCEngineClient_H_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "SCEngine.h"
#include "sc_platform/PlatformImpl.h"
#include "sc/graphics/VulkanManager.h"
#include "sc/common/CallbackPointer.h"
#include <fstream>

namespace sc {

class SCEngineClient {
public:
    SCEngineClient() {
        initWindow();
        initVulkan();
        initEngine();
    }

    ~SCEngineClient() {
        delete mEngine;
        delete mVulkanManager;
        delete mPlatform;

        glfwDestroyWindow(mWindow);
        glfwTerminate();
    }

    void run() {
        while (!glfwWindowShouldClose(mWindow)) {
            glfwPollEvents();

            int width, height;
            glfwGetFramebufferSize(mWindow, &width, &height);

            mEngine->update(false);
            auto commandBuffer = mVulkanManager->preDrawFrame();
            mEngine->draw(true, static_cast<uint32_t>(width), static_cast<uint32_t>(height), 0, 0, commandBuffer, false);
            mVulkanManager->postDrawFrame();
        }
    }

private:
    GLFWwindow* mWindow;
    Platform* mPlatform;
    VulkanManager* mVulkanManager;
    CallbackPointer mCallbackPointer;
    IEngine* mEngine;

    void initWindow() {
        glfwSetErrorCallback(glfwErrorCallback);

        if (glfwInit() != GLFW_TRUE) {
            throw std::runtime_error("Failed to initialize GLFW.");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        mWindow = glfwCreateWindow(1920, 1080, WINDOW_TITLE, nullptr, nullptr);
        if (mWindow == nullptr) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window.");
        }
        glfwMaximizeWindow(mWindow);

        glfwSetWindowUserPointer(mWindow, this);
        glfwSetScrollCallback(mWindow, ScrollCallback);

        initCallbackPointer();
    }

    static void glfwErrorCallback(int error, const char* description) {
        std::cout << "GLFW error occured. Code: " << error << ". Description: " << description << std::endl;
    }

    static void ScrollCallback(GLFWwindow* window, double dx, double dy) {
        auto client = reinterpret_cast<SCEngineClient*>(glfwGetWindowUserPointer(window));

        if (client->mCallbackPointer.mScrollCallback) {
            client->mCallbackPointer.mScrollCallback(dx, dy);
        }
    }

    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
        auto client = reinterpret_cast<SCEngineClient*>(glfwGetWindowUserPointer(window));
        client->mVulkanManager->framebufferResized();
    }

    void initCallbackPointer() {
        mCallbackPointer.mGetKey = [=](int key) {
            return glfwGetKey(mWindow, key);
        };
        mCallbackPointer.mGetMouseButton = [=](int button) {
            return glfwGetMouseButton(mWindow, button);
        };
        mCallbackPointer.mGetCursorPos = [=](float& x, float& y) {
            double xpos, ypos;
            glfwGetCursorPos(mWindow, &xpos, &ypos);
            x = (float)xpos;
            y = (float)ypos;
        };
    }

    void initVulkan() {
        mPlatform = PlatformImpl::build(mWindow);
        mVulkanManager = new VulkanManager(mPlatform, false);
        glfwSetFramebufferSizeCallback(mWindow, FramebufferSizeCallback);
    }

    void initEngine() {
        mEngine = scCreate(mPlatform, mVulkanManager, mCallbackPointer, "../Assets");
        std::ifstream i("../Assets/Scenes/Scene.json");
        nlohmann::json j;
        if (i.good()) {
            i >> j;
        } else {
            std::cout << "No Scene.json" << std::endl;
        }
        mEngine->load(j);
    }
};

} // namespace sc

#endif // _SCEngineClient_H_
