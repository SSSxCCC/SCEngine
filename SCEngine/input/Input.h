#ifndef _Input_H_
#define _Input_H_

#include "glfw/glfw3.h"

// handle input in game
class Input {
public:
	void reset() {
		mScrollX = 0.0f;
		mScrollY = 0.0f;
	}

	int getKey(int key) {
		return mFocus ? glfwGetKey(mWindow, key) : GLFW_RELEASE;
	}

	int getMouseButton(int button) {
		return mFocus ? glfwGetMouseButton(mWindow, button) : GLFW_RELEASE;
	}

	void getScroll(float& scrollX, float& scrollY) {
		scrollX = mFocus ? mScrollX : 0;
		scrollY = mFocus ? mScrollY : 0;
	}

	void getCursorPosition(float& x, float& y) {
		double xpos, ypos;
		glfwGetCursorPos(mWindow, &xpos, &ypos);
		x = xpos - mCursorOffsetX;
		y = ypos - mCursorOffsetY;
	}

	void setWindow(GLFWwindow* window) {
		mWindow = window;
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
	GLFWwindow* mWindow = nullptr;
	bool mFocus;
	float mScrollX, mScrollY;
	float mCursorOffsetX, mCursorOffsetY;
};

extern Input gInput, gEditorInput;

#endif // _Input_H_
