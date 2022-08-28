﻿#include <iostream>

#include "glad/gl.h"
#include "box2d/box2d.h"
#include "imgui/imgui.h"
#include "imgui_backends/imgui_impl_glfw.h"
#include "imgui_backends/imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"
#include "nlohmann/json.hpp"

#include "core/GameWorld.h"
#include "core/GameObject.h"
#include "core/Input.h"
#include "core/Camera.h"
#include "editor/EditorCameraController.h"
#include "editor/GameWorldEditor.h"
#include "common/RectangleRender.h"
#include "common/RigidBody.h"
#include "common/RectangleCollider.h"

void glfwErrorCallback(int error, const char* description) {
	fprintf(stderr, "GLFW error occured. Code: %d. Description: %s\n", error, description);
}

static void ScrollCallback(GLFWwindow* window, double dx, double dy) {
	ImGui_ImplGlfw_ScrollCallback(window, dx, dy);

	if (gEditorFocus) {
		gEditorInput.mScrollX = (float)dx;
		gEditorInput.mScrollY = (float)dy;
	}

	if (gGameFocus) {
		gInput.mScrollX = (float)dx;
		gInput.mScrollY = (float)dy;
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

	/*if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT || key == GLFW_KEY_A) {
			gCameraX -= 1;
		} else if (key == GLFW_KEY_RIGHT || key == GLFW_KEY_D) {
			gCameraX += 1;
		} else if (key == GLFW_KEY_UP || key == GLFW_KEY_W) {
			gCameraY += 1;
		} else if (key == GLFW_KEY_DOWN || key == GLFW_KEY_S) {
			gCameraY -= 1;
		}
	} else if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT || key == GLFW_KEY_A) {
			gCameraX += 1;
		} else if (key == GLFW_KEY_RIGHT || key == GLFW_KEY_D) {
			gCameraX -= 1;
		} else if (key == GLFW_KEY_UP || key == GLFW_KEY_W) {
			gCameraY -= 1;
		} else if (key == GLFW_KEY_DOWN || key == GLFW_KEY_S) {
			gCameraY += 1;
		}
	}*/
}

static void CharCallback(GLFWwindow* window, unsigned int c) {
	ImGui_ImplGlfw_CharCallback(window, c);
}

static void MouseButtonCallback(GLFWwindow* window, int32 button, int32 action, int32 mods) {
	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

	/*double xd, yd;
	glfwGetCursorPos(window, &xd, &yd);
	b2Vec2 ps((float)xd, (float)yd);

	// Use the mouse to move things around.
	if (button == GLFW_MOUSE_BUTTON_1) {
		//<##>
		//ps.Set(0, 0);
		b2Vec2 pw = g_camera.ConvertScreenToWorld(ps);
		if (action == GLFW_PRESS) {
			if (mods == GLFW_MOD_SHIFT) {
				s_test->ShiftMouseDown(pw);
			} else {
				s_test->MouseDown(pw);
			}
		}

		if (action == GLFW_RELEASE) {
			s_test->MouseUp(pw);
		}
	} else if (button == GLFW_MOUSE_BUTTON_2) {
		if (action == GLFW_PRESS) {
			s_clickPointWS = g_camera.ConvertScreenToWorld(ps);
			s_rightMouseDown = true;
		}

		if (action == GLFW_RELEASE) {
			s_rightMouseDown = false;
		}
	}*/
}

static void CursorPosCallback(GLFWwindow*, double xd, double yd) {
	/*b2Vec2 ps((float)xd, (float)yd);

	b2Vec2 pw = g_camera.ConvertScreenToWorld(ps);
	s_test->MouseMove(pw);

	if (s_rightMouseDown) {
		b2Vec2 diff = pw - s_clickPointWS;
		g_camera.m_center.x -= diff.x;
		g_camera.m_center.y -= diff.y;
		s_clickPointWS = g_camera.ConvertScreenToWorld(ps);
	}*/
}

void processInput(GLFWwindow* window) {
	/*if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		gCameraX -= 1;
	} else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		gCameraX += 1;
	} else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		gCameraY += 1;
	} else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		gCameraY -= 1;
	}*/
}

int main() {
	glfwSetErrorCallback(glfwErrorCallback);

	if (glfwInit() != GLFW_TRUE) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* mainWindow = glfwCreateWindow(800 * gScale, 600 * gScale, "SCEngine", NULL, NULL);

	if (mainWindow == NULL) {
		fprintf(stderr, "Failed to create GLFW window.\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(mainWindow);

	int version = gladLoadGL(glfwGetProcAddress);
	printf("GLAD %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
	printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("Box2D Version %d.%d.%d\n", b2_version.major, b2_version.minor, b2_version.revision);

	glfwSetScrollCallback(mainWindow, ScrollCallback);
	glfwSetWindowSizeCallback(mainWindow, WindowSizeCallback);
	glfwSetKeyCallback(mainWindow, KeyCallback);
	glfwSetCharCallback(mainWindow, CharCallback);
	glfwSetMouseButtonCallback(mainWindow, MouseButtonCallback);
	glfwSetCursorPosCallback(mainWindow, CursorPosCallback);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	if (!ImGui_ImplGlfw_InitForOpenGL(mainWindow, false)) {
		printf("ImGui_ImplGlfw_InitForOpenGL failed\n");
		assert(false);
	}

	if (!ImGui_ImplOpenGL3_Init(NULL)) {
		printf("ImGui_ImplOpenGL3_Init failed\n");
		assert(false);
	}

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	// scale font so that we can see texts in high dpi display clearly
	ImFontConfig fontConfig;
	fontConfig.SizePixels = 13.0f * gScale;
	ImFont* font = ImGui::GetIO().Fonts->AddFontDefault(&fontConfig);
	//ImGui::GetStyle().ScaleAllSizes(gScale);

	// Add some GameObject to game world
	auto gameWorld = std::make_shared<GameWorld>();

	auto cameraObject = std::make_shared<GameObject>();
	cameraObject->mName = "Camera";
	auto camera = std::make_shared<Camera>();
	int editorWidth = 400 * gScale, editorHeight = 300 * gScale;
	camera->setSize(editorWidth / gScale, editorHeight / gScale);
	auto editorCameraController = std::make_shared<EditorCameraController>();
	cameraObject->addScript(camera);
	cameraObject->addScript(editorCameraController);
	gameWorld->addGameObject(cameraObject);

	auto rectangleRender = std::make_shared<RectangleRender>();
	auto rigidBody = std::make_shared<RigidBody>();
	rigidBody->mBodyDef.type = b2_dynamicBody;
	auto rectangleCollider = std::make_shared<RectangleCollider>();
	auto gameObject = std::make_shared<GameObject>();
	gameObject->mName = "Box1";
	gameObject->mTransform.mScaleX = 10.f;
	gameObject->mTransform.mScaleY = 10.f;
	gameObject->mTransform.mPosX = 0.0f;
	gameObject->mTransform.mPosY = 100.0f;
	gameObject->addScript(rectangleRender);
	gameObject->addScript(rigidBody);
	gameObject->addScript(rectangleCollider);
	gameWorld->addGameObject(gameObject);

	auto gameObject2 = gameObject->clone();
	gameObject2->mName = "Box2";
	gameObject2->mTransform.mPosX = 5.0f;
	gameObject2->mTransform.mPosY = 120.0f;
	gameWorld->addGameObject(gameObject2);
	
	auto groundObject = gameObject->clone();
	groundObject->mName = "Ground";
	groundObject->getScript<RigidBody>()->mBodyDef.type = b2_staticBody;
	groundObject->mTransform.mPosX = 0.0f;
	groundObject->mTransform.mPosY = -100.0f;
	groundObject->mTransform.mScaleX = 400.0f;
	groundObject->mTransform.mScaleY = 80.f;
	gameWorld->addGameObject(groundObject);

	gameWorld->create();

	nlohmann::json j = gameWorld->getData();
	std::cout << j << std::endl;

	GameWorldEditor worldEditor;

	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, editorWidth, editorHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, editorWidth, editorHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	gInput.setWindow(mainWindow);
	gEditorInput.setWindow(mainWindow);
	float lastTime = glfwGetTime();
	while (!glfwWindowShouldClose(mainWindow)) {
		gameWorld->mCurrentTime = glfwGetTime();
		gameWorld->mDeltaTime = gameWorld->mCurrentTime - lastTime;
		lastTime = gameWorld->mCurrentTime;

		gInput.reset();
		gEditorInput.reset();
		glfwPollEvents();
		processInput(mainWindow);
		
		//std::cout << "currentTime=" << currentTime << ", gCameraXY=" << gCameraX << "," << gCameraY << std::endl;
		//glfwGetWindowSize(mainWindow, &width, &height);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glViewport(0, 0, editorWidth, editorHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		gameWorld->mMainCamera->setSize(editorWidth / gScale, editorHeight / gScale);
		gameWorld->update();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		int bufferWidth, bufferHeight;
		glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);
		glViewport(0, 0, bufferWidth, bufferHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		worldEditor.update(gameWorld);

		ImGui::Begin("restart");
		if (ImGui::Button("restart")) {
			gameWorld->destroy();
			GameWorldData gameWorldData = j;
			gameWorld = GameWorld::create(gameWorldData);
			gameWorld->create();
		}
		ImGui::End();

		ImGui::Begin("editor");
		gEditorFocus = ImGui::IsWindowFocused();
		ImGui::Image((ImTextureID)texture, ImVec2(editorWidth, editorHeight), ImVec2(0, 1.0f), ImVec2(1.0f, 0));
		ImVec2 windowSize = ImGui::GetWindowSize();
		windowSize.y = std::max(windowSize.y - 50.0f, 1.0f);
		if (editorWidth != windowSize.x || editorHeight != windowSize.y) {
			editorWidth = windowSize.x;
			editorHeight = windowSize.y;

			glDeleteTextures(1, &texture);
			glDeleteRenderbuffers(1, &rbo);

			glBindFramebuffer(GL_FRAMEBUFFER, fbo);

			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, editorWidth, editorHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glBindTexture(GL_TEXTURE_2D, 0);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

			glGenRenderbuffers(1, &rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, editorWidth, editorHeight);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);

			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		ImGui::End();

		ImGui::ShowDemoWindow();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(mainWindow);
	}

	gameWorld->destroy();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	glfwTerminate();

	std::cout << "Hello CMake." << std::endl;
	return 0;
}
