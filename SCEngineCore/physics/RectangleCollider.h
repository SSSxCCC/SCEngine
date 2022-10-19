#ifndef _RectangleCollider_H_
#define _RectangleCollider_H_

#include "physics/Collider.h"

class RectangleCollider : public Collider {
public:
	void onCreate() override;
	std::string getName() override { return "RectangleCollider"; }
private:
	b2PolygonShape mRectangleShape;
};

REGISTER_SCRIPT(RectangleCollider)

#endif // _RectangleCollider_H_
