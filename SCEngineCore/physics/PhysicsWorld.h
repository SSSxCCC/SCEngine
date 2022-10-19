#ifndef _PhysicsWorld_H_
#define _PhysicsWorld_H_

#include "box2d/box2d.h"
#include "core/Script.h"

class PhysicsWorld : public Script {
public:
	void onCreate() override;
	void onUpdate() override;
	void onDestroy() override;
	std::string getName() override { return "PhysicsWorld"; }
	b2World* mWorld;
	int mFps = 120;
private:
	float mLastStepTime = 0.0f;
};
REGISTER_SCRIPT(PhysicsWorld)

#endif // _PhysicsWorld_H_
