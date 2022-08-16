#ifndef _RigidBody_H_
#define _RigidBody_H_

#include "box2d/box2d.h"
#include "core/script.hpp"
#include "common/Collider.hpp"

class RigidBody : public Script {
public:
	void onCreate() override {
		mBodyDef.position.Set(mGameObject->mTransform.mPosX, mGameObject->mTransform.mPosY);
		mBodyDef.angle = mGameObject->mTransform.mRotation;
		mBody = mGameObject->mGameWorld->mPhysicsWorld.CreateBody(&mBodyDef);
	}

	void onStart() override {
		for (auto collider : mGameObject->getScripts<Collider>()) {
			mBody->CreateFixture(&collider->mFixtureDef);
		}
	}

	void onUpdate() override {
		b2Vec2 position = mBody->GetPosition();
		float angle = mBody->GetAngle();
		mGameObject->mTransform.mPosX = position.x;
		mGameObject->mTransform.mPosY = position.y;
		mGameObject->mTransform.mRotation = angle;
	}

	void onDestroy() override {
		mGameObject->mGameWorld->mPhysicsWorld.DestroyBody(mBody);
		mBody = nullptr;
	}

	std::shared_ptr<Script> clone() override {
		auto newRigidBody = std::make_shared<RigidBody>();
		newRigidBody->mBodyDef = mBodyDef;
		return newRigidBody;
	}

	b2BodyDef mBodyDef;
private:
	b2Body* mBody = nullptr;
};

#endif // _RigidBody_H_
