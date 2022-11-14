#include "physics2d/RectangleCollider2D.h"
#include "core/GameObject.h"
#include "core/Transform2D.h"

void RectangleCollider2D::onCreate() {
	auto transform2D = mGameObject->getScript<Transform2D>();
	mRectangleShape.SetAsBox((transform2D ? transform2D->mScale.x : 1.0f) * 0.5f, (transform2D ? transform2D->mScale.y : 1.0f) * 0.5f);

	mFixtureDef.shape = &mRectangleShape;
	mFixtureDef.density = 1.0f;
	mFixtureDef.friction = 0.3f;
	mFixtureDef.restitution = 1.0f;
}
