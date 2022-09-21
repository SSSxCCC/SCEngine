#include "physics/RectangleCollider.h"
#include "core/GameObject.h"

void RectangleCollider::onCreate() {
	auto transform = mGameObject->getScript<Transform>();
	mRectangleShape.SetAsBox((transform ? transform->mScaleX : 1.0f) * 0.5f, (transform ? transform->mScaleY : 1.0f) * 0.5f);

	mFixtureDef.shape = &mRectangleShape;
	mFixtureDef.density = 1.0f;
	mFixtureDef.friction = 0.3f;
	mFixtureDef.restitution = 1.0f;
}
