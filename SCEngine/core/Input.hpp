#ifndef _Input_H_
#define _Input_H_

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
} gInput;

#endif // _Input_H_
