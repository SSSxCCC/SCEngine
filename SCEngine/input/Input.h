#ifndef _Input_H_
#define _Input_H_

#include "glfw/glfw3.h"

extern bool gGameFocus, gEditorFocus;

// handle input in game
class Input {
public:
	Input(bool editor) : mEditor(editor) { }

	void reset() {
		mScrollX = 0.0f;
		mScrollY = 0.0f;
	}

	int getKey(int key) {
		if ((mEditor && gEditorFocus) || (!mEditor && gGameFocus)) {
			return glfwGetKey(mWindow, key);
		} else {
			return GLFW_RELEASE;
		}
	}

	void setWindow(GLFWwindow* window) {
		mWindow = window;
	}

	float mScrollX;
	float mScrollY;
private:
	GLFWwindow* mWindow = nullptr;
	bool mEditor;
};

extern Input gInput, gEditorInput;

#endif // _Input_H_
