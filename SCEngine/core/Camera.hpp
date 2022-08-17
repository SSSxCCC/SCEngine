#ifndef _Camera_H_
#define _Camera_H_

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "core/Script.hpp"

int gCameraX = 0;
int gCameraY = 0;

class Camera {
public:
	Camera(int width, int height) { setSize(width, height); }
	void getSize(int& outWidth, int& outHeight) { outWidth = mHalfWidth * 2; outHeight = mHalfHeight * 2; }
	void setSize(int width, int height) { mHalfWidth = width / 2; mHalfHeight = height / 2; }
	void move(float xOffset, float yOffset) { mCenterX += xOffset; mCenterY += yOffset; }
	void zoomIn() { mZoom /= 1.1f; }
	void zoomOut() { mZoom *= 1.1f; }
	void reset() { mCenterX = 0.0f; mCenterY = 0.0f; mZoom = defaultZoom; }

	glm::mat4 buildProjectionMatrix() {
		glm::vec3 cameraPos = glm::vec3(mCenterX, mCenterY, 3.0f);
		glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glm::mat4 projection = glm::ortho((float)-mHalfWidth * mZoom, (float)mHalfWidth * mZoom, (float)-mHalfHeight * mZoom, (float)mHalfHeight * mZoom, -1000.0f, 1000.0f);
		return projection * view;
	}

	void worldToScreen(float worldX, float worldY, float& screenX, float& screenY) {
		float left = mCenterX - (float)mHalfWidth * mZoom;
		float top = mCenterY + (float)mHalfHeight * mZoom;
		screenX = (worldX - left) / mZoom;
		screenY = (top - worldY) / mZoom;
	}

	void screenToWorld(float screenX, float screenY, float& worldX, float& worldY) {
		float left = mCenterX - (float)mHalfWidth * mZoom;
		float top = mCenterY + (float)mHalfHeight * mZoom;
		worldX = left + screenX * mZoom;
		worldY = top - screenY * mZoom;
	}

	void onUpdate() {
		if (gInput.mScrollY > 0.0f) {
			zoomIn();
		} else if (gInput.mScrollY < 0.0f) {
			zoomOut();
		}

		if (gInput.getKey(GLFW_KEY_HOME) == GLFW_PRESS) {
			reset();
		}

		gCameraX = 0;
		gCameraY = 0;
		if (gInput.getKey(GLFW_KEY_LEFT) == GLFW_PRESS || gInput.getKey(GLFW_KEY_A) == GLFW_PRESS) {
			std::cout << "111" << std::endl;
			gCameraX -= 1;
		}
		if (gInput.getKey(GLFW_KEY_RIGHT) == GLFW_PRESS || gInput.getKey(GLFW_KEY_D) == GLFW_PRESS) {
			std::cout << "222" << std::endl;
			gCameraX += 1;
		}
		if (gInput.getKey(GLFW_KEY_UP) == GLFW_PRESS || gInput.getKey(GLFW_KEY_W) == GLFW_PRESS) {
			std::cout << "333" << std::endl;
			gCameraY += 1;
		}
		if (gInput.getKey(GLFW_KEY_DOWN) == GLFW_PRESS || gInput.getKey(GLFW_KEY_S) == GLFW_PRESS) {
			std::cout << "444" << std::endl;
			gCameraY -= 1;
		}
	}

	const float moveSpeed = 1000.0f;
	const float defaultZoom = 1.0f / gScale;


private:
	int mHalfWidth, mHalfHeight;
	float mCenterX = 0.0f, mCenterY = 0.0f;
	float mZoom = defaultZoom;
} gCamera(800 * gScale, 600 * gScale);

#endif // _Camera_H_
