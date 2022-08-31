#ifndef _SubWindow_H_
#define _SubWindow_H_

class SubWindow {
public:
	SubWindow();
	~SubWindow();
	void update();
	bool isFocus() { return mFocus; }
	int getWidth() { return mWidth; }
	int getHeight() { return mHeight; }
	unsigned int getTexture() { return mTexture; }
	unsigned int getFbo() { return mFbo; }
private:
	bool mFocus;
	int mWidth;
	int mHeight;
	unsigned int mTexture;
	unsigned int mFbo;
	unsigned int mRbo;
	void updateFrameBuffer();
};

#endif // _SubWindow_H_
