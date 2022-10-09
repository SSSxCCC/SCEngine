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
				float position[3] = { scriptData.get<float>("mPosX"), scriptData.get<float>("mPosY"), scriptData.get<float>("mZ") };
				ImGui::DragFloat3("position", position);
				if (scriptData.get<float>("mPosX") != position[0] || scriptData.get<float>("mPosY") != position[1] || scriptData.get<float>("mZ") != position[2]) {
					scriptData.set("mPosX", position[0]); scriptData.set("mPosY", position[1]); scriptData.set("mZ", position[2]);
					modify = true;
				}

				float rotation = glm::degrees(scriptData.get<float>("mRotation"));
				ImGui::DragFloat("rotation", &rotation);
				if (glm::degrees(scriptData.get<float>("mRotation")) != rotation) {
					scriptData.set("mRotation", glm::radians(rotation));
					modify = true;
				}

				float scale[2] = { scriptData.get<float>("mScaleX"), scriptData.get<float>("mScaleY") };
				ImGui::DragFloat2("scale", scale);
				if (scriptData.get<float>("mScaleX") != scale[0] || scriptData.get<float>("mScaleY") != scale[1]) {
					scriptData.set("mScaleX", scale[0]); scriptData.set("mScaleY", scale[1]);
					modify = true;
				}
			} else {
				for (const auto& dataName : scriptData.dataList) {
					int type = scriptData.getType(dataName);
					if (type == TYPE_FLOAT) {
						float floatData = scriptData.get<float>(dataName);
						ImGui::InputFloat(dataName.c_str(), &floatData);
						if (scriptData.get<float>(dataName) != floatData) {
							scriptData.set(dataName, floatData);
							modify = true;
						}
					} else if (type == TYPE_INT) {
						int intData = scriptData.get<int>(dataName);
						ImGui::InputInt(dataName.c_str(), &intData);
						if (scriptData.get<int>(dataName) != intData) {
							scriptData.set(dataName, intData);
							modify = true;
						}
					} else if (type == TYPE_STRING) {
						std::string stringData = scriptData.get<std::string>(dataName);
						ImGui::InputText(dataName.c_str(), &stringData);
						if (scriptData.get<std::string>(dataName) != stringData) {
							scriptData.set(dataName, stringData);
							modify = true;
						}
					} else assert(false);
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
