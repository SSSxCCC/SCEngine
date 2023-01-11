#ifndef _Transform2D_H_
#define _Transform2D_H_

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "sc/core/Script.h"

namespace sc {

// Keep a GameObject's position, rotation and scale. Every GameObject must have a Transform
class Transform2D : public Script {
public:
	SCRIPT_BODY(Transform2D)
	glm::vec3 mPosition;
	float mRotation; // this is radians, use glm::degrees(mRotation) to get degree
    glm::vec2 mScale;
	ScriptData getData() override;
	void setData(const ScriptData& data) override;
	glm::mat4 buildModelMatrix();
};
REGISTER_SCRIPT(Transform2D)

#endif // _Transform2D_H_

} // namespace sc
