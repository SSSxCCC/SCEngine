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

	void setWindow(GLFWwindow* window) {
		mWindow = window;
	}

	void setFocus(bool focus) {
		mFocus = focus;
	}

	bool isFocus() {
		return mFocus;
	}

	float mScrollX;
	float mScrollY;
private:
	GLFWwindow* mWindow = nullptr;
	bool mFocus;
};

extern Input gInput, gEditorInput;

#endif // _Input_H_
