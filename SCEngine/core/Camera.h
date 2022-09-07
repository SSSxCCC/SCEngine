#ifndef _Camera_H_
#define _Camera_H_

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "core/Script.h"
#include "utility/Window.h"

// Predefine GameWorld here to solve circular reference problem
class GameWorld;

class Camera : public Script {
public:
	void onCreate() override;
	void onDestroy() override;
	void getSize(int& outWidth, int& outHeight) { outWidth = mHalfWidth * 2; outHeight = mHalfHeight * 2; }
	void setSize(int width, int height) { mHalfWidth = width / 2; mHalfHeight = height / 2; }
	glm::mat4 buildProjectionMatrix();
	void worldToScreen(float worldX, float worldY, float& screenX, float& screenY);
	void screenToWorld(float screenX, float screenY, float& worldX, float& worldY);
	std::string getName() override { return "Camera"; }
	void setData(const ScriptData& data) override;
	ScriptData getData() override;
	int mHalfWidth, mHalfHeight;
	float mZoom = 1.0f / gScale;
private:
	bool mEditor; // Am I the editor camera?
};

REGISTER_SCRIPT(Camera)

#endif // _Camera_H_
