#ifndef _CallbackPointer_H_
#define _CallbackPointer_H_

#include <functional>

namespace sc {

// Store callback function pointers
// We can't use function pointer here because 'a lambda can only be converted to a function pointer if it does not capture'
class CallbackPointer {
public:
    // Call by core module.
    std::function<int(int)> mGetKey;
    std::function<int(int)> mGetMouseButton;
    std::function<void(float&, float&)> mGetCursorPos;

    // Assign by core module
    std::function<void(double,double)> mScrollCallback;

    // We must reset these std::funtion before we destroy core module in SCEngineEditor, because std::funtion will free previous function when assign to new function, and program will crash if previous function has been destroyed with core mudule.
    void reset() {
        mScrollCallback = nullptr;
    }
};

} // namespace sc

#endif // _CallbackPointer_H_
