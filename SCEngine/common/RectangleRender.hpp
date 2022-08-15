#ifndef _RectangleRender_H_
#define _RectangleRender_H_

#include "glad/gl.h"
#include "core/Script.hpp"

class RectangleRender : public Script {
public:
	void onCreate() override {
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		float vertices[] = {
			// ---- vertex ----   ---- color ----
			1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
			1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
			-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
			-1.0f, 1.0f, 0.0f,   1.0f, 1.0f, 1.0f,
		};
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		unsigned int indices[] = {
			0, 1, 2,
			0, 2, 3,
		};
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		sCheckGLError();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		mGameObject->mTransform.mScaleX = 30.f;
		mGameObject->mTransform.mScaleY = 30.f;
	}

	void onUpdate() override {
		mGameObject->mTransform.mDegree = mGameObject->mGameWorld->mCurrentTime * 100.0f;
		onDraw();
	}

	void onDestroy() override {
		shader.destroy();
	}

	std::shared_ptr<Script> clone() override {
		return std::make_shared<RectangleRender>();
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
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glDrawArrays(GL_TRIANGLES, 0, 3);

		sCheckGLError();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	Shader shader = Shader("shaders/shader.vs", "shaders/shader.fs");
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
};

#endif // _RectangleRender_H_
