#ifndef _RectangleCollider_H_
#define _RectangleCollider_H_

#include "common/Collider.hpp"

class RectangleCollider : public Collider {
public:
	void onCreate() override {
		mRectangleShape.SetAsBox(mGameObject->mTransform.mScaleX * 0.5f, mGameObject->mTransform.mScaleY * 0.5f);

		mFixtureDef.shape = &mRectangleShape;
		mFixtureDef.density = 1.0f;
		mFixtureDef.friction = 0.3f;
		mFixtureDef.restitution = 1.0f;
	}

	std::shared_ptr<Script> clone() override {
		return std::make_shared<RectangleCollider>();
	}

private:
	b2PolygonShape mRectangleShape;
};

#endif // _RectangleCollider_H_
