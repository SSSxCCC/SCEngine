#include "editor/PhysicsDebugDraw.h"
#include "utility/Shader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "core/GameWorld.h"
#include "imgui/imgui.h"

static void sCheckGLError() {
	GLenum errCode = glGetError();
	if (errCode != GL_NO_ERROR) {
		fprintf(stderr, "OpenGL error = %d\n", errCode);
		//assert(false);
	}
}

struct GLRenderPoints {
	void Create() {
		// Generate
		glGenVertexArrays(1, &m_vaoId);
		glGenBuffers(3, m_vboIds);

		glBindVertexArray(m_vaoId);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		// Vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_colors), m_colors, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[2]);
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_sizes), m_sizes, GL_DYNAMIC_DRAW);

		sCheckGLError();

		// Cleanup
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		m_count = 0;
	}

	void Destroy() {
		if (m_vaoId) {
			glDeleteVertexArrays(1, &m_vaoId);
			glDeleteBuffers(3, m_vboIds);
			m_vaoId = 0;
		}

		m_shader.destroy();
	}

	void Vertex(const b2Vec2& v, const b2Color& c, float size) {
		if (m_count == e_maxVertices)
			Flush();

		m_vertices[m_count] = v;
		m_colors[m_count] = c;
		m_sizes[m_count] = size;
		++m_count;
	}

	void setProjectionMatrix(float* pm) {
		mProjectionMatrix = pm;
	}

	void Flush() {
		if (m_count == 0)
			return;

		m_shader.use();
		m_shader.setMat4("projectionMatrix", mProjectionMatrix);

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
	Shader m_shader = Shader("shaders/shaderPoints.vs", "shaders/shaderPoints.fs");
	float* mProjectionMatrix;
};


struct GLRenderLines {
	void Create() {
		// Generate
		glGenVertexArrays(1, &m_vaoId);
		glGenBuffers(2, m_vboIds);

		glBindVertexArray(m_vaoId);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		// Vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_colors), m_colors, GL_DYNAMIC_DRAW);

		sCheckGLError();

		// Cleanup
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		m_count = 0;
	}

	void Destroy() {
		if (m_vaoId) {
			glDeleteVertexArrays(1, &m_vaoId);
			glDeleteBuffers(2, m_vboIds);
			m_vaoId = 0;
		}

		m_shader.destroy();
	}

	void Vertex(const b2Vec2& v, const b2Color& c) {
		if (m_count == e_maxVertices)
			Flush();

		m_vertices[m_count] = v;
		m_colors[m_count] = c;
		++m_count;
	}

	void setProjectionMatrix(float* pm) {
		mProjectionMatrix = pm;
	}

	void Flush() {
		if (m_count == 0)
			return;

		m_shader.use();
		m_shader.setMat4("projectionMatrix", mProjectionMatrix);

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
	Shader m_shader = Shader("shaders/shaderLines.vs", "shaders/shaderLines.fs");
	float* mProjectionMatrix;
};


struct GLRenderTriangles {
	void Create() {
		// Generate
		glGenVertexArrays(1, &m_vaoId);
		glGenBuffers(2, m_vboIds);

		glBindVertexArray(m_vaoId);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		// Vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_colors), m_colors, GL_DYNAMIC_DRAW);

		sCheckGLError();

		// Cleanup
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		m_count = 0;
	}

	void Destroy() {
		if (m_vaoId) {
			glDeleteVertexArrays(1, &m_vaoId);
			glDeleteBuffers(2, m_vboIds);
			m_vaoId = 0;
		}

		m_shader.destroy();
	}

	void Vertex(const b2Vec2& v, const b2Color& c) {
		if (m_count == e_maxVertices)
			Flush();

		m_vertices[m_count] = v;
		m_colors[m_count] = c;
		++m_count;
	}

	void setProjectionMatrix(float* pm) {
		mProjectionMatrix = pm;
	}

	void Flush() {
		if (m_count == 0)
			return;

		m_shader.use();
		m_shader.setMat4("projectionMatrix", mProjectionMatrix);

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
	Shader m_shader = Shader("shaders/shaderTriangles.vs", "shaders/shaderTriangles.fs");
	float* mProjectionMatrix;
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
		// TODO: worldToScreen implement here
		//mGameWorld->mMainCamera->worldToScreen(pw.x, pw.y, screenX, screenY);
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

	void setProjectionMatrix(float* pm) {
		m_triangles->setProjectionMatrix(pm);
		m_lines->setProjectionMatrix(pm);
		m_points->setProjectionMatrix(pm);
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


void PhysicsDebugDraw::onCreate() {
	mDebugDraw = new DebugDraw;
	mDebugDraw->Create();
	mPhysicsWorld = mGameObject->getScript<PhysicsWorld>();
	mPhysicsWorld->mWorld->SetDebugDraw(mDebugDraw);
}

void PhysicsDebugDraw::onDraw(float* projectionMatrix, bool forEditor) {
	if (forEditor) {
		mDebugDraw->setProjectionMatrix(projectionMatrix);
		mDebugDraw->SetFlags(b2Draw::e_shapeBit | b2Draw::e_jointBit | b2Draw::e_aabbBit | b2Draw::e_centerOfMassBit);
		mPhysicsWorld->mWorld->DebugDraw();
		mDebugDraw->Flush();
	}
}

void PhysicsDebugDraw::onDestroy() {
	if (mPhysicsWorld && mPhysicsWorld->mWorld) {
		mPhysicsWorld->mWorld->SetDebugDraw(nullptr);
	}
	mPhysicsWorld = nullptr;
	mDebugDraw->Destroy();
	delete mDebugDraw;
	mDebugDraw = nullptr;
}
