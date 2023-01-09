#ifndef _RectangleCollider2D_H_
#define _RectangleCollider2D_H_

#include "physics2d/Collider2D.h"

class RectangleCollider2D : public Collider2D {
public:
	SCRIPT_BODY(RectangleCollider2D)
	void onCreate() override;
private:
	b2PolygonShape mRectangleShape;
};

REGISTER_SCRIPT(RectangleCollider2D)

#endif // _RectangleCollider2D_H_
