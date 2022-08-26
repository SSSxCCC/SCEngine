#include "editor/GameWorldEditor.h"
#include "imgui/imgui.h"
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
	ImGui::ListBox("", &current, itemsGetter, (void*)&data, data.size());
	ImGui::End();

	if (current >= 0) {
		auto gameObject = data[current];
		ImGui::Begin("GameObject");
		ImGui::Text(gameObject->mName.c_str());

		ImGui::Text(("Id: " + std::to_string(gameObject->mId)).c_str());

		ImGui::Text("--------------------------------------------------");

		ImGui::Text("Transform");
		ImGui::Text(("posX: " + std::to_string(gameObject->mTransform.mPosX) + ", posY: " + std::to_string(gameObject->mTransform.mPosY) + ", z: " + std::to_string(gameObject->mTransform.mZ)).c_str());
		ImGui::Text(("rotation: " + std::to_string(gameObject->mTransform.mRotation)).c_str());
		ImGui::Text(("scaleX: " + std::to_string(gameObject->mTransform.mScaleX) + ", scaleY: " + std::to_string(gameObject->mTransform.mScaleY)).c_str());

		for (const auto& script : gameObject->mScripts) {
			ImGui::Text("--------------------------------------------------");
			ImGui::Text(script->getName().c_str());
			const ScriptData scriptData = script->getData();
			for (const auto& dataName : scriptData.dataList) {
				std::string dataValue;
				if (scriptData.floatData.contains(dataName)) {
					dataValue = std::to_string(scriptData.getFloat(dataName));
				} else if (scriptData.intData.contains(dataName)) {
					dataValue = std::to_string(scriptData.getInt(dataName));
				} else if (scriptData.stringData.contains(dataName)) {
					dataValue = scriptData.getString(dataName);
				} else {
					assert(false);
				}
				ImGui::Text((dataName + ": " + dataValue).c_str());
			}
		}

		ImGui::End();
	}
}
