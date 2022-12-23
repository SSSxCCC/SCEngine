#ifndef _SCEngineEditor_H_
#define _SCEngineEditor_H_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define NOMINMAX
#include <Windows.h>
#include <iostream>
#include <exception>
#include <filesystem>
#include "imgui/imgui.h"
#include "imgui_backends/imgui_impl_glfw.h"
#include "imgui_backends/imgui_impl_vulkan.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"
//#include "editor/GameWorldEditor.h"
#include "editor/SubWindow.h"
#include "data/GameWorldData.h"
#include "graphics/VulkanManager.h"
#include "common/CallbackPointer.h"
namespace fs = std::filesystem;

// This is the class which stores function pointers of SCEngine.dll
class SCEnginePointer {
public:
    using SCEngine_init_fn = void (*)(CallbackPointer&, const fs::path&);
    using SCEngine_update_fn = GameWorldData& (*)(bool);
    using SCEngine_draw_fn = void (*)(bool, int, int, float, float, VkCommandBuffer, bool);
    using SCEngine_runGame_fn = void (*)();
    using SCEngine_stopGame_fn = void (*)();
    using SCEngine_save_fn = nlohmann::json (*)();
    using SCEngine_load_fn = void (*)(const nlohmann::json&);
    using SCEngine_close_fn = void (*)();
    SCEngine_init_fn init;
    SCEngine_update_fn update;
    SCEngine_draw_fn draw;
    SCEngine_runGame_fn runGame;
    SCEngine_stopGame_fn stopGame;
    SCEngine_save_fn save;
    SCEngine_load_fn load;
    SCEngine_close_fn close;

    bool loadLibrary(const fs::path& dllFile) {
        std::cout << "Load library " << dllFile << std::endl;
        if ((dll = LoadLibraryW(dllFile.c_str())) == nullptr) {
            int error = GetLastError();
            std::ifstream is(dllFile);
            std::cout << "LoadLibrary Error! dll == nullptr, dllFile=" << dllFile << ", good=" << is.good() << ", error=" << error << std::endl;
            return false;
        }
        return loadFunction(init, "init") && loadFunction(update, "update") && loadFunction(draw, "draw") && loadFunction(runGame, "runGame") && loadFunction(stopGame, "stopGame") && loadFunction(save, "save") && loadFunction(load, "load") && loadFunction(close, "close");
    }

    void freeLibrary() {
        FreeLibrary(dll);
        std::cout << "Free library" << std::endl;
    }

private:
    HMODULE dll;

    template <typename F>
    bool loadFunction(F& f, std::string name) {
        if ((f = (F)GetProcAddress(dll, name.c_str())) == nullptr) {
            std::cout << "LoadLibrary Error! " << name << " == nullptr" << std::endl;
            freeLibrary();
            return false;
        }
        return true;
    }
};

class SCEngineEditor {
public:
    SCEngineEditor() {
        initWindow();
        initVulkan();
        initImgui();
    }

    ~SCEngineEditor() {
        if (!mProjectDir.empty()) {
            closeProject();
        }

        vkDeviceWaitIdle(mVulkanManager->mDevice);

        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        delete mEditorWindow;
        delete mGameWindow;
        delete mVulkanManager;

        glfwDestroyWindow(mWindow);
        glfwTerminate();
    }

    void run() {
        while (!glfwWindowShouldClose(mWindow)) {
            glfwPollEvents();

            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            int duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - mLastTime).count();
            mFrame++;
            if (duration > 1000) {
                mFps = (float) mFrame * 1000.0f / (float) duration;
                mFrame = 0;
                mLastTime = std::chrono::steady_clock::now();
            }

            ImGui::Begin("Status");
            ImGui::Text("fps: %f", mFps);
            ImGui::End();

            ImGui::Begin("Project");
            ImGui::Text(mProjectDir.empty() ? "No project is opened" : ("Opened project: " + mProjectDir.string()).c_str());
            if (ImGui::Button("Open/Create project")) {
                ImGuiFileDialog::Instance()->OpenDialog("ChooseDirDialogKey", "Open/Create project", nullptr, ".");
            }
            if (!mProjectDir.empty()) {
                if (ImGui::Button("Close project")) {
                    mProjectDir = "";
                    closeProject();
                }
                if (ImGui::Button("Build")) {
                    closeProject();
                    buildProject();
                    if (!loadProject()) {
                        mProjectDir = "";
                    }
                }
            }
            ImGui::End();
            if (ImGuiFileDialog::Instance()->Display("ChooseDirDialogKey")) {
                if (ImGuiFileDialog::Instance()->IsOk()) {
                    std::string directory = ImGuiFileDialog::Instance()->GetCurrentPath();
                    std::cout << "directory=" << directory << std::endl;
                    if (!mProjectDir.empty()) {
                        closeProject();
                    }
                    mProjectDir = directory;
                    buildProject();
                    if (!loadProject()) {
                        mProjectDir = "";
                    }
                }
                ImGuiFileDialog::Instance()->Close();
            }

            if (!mProjectDir.empty()) {
                /*auto& gameWorldData = mSCEngine.update(mEditorMode);
                worldEditor.doFrame(gameWorldData);*/

                VkCommandBuffer commandBuffer = mEditorWindow->preDrawFrame();
                mSCEngine.draw(mEditorWindow->isFocus(), mEditorWindow->getWidth(), mEditorWindow->getHeight(), mEditorWindow->getCursorScreenPos().x, mEditorWindow->getCursorScreenPos().y, commandBuffer, true);
                mEditorWindow->postDrawFrame();
                if (!mEditorMode) {
                    commandBuffer = mGameWindow->preDrawFrame();
                    mSCEngine.draw(mGameWindow->isFocus(), mGameWindow->getWidth(), mGameWindow->getHeight(), mGameWindow->getCursorScreenPos().x, mGameWindow->getCursorScreenPos().y, commandBuffer, false);
                    mGameWindow->postDrawFrame();
                }

                ImGui::Begin("Game");
                if (mEditorMode) {
                    if (ImGui::Button("Run")) {
                        mEditorMode = false;
                        mSCEngine.runGame();
                    } else {
                        if (ImGui::Button("Save")) {
                            saveGame();
                        }
                        if (ImGui::Button("Load")) {
                            loadGame();
                        }
                    }
                } else {
                    if (ImGui::Button("Stop")) {
                        mEditorMode = true;
                        mSCEngine.stopGame();
                    }
                }
                ImGui::End();
            }

            ImGui::ShowDemoWindow();

            ImGui::Render();
            auto commandBuffer = mVulkanManager->preDrawFrame();
            if (commandBuffer != VK_NULL_HANDLE) {
                ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
                mVulkanManager->postDrawFrame();
            }
        }
    }
private:
    GLFWwindow* mWindow;
    float mScale = 1.0f;
    CallbackPointer mCallbackPointer;
    fs::path mProjectDir = "";
    SCEnginePointer mSCEngine;

    VulkanManager* mVulkanManager;

    bool mEditorMode = true;
	SubWindow* mEditorWindow;
    SubWindow* mGameWindow;
	//GameWorldEditor mWorldEditor;

    std::chrono::steady_clock::time_point mLastTime = std::chrono::steady_clock::now();
    int mFrame = 0;
    float mFps = 0.0f;

    void initWindow() {
        glfwSetErrorCallback(glfwErrorCallback);

        if (glfwInit() != GLFW_TRUE) {
            throw std::runtime_error("Failed to initialize GLFW.");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        mWindow = glfwCreateWindow(1920, 1080, "SCEngineEditor", nullptr, nullptr);
        if (mWindow == nullptr) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window.");
        }

        glfwSetWindowUserPointer(mWindow, this);
        glfwSetScrollCallback(mWindow, ScrollCallback);
        glfwSetWindowSizeCallback(mWindow, WindowSizeCallback);
        glfwSetKeyCallback(mWindow, KeyCallback);
        glfwSetCharCallback(mWindow, CharCallback);
        glfwSetMouseButtonCallback(mWindow, MouseButtonCallback);
        glfwSetCursorPosCallback(mWindow, CursorPosCallback);
        glfwSetCursorEnterCallback(mWindow, CursorEnterCallback);
        glfwSetWindowFocusCallback(mWindow, WindowFocusCallback);
        glfwSetMonitorCallback(MonitorCallback);

        initCallbackPointer();
    }

    void initVulkan() {
        mVulkanManager = new VulkanManager(std::make_shared<Platform>(mWindow));
        glfwSetFramebufferSizeCallback(mWindow, FramebufferSizeCallback);
        mEditorWindow = new SubWindow("editor", mVulkanManager);
        mGameWindow = new SubWindow("game", mVulkanManager);
    }

    void initImgui() {
        IMGUI_CHECKVERSION();
	    ImGui::CreateContext();

        if (!ImGui_ImplGlfw_InitForVulkan(mWindow, false)) {
            throw std::runtime_error("ImGui_ImplGlfw_InitForOpenGL failed.");
        }

        ImGui_ImplVulkan_InitInfo initInfo{};
        initInfo.Instance = mVulkanManager->mInstance;
        initInfo.PhysicalDevice = mVulkanManager->mPhysicalDevice;
        initInfo.Device = mVulkanManager->mDevice;
        VulkanManager::QueueFamilyIndices queueFamilyIndices = mVulkanManager->findQueueFamilies(mVulkanManager->mPhysicalDevice);
        initInfo.QueueFamily = queueFamilyIndices.graphicsFamily.value();
        initInfo.Queue = mVulkanManager->mGraphicsQueue;
        initInfo.PipelineCache = nullptr;
        initInfo.DescriptorPool = mVulkanManager->mDescriptorPool;
        initInfo.Subpass = 0;
        initInfo.MinImageCount = mVulkanManager->MAX_FRAMES_IN_FLIGHT;
        initInfo.ImageCount = mVulkanManager->MAX_FRAMES_IN_FLIGHT;
        initInfo.MSAASamples = mVulkanManager->mMsaaSamples;
        initInfo.Allocator = nullptr;
        initInfo.CheckVkResultFn = nullptr;
        if (!ImGui_ImplVulkan_Init(&initInfo, mVulkanManager->mRenderPass)) {
            throw std::runtime_error("ImGui_ImplVulkan_Init failed.");
        }

        glfwSetWindowContentScaleCallback(mWindow, WindowContentScaleCallback);
        float xscale, yscale;
        glfwGetWindowContentScale(mWindow, &xscale, &yscale);
        WindowContentScaleCallback(mWindow, xscale, yscale);

        ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;
    }

    static void glfwErrorCallback(int error, const char* description) {
        std::cout << "GLFW error occured. Code: " << error << ". Description: " << description << std::endl;
    }

    static void ScrollCallback(GLFWwindow* window, double dx, double dy) {
        ImGui_ImplGlfw_ScrollCallback(window, dx, dy);

        auto editor = reinterpret_cast<SCEngineEditor*>(glfwGetWindowUserPointer(window));

        if (editor->mCallbackPointer.mScrollCallback) {
            editor->mCallbackPointer.mScrollCallback(dx, dy);
        }

        if (ImGui::GetIO().WantCaptureMouse) {
            return;
        }
    }

    static void MonitorCallback(GLFWmonitor* monitor, int event) {
        ImGui_ImplGlfw_MonitorCallback(monitor, event);
    }

    static void WindowSizeCallback(GLFWwindow*, int width, int height) {
        // gCamera.setSize(width, height);
    }

    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
        if (ImGui::GetIO().WantCaptureKeyboard) {
            return;
        }

        if (action == GLFW_PRESS) {
            switch (key) {
                case GLFW_KEY_ESCAPE:
                    glfwSetWindowShouldClose(window, GL_TRUE);
                    break;
                default:
                    break;
            }
        }
    }

    static void CharCallback(GLFWwindow* window, unsigned int c) {
        ImGui_ImplGlfw_CharCallback(window, c);
    }

    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    }

    static void CursorPosCallback(GLFWwindow* window, double xd, double yd) {
        ImGui_ImplGlfw_CursorPosCallback(window, xd, yd);
    }

    static void CursorEnterCallback(GLFWwindow* window, int entered) {
        ImGui_ImplGlfw_CursorEnterCallback(window, entered);
    }

    static void WindowFocusCallback(GLFWwindow* window, int focused) {
        ImGui_ImplGlfw_WindowFocusCallback(window, focused);
    }

    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
        auto editor = reinterpret_cast<SCEngineEditor*>(glfwGetWindowUserPointer(window));
        editor->mVulkanManager->framebufferResized();
    }

    static void WindowContentScaleCallback(GLFWwindow* window, float xscale, float yscale) {
        auto editor = reinterpret_cast<SCEngineEditor*>(glfwGetWindowUserPointer(window));
        editor->mScale = xscale;
        if (xscale != yscale) {
            std::cout << "WindowContentScaleCallback: xscale and yscale are not equal! xcale=" << xscale << ", yscale=" << yscale << std::endl;
            editor->mScale = (xscale + yscale) / 2.0f;
        }
        // scale font so that we can see texts in high dpi display clearly
        ImFontConfig fontConfig;
        fontConfig.SizePixels = 13.0f * editor->mScale;
        ImGui::GetIO().Fonts->Clear();
        ImFont* font = ImGui::GetIO().Fonts->AddFontDefault(&fontConfig);
        // ImGui::GetStyle().ScaleAllSizes(mScale);
        // ImGui::GetIO().Fonts->Build();
        auto commandBuffer = editor->mVulkanManager->beginSingleTimeCommands();
        ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
        editor->mVulkanManager->endSingleTimeCommands(commandBuffer);
        ImGui_ImplVulkan_DestroyFontUploadObjects();
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

    // Load game from GameWorldData.json
    void loadGame() {
        std::ifstream i(mProjectDir / "GameWorldData.json");
        nlohmann::json j;
        if (i.good()) {
            i >> j;
        } else {
            std::cout << "No GameWorldData.json" << std::endl;
        }
        mSCEngine.load(j);
    }

    // Save game to GameWorldData.json
    void saveGame() {
        nlohmann::json j = mSCEngine.save();
        std::ofstream o(mProjectDir / "GameWorldData.json");
        o << std::setw(4) << j << std::endl;
    }

    // Build project source codes to SCEngine.dll.
    void buildProject() {
        assert(!mProjectDir.empty());

        // prepare
        fs::path coreSource = getExePath().parent_path().parent_path() / "source" / "SCEngineCore";
        fs::path fileCMakeLists = mProjectDir / "CMakeLists.txt";
        std::ofstream ofs(fileCMakeLists);
        ofs << R"(cmake_minimum_required (VERSION 3.12))" << std::endl
            << R"(project ("SCEngineProject"))" << std::endl
            #ifdef SANITIZE
            << R"(add_compile_options(-fsanitize=address))" << std::endl
            #endif
            << R"(add_subdirectory (")" << cmakePath(coreSource.string()) << R"(" "SCEngineCore"))" << std::endl
            << R"(add_library (SCEngine SHARED ")" << cmakePath((coreSource / "SCEngine.cpp").string()) << R"("))" << std::endl
            << R"(set_property (TARGET SCEngine PROPERTY CXX_STANDARD 20))" << std::endl
            << R"(target_link_libraries (SCEngine PUBLIC SCEngineCore))" << std::endl
            << R"(install (TARGETS SCEngine DESTINATION bin))" << std::endl;
        ofs.close();

        // build
        fs::path cmakeExe = R"(..\compiler\cmake-3.25.0-rc1-windows-x86_64\bin\cmake.exe)";
        fs::path sourceDir = mProjectDir;
        fs::path buildDir = mProjectDir / "build";
        fs::path installDir = buildDir / "install";
        std::string cmd = cmakeExe.string() + " -S " + sourceDir.string() + " -B " + buildDir.string();
        std::string result = exec(cmd); // TODO: handle result
        cmd = cmakeExe.string() + " --build " + buildDir.string() + " --config Debug -j8";
        result = exec(cmd); // TODO: handle result
        cmd = cmakeExe.string() + " --install " + buildDir.string() + " --config Debug --prefix " + installDir.string();
        result = exec(cmd); // TODO: handle result
    }

    // Load project from SCEngine.dll.
    bool loadProject() {
        assert(!mProjectDir.empty());
        if (!mSCEngine.loadLibrary(mProjectDir / "build" / "install" / "bin" / "SCEngine.dll")) {
            return false;
        }
        mSCEngine.init(mCallbackPointer, mProjectDir / "build" / "install" / "asset");
        loadGame();
        return true;
    }

    // Close SCEngine.dll.
    void closeProject() {
        mSCEngine.close();
        mSCEngine.freeLibrary();
    }

    // Help function: execute exe program in windows and return console output.
    std::string exec(const std::string& cmd) {
        std::cout << "cmd start ----------------------------------" << std::endl;
        std::cout << cmd << std::endl;
        std::array<char, 128> buffer;
        std::string result;
        std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd.c_str(), "r"), _pclose);
        if (!pipe) {
            std::cout << "popen() failed!" << std::endl;
            //throw std::runtime_error("popen() failed!");
        }
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
            std::cout << buffer.data();
        }
        std::cout << "cmd end ----------------------------------" << std::endl;
        return result;
    }

    // Help function: get current exe file path.
    fs::path getExePath() {
        char fileName[MAX_PATH];
        DWORD r = GetModuleFileNameA(nullptr, fileName, MAX_PATH);
        if (r == 0) {
            std::cout << "GetModuleFileNameA error=" << GetLastError() << std::endl;
            assert(false);
        }
        return fileName;
    }

    // Help function: convert windows path to cmake path. (convert \ to /)
    std::string cmakePath(const std::string& windowsPath) {
        std::string cmakePath = windowsPath;
        std::replace(cmakePath.begin(), cmakePath.end(), '\\', '/');
        return std::move(cmakePath);
    }
};

#endif // _SCEngineEditor_H_
