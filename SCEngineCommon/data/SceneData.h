#ifndef _SceneData_H_
#define _SceneData_H_

#include "data/GameObjectData.h"

// Store all the data in a Scene
struct SceneData {
    std::vector<GameObjectData> gameObjectsData;

    // indicate how this Scene was edited, editor use only
    EditType editType = EditType::None;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SceneData, gameObjectsData)

#endif // _SceneData_H_
