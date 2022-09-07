#ifndef _Transform_H_
#define _Transform_H_

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "nlohmann/json.hpp"

// Keep a GameObject's position, rotation and scale
class Transform {
public:
	float mPosX, mPosY, mZ;
	float mRotation; // this is radians, use glm::degrees(mRotation) to get degree
	float mScaleX, mScaleY;
	Transform() : mPosX(0.0f), mPosY(0.0f), mZ(0.0f), mRotation(0.0f), mScaleX(0.0f), mScaleY(0.0f) { }
	glm::mat4 buildModelMatrix() {
		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(mPosX, mPosY, mZ));
		model = glm::rotate(model, mRotation, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(mScaleX, mScaleY, 1.0f));
		return model;
	}
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Transform, mPosX, mPosY, mZ, mRotation, mScaleX, mScaleY)

#endif // _Transform_H_
