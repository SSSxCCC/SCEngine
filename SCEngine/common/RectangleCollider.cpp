#include "common/RectangleCollider.h"
#include "core/GameObject.h"

void RectangleCollider::onCreate() {
	mRectangleShape.SetAsBox(mGameObject->mTransform.mScaleX * 0.5f, mGameObject->mTransform.mScaleY * 0.5f);

	mFixtureDef.shape = &mRectangleShape;
	mFixtureDef.density = 1.0f;
	mFixtureDef.friction = 0.3f;
	mFixtureDef.restitution = 1.0f;
}

std::shared_ptr<Script> RectangleCollider::clone() {
	return std::make_shared<RectangleCollider>();
}
