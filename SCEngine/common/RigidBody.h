#ifndef _RigidBody_H_
#define _RigidBody_H_

#include "box2d/box2d.h"
#include "core/script.h"

class RigidBody : public Script {
public:
	void onCreate() override;
	void onStart() override;
	void onUpdate() override;
	void onDestroy() override;
	std::shared_ptr<Script> clone() override;
	std::string getName() override { return "RigidBody"; }
	b2BodyDef mBodyDef;
private:
	b2Body* mBody = nullptr;
};

REGISTER_SCRIPT(RigidBody)

#endif // _RigidBody_H_
