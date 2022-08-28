#ifndef _Input_H_
#define _Input_H_

#include "glfw/glfw3.h"

// handle input in game
class Input {
public:
	float mScrollX;
	float mScrollY;

	void reset() {
		mScrollX = 0.0f;
		mScrollY = 0.0f;
	}

	int getKey(int key) {
		return glfwGetKey(mWindow, key);
	}

	void setWindow(GLFWwindow* window) {
		mWindow = window;
	}
private:
	GLFWwindow* mWindow = nullptr;
};

extern Input gInput, gEditorInput;

extern bool gGameFocus, gEditorFocus;

#endif // _Input_H_
