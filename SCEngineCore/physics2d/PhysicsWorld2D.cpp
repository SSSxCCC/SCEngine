#include "physics2d/PhysicsWorld2D.h"
#include "core/GameWorld.h"

void PhysicsWorld2D::onCreate() {
	b2Vec2 gravity = b2Vec2(0.0f, -10.0f);
	mWorld = new b2World(gravity);
	mGameObject->mGameWorld->mPhysicsWorld2D = std::dynamic_pointer_cast<PhysicsWorld2D>(shared_from_this());
}

void PhysicsWorld2D::onUpdate() {
	int velocityIterations = 8;
	int positionIterations = 3;
	float timeStep = 1.0f / mFps;
	if (mGameObject->mGameWorld->mCurrentTime - mLastStepTime >= timeStep) {
		mLastStepTime = mGameObject->mGameWorld->mCurrentTime;
		mWorld->Step(timeStep, velocityIterations, positionIterations);
	}
}

void PhysicsWorld2D::onDestroy() {
	if (mGameObject->mGameWorld->mPhysicsWorld2D == shared_from_this()) {
		mGameObject->mGameWorld->mPhysicsWorld2D = nullptr;
	}
	delete mWorld;
	mWorld = nullptr;
}
