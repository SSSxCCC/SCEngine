#include "physics/PhysicsWorld.h"
#include "core/GameWorld.h"

void PhysicsWorld::onCreate() {
	b2Vec2 gravity = b2Vec2(0.0f, -10.0f);
	mWorld = new b2World(gravity);
	mGameObject->mGameWorld->mPhysicsWorld = std::dynamic_pointer_cast<PhysicsWorld>(shared_from_this());
}

void PhysicsWorld::onUpdate() {
	int velocityIterations = 8;
	int positionIterations = 3;
	mWorld->Step(mGameObject->mGameWorld->mDeltaTime, velocityIterations, positionIterations);
}

void PhysicsWorld::onDestroy() {
	if (mGameObject->mGameWorld->mPhysicsWorld == shared_from_this()) {
		mGameObject->mGameWorld->mPhysicsWorld = nullptr;
	}
	delete mWorld;
	mWorld = nullptr;
}
