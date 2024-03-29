#ifndef _SCEngineEditor_H_
#define _SCEngineEditor_H_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define NOMINMAX
#include <Windows.h>
#include <iostream>
#include <format>
#include <exception>
#include <filesystem>
#include "imgui/imgui.h"
#include "imgui_backends/imgui_impl_glfw.h"
#include "imgui_backends/imgui_impl_vulkan.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "sc_editor/SceneEditor.h"
#include "sc_editor/SubWindow.h"
#include "sc/data/SceneData.h"
#include "sc/graphics/VulkanManager.h"
#include "sc/common/CallbackPointer.h"
#include "sc/common/IEngine.h"
#include "sc_platform/PlatformImpl.h"
namespace fs = std::filesystem;

namespace sc {

// This is the class which stores function pointers of SCEngine.dll
class SCEnginePointer {
public:
    using scCreate_fn = IEngine* (*)(Platform*, VulkanManager*, CallbackPointer&, const fs::path&);
    scCreate_fn scCreate;

    bool loadLibrary(const fs::path& dllFile) {
        std::cout << "Load library " << dllFile << std::endl;
        if ((dll = LoadLibraryW(dllFile.c_str())) == nullptr) {
            int error = GetLastError();
            std::ifstream is(dllFile);
            std::cout << "LoadLibrary Error! dll == nullptr, dllFile=" << dllFile << ", good=" << is.good() << ", error=" << error << std::endl;
            return false;
        }
        return loadFunction(scCreate, "scCreate");
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
        initSourceDir();
    }

    ~SCEngineEditor() {
        if (!mProjectDir.empty()) {
            closeProject();
        }

        vkDeviceWaitIdle(mVulkanManager->mDevice);

        delete mEditorWindow;
        delete mGameWindow;

        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        delete mVulkanManager;
        delete mPlatform;

        glfwDestroyWindow(mWindow);
        glfwTerminate();
    }

    void run() {
        while (!glfwWindowShouldClose(mWindow)) {
            glfwPollEvents();

            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            WindowInfo windowInfo;
            glfwGetFramebufferSize(mWindow, &windowInfo.width, &windowInfo.height);

            if (ImGui::BeginMainMenuBar()) {
                windowInfo.menuBarHeight = ImGui::GetWindowSize().y;
                if (ImGui::BeginMenu("Project")) {
                    ImGui::MenuItem(mProjectDir.empty() ? "No project is opened" : ("Opened project: " + mProjectDir.string()).c_str(), nullptr, nullptr, false);
                    if (ImGui::MenuItem("Open/Create project")) {
                        ImGuiFileDialog::Instance()->OpenDialog("ChooseDirDialogKey", "Open/Create project", nullptr, ".");
                    }
                    if (!mProjectDir.empty()) {
                        if (ImGui::MenuItem("Close project")) {
                            mProjectDir = "";
                            closeProject();
                        }
                        if (ImGui::MenuItem("Build")) {
                            closeProject();
                            buildProject();
                            if (!loadProject()) {
                                mProjectDir = "";
                            }
                        }
                        if (ImGui::BeginMenu("Export")) {
                            if (ImGui::MenuItem("Windows")) {
                                buildWindows();
                            }
                            if (ImGui::MenuItem("Android")) {
                                buildAndroid();
                            }
                            ImGui::EndMenu();
                        }
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }

            if (ImGuiFileDialog::Instance()->Display("ChooseDirDialogKey", 32, { 500.0f * mScale, 400.0f * mScale })) {
                if (ImGuiFileDialog::Instance()->IsOk()) {
                    std::string directory = ImGuiFileDialog::Instance()->GetCurrentPath();
                    std::cout << "directory=" << directory << std::endl;
                    if (!mProjectDir.empty()) {
                        closeProject();
                    }
                    mProjectDir = directory;
                    createProject();
                    buildProject();
                    if (!loadProject()) {
                        mProjectDir = "";
                    }
                }
                ImGuiFileDialog::Instance()->Close();
            }

            if (!mProjectDir.empty()) {
                auto& sceneData = mEngine->update(mEditorMode);
                mSceneEditor.doFrame(sceneData, windowInfo);

                ImGui::SetNextWindowPos({ windowInfo.width * 0.2f, windowInfo.menuBarHeight }, ImGuiCond_FirstUseEver);
                ImGui::SetNextWindowSize({ windowInfo.width * 0.8f, windowInfo.height - windowInfo.menuBarHeight }, ImGuiCond_FirstUseEver);
                VkCommandBuffer commandBuffer = mEditorWindow->preDrawFrame();
                mEngine->draw(mEditorWindow->isFocus(), mEditorWindow->getWidth(), mEditorWindow->getHeight(), mEditorWindow->getCursorScreenPos().x, mEditorWindow->getCursorScreenPos().y, commandBuffer, true);
                mEditorWindow->postDrawFrame();
                if (!mEditorMode) {
                    ImGui::SetNextWindowPos({ windowInfo.width * 0.2f, windowInfo.menuBarHeight }, ImGuiCond_FirstUseEver);
                    ImGui::SetNextWindowSize({ windowInfo.width * 0.8f, windowInfo.height - windowInfo.menuBarHeight }, ImGuiCond_FirstUseEver);
                    commandBuffer = mGameWindow->preDrawFrame();
                    mEngine->draw(mGameWindow->isFocus(), mGameWindow->getWidth(), mGameWindow->getHeight(), mGameWindow->getCursorScreenPos().x, mGameWindow->getCursorScreenPos().y, commandBuffer, false);
                    mGameWindow->postDrawFrame();
                }

                if (ImGui::BeginMainMenuBar()) {
                    if (ImGui::BeginMenu("Game")) {
                        if (mEditorMode) {
                            if (ImGui::MenuItem("Run")) {
                                mEditorMode = false;
                                mEngine->runGame();
                            } else {
                                if (ImGui::MenuItem("Save")) {
                                    saveGame();
                                }
                                if (ImGui::MenuItem("Load")) {
                                    loadGame();
                                }
                            }
                        } else {
                            if (ImGui::MenuItem("Stop")) {
                                mEditorMode = true;
                                mEngine->stopGame();
                            }
                        }
                        ImGui::EndMenu();
                    }
                    ImGui::EndMainMenuBar();
                }
            }

            ImGui::ShowDemoWindow();

            int64_t duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - mLastTime).count();
            mFrame++;
            if (duration > 1000i64) {
                mFps = static_cast<float>(mFrame) * 1000.0f / static_cast<float>(duration);
                mFrame = 0;
                mLastTime = std::chrono::steady_clock::now();
            }

            if (ImGui::BeginMainMenuBar()) {
                ImGui::BeginMenu(std::format("fps: {}", mFps).c_str(), false);
                ImGui::EndMainMenuBar();
            }

            ImGui::Render();
            auto commandBuffer = mVulkanManager->preDrawFrame();
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
            mVulkanManager->postDrawFrame();
        }
    }
private:
    GLFWwindow* mWindow;
    float mScale = 1.0f;
    CallbackPointer mCallbackPointer;
    fs::path mSourceDir;
    fs::path mProjectDir = "";
    SCEnginePointer mSCEnginePointer;
    IEngine* mEngine;

    Platform* mPlatform;
    VulkanManager* mVulkanManager;

    bool mEditorMode = true;
	SubWindow* mEditorWindow;
    SubWindow* mGameWindow;
    SceneEditor mSceneEditor;

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
        glfwMaximizeWindow(mWindow);

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
        mPlatform = PlatformImpl::build(mWindow);
        mVulkanManager = new VulkanManager(mPlatform, true);
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
        ImGui::GetIO().ConfigWindowsResizeFromEdges = true;
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    }

    void initSourceDir() {
        auto isSourceDir = [](const fs::path& dir) {
            return fs::exists(dir / "SCEngineCore") && fs::exists(dir / "SCEngineCommon") &&
                   fs::exists(dir / "Dependency") && fs::exists(dir / "Platforms");
        };
        fs::path sourceDir = getExePath().parent_path().parent_path() / "source";
        if (!isSourceDir(sourceDir)) {
            sourceDir = getExePath().parent_path().parent_path().parent_path().parent_path();
            if (!isSourceDir(sourceDir)) {
                throw std::runtime_error("SCEngineEditor source code directory is not found!");
            }
        }
        mSourceDir = sourceDir;
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

    // Load game from Scene.json
    void loadGame() {
        std::ifstream i(mProjectDir / "Scenes" / "Scene.json");
        nlohmann::json j;
        if (i.good()) {
            i >> j;
        } else {
            std::cout << "No Scene.json" << std::endl;
        }
        mEngine->load(j);
    }

    // Save game to Scene.json
    void saveGame() {
        nlohmann::json j = mEngine->save();
        std::ofstream o(mProjectDir / "Scenes" / "Scene.json");
        o << std::setw(4) << j << std::endl;
    }

    // Create project initial files and directories: dir 'Assets', dir 'Scenes', dir 'Scripts', file '.gitignore'
    void createProject() {
        if (mProjectDir.empty()) throw std::runtime_error("SCEngineEditor::createProject mProjectDir is empty!");

        fs::create_directory(mProjectDir / "Assets");
        fs::create_directory(mProjectDir / "Scenes");
        fs::create_directory(mProjectDir / "Scripts");
        std::ofstream ofs(mProjectDir / ".gitignore");
        ofs << R"(Builds)" << std::endl << R"(CMakeLists.txt)" << std::endl;
        ofs.close();
    }

    // Build project source codes to SCEngine.dll.
    void buildProject() {
        if (mProjectDir.empty()) throw std::runtime_error("SCEngineEditor::buildProject mProjectDir is empty!");

        createProjectCmakeLists();

        fs::path sourceDir = mProjectDir;
        fs::path buildDir = mProjectDir / "Builds";
        fs::path installDir = buildDir / "Install";
        std::string cmd = "cmake -S " + sourceDir.string() + " -B " + buildDir.string();
        std::string result = exec(cmd); // TODO: handle result
        cmd = "cmake --build " + buildDir.string() + " --config Debug -j8";
        result = exec(cmd); // TODO: handle result
        cmd = "cmake --install " + buildDir.string() + " --config Debug --prefix " + installDir.string();
        result = exec(cmd); // TODO: handle result
    }

    // Create CMakeLists.txt in mProjectDir
    void createProjectCmakeLists() {
        fs::path coreSource = mSourceDir / "SCEngineCore";
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
            << R"(install (TARGETS SCEngine DESTINATION bin))" << std::endl
            << R"(install (DIRECTORY "Assets/." DESTINATION "Assets"))" << std::endl
            << R"(install (DIRECTORY "Scenes" DESTINATION "Assets"))" << std::endl;
        ofs.close();
    }

    // Load project from SCEngine.dll.
    bool loadProject() {
        if (mProjectDir.empty()) throw std::runtime_error("SCEngineEditor::loadProject mProjectDir is empty!");
        if (!mSCEnginePointer.loadLibrary(mProjectDir / "Builds" / "Install" / "bin" / "SCEngine.dll")) {
            return false;
        }
        mEngine = mSCEnginePointer.scCreate(mPlatform, mVulkanManager, mCallbackPointer, mProjectDir / "Builds" / "Install" / "Assets");
        loadGame();
        return true;
    }

    // Close SCEngine.dll.
    void closeProject() {
        delete mEngine;
        mEngine = nullptr;
        mSCEnginePointer.freeLibrary();
    }

    // Build windows exe
    void buildWindows() {
        if (mProjectDir.empty()) throw std::runtime_error("SCEngineEditor::buildWindows mProjectDir is empty!");

        createWindowsCmakeLists();

        fs::path sourceDir = mSourceDir / "Platforms" / "Windows";
        fs::path buildDir = mProjectDir / "Builds" / "Windows";
        fs::path installDir = buildDir / "Install";
        std::string cmd = "cmake -S " + sourceDir.string() + " -B " + buildDir.string();
        std::string result = exec(cmd);  // TODO: handle result
        cmd = "cmake --build " + buildDir.string() + " --config Debug -j8";
        result = exec(cmd);  // TODO: handle result
        cmd = "cmake --install " + buildDir.string() + " --config Debug --prefix " + installDir.string();
        result = exec(cmd);  // TODO: handle result
    }

    void createWindowsCmakeLists() {
        fs::path fileCMakeLists = mSourceDir / "Platforms" / "Windows" / "CMakeLists.txt";
        std::ofstream ofs(fileCMakeLists);
        ofs << R"(cmake_minimum_required (VERSION 3.12))" << std::endl
            << R"(project ("SCEngineClient"))" << std::endl
            #ifdef SANITIZE
            << R"(add_compile_options(-fsanitize=address))" << std::endl
            #endif
            << R"(add_executable (SCEngineClinet "main.cpp" "../Common/sc_platform/PlatformImpl.cpp"))" << std::endl
            << R"(target_include_directories (SCEngineClinet PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} "../Common"))" << std::endl
            << R"(set_property (TARGET SCEngineClinet PROPERTY CXX_STANDARD 20))" << std::endl
            << R"(add_subdirectory ("../../Dependency/glfw" "glfw"))" << std::endl
            << R"(add_subdirectory (")" << cmakePath(mProjectDir.string()) << R"(" "SCEngine"))" << std::endl
            << R"(target_link_libraries (SCEngineClinet PUBLIC glfw))" << std::endl
            << R"(target_link_libraries (SCEngineClinet PUBLIC SCEngine))" << std::endl
            << R"(add_compile_definitions (WINDOW_TITLE=")" << cmakePath(mProjectDir.filename().string()) << R"("))" << std::endl
            << R"(install (TARGETS SCEngineClinet DESTINATION bin))" << std::endl;
        ofs.close();
    }

    // Build android apk
    void buildAndroid() {
        if (mProjectDir.empty()) throw std::runtime_error("SCEngineEditor::buildAndroid mProjectDir is empty!");

        createAndroidCmakeLists();

        fs::path assetsDir = mSourceDir / "Platforms" / "Android" / "app" / "src" / "main" / "assets";
        fs::path engineAssetDir = mSourceDir / "SCEngineCore" / "sc" / "asset" / "SCEngineAsset";
        fs::path projectAssetDir = mProjectDir / "Assets";
        fs::path projectSceneDir = mProjectDir / "Scenes";
        fs::remove_all(assetsDir);
        fs::create_directory(assetsDir);
        fs::copy(engineAssetDir, assetsDir / engineAssetDir.filename(), fs::copy_options::recursive);
        fs::copy(projectSceneDir, assetsDir / projectSceneDir.filename(), fs::copy_options::recursive);
        fs::copy(projectAssetDir, assetsDir, fs::copy_options::recursive);  // TODO: handle copy error

        fs::path sourceDir = mSourceDir / "Platforms" / "Android";
        std::string cmd = "cd " + sourceDir.string() + " && .\\gradlew.bat assembleDebug";
        std::string result = exec(cmd);  // TODO: handle result

        fs::path apkFile = mSourceDir / "Platforms" / "Android" / "app" / "build" / "outputs" / "apk" / "debug" / "app-debug.apk";
        fs::path dstApkFile = mProjectDir / "Builds" / "Android" / apkFile.filename();
        fs::create_directory(dstApkFile.parent_path());
        fs::copy(apkFile, dstApkFile, fs::copy_options::overwrite_existing);
        cmd = "adb install " + dstApkFile.string();
        result = exec(cmd);  // TODO: handle result
        cmd = "adb shell am start -n \"com.example.vulkanapp/com.example.vulkanapp.MainActivity\" -a android.intent.action.MAIN -c android.intent.category.LAUNCHER";
        result = exec(cmd);  // TODO: handle result
    }

    void createAndroidCmakeLists() {
        fs::path fileCMakeLists = mSourceDir / "Platforms" / "Android" / "app" / "src" / "main" / "cpp" / "CMakeLists.txt";
        std::ofstream ofs(fileCMakeLists);
        ofs << R"(cmake_minimum_required (VERSION 3.18.1))" << std::endl
            << R"(project ("vulkanapp"))" << std::endl
            << R"(add_library (vulkanapp SHARED main.cpp AndroidOut.cpp "../../../../../Common/sc_platform/PlatformImpl.cpp"))" << std::endl
            << R"(target_include_directories (vulkanapp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} "../../../../../Common"))" << std::endl
            << R"(set_property (TARGET vulkanapp PROPERTY CXX_STANDARD 20))" << std::endl
            << R"(find_library (log-lib log))" << std::endl
            << R"(find_package (game-activity REQUIRED CONFIG))" << std::endl
            << R"(add_subdirectory (")" << cmakePath(mProjectDir.string()) << R"(" "SCEngine"))" << std::endl
            << R"(target_link_libraries (vulkanapp android game-activity::game-activity jnigraphics ${log-lib} vulkan SCEngine))" << std::endl;
        ofs.close();
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
        while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
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
        if (r == 0) throw std::runtime_error("SCEngineEditor::getExePath GetModuleFileNameA error: " + GetLastError());
        return fileName;
    }

    // Help function: convert windows path to cmake path. (convert \ to /)
    std::string cmakePath(const std::string& windowsPath) {
        std::string cmakePath = windowsPath;
        std::replace(cmakePath.begin(), cmakePath.end(), '\\', '/');
        return std::move(cmakePath);
    }
};

} // namespace sc

#endif // _SCEngineEditor_H_
