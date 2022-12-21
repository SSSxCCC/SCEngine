#ifndef _CallbackPointer_H_
#define _CallbackPointer_H_

#include <functional>

class CallbackPointer {
public:
    // Call by core module. (We can't use function pointer here because 'a lambda can only be converted to a function pointer if it does not capture')
    std::function<int(int)> mGetKey;
    std::function<int(int)> mGetMouseButton;
    std::function<void(float&, float&)> mGetCursorPos;

    // Assign by core module
    using ScrollCallback_fn = void (*)(double, double);
    ScrollCallback_fn mScrollCallback = nullptr;

    void reset() {
        mScrollCallback = nullptr;
    }
};

#endif // _CallbackPointer_H_
