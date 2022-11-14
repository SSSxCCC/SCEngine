#ifndef _PhysicsWorld2D_H_
#define _PhysicsWorld2D_H_

#include "box2d/box2d.h"
#include "core/Script.h"

class PhysicsWorld2D : public Script {
public:
	void onCreate() override;
	void onUpdate() override;
	void onDestroy() override;
	std::string getName() override { return "PhysicsWorld2D"; }
	b2World* mWorld;
	int mFps = 120;
private:
	float mLastStepTime = 0.0f;
};
REGISTER_SCRIPT(PhysicsWorld2D)

#endif // _PhysicsWorld2D_H_
