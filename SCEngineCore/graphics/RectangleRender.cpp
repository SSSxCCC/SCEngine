#include "graphics/RectangleRender.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "core/GameObject.h"
#include "core/GameWorld.h"
#include "core/Transform2D.h"

void RectangleRender::onCreate() {
	float vertices[] = {
		// ---- vertex ----   ---- color ----
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, 0.0f,   1.0f, 1.0f, 1.0f,
	};
}

void RectangleRender::onDraw(float* projectionMatrix, bool forEditor) {
	
}

void RectangleRender::onDestroy() {
	
}
