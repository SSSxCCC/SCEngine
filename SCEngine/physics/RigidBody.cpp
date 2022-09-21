#include "physics/RigidBody.h"
#include "physics/Collider.h"
#include "core/GameObject.h"
#include "core/GameWorld.h"

void RigidBody::onCreate() {
	auto transform = mGameObject->getScript<Transform>();
	if (transform) {
		mBodyDef.position.Set(transform->mPosX, transform->mPosY);
		mBodyDef.angle = transform->mRotation;
	}
	mBody = mGameObject->mGameWorld->mPhysicsWorld->mWorld->CreateBody(&mBodyDef);
}

void RigidBody::onStart() {
	for (const auto& collider : mGameObject->getScripts<Collider>()) {
		mBody->CreateFixture(&collider->mFixtureDef);
	}
}

void RigidBody::onUpdate() {
	auto transform = mGameObject->getScript<Transform>();
	if (transform) {
		b2Vec2 position = mBody->GetPosition();
		float angle = mBody->GetAngle();
		transform->mPosX = position.x;
		transform->mPosY = position.y;
		transform->mRotation = angle;
	}
}

void RigidBody::onDestroy() {
	mGameObject->mGameWorld->mPhysicsWorld->mWorld->DestroyBody(mBody);
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
