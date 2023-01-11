#ifndef _GameObjectData_H_
#define _GameObjectData_H_

#include "sc/data/ScriptData.h"

namespace sc {

// Store all the data in a GameObject
struct GameObjectData {
	int id;
	std::string name;
	std::vector<ScriptData> scriptsData;

	// indicate how this GameObject was edited, editor use only
	EditType editType = EditType::None;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GameObjectData, id, name, scriptsData)

} // namespace sc

#endif // _GameObjectData_H_
