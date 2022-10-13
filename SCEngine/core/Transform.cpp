#include "core/Transform.h"

ScriptData Transform::getData() {
	auto data = Script::getData();
	data.add("mPosition", mPosition);
	data.add("mRotation", glm::degrees(mRotation));
	data.add("mScale", mScale);
	return data;
}

void Transform::setData(const ScriptData& data) {
    mPosition = data.get<glm::vec3>("mPosition");
	mRotation = glm::radians(data.get<float>("mRotation"));
	mScale = data.get<glm::vec2>("mScale");
}

glm::mat4 Transform::buildModelMatrix() {
	glm::mat4 model(1.0f);
	model = glm::translate(model, mPosition);
	model = glm::rotate(model, mRotation, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(mScale, 1.0f));
	return model;
}
