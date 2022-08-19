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
	std::shared_ptr<Script> clone() override;

private:
	Shader mShader = Shader("shaders/shader.vs", "shaders/shader.fs");
	unsigned int mVAO;
	unsigned int mVBO;
	unsigned int mEBO;
};

#endif // _RectangleRender_H_
