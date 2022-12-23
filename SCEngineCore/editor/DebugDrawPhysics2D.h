#ifndef _DebugDrawPhysics2D_H_
#define _DebugDrawPhysics2D_H_

#include "physics2d/PhysicsWorld2D.h"

class DebugDraw;

class DebugDrawPhysics2D : public Script {
public:
	void onCreate() override;
	void onDraw(const DrawData& drawData) override;
	void onDestroy() override;
	std::string getName() override { return "DebugDrawPhysics2D"; }
private:
	DebugDraw* mDebugDraw;
	std::shared_ptr<PhysicsWorld2D> mPhysicsWorld2D;
};
REGISTER_SCRIPT(DebugDrawPhysics2D)

#endif // _PhysicsDebugDraw_H_
