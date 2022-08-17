#ifndef _Collider_H_
#define _Collider_H_

#include "box2d/box2d.h"
#include "core/Script.h"

class Collider : public Script {
public:
	b2FixtureDef mFixtureDef;
};

#endif // _Collider_H_
