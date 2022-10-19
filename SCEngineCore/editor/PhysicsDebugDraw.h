#ifndef _PhysicsDebugDraw_H_
#define _PhysicsDebugDraw_H_

#include "physics/PhysicsWorld.h"

class DebugDraw;

class PhysicsDebugDraw : public Script {
public:
	void onCreate() override;
	void onDraw(float* projectionMatrix, bool forEditor = false) override;
	void onDestroy() override;
	std::string getName() override { return "PhysicsDebugDraw"; }
private:
	DebugDraw* mDebugDraw;
	std::shared_ptr<PhysicsWorld> mPhysicsWorld;
};
REGISTER_SCRIPT(PhysicsDebugDraw)

#endif // _PhysicsDebugDraw_H_
