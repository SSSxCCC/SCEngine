#ifndef _Input_H_
#define _Input_H_

#include <functional>
#include "sc/input/InputConstants.h"

namespace sc {

// handle input in game
class Input {
public:
    static std::function<int(int)> sGetKey;
    static std::function<int(int)> sGetMouseButton;
    static std::function<void(float&,float&)> sGetCursorPos;

	void reset() {
		mScrollX = 0.0f;
		mScrollY = 0.0f;
	}

	int getKey(int key) {
		return mFocus ? sGetKey(key) : RELEASE;
	}

	int getMouseButton(int button) {
		return mFocus ? sGetMouseButton(button) : RELEASE;
	}

	void getScroll(float& scrollX, float& scrollY) {
		scrollX = mFocus ? mScrollX : 0;
		scrollY = mFocus ? mScrollY : 0;
	}

	void getCursorPosition(float& x, float& y) {
        sGetCursorPos(x, y);
		x -= mCursorOffsetX;
		y -= mCursorOffsetY;
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

} // namespace sc

#endif // _Input_H_
