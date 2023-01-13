#ifndef _Input_H_
#define _Input_H_

#include "sc/input/InputConstants.h"

namespace sc {

// Predefine InputManager here to solve circular reference problem
class InputManager;

// handle input in game
class Input {
public:
	Input(InputManager* inputManager);
	int getKey(int key);
	int getMouseButton(int button);
	void getScroll(float& scrollX, float& scrollY);
	void getCursorPosition(float& x, float& y);

private:
    InputManager* mInputManager;
	bool mFocus;
	float mScrollX, mScrollY;
	float mCursorOffsetX, mCursorOffsetY;

	void setFocus(bool focus);
	void setScroll(float scrollX, float scrollY);
	void setCursorOffset(float offsetX, float offsetY);
	void reset();

friend class InputManager;
friend class Engine;
};

} // namespace sc

#endif // _Input_H_
