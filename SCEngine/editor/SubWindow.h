#ifndef _SubWindow_H_
#define _SubWindow_H_

#include <string>

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
	unsigned int getTexture() { return mTexture; }
private:
	std::string mTitle;
	bool mFocus;
	int mWidth;
	int mHeight;
	unsigned int mTexture;
	unsigned int mFbo;
	unsigned int mRbo;
	void updateFrameBuffer();
};

#endif // _SubWindow_H_
