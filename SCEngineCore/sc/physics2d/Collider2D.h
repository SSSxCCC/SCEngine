#ifndef _Collider2D_H_
#define _Collider2D_H_

#include "box2d/box2d.h"
#include "sc/core/Script.h"

namespace sc {

class Collider2D : public Script {
public:
	b2FixtureDef mFixtureDef;
};

} // namespace sc

#endif // _Collider2D_H_
