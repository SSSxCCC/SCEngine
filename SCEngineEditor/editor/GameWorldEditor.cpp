/*#include "editor/GameWorldEditor.h"
#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"
#include "glm/glm.hpp"
#include <string>

void GameWorldEditor::update(const std::shared_ptr<GameWorld>& gameWorld) {
	std::vector<std::shared_ptr<GameObject>> data;
	for (int id : gameWorld->mGameObjectIds) {
		data.push_back(gameWorld->mGameObjects[id]);
	}
	auto itemsGetter = [](void* data, int idx, const char** out_text) {
		auto worldData = (std::vector<std::shared_ptr<GameObject>>*) data;
		auto gameObject = (*worldData)[idx];
		*out_text = gameObject->mName.c_str();
		return true;
	};
	ImGui::Begin("GameWorld");
	static int current = 0;
	ImGui::ListBox("GameObjects", &current, itemsGetter, (void*)&data, data.size());
	ImGui::End();

	if (current >= 0) {
		auto gameObject = data[current];
		ImGui::Begin("GameObject");
		std::string name = gameObject->mName;
		ImGui::InputText("name", &name);
		gameObject->mName = name;

		ImGui::Text(("Id: " + std::to_string(gameObject->mId)).c_str());

		ImGui::Text("--------------------------------------------------");

		ImGui::Text("Transform");

		float position[3] = { gameObject->mTransform.mPosX, gameObject->mTransform.mPosY, gameObject->mTransform.mZ };
		ImGui::DragFloat3("position", position);
		gameObject->mTransform.mPosX = position[0]; gameObject->mTransform.mPosY = position[1]; gameObject->mTransform.mZ = position[2];

		float rotation = glm::degrees(gameObject->mTransform.mRotation);
		ImGui::DragFloat("rotation", &rotation);
		gameObject->mTransform.mRotation = glm::radians(rotation);

		float scale[2] = { gameObject->mTransform.mScaleX, gameObject->mTransform.mScaleY };
		ImGui::DragFloat2("scale", scale);
		gameObject->mTransform.mScaleX = scale[0]; gameObject->mTransform.mScaleY = scale[1];

		for (const auto& script : gameObject->mScripts) {
			ImGui::Text("--------------------------------------------------");
			ImGui::Text(script->getName().c_str());
			ScriptData scriptData = script->getData();
			for (const auto& dataName : scriptData.dataList) {
				if (scriptData.floatData.contains(dataName)) {
					float floatData = scriptData.getFloat(dataName);
					ImGui::InputFloat(dataName.c_str(), &floatData);
					scriptData.setFloat(dataName, floatData);
				} else if (scriptData.intData.contains(dataName)) {
					int intData = scriptData.getInt(dataName);
					ImGui::InputInt(dataName.c_str(), &intData);
					scriptData.setInt(dataName, intData);
				} else if (scriptData.stringData.contains(dataName)) {
					std::string stringData = scriptData.getString(dataName);
					ImGui::InputText(dataName.c_str(), &stringData);
					scriptData.setString(dataName, stringData);
				} else {
					assert(false);
				}
			}
			script->setData(scriptData);
		}

		ImGui::End();
	}
}*/
