#ifndef _RigidBody2D_H_
#define _RigidBody2D_H_

#include "box2d/box2d.h"
#include "core/script.h"

class RigidBody2D : public Script {
public:
	SCRIPT_BODY(RigidBody2D)
	void onCreate() override;
	void onStart() override;
	void onUpdate() override;
	void onDestroy() override;
	void setData(const ScriptData& data) override;
	ScriptData getData() override;

	b2BodyDef mBodyDef;
private:
	b2Body* mBody = nullptr;
};

REGISTER_SCRIPT(RigidBody2D)

#endif // _RigidBody2D_H_
