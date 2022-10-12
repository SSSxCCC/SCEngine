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
                modify |= scriptData.set("mPosX", position[0]); modify |= scriptData.set("mPosY", position[1]); modify |= scriptData.set("mZ", position[2]);

				float rotation = glm::degrees(scriptData.get<float>("mRotation"));
				ImGui::DragFloat("rotation", &rotation);
                modify |= scriptData.set("mRotation", glm::radians(rotation));

				float scale[2] = { scriptData.get<float>("mScaleX"), scriptData.get<float>("mScaleY") };
				ImGui::DragFloat2("scale", scale);
                modify |= scriptData.set("mScaleX", scale[0]); modify |= scriptData.set("mScaleY", scale[1]);
			} else {
				for (const auto& dataName : scriptData.dataList) {
					int type = scriptData.getType(dataName);
					if (type == TYPE_INT) {
						int intData = scriptData.get<int>(dataName);
						auto limit = scriptData.getLimit(dataName);
						if (auto enumLimit = std::dynamic_pointer_cast<EnumLimit>(limit)) {
							auto it = std::find(enumLimit->mEnumValues.begin(), enumLimit->mEnumValues.end(), intData);
							int index = it == enumLimit->mEnumValues.end() ? 0 : (it - enumLimit->mEnumValues.begin());
							ImGui::Combo(dataName.c_str(), &index, [](void* data, int idx, const char** out_text) {
								auto enumNames = (std::vector<std::string>*) data;
								*out_text = (*enumNames)[idx].c_str();
								return true;
							}, &enumLimit->mEnumNames, enumLimit->mEnumNames.size());
                            modify |= scriptData.set(dataName, enumLimit->mEnumValues[index]);
						} else {
							ImGui::InputInt(dataName.c_str(), &intData);
                            modify |= scriptData.set(dataName, intData);
						}
					} else if (type == TYPE_FLOAT) {
						float floatData = scriptData.get<float>(dataName);
						ImGui::InputFloat(dataName.c_str(), &floatData);
                        modify |= scriptData.set(dataName, floatData);
					} else if (type == TYPE_STRING) {
						std::string stringData = scriptData.get<std::string>(dataName);
						ImGui::InputText(dataName.c_str(), &stringData);
                        modify |= scriptData.set(dataName, stringData);
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
