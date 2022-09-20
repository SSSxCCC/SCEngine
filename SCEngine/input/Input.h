#ifndef _Input_H_
#define _Input_H_

// handle input in game
class Input {
public:
	void reset() {
		mScrollX = 0.0f;
		mScrollY = 0.0f;
	}

	int getKey(int key) {
		//return mFocus ? glfwGetKey(mWindow, key) : GLFW_RELEASE;
		return 0;
	}

	int getMouseButton(int button) {
		//return mFocus ? glfwGetMouseButton(mWindow, button) : GLFW_RELEASE;
		return 0;
	}

	void getScroll(float& scrollX, float& scrollY) {
		scrollX = mFocus ? mScrollX : 0;
		scrollY = mFocus ? mScrollY : 0;
	}

	void getCursorPosition(float& x, float& y) {
		//double xpos, ypos;
		//glfwGetCursorPos(mWindow, &xpos, &ypos);
		//x = xpos - mCursorOffsetX;
		//y = ypos - mCursorOffsetY;
		x = 0; y = 0;
	}

	void setFocus(bool focus) {
		mFocus = focus;
	}

	void setScroll(float scrollX, float scrollY) {
		mScrollX = scrollX;
		mScrollY = scrollY;
	}

	void setCursorOffset(float offsetX, float offsetY) {
		mCursorOffsetX = offsetX;
		mCursorOffsetY = offsetY;
	}
private:
	bool mFocus;
	float mScrollX, mScrollY;
	float mCursorOffsetX, mCursorOffsetY;
};

extern Input gInput, gEditorInput;

#endif // _Input_H_
