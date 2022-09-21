#include "core/Transform.h"

ScriptData Transform::getData() {
	auto data = Script::getData();
	data.addFloat("mPosX", mPosX);
	data.addFloat("mPosY", mPosY);
	data.addFloat("mZ", mZ);
	data.addFloat("mRotation", mRotation);
	data.addFloat("mScaleX", mScaleX);
	data.addFloat("mScaleY", mScaleY);
	return std::move(data);
}

void Transform::setData(const ScriptData& data) {
	mPosX = data.getFloat("mPosX");
	mPosY = data.getFloat("mPosY");
	mZ = data.getFloat("mZ");
	mRotation = data.getFloat("mRotation");
	mScaleX = data.getFloat("mScaleX");
	mScaleY = data.getFloat("mScaleY");
}

glm::mat4 Transform::buildModelMatrix() {
	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(mPosX, mPosY, mZ));
	model = glm::rotate(model, mRotation, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(mScaleX, mScaleY, 1.0f));
	return model;
}
