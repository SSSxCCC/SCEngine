#ifndef _Transform_H_
#define _Transform_H_

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

// Keep a GameObject's position, rotation and scale
class Transform {
public:
	float mPosX, mPosY;
	float mDegree;
	float mScaleX, mScaleY;
	Transform() : mPosX(0.0f), mPosY(0.0f), mDegree(0.0f), mScaleX(0.0f), mScaleY(0.0f) { }
	glm::mat4 buildModelMatrix() {
		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(mPosX, mPosY, 0.0f));
		model = glm::rotate(model, glm::radians(mDegree), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(mScaleX, mScaleY, 1.0f));
		return model;
	}
};

#endif // _Transform_H_
