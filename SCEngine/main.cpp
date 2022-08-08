#include "glad/gl.h"
#include "box2d/box2d.h"
#include "imgui/imgui.h"
#include "imgui_backends/imgui_impl_glfw.h"
#include "imgui_backends/imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


class Shader {
public:
	unsigned int ID;
	// constructor generates the shader on the fly
	// ------------------------------------------------------------------------
	Shader(const char* vertexPath, const char* fragmentPath) {
		// 1. retrieve the vertex/fragment source code from filePath
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		// ensure ifstream objects can throw exceptions:
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			// open files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			// read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			// convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure& e) {
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
		}
		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();
		// 2. compile shaders
		unsigned int vertex, fragment;
		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "VERTEX");
		// fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");
		// shader Program
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");
		// delete the shaders as they're linked into our program now and no longer necessary
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
	// activate the shader
	// ------------------------------------------------------------------------
	void use() {
		glUseProgram(ID);
	}
	// utility uniform functions
	void setBool(const std::string& name, bool value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	void setInt(const std::string& name, int value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setFloat(const std::string& name, float value) const {
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setMat4(const std::string& name, float* value) const {
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value);
	}

private:
	// utility function for checking shader compilation/linking errors.
	// ------------------------------------------------------------------------
	void checkCompileErrors(unsigned int shader, std::string type) {
		int success;
		char infoLog[1024];
		if (type != "PROGRAM") {
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else {
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}
};


class Camera {
public:
	Camera(int width, int height) : mWidth(width), mHeight(height) { }
	void getSize(int& outWidth, int& outHeight) { outWidth = mWidth; outHeight = mHeight; }
	void setSize(int width, int height) { mWidth = width; mHeight = height; }
private:
	int mWidth, mHeight;
	int mCenterX = 0, mCenterY = 0;
	float mZoom = 1.0f;
};
Camera gCamera(800, 600);

void glfwErrorCallback(int error, const char* description) {
	fprintf(stderr, "GLFW error occured. Code: %d. Description: %s\n", error, description);
}

static void ScrollCallback(GLFWwindow* window, double dx, double dy) {
	ImGui_ImplGlfw_ScrollCallback(window, dx, dy);
	if (ImGui::GetIO().WantCaptureMouse) {
		return;
	}

	//if (dy > 0) {
	//	g_camera.m_zoom /= 1.1f;
	//}
	//else {
	//	g_camera.m_zoom *= 1.1f;
	//}
}

static void WindowSizeCallback(GLFWwindow*, int width, int height) {
	gCamera.setSize(width, height);
}

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
	if (ImGui::GetIO().WantCaptureKeyboard) {
		return;
	}

	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			std::cout << "GLFW_KEY_ESCAPE" << std::endl;
			// Quit
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		case GLFW_KEY_LEFT:
			std::cout << "GLFW_KEY_LEFT" << std::endl;
			break;
		case GLFW_KEY_RIGHT:
			std::cout << "GLFW_KEY_RIGHT" << std::endl;
			break;
		case GLFW_KEY_DOWN:
			std::cout << "GLFW_KEY_DOWN" << std::endl;
			break;
		case GLFW_KEY_UP:
			std::cout << "GLFW_KEY_UP" << std::endl;
			break;
		case GLFW_KEY_HOME:
			std::cout << "GLFW_KEY_HOME" << std::endl;
			break;
		default:
			break;
		}
	} else if (action == GLFW_RELEASE) {
		std::cout << "GLFW_RELEASE" << std::endl;
	}
	// else GLFW_REPEAT
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

int main() {
	b2Vec2 gravity(0.0f, -10.0f);
	b2World world(gravity);

	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, -10.0f);
	b2Body* groundBody = world.CreateBody(&groundBodyDef);
	b2PolygonShape groundBox;
	groundBox.SetAsBox(50.0f, 10.0f);
	groundBody->CreateFixture(&groundBox, 0.0f);

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(0.0f, 4.0f);
	b2Body* body = world.CreateBody(&bodyDef);
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(1.0f, 1.0f);
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;
	fixtureDef.restitution = 1.0f;
	body->CreateFixture(&fixtureDef);

	float timeStep = 1.0f / 60.0f;
	int32 velocityIterations = 8;
	int32 positionIterations = 3;

	for (int32 i = 0; i < 360; ++i) {
		world.Step(timeStep, velocityIterations, positionIterations);
		b2Vec2 position = body->GetPosition();
		float angle = body->GetAngle();
		printf("%4.2f %4.2f %4.2f\n", position.x, position.y, angle);
	}
	
	printf("Box2D Version %d.%d.%d\n", b2_version.major, b2_version.minor, b2_version.revision);

	glfwSetErrorCallback(glfwErrorCallback);

	if (glfwInit() != GLFW_TRUE) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	int width, height;
	gCamera.getSize(width, height);
	GLFWwindow* mainWindow = glfwCreateWindow(width, height, "SCEngine", NULL, NULL);

	if (mainWindow == NULL) {
		fprintf(stderr, "Failed to create GLFW window.\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(mainWindow);

	int version = gladLoadGL(glfwGetProcAddress);
	printf("GLAD %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
	printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

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

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	float vertices[] = {
		// ---- vertex ----   ---- color ----
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,
	};
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	Shader shader("shader.vs", "shader.fs");

	while (!glfwWindowShouldClose(mainWindow)) {
		//glfwGetWindowSize(mainWindow, &width, &height);
		glfwGetFramebufferSize(mainWindow, &width, &height);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
		glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::mat4 model = glm::rotate(glm::scale(glm::mat4(1.0f), glm::vec3(400.0f, 300.0f, 1.0f)), glm::radians(15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glm::mat4 projection = glm::ortho((float)(-width/2), (float) (width/2), (float)(-height/2), (float) (height/2), -1000.0f, 1000.0f);
		std::cout << glm::to_string(projection) << std::endl;
		shader.use();
		shader.setMat4("model", glm::value_ptr(model));
		shader.setMat4("view", glm::value_ptr(view));
		shader.setMat4("projection", glm::value_ptr(projection));
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("imgui");
		ImGui::Text("Text");
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(mainWindow);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	glfwTerminate();

	std::cout << "Hello CMake." << std::endl;
	return 0;
}
