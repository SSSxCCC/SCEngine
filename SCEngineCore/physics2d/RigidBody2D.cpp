#include "physics2d/RigidBody2D.h"
#include "physics2d/Collider2D.h"
#include "core/GameObject.h"
#include "core/Scene.h"
#include "core/Transform2D.h"

void RigidBody2D::onCreate() {
	auto transform2D = mGameObject->getScript<Transform2D>();
	if (transform2D) {
		mBodyDef.position.Set(transform2D->mPosition.x, transform2D->mPosition.y);
		mBodyDef.angle = transform2D->mRotation;
	}
	mBody = mGameObject->mScene->mPhysicsWorld2D->mWorld->CreateBody(&mBodyDef);
}

void RigidBody2D::onStart() {
	for (const auto& collider : mGameObject->getScripts<Collider2D>()) {
		mBody->CreateFixture(&collider->mFixtureDef);
	}
}

void RigidBody2D::onUpdate() {
	auto transform2D = mGameObject->getScript<Transform2D>();
	if (transform2D) {
		b2Vec2 position = mBody->GetPosition();
		float angle = mBody->GetAngle();
		transform2D->mPosition.x = position.x;
		transform2D->mPosition.y = position.y;
		transform2D->mRotation = angle;
	}
}

void RigidBody2D::onDestroy() {
	mGameObject->mScene->mPhysicsWorld2D->mWorld->DestroyBody(mBody);
	mBody = nullptr;
}

void RigidBody2D::setData(const ScriptData& data) {
	mBodyDef.type = static_cast<b2BodyType>(data.get<int>("mBodyDef.type"));
}

ScriptData RigidBody2D::getData() {
	ScriptData data = Script::getData();
	std::vector<std::string> enumNames = { "static", "kinematic", "dynamic" };
	std::vector<int> enumValues = { b2BodyType::b2_staticBody, b2BodyType::b2_kinematicBody, b2BodyType::b2_dynamicBody };
	data.add<int>("mBodyDef.type", mBodyDef.type, std::make_shared<EnumLimit>(enumNames, enumValues));
	return data;
}
