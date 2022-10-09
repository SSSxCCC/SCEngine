#include "core/Transform.h"

ScriptData Transform::getData() {
	auto data = Script::getData();
	data.add("mPosX", mPosX);
	data.add("mPosY", mPosY);
	data.add("mZ", mZ);
	data.add("mRotation", mRotation);
	data.add("mScaleX", mScaleX);
	data.add("mScaleY", mScaleY);
	return data;
}

void Transform::setData(const ScriptData& data) {
	mPosX = data.get<float>("mPosX");
	mPosY = data.get<float>("mPosY");
	mZ = data.get<float>("mZ");
	mRotation = data.get<float>("mRotation");
	mScaleX = data.get<float>("mScaleX");
	mScaleY = data.get<float>("mScaleY");
}

glm::mat4 Transform::buildModelMatrix() {
	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(mPosX, mPosY, mZ));
	model = glm::rotate(model, mRotation, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(mScaleX, mScaleY, 1.0f));
	return model;
}
