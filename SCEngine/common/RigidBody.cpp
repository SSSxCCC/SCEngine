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
	for (const auto& collider : mGameObject->getScripts<Collider>()) {
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

void RigidBody::setData(const ScriptData& data) {
	mBodyDef.type = static_cast<b2BodyType>(data.getInt("mBodyDef.type"));
}

ScriptData RigidBody::getData() {
	ScriptData data = Script::getData();
	data.addInt("mBodyDef.type", mBodyDef.type);
	return std::move(data);
}
