#include "sc_editor/SceneEditor.h"
#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"
#include <string>

namespace sc {

void SceneEditor::doFrame(SceneData& sceneData, const WindowInfo& windowInfo) {
	auto itemsGetter = [](void* data, int idx, const char** out_text) {
		auto worldData = (std::vector<GameObjectData>*) data;
		auto& gameObject = (*worldData)[idx];
		*out_text = gameObject.name.c_str();
		return true;
	};
    ImGui::SetNextWindowPos({ 0.f, windowInfo.menuBarHeight }, ImGuiCond_FirstUseEver);
    float sceneHeight = (windowInfo.height - windowInfo.menuBarHeight) * 0.2f;
    ImGui::SetNextWindowSize({ windowInfo.width * 0.2f, sceneHeight }, ImGuiCond_FirstUseEver);
	ImGui::Begin("Scene");
	static int current = 0;
	ImGui::ListBox("GameObjects", &current, itemsGetter, (void*)&sceneData.gameObjectsData, static_cast<int>(sceneData.gameObjectsData.size()));
	ImGui::End();

	if (current >= 0) {
		auto& gameObjectData = sceneData.gameObjectsData[current];
        ImGui::SetNextWindowPos({ 0.f, windowInfo.menuBarHeight + sceneHeight }, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize({ windowInfo.width * 0.2f, windowInfo.height - sceneHeight - windowInfo.menuBarHeight }, ImGuiCond_FirstUseEver);
		ImGui::Begin("GameObject");
        #ifdef SANITIZE
        char str[512];
        for (int i = 0; i < gameObjectData.name.size(); i++) {
            str[i] = gameObjectData.name[i];
        }
        str[gameObjectData.name.size()] = '\0';
        ImGui::InputText("name", str, 512);
        std::string name = str;
        #else
        std::string name = gameObjectData.name;
		ImGui::InputText("name", &name);
        #endif
		if (gameObjectData.name != name) {
			gameObjectData.name = name;
			gameObjectData.editType = EditType::Modify;
			sceneData.editType = EditType::Modify;
		}

		ImGui::Text(("Id: " + std::to_string(gameObjectData.id)).c_str());

		for (auto& scriptData : gameObjectData.scriptsData) {
			ImGui::Text("--------------------------------------------------");
			ImGui::Text(scriptData.name.c_str());
			bool modify = false;
            for (auto& dataName : scriptData.dataList) {
                #ifdef SANITIZE // when delete this, add const to dataName!
                if (dataName.c_str()[dataName.size()]) {
                    dataName += '\0';
                    dataName.pop_back();
                }
                #endif
                int type = scriptData.getType(dataName);
                if (type == TYPE_INT) {
                    auto intData = scriptData.get<int>(dataName);
                    auto limit = scriptData.getLimit(dataName);
                    if (auto enumLimit = std::dynamic_pointer_cast<EnumLimit>(limit)) {
                        auto it = std::find(enumLimit->mEnumValues.begin(), enumLimit->mEnumValues.end(), intData);
                        int index = static_cast<int>(it == enumLimit->mEnumValues.end() ? 0 : (it - enumLimit->mEnumValues.begin()));
                        ImGui::Combo(dataName.c_str(), &index, [](void* data, int idx, const char** out_text) {
                            auto enumNames = (std::vector<std::string>*) data;
                            *out_text = (*enumNames)[idx].c_str();
                            return true;
                        }, &enumLimit->mEnumNames, static_cast<int>(enumLimit->mEnumNames.size()));
                        modify |= scriptData.set(dataName, enumLimit->mEnumValues[index]);
                    } else {
                        ImGui::InputInt(dataName.c_str(), &intData);
                        modify |= scriptData.set(dataName, intData);
                    }
                } else if (type == TYPE_FLOAT) {
                    auto floatData = scriptData.get<float>(dataName);
                    ImGui::DragFloat(dataName.c_str(), &floatData);
                    modify |= scriptData.set(dataName, floatData);
                } else if (type == TYPE_STRING) {
                    auto stringData = scriptData.get<std::string>(dataName);
                    #ifdef SANITIZE
                    char str[512];
                    for (int i = 0; i < stringData.size(); i++) {
                        str[i] = stringData[i];
                    }
                    str[stringData.size()] = '\0';
                    ImGui::InputText(dataName.c_str(), str, 512);
                    stringData = str;
                    #else
                    ImGui::InputText(dataName.c_str(), &stringData);
                    #endif
                    modify |= scriptData.set(dataName, stringData);
                } else if (type == TYPE_VEC2) {
                    auto vecData = scriptData.get<glm::vec2>(dataName);
                    ImGui::DragFloat2(dataName.c_str(), &vecData.x);
                    modify |= scriptData.set(dataName, vecData);
                } else if (type == TYPE_VEC3) {
                    auto vecData = scriptData.get<glm::vec3>(dataName);
                    ImGui::DragFloat3(dataName.c_str(), &vecData.x);
                    modify |= scriptData.set(dataName, vecData);
                } else if (type == TYPE_VEC4) {
                    auto vecData = scriptData.get<glm::vec4>(dataName);
                    ImGui::DragFloat4(dataName.c_str(), &vecData.x);
                    modify |= scriptData.set(dataName, vecData);
                } else throw std::runtime_error("SceneEditor::doFrame unknown ScriptVar data type: " + type);
            }
			if (modify) {
				scriptData.editType = EditType::Modify;
				gameObjectData.editType = EditType::Modify;
				sceneData.editType = EditType::Modify;
			}
		}

		ImGui::End();
	}
}

} // namespace sc
