#ifndef _PhysicsWorld2D_H_
#define _PhysicsWorld2D_H_

#include "box2d/box2d.h"
#include "core/Script.h"

namespace sc {

class PhysicsWorld2D : public Script {
public:
	SCRIPT_BODY(PhysicsWorld2D)
	void onCreate() override;
	void onUpdate() override;
	void onDestroy() override;
	b2World* mWorld;
	int mFps = 120;
private:
	float mLastStepTime = 0.0f;
};
REGISTER_SCRIPT(PhysicsWorld2D)

} // namespace sc

#endif // _PhysicsWorld2D_H_
