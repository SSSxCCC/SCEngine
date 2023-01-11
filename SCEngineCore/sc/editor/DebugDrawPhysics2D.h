#ifndef _DebugDrawPhysics2D_H_
#define _DebugDrawPhysics2D_H_

#include "sc/physics2d/PhysicsWorld2D.h"

namespace sc {

class DebugDraw;

class DebugDrawPhysics2D : public Script {
public:
	SCRIPT_BODY(DebugDrawPhysics2D)
	void onCreate() override;
	void onDraw(const DrawData& drawData) override;
	void onDestroy() override;
private:
	DebugDraw* mDebugDraw;
	std::shared_ptr<PhysicsWorld2D> mPhysicsWorld2D;
};
REGISTER_SCRIPT(DebugDrawPhysics2D)

} // namespace sc

#endif // _PhysicsDebugDraw_H_
