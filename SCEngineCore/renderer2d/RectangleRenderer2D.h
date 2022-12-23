#ifndef _RectangleRenderer2D_H_
#define _RectangleRenderer2D_H_

#include "core/Script.h"

class RectangleRenderer2D : public Script {
public:
	void onCreate() override;
	void onDraw(const DrawData& drawData) override;
	void onDestroy() override;
	std::string getName() override { return "RectangleRenderer2D"; }
};

REGISTER_SCRIPT(RectangleRenderer2D)

#endif // _RectangleRenderer2D_H_
