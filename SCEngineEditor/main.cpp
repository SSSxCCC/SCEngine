#define NOMINMAX
#include <Windows.h>
#include <iostream>
#include <filesystem>
#include <assert.h>
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui_backends/imgui_impl_glfw.h"
#include "imgui_backends/imgui_impl_opengl3.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "common/OpenGLPointer.h"
#include "common/CallbackPointer.h"
#include "editor/GameWorldEditor.h"
#include "editor/SubWindow.h"
#include "data/GameWorldData.h"
namespace fs = std::filesystem;

float gScale = 1.0f;
GLFWwindow* gWindow;
CallbackPointer gCallbackPointer;
OpenGLPointer gOpenGLPointer;
fs::path gProjectDir = "";

void initCallbackPointer() {
    gCallbackPointer.mGetKey = [](int key) {
        return glfwGetKey(gWindow, key);
    };
    gCallbackPointer.mGetMouseButton = [](int button) {
        return glfwGetMouseButton(gWindow, button);
    };
    gCallbackPointer.mGetCursorPos = [](float& x, float& y) {
        double xpos, ypos;
        glfwGetCursorPos(gWindow, &xpos, &ypos);
        x = (float) xpos; y = (float) ypos;
    };
}

static void sCheckGLError() {
	GLenum errCode = glGetError();
	if (errCode != GL_NO_ERROR) {
        std::cout << "OpenGL error = " << errCode << std::endl;
	}
}

static void glfwErrorCallback(int error, const char* description) {
    std::cout << "GLFW error occured. Code: " << error << ". Description: " << description << std::endl;
}

static void ScrollCallback(GLFWwindow* window, double dx, double dy) {
	ImGui_ImplGlfw_ScrollCallback(window, dx, dy);

	if (gCallbackPointer.mScrollCallback) {
		gCallbackPointer.mScrollCallback(dx, dy);
	}

	if (ImGui::GetIO().WantCaptureMouse) {
		return;
	}
}

static void WindowSizeCallback(GLFWwindow*, int width, int height) {
	//gCamera.setSize(width, height);
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

static void WindowContentScaleCallback(GLFWwindow* window, float xscale, float yscale) {
	gScale = xscale;
	if (xscale != yscale) {
		std::cout << "WindowContentScaleCallback: xscale and yscale are not equal! xcale=" << xscale << ", yscale=" << yscale << std::endl;
		gScale = (xscale + yscale) / 2.0f;
	}
	// scale font so that we can see texts in high dpi display clearly
	ImFontConfig fontConfig;
	fontConfig.SizePixels = 13.0f * gScale;
	ImGui::GetIO().Fonts->Clear();
	ImFont* font = ImGui::GetIO().Fonts->AddFontDefault(&fontConfig);
	//ImGui::GetStyle().ScaleAllSizes(gScale);
	//ImGui::GetIO().Fonts->Build();
	ImGui_ImplOpenGL3_CreateFontsTexture();
}

class SCEngine {
public:
	using SCEngine_init_fn = void(*)(OpenGLPointer&, CallbackPointer&, const fs::path&);
	using SCEngine_doFrame_fn = GameWorldData&(*)(bool);
	using SCEngine_doEditorFrame_fn = void(*)(bool,int,int,float,float);
	using SCEngine_doGameFrame_fn = void(*)(bool,int,int,float,float);
	using SCEngine_runGame_fn = void(*)();
	using SCEngine_stopGame_fn = void(*)();
	using SCEngine_save_fn = void(*)();
	using SCEngine_load_fn = void(*)();
	using SCEngine_close_fn = void(*)();
	SCEngine_init_fn init;
	SCEngine_doFrame_fn doFrame;
	SCEngine_doEditorFrame_fn doEditorFrame;
	SCEngine_doGameFrame_fn doGameFrame;
	SCEngine_runGame_fn runGame;
	SCEngine_stopGame_fn stopGame;
	SCEngine_save_fn save;
	SCEngine_load_fn load;
	SCEngine_close_fn close;
	void loadLibrary(const fs::path& dllFile) {
        std::cout << "Load library " << dllFile << std::endl;
		dll = LoadLibraryW(dllFile.c_str());
        if (dll == nullptr) {
            int error = GetLastError();
            std::ifstream is(dllFile);
            std::cout << "LoadLibrary Error! dll=" << dll << ", dllFile=" << dllFile << ", good=" << is.good() << ", error=" << error << std::endl;
        }
		init = (SCEngine_init_fn)GetProcAddress(dll, "init");
		doFrame = (SCEngine_doFrame_fn)GetProcAddress(dll, "doFrame");
		doEditorFrame = (SCEngine_doEditorFrame_fn)GetProcAddress(dll, "doEditorFrame");
		doGameFrame = (SCEngine_doGameFrame_fn)GetProcAddress(dll, "doGameFrame");
		runGame = (SCEngine_runGame_fn)GetProcAddress(dll, "runGame");
		stopGame = (SCEngine_stopGame_fn)GetProcAddress(dll, "stopGame");
		save = (SCEngine_save_fn)GetProcAddress(dll, "save");
		load = (SCEngine_load_fn)GetProcAddress(dll, "load");
		close = (SCEngine_close_fn)GetProcAddress(dll, "close");
	}
	void freeLibrary() {
		FreeLibrary(dll);
        std::cout << "Free library" << std::endl;
	}
private:
	HMODULE dll;
} gSCEngine;

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

void buildProject() {
    assert(!gProjectDir.empty());
    fs::path cmakeExe = R"(..\compiler\cmake-3.25.0-rc1-windows-x86_64\bin\cmake.exe)";
    fs::path sourceDir = gProjectDir;
    fs::path buildDir = gProjectDir / "build";
    fs::path installDir = buildDir / "install";
    std::string cmd = cmakeExe.string() + " -S " + sourceDir.string() + " -B " + buildDir.string();
    std::string result = exec(cmd); // TODO: handle result
    cmd = cmakeExe.string() + " --build " + buildDir.string() + " --config Debug -j8";
    result = exec(cmd); // TODO: handle result
    cmd = cmakeExe.string() + " --install " + buildDir.string() + " --config Debug --prefix " + installDir.string();
    result = exec(cmd); // TODO: handle result
}

void loadProject() {
    assert(!gProjectDir.empty());
    gSCEngine.loadLibrary(gProjectDir / "build" / "install" / "bin" / "SCEngine.dll");
    // we must reset these function pointers before calling SCEngine::init, otherwise the program will crash
    // TODO: figure out why the program crashes
    gCallbackPointer.reset();
    gSCEngine.init(gOpenGLPointer, gCallbackPointer, gProjectDir / "build" / "install" / "asset");
}

void closeProject() {
    gSCEngine.close();
    gSCEngine.freeLibrary();
}

int main() {
    //std::ofstream out("output.txt");
    //std::cout.rdbuf(out.rdbuf());

	glfwSetErrorCallback(glfwErrorCallback);

	if (glfwInit() != GLFW_TRUE) {
        std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	gWindow = glfwCreateWindow(1920, 1080, "SCEngine", nullptr, nullptr);
	if (gWindow == nullptr) {
        std::cout << "Failed to create GLFW gWindow." << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(gWindow);
	int version = gladLoadGL(glfwGetProcAddress);
    std::cout << "GLAD " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;
    std::cout << "OpenGL " << glGetString(GL_VERSION) << ", GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	glfwSetScrollCallback(gWindow, ScrollCallback);
	glfwSetWindowSizeCallback(gWindow, WindowSizeCallback);
	glfwSetKeyCallback(gWindow, KeyCallback);
	glfwSetCharCallback(gWindow, CharCallback);
	glfwSetMouseButtonCallback(gWindow, MouseButtonCallback);
	glfwSetCursorPosCallback(gWindow, CursorPosCallback);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	if (!ImGui_ImplGlfw_InitForOpenGL(gWindow, false)) {
        std::cout << "ImGui_ImplGlfw_InitForOpenGL failed" << std::endl;
		assert(false);
	}

	if (!ImGui_ImplOpenGL3_Init(nullptr)) {
        std::cout << "ImGui_ImplOpenGL3_Init failed" << std::endl;
		assert(false);
	}

	glfwSetWindowContentScaleCallback(gWindow, WindowContentScaleCallback);
	float xscale, yscale;
	glfwGetWindowContentScale(gWindow, &xscale, &yscale);
	WindowContentScaleCallback(gWindow, xscale, yscale);

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;

    initCallbackPointer();
    gOpenGLPointer = OpenGLPointer();

	bool editorMode = true;
	SubWindow editorWindow("editor"), gameWindow("game");
	GameWorldEditor worldEditor;

	while (!glfwWindowShouldClose(gWindow)) {
		glfwPollEvents();
		//glfwGetWindowSize(mainWindow, &width, &height);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

        int bufferWidth, bufferHeight;
        glfwGetFramebufferSize(gWindow, &bufferWidth, &bufferHeight);
        glViewport(0, 0, bufferWidth, bufferHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui::Begin("Project");
        ImGui::Text(gProjectDir.empty() ? "No project is opened" : ("Opened project: " + gProjectDir.string()).c_str());
        ImGui::Button("Create new project"); // TODO: implement new project
        if (ImGui::Button("Open project")) {
            ImGuiFileDialog::Instance()->OpenDialog("ChooseDirDialogKey", "Open project", nullptr, ".");
        }
        if (!gProjectDir.empty()) {
            if (ImGui::Button("Close project")) {
                gProjectDir = "";
                closeProject();
            }
            if (ImGui::Button("Build")) {
                closeProject();
                buildProject();
                loadProject();
            }
        }
        ImGui::End();
        if (ImGuiFileDialog::Instance()->Display("ChooseDirDialogKey")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                std::string directory = ImGuiFileDialog::Instance()->GetCurrentPath();
                std::cout << "directory=" << directory << std::endl;
                if (!gProjectDir.empty()) {
                    closeProject();
                }
                gProjectDir = directory;
                buildProject();
                loadProject();
            }
            ImGuiFileDialog::Instance()->Close();
        }

        if (!gProjectDir.empty()) {
            auto& gameWorldData = gSCEngine.doFrame(editorMode);
            worldEditor.doFrame(gameWorldData);

            editorWindow.update();
            editorWindow.bind();
            gSCEngine.doEditorFrame(editorWindow.isFocus(), editorWindow.getWidth(), editorWindow.getHeight(), editorWindow.getCursorScreenPos().x, editorWindow.getCursorScreenPos().y);
            editorWindow.unbind();
            if (!editorMode) {
                gameWindow.update();
                gameWindow.bind();
                gSCEngine.doGameFrame(gameWindow.isFocus(), gameWindow.getWidth(), gameWindow.getHeight(), gameWindow.getCursorScreenPos().x, gameWindow.getCursorScreenPos().y);
                gameWindow.unbind();
            }

            glViewport(0, 0, bufferWidth, bufferHeight);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            ImGui::Begin("Game");
            if (editorMode) {
                if (ImGui::Button("Run")) {
                    editorMode = false;
                    gSCEngine.runGame();
                } else {
                    if (ImGui::Button("Save")) {
                        gSCEngine.save();
                    }
                    if (ImGui::Button("Load")) {
                        gSCEngine.load();
                    }
                }
            } else {
                if (ImGui::Button("Stop")) {
                    editorMode = true;
                    gSCEngine.stopGame();
                }
            }
            ImGui::End();
        }

		ImGui::ShowDemoWindow();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // TODO: fix GLError 1281 in this line when switch between game mode and editor mode!
		sCheckGLError();

		glfwSwapBuffers(gWindow);
	}

    if (!gProjectDir.empty()) {
        closeProject();
    }
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	glfwTerminate();
	return 0;
}
