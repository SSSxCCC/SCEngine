class Camera {
public:
	Camera(int width, int height) { setSize(width, height); }
	void getSize(int& outWidth, int& outHeight) { outWidth = mHalfWidth * 2; outHeight = mHalfHeight * 2; }
	void setSize(int width, int height) { mHalfWidth = width / 2; mHalfHeight = height / 2; }
	void move(float xOffset, float yOffset) { mCenterX += xOffset; mCenterY += yOffset; }
	void zoomIn() { mZoom /= 1.1f; }
	void zoomOut() { mZoom *= 1.1f; }
	void reset() { mCenterX = 0.0f; mCenterY = 0.0f; mZoom = 1.0f; }
	glm::mat4 buildProjectionMatrix() {
		glm::vec3 cameraPos = glm::vec3(mCenterX, mCenterY, 3.0f);
		glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glm::mat4 projection = glm::ortho((float)-mHalfWidth * mZoom, (float)mHalfWidth * mZoom, (float)-mHalfHeight * mZoom, (float)mHalfHeight * mZoom, -1000.0f, 1000.0f);
		return projection * view;
	}
	const float moveSpeed = 1000.0f;
private:
	int mHalfWidth, mHalfHeight;
	float mCenterX = 0.0f, mCenterY = 0.0f;
	float mZoom = 1.0f;
};

Camera gCamera(800, 600);
int gCameraX = 0;
int gCameraY = 0;
