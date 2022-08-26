#ifndef _RectangleRender_H_
#define _RectangleRender_H_

#include "glad/gl.h"
#include "core/Script.h"
#include "utility/Shader.h"

class RectangleRender : public Script {
public:
	void onCreate() override;
	void onDraw() override;
	void onDestroy() override;
	std::string getName() override { return "RectangleRender"; }
private:
	Shader *mShader;
	unsigned int mVAO;
	unsigned int mVBO;
	unsigned int mEBO;
};

REGISTER_SCRIPT(RectangleRender)

#endif // _RectangleRender_H_
