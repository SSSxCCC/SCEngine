#include "sc/input/Input.h"
#include "sc/input/InputManager.h"

namespace sc {

Input::Input(InputManager* inputManager) : mInputManager(inputManager) { }

int Input::getKey(int key) {
    return mFocus ? mInputManager->mGetKey(key) : RELEASE;
}

int Input::getMouseButton(int button) {
    return mFocus ? mInputManager->mGetMouseButton(button) : RELEASE;
}

void Input::getScroll(float& scrollX, float& scrollY) {
    scrollX = mFocus ? mScrollX : 0;
    scrollY = mFocus ? mScrollY : 0;
}

void Input::getCursorPosition(float& x, float& y) {
    mInputManager->mGetCursorPos(x, y);
    x -= mCursorOffsetX;
    y -= mCursorOffsetY;
}

void Input::setFocus(bool focus) {
    mFocus = focus;
}

void Input::setScroll(float scrollX, float scrollY) {
    mScrollX = scrollX;
    mScrollY = scrollY;
}

void Input::setCursorOffset(float offsetX, float offsetY) {
    mCursorOffsetX = offsetX;
    mCursorOffsetY = offsetY;
}

void Input::reset() {
    mScrollX = 0.0f;
    mScrollY = 0.0f;
}

} // namespace sc
