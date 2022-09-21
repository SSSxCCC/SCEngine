#ifndef _Transform_H_
#define _Transform_H_

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "core/Script.h"

// Keep a GameObject's position, rotation and scale. Every GameObject must have a Transform
class Transform : public Script {
public:
	float mPosX, mPosY, mZ;
	float mRotation; // this is radians, use glm::degrees(mRotation) to get degree
	float mScaleX, mScaleY;
	std::string getName() override { return "Transform"; }
	ScriptData getData() override;
	void setData(const ScriptData& data) override;
	glm::mat4 buildModelMatrix();
};
REGISTER_SCRIPT(Transform)

#endif // _Transform_H_
