#ifndef _SHAPE_RENDER_H_
#define _SHAPE_RENDER_H_

#include "glad/gl.h"
#include "core/Script.hpp"

class ShapeRender : public Script {
public:
	using Script::Script;
	void onCreate() override {
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		float vertices[] = {
			// ---- vertex ----   ---- color ----
			0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,
		};

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		sCheckGLError();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		mGameObject->mTransform.mScaleX = 400.f;
		mGameObject->mTransform.mScaleY = 300.f;
	}

	void onUpdate() override {
		mGameObject->mTransform.mDegree = mGameObject->mGameWorld->mCurrentTime * 100.0f;
		onDraw();
	}

	void onDestroy() override {
		shader.destroy();
	}

private:
	void onDraw() {
		glm::mat4 model = mGameObject->mTransform.buildModelMatrix();
		glm::mat4 projection = gCamera.buildProjectionMatrix();
		shader.use();
		shader.setMat4("model", glm::value_ptr(model));
		shader.setMat4("projection", glm::value_ptr(projection));
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		sCheckGLError();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	Shader shader = Shader("shaders/shader.vs", "shaders/shader.fs");
	unsigned int VAO;
	unsigned int VBO;
};

#endif // _SHAPE_RENDER_H_
