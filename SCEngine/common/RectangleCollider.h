#ifndef _RectangleCollider_H_
#define _RectangleCollider_H_

#include "common/Collider.h"

class RectangleCollider : public Collider {
public:
	void onCreate() override;
	std::shared_ptr<Script> clone() override;
private:
	b2PolygonShape mRectangleShape;
};

#endif // _RectangleCollider_H_
