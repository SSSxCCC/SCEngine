﻿#include "glad/gl.h"
#include "box2d/box2d.h"
#include "imgui/imgui.h"
#include "imgui_backends/imgui_impl_glfw.h"
#include "imgui_backends/imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Shader.hpp"
#include "Camera.hpp"



static void sCheckGLError() {
	GLenum errCode = glGetError();
	if (errCode != GL_NO_ERROR) {
		fprintf(stderr, "OpenGL error = %d\n", errCode);
		assert(false);
	}
}


struct GLRenderPoints
{
	void Create()
	{
		m_projectionUniform = glGetUniformLocation(m_shader.ID, "projectionMatrix");
		m_vertexAttribute = 0;
		m_colorAttribute = 1;
		m_sizeAttribute = 2;

		// Generate
		glGenVertexArrays(1, &m_vaoId);
		glGenBuffers(3, m_vboIds);

		glBindVertexArray(m_vaoId);
		glEnableVertexAttribArray(m_vertexAttribute);
		glEnableVertexAttribArray(m_colorAttribute);
		glEnableVertexAttribArray(m_sizeAttribute);

		// Vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
		glVertexAttribPointer(m_vertexAttribute, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
		glVertexAttribPointer(m_colorAttribute, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_colors), m_colors, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[2]);
		glVertexAttribPointer(m_sizeAttribute, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_sizes), m_sizes, GL_DYNAMIC_DRAW);

		sCheckGLError();

		// Cleanup
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		m_count = 0;
	}

	void Destroy()
	{
		if (m_vaoId)
		{
			glDeleteVertexArrays(1, &m_vaoId);
			glDeleteBuffers(3, m_vboIds);
			m_vaoId = 0;
		}

		m_shader.destroy();
	}

	void Vertex(const b2Vec2& v, const b2Color& c, float size)
	{
		if (m_count == e_maxVertices)
			Flush();

		m_vertices[m_count] = v;
		m_colors[m_count] = c;
		m_sizes[m_count] = size;
		++m_count;
	}

	void Flush()
	{
		if (m_count == 0)
			return;

		m_shader.use();

		glm::mat4 proj = gCamera.buildProjectionMatrix();
		glUniformMatrix4fv(m_projectionUniform, 1, GL_FALSE, glm::value_ptr(proj));

		glBindVertexArray(m_vaoId);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(b2Vec2), m_vertices);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(b2Color), m_colors);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[2]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(float), m_sizes);

		glEnable(GL_PROGRAM_POINT_SIZE);
		glDrawArrays(GL_POINTS, 0, m_count);
		glDisable(GL_PROGRAM_POINT_SIZE);

		sCheckGLError();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glUseProgram(0);

		m_count = 0;
	}

	enum { e_maxVertices = 512 };
	b2Vec2 m_vertices[e_maxVertices];
	b2Color m_colors[e_maxVertices];
	float m_sizes[e_maxVertices];

	int32 m_count;

	GLuint m_vaoId;
	GLuint m_vboIds[3];
	//GLuint m_programId;
	Shader m_shader = Shader("shaders/shaderPoints.vs", "shaders/shaderPoints.fs");
	GLint m_projectionUniform;
	GLint m_vertexAttribute;
	GLint m_colorAttribute;
	GLint m_sizeAttribute;
};

//
struct GLRenderLines
{
	void Create()
	{
		m_projectionUniform = glGetUniformLocation(m_shader.ID, "projectionMatrix");
		m_vertexAttribute = 0;
		m_colorAttribute = 1;

		// Generate
		glGenVertexArrays(1, &m_vaoId);
		glGenBuffers(2, m_vboIds);

		glBindVertexArray(m_vaoId);
		glEnableVertexAttribArray(m_vertexAttribute);
		glEnableVertexAttribArray(m_colorAttribute);

		// Vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
		glVertexAttribPointer(m_vertexAttribute, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
		glVertexAttribPointer(m_colorAttribute, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_colors), m_colors, GL_DYNAMIC_DRAW);

		sCheckGLError();

		// Cleanup
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		m_count = 0;
	}

	void Destroy()
	{
		if (m_vaoId)
		{
			glDeleteVertexArrays(1, &m_vaoId);
			glDeleteBuffers(2, m_vboIds);
			m_vaoId = 0;
		}

		m_shader.destroy();
	}

	void Vertex(const b2Vec2& v, const b2Color& c)
	{
		if (m_count == e_maxVertices)
			Flush();

		m_vertices[m_count] = v;
		m_colors[m_count] = c;
		++m_count;
	}

	void Flush()
	{
		if (m_count == 0)
			return;

		m_shader.use();

		glm::mat4 proj = gCamera.buildProjectionMatrix();
		glUniformMatrix4fv(m_projectionUniform, 1, GL_FALSE, glm::value_ptr(proj));

		glBindVertexArray(m_vaoId);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(b2Vec2), m_vertices);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(b2Color), m_colors);

		glDrawArrays(GL_LINES, 0, m_count);

		sCheckGLError();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glUseProgram(0);

		m_count = 0;
	}

	enum { e_maxVertices = 2 * 512 };
	b2Vec2 m_vertices[e_maxVertices];
	b2Color m_colors[e_maxVertices];

	int32 m_count;

	GLuint m_vaoId;
	GLuint m_vboIds[2];
	//GLuint m_programId;
	Shader m_shader = Shader("shaders/shaderLines.vs", "shaders/shaderLines.fs");
	GLint m_projectionUniform;
	GLint m_vertexAttribute;
	GLint m_colorAttribute;
};

//
struct GLRenderTriangles
{
	void Create()
	{
		m_projectionUniform = glGetUniformLocation(m_shader.ID, "projectionMatrix");
		m_vertexAttribute = 0;
		m_colorAttribute = 1;

		// Generate
		glGenVertexArrays(1, &m_vaoId);
		glGenBuffers(2, m_vboIds);

		glBindVertexArray(m_vaoId);
		glEnableVertexAttribArray(m_vertexAttribute);
		glEnableVertexAttribArray(m_colorAttribute);

		// Vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
		glVertexAttribPointer(m_vertexAttribute, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
		glVertexAttribPointer(m_colorAttribute, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_colors), m_colors, GL_DYNAMIC_DRAW);

		sCheckGLError();

		// Cleanup
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		m_count = 0;
	}

	void Destroy()
	{
		if (m_vaoId)
		{
			glDeleteVertexArrays(1, &m_vaoId);
			glDeleteBuffers(2, m_vboIds);
			m_vaoId = 0;
		}

		m_shader.destroy();
	}

	void Vertex(const b2Vec2& v, const b2Color& c)
	{
		if (m_count == e_maxVertices)
			Flush();

		m_vertices[m_count] = v;
		m_colors[m_count] = c;
		++m_count;
	}

	void Flush()
	{
		if (m_count == 0)
			return;

		m_shader.use();

		glm::mat4 proj = gCamera.buildProjectionMatrix();
		glUniformMatrix4fv(m_projectionUniform, 1, GL_FALSE, glm::value_ptr(proj));

		glBindVertexArray(m_vaoId);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(b2Vec2), m_vertices);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(b2Color), m_colors);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDrawArrays(GL_TRIANGLES, 0, m_count);
		glDisable(GL_BLEND);

		sCheckGLError();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glUseProgram(0);

		m_count = 0;
	}

	enum { e_maxVertices = 3 * 512 };
	b2Vec2 m_vertices[e_maxVertices];
	b2Color m_colors[e_maxVertices];

	int32 m_count;

	GLuint m_vaoId;
	GLuint m_vboIds[2];
	//GLuint m_programId;
	Shader m_shader = Shader("shaders/shaderTriangles.vs", "shaders/shaderTriangles.fs");
	GLint m_projectionUniform;
	GLint m_vertexAttribute;
	GLint m_colorAttribute;
};

// This class implements debug drawing callbacks that are invoked
// inside b2World::Step.
class DebugDraw : public b2Draw {
public:
	DebugDraw() {
		m_showUI = true;
		m_points = NULL;
		m_lines = NULL;
		m_triangles = NULL;
	}

	~DebugDraw() {
		b2Assert(m_points == NULL);
		b2Assert(m_lines == NULL);
		b2Assert(m_triangles == NULL);
	}

	void Create() {
		m_points = new GLRenderPoints;
		m_points->Create();
		m_lines = new GLRenderLines;
		m_lines->Create();
		m_triangles = new GLRenderTriangles;
		m_triangles->Create();
	}

	void Destroy() {
		m_points->Destroy();
		delete m_points;
		m_points = NULL;

		m_lines->Destroy();
		delete m_lines;
		m_lines = NULL;

		m_triangles->Destroy();
		delete m_triangles;
		m_triangles = NULL;
	}

	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override {
		b2Vec2 p1 = vertices[vertexCount - 1];
		for (int32 i = 0; i < vertexCount; ++i) {
			b2Vec2 p2 = vertices[i];
			m_lines->Vertex(p1, color);
			m_lines->Vertex(p2, color);
			p1 = p2;
		}
	}

	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override {
		b2Color fillColor(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);

		for (int32 i = 1; i < vertexCount - 1; ++i) {
			m_triangles->Vertex(vertices[0], fillColor);
			m_triangles->Vertex(vertices[i], fillColor);
			m_triangles->Vertex(vertices[i + 1], fillColor);
		}

		b2Vec2 p1 = vertices[vertexCount - 1];
		for (int32 i = 0; i < vertexCount; ++i) {
			b2Vec2 p2 = vertices[i];
			m_lines->Vertex(p1, color);
			m_lines->Vertex(p2, color);
			p1 = p2;
		}
	}

	void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) override {
		const float k_segments = 16.0f;
		const float k_increment = 2.0f * b2_pi / k_segments;
		float sinInc = sinf(k_increment);
		float cosInc = cosf(k_increment);
		b2Vec2 r1(1.0f, 0.0f);
		b2Vec2 v1 = center + radius * r1;
		for (int32 i = 0; i < k_segments; ++i) {
			// Perform rotation to avoid additional trigonometry.
			b2Vec2 r2;
			r2.x = cosInc * r1.x - sinInc * r1.y;
			r2.y = sinInc * r1.x + cosInc * r1.y;
			b2Vec2 v2 = center + radius * r2;
			m_lines->Vertex(v1, color);
			m_lines->Vertex(v2, color);
			r1 = r2;
			v1 = v2;
		}
	}

	void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) override {
		const float k_segments = 16.0f;
		const float k_increment = 2.0f * b2_pi / k_segments;
		float sinInc = sinf(k_increment);
		float cosInc = cosf(k_increment);
		b2Vec2 v0 = center;
		b2Vec2 r1(cosInc, sinInc);
		b2Vec2 v1 = center + radius * r1;
		b2Color fillColor(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);
		for (int32 i = 0; i < k_segments; ++i) {
			// Perform rotation to avoid additional trigonometry.
			b2Vec2 r2;
			r2.x = cosInc * r1.x - sinInc * r1.y;
			r2.y = sinInc * r1.x + cosInc * r1.y;
			b2Vec2 v2 = center + radius * r2;
			m_triangles->Vertex(v0, fillColor);
			m_triangles->Vertex(v1, fillColor);
			m_triangles->Vertex(v2, fillColor);
			r1 = r2;
			v1 = v2;
		}

		r1.Set(1.0f, 0.0f);
		v1 = center + radius * r1;
		for (int32 i = 0; i < k_segments; ++i) {
			b2Vec2 r2;
			r2.x = cosInc * r1.x - sinInc * r1.y;
			r2.y = sinInc * r1.x + cosInc * r1.y;
			b2Vec2 v2 = center + radius * r2;
			m_lines->Vertex(v1, color);
			m_lines->Vertex(v2, color);
			r1 = r2;
			v1 = v2;
		}

		// Draw a line fixed in the circle to animate rotation.
		b2Vec2 p = center + radius * axis;
		m_lines->Vertex(center, color);
		m_lines->Vertex(p, color);
	}

	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override {
		m_lines->Vertex(p1, color);
		m_lines->Vertex(p2, color);
	}

	void DrawTransform(const b2Transform& xf) override {
		const float k_axisScale = 0.4f;
		b2Color red(1.0f, 0.0f, 0.0f);
		b2Color green(0.0f, 1.0f, 0.0f);
		b2Vec2 p1 = xf.p, p2;

		m_lines->Vertex(p1, red);
		p2 = p1 + k_axisScale * xf.q.GetXAxis();
		m_lines->Vertex(p2, red);

		m_lines->Vertex(p1, green);
		p2 = p1 + k_axisScale * xf.q.GetYAxis();
		m_lines->Vertex(p2, green);
	}

	void DrawPoint(const b2Vec2& p, float size, const b2Color& color) override {
		m_points->Vertex(p, color, size);
	}

	void DrawString(int x, int y, const char* string, ...) {
		if (m_showUI == false) {
			return;
		}

		va_list arg;
		va_start(arg, string);
		ImGui::Begin("Overlay", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar);
		ImGui::SetCursorPos(ImVec2(float(x), float(y)));
		ImGui::TextColoredV(ImColor(230, 153, 153, 255), string, arg);
		ImGui::End();
		va_end(arg);
	}

	void DrawString(const b2Vec2& pw, const char* string, ...) {
		float screenX, screenY;
		gCamera.worldToScreen(pw.x, pw.y, screenX, screenY);
		b2Vec2 ps(screenX, screenY);

		va_list arg;
		va_start(arg, string);
		ImGui::Begin("Overlay", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar);
		ImGui::SetCursorPos(ImVec2(ps.x, ps.y));
		ImGui::TextColoredV(ImColor(230, 153, 153, 255), string, arg);
		ImGui::End();
		va_end(arg);
	}

	void DrawAABB(b2AABB* aabb, const b2Color& c) {
		b2Vec2 p1 = aabb->lowerBound;
		b2Vec2 p2 = b2Vec2(aabb->upperBound.x, aabb->lowerBound.y);
		b2Vec2 p3 = aabb->upperBound;
		b2Vec2 p4 = b2Vec2(aabb->lowerBound.x, aabb->upperBound.y);

		m_lines->Vertex(p1, c);
		m_lines->Vertex(p2, c);

		m_lines->Vertex(p2, c);
		m_lines->Vertex(p3, c);

		m_lines->Vertex(p3, c);
		m_lines->Vertex(p4, c);

		m_lines->Vertex(p4, c);
		m_lines->Vertex(p1, c);
	}

	void Flush() {
		m_triangles->Flush();
		m_lines->Flush();
		m_points->Flush();
	}

	bool m_showUI;
	GLRenderPoints* m_points;
	GLRenderLines* m_lines;
	GLRenderTriangles* m_triangles;
};


void glfwErrorCallback(int error, const char* description) {
	fprintf(stderr, "GLFW error occured. Code: %d. Description: %s\n", error, description);
}

static void ScrollCallback(GLFWwindow* window, double dx, double dy) {
	ImGui_ImplGlfw_ScrollCallback(window, dx, dy);
	if (ImGui::GetIO().WantCaptureMouse) {
		return;
	}

	if (dy > 0) {
		gCamera.zoomIn();
	} else {
		gCamera.zoomOut();
	}
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
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		case GLFW_KEY_HOME:
			gCamera.reset();
			break;
		default:
			break;
		}
	}

	if (action == GLFW_PRESS) {
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
	}
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
	b2Vec2 gravity(0.0f, -10.0f);
	b2World world(gravity);

	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, -100.0f);
	b2Body* groundBody = world.CreateBody(&groundBodyDef);
	b2PolygonShape groundBox;
	groundBox.SetAsBox(500.0f, 100.0f);
	groundBody->CreateFixture(&groundBox, 0.0f);

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(0.0f, 40.0f);
	b2Body* body = world.CreateBody(&bodyDef);
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(10.0f, 10.0f);
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;
	fixtureDef.restitution = 1.0f;
	body->CreateFixture(&fixtureDef);

	float timeStep = 1.0f / 60.0f;
	int32 velocityIterations = 8;
	int32 positionIterations = 3;
	
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

	Shader shader("shaders/shader.vs", "shaders/shader.fs");

	DebugDraw debugDraw;
	debugDraw.Create();
	world.SetDebugDraw(&debugDraw);

	float lastTime = glfwGetTime();
	while (!glfwWindowShouldClose(mainWindow)) {
		float currentTime = glfwGetTime();
		float deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		processInput(mainWindow);
		//std::cout << "deltaTime=" << deltaTime << ", gCameraXY=" << gCameraX << "," << gCameraY << std::endl;
		gCamera.move((float)gCameraX * gCamera.moveSpeed * deltaTime, (float)gCameraY * gCamera.moveSpeed * deltaTime);

		//glfwGetWindowSize(mainWindow, &width, &height);
		int bufferWidth, bufferHeight;
		glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);
		gCamera.getSize(width, height);
		if (width != bufferWidth || height != bufferHeight) {
			std::cout << "bufferSize=" << bufferWidth << "x" << bufferHeight << ", windowSize=" << width << "x" << height << std::endl;
		}
		glViewport(0, 0, bufferWidth, bufferHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*glm::mat4 model = glm::rotate(glm::scale(glm::mat4(1.0f), glm::vec3(400.0f, 300.0f, 1.0f)), glm::radians(15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = gCamera.buildProjectionMatrix();
		shader.use();
		shader.setMat4("model", glm::value_ptr(model));
		shader.setMat4("view", glm::value_ptr(view));
		shader.setMat4("projection", glm::value_ptr(projection));
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);*/

		uint32 flags = b2Draw::e_shapeBit | b2Draw::e_jointBit | b2Draw::e_aabbBit | b2Draw::e_centerOfMassBit;
		debugDraw.SetFlags(flags);
		world.Step(timeStep, velocityIterations, positionIterations);
		world.DebugDraw();
		debugDraw.Flush();
		b2Vec2 position = body->GetPosition();
		float angle = body->GetAngle();
		printf("%4.2f %4.2f %4.2f\n", position.x, position.y, angle);

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

	debugDraw.Destroy();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	glfwTerminate();

	std::cout << "Hello CMake." << std::endl;
	return 0;
}
