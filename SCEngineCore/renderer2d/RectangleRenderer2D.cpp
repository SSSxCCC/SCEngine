#include "renderer2d/RectangleRenderer2D.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "core/GameObject.h"
#include "core/GameWorld.h"
#include "core/Transform2D.h"

void RectangleRenderer2D::onCreate() {
	float vertices[] = {
		// ---- vertex ----   ---- color ----
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, 0.0f,   1.0f, 1.0f, 1.0f,
	};
}

void RectangleRenderer2D::onDraw(const DrawData& drawData) {
	
}

void RectangleRenderer2D::onDestroy() {
	
}
