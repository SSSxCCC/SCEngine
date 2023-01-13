#ifndef _InputManager_H_
#define _InputManager_H_

#include "sc/input/Input.h"
#include "sc/common/CallbackPointer.h"
#include <functional>

namespace sc {

class InputManager {
public:
    Input mInput, mEditorInput;
    InputManager(CallbackPointer& callbackPointer);
private:
    std::function<int(int)> mGetKey;
    std::function<int(int)> mGetMouseButton;
    std::function<void(float&,float&)> mGetCursorPos;

friend class Input;
};

} // namespace sc

#endif // _InputManager_H_
