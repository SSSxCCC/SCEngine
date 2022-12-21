#ifndef _RectangleRender_H_
#define _RectangleRender_H_

#include "core/Script.h"

class RectangleRender : public Script {
public:
	void onCreate() override;
	void onDraw(float* projectionMatrix, bool forEditor) override;
	void onDestroy() override;
	std::string getName() override { return "RectangleRender"; }
};

REGISTER_SCRIPT(RectangleRender)

#endif // _RectangleRender_H_
