#ifndef _RectangleCollider2D_H_
#define _RectangleCollider2D_H_

#include "physics2d/Collider2D.h"

class RectangleCollider2D : public Collider2D {
public:
	void onCreate() override;
	std::string getName() override { return "RectangleCollider2D"; }
private:
	b2PolygonShape mRectangleShape;
};

REGISTER_SCRIPT(RectangleCollider2D)

#endif // _RectangleCollider2D_H_
