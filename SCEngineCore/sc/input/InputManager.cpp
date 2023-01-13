#include "sc/input/InputManager.h"

namespace sc {

InputManager::InputManager(CallbackPointer& callbackPointer) : mInput(this), mEditorInput(this),
        mGetKey(callbackPointer.mGetKey), mGetMouseButton(callbackPointer.mGetMouseButton), mGetCursorPos(callbackPointer.mGetCursorPos) {
    callbackPointer.mScrollCallback = [&](double dx, double dy) {
        mEditorInput.setScroll((float)dx, (float)dy);
        mInput.setScroll((float)dx, (float)dy);
    };
}

} // namespace sc
