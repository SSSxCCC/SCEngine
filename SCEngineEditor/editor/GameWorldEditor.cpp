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
		gameObjectData.name = name;

		ImGui::Text(("Id: " + std::to_string(gameObjectData.id)).c_str());

		for (auto& scriptData : gameObjectData.scriptsData) {
			ImGui::Text("--------------------------------------------------");
			ImGui::Text(scriptData.name.c_str());
			if (scriptData.name == "Transform") {
				float position[3] = { scriptData.getFloat("mPosX"), scriptData.getFloat("mPosY"), scriptData.getFloat("mZ") };
				ImGui::DragFloat3("position", position);
				scriptData.setFloat("mPosX", position[0]); scriptData.setFloat("mPosY", position[1]); scriptData.setFloat("mZ", position[2]);

				float rotation = glm::degrees(scriptData.getFloat("mRotation"));
				ImGui::DragFloat("rotation", &rotation);
				scriptData.setFloat("mRotation", glm::radians(rotation));

				float scale[2] = { scriptData.getFloat("mScaleX"), scriptData.getFloat("mScaleY") };
				ImGui::DragFloat2("scale", scale);
				scriptData.setFloat("mScaleX", scale[0]); scriptData.setFloat("mScaleY", scale[1]);
			} else {
				for (const auto& dataName : scriptData.dataList) {
					if (scriptData.floatData.contains(dataName)) {
						float floatData = scriptData.getFloat(dataName);
						ImGui::InputFloat(dataName.c_str(), &floatData);
						scriptData.setFloat(dataName, floatData);
					}
					else if (scriptData.intData.contains(dataName)) {
						int intData = scriptData.getInt(dataName);
						ImGui::InputInt(dataName.c_str(), &intData);
						scriptData.setInt(dataName, intData);
					}
					else if (scriptData.stringData.contains(dataName)) {
						std::string stringData = scriptData.getString(dataName);
						ImGui::InputText(dataName.c_str(), &stringData);
						scriptData.setString(dataName, stringData);
					}
					else {
						assert(false);
					}
				}
			}
		}

		ImGui::End();
	}
}
