#ifndef _SubWindow_H_
#define _SubWindow_H_

#include <string>
#include "imgui/imgui.h"

class SubWindow {
public:
	SubWindow(const std::string& title);
	~SubWindow();
	void update();
	void bind();
	void unbind();
	bool isFocus() { return mFocus; }
	int getWidth() { return mWidth; }
	int getHeight() { return mHeight; }
	ImVec2 getCursorScreenPos() { return mCursorScreenPos; }
	unsigned int getTexture() { return mTexture; }
private:
	std::string mTitle;
	bool mFocus;
	int mWidth;
	int mHeight;
	ImVec2 mCursorScreenPos;

	unsigned int mTexture;
	unsigned int mFbo;
	unsigned int mRbo;

	void updateFrameBuffer();
};

#endif // _SubWindow_H_
