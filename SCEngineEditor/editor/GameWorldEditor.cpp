#include "editor/GameWorldEditor.h"
#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"
#include "glm/glm.hpp"
#include <string>

void GameWorldEditor::doFrame(GameWorldData& gameWorldData) {
	auto itemsGetter = [](void* data, int idx, const char** out_text) {
		auto worldData = (std::vector<GameObjectData>*) data;
		auto& gameObject = (*worldData)[idx];
		*out_text = gameObject.name.c_str();
		return true;
	};
	ImGui::Begin("GameWorld");
	static int current = 0;
	ImGui::ListBox("GameObjects", &current, itemsGetter, (void*)&gameWorldData.gameObjectsData, gameWorldData.gameObjectsData.size());
	ImGui::End();

	if (current >= 0) {
		auto& gameObjectData = gameWorldData.gameObjectsData[current];
		ImGui::Begin("GameObject");
		std::string name = gameObjectData.name;
		ImGui::InputText("name", &name);
		if (gameObjectData.name != name) {
			gameObjectData.name = name;
			gameObjectData.editType = EditType::Modify;
			gameWorldData.editType = EditType::Modify;
		}

		ImGui::Text(("Id: " + std::to_string(gameObjectData.id)).c_str());

		for (auto& scriptData : gameObjectData.scriptsData) {
			ImGui::Text("--------------------------------------------------");
			ImGui::Text(scriptData.name.c_str());
			bool modify = false;
			if (scriptData.name == "Transform") {
				float position[3] = { scriptData.getFloat("mPosX"), scriptData.getFloat("mPosY"), scriptData.getFloat("mZ") };
				ImGui::DragFloat3("position", position);
				if (scriptData.getFloat("mPosX") != position[0] || scriptData.getFloat("mPosY") != position[1] || scriptData.getFloat("mZ") != position[2]) {
					scriptData.setFloat("mPosX", position[0]); scriptData.setFloat("mPosY", position[1]); scriptData.setFloat("mZ", position[2]);
					modify = true;
				}

				float rotation = glm::degrees(scriptData.getFloat("mRotation"));
				ImGui::DragFloat("rotation", &rotation);
				if (glm::degrees(scriptData.getFloat("mRotation")) != rotation) {
					scriptData.setFloat("mRotation", glm::radians(rotation));
					modify = true;
				}

				float scale[2] = { scriptData.getFloat("mScaleX"), scriptData.getFloat("mScaleY") };
				ImGui::DragFloat2("scale", scale);
				if (scriptData.getFloat("mScaleX") != scale[0] || scriptData.getFloat("mScaleY") != scale[1]) {
					scriptData.setFloat("mScaleX", scale[0]); scriptData.setFloat("mScaleY", scale[1]);
					modify = true;
				}
			} else {
				for (const auto& dataName : scriptData.dataList) {
					if (scriptData.floatData.contains(dataName)) {
						float floatData = scriptData.getFloat(dataName);
						ImGui::InputFloat(dataName.c_str(), &floatData);
						if (scriptData.getFloat(dataName) != floatData) {
							scriptData.setFloat(dataName, floatData);
							modify = true;
						}
					} else if (scriptData.intData.contains(dataName)) {
						int intData = scriptData.getInt(dataName);
						ImGui::InputInt(dataName.c_str(), &intData);
						if (scriptData.getInt(dataName) != intData) {
							scriptData.setInt(dataName, intData);
							modify = true;
						}
					} else if (scriptData.stringData.contains(dataName)) {
						std::string stringData = scriptData.getString(dataName);
						ImGui::InputText(dataName.c_str(), &stringData);
						if (scriptData.getString(dataName) != stringData) {
							scriptData.setString(dataName, stringData);
							modify = true;
						}
					} else {
						assert(false);
					}
				}
			}
			if (modify) {
				scriptData.editType = EditType::Modify;
				gameObjectData.editType = EditType::Modify;
				gameWorldData.editType = EditType::Modify;
			}
		}

		ImGui::End();
	}
}
