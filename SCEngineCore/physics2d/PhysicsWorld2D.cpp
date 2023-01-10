#include "physics2d/PhysicsWorld2D.h"
#include "core/Scene.h"

namespace sc {

void PhysicsWorld2D::onCreate() {
	b2Vec2 gravity = b2Vec2(0.0f, -10.0f);
	mWorld = new b2World(gravity);
	mGameObject->mScene->mPhysicsWorld2D = std::dynamic_pointer_cast<PhysicsWorld2D>(shared_from_this());
}

void PhysicsWorld2D::onUpdate() {
	int velocityIterations = 8;
	int positionIterations = 3;
	float timeStep = 1.0f / mFps;
	if (mGameObject->mScene->mCurrentTime - mLastStepTime >= timeStep) {
		mLastStepTime = mGameObject->mScene->mCurrentTime;
		mWorld->Step(timeStep, velocityIterations, positionIterations);
	}
}

void PhysicsWorld2D::onDestroy() {
	if (mGameObject->mScene->mPhysicsWorld2D == shared_from_this()) {
		mGameObject->mScene->mPhysicsWorld2D = nullptr;
	}
	delete mWorld;
	mWorld = nullptr;
}

} // namespace sc
