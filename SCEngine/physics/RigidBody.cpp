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
	mBodyDef.type = static_cast<b2BodyType>(data.get<int>("mBodyDef.type"));
}

ScriptData RigidBody::getData() {
	ScriptData data = Script::getData();
	std::vector<std::string> enumNames = { "static", "kinematic", "dynamic" };
	std::vector<int> enumValues = { b2BodyType::b2_staticBody, b2BodyType::b2_kinematicBody, b2BodyType::b2_dynamicBody };
	data.add<int>("mBodyDef.type", mBodyDef.type, std::make_shared<EnumLimit>(enumNames, enumValues));
	return data;
}
