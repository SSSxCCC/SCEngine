#include "common/RigidBody.h"
#include "common/Collider.h"
#include "core/GameObject.h"
#include "core/GameWorld.h"

void RigidBody::onCreate() {
	mBodyDef.position.Set(mGameObject->mTransform.mPosX, mGameObject->mTransform.mPosY);
	mBodyDef.angle = mGameObject->mTransform.mRotation;
	mBody = mGameObject->mGameWorld->mPhysicsWorld.CreateBody(&mBodyDef);
}

void RigidBody::onStart() {
	for (auto collider : mGameObject->getScripts<Collider>()) {
		mBody->CreateFixture(&collider->mFixtureDef);
	}
}

void RigidBody::onUpdate() {
	b2Vec2 position = mBody->GetPosition();
	float angle = mBody->GetAngle();
	mGameObject->mTransform.mPosX = position.x;
	mGameObject->mTransform.mPosY = position.y;
	mGameObject->mTransform.mRotation = angle;
}

void RigidBody::onDestroy() {
	mGameObject->mGameWorld->mPhysicsWorld.DestroyBody(mBody);
	mBody = nullptr;
}

std::shared_ptr<Script> RigidBody::clone() {
	auto newRigidBody = std::make_shared<RigidBody>();
	newRigidBody->mBodyDef = mBodyDef;
	return newRigidBody;
}
