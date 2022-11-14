#ifndef _Collider2D_H_
#define _Collider2D_H_

#include "box2d/box2d.h"
#include "core/Script.h"

class Collider2D : public Script {
public:
	b2FixtureDef mFixtureDef;
};

#endif // _Collider2D_H_
