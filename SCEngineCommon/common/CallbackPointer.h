#ifndef _CallbackPointer_H_
#define _CallbackPointer_H_

#include <functional>

class CallbackPointer {
public:
    // call by core module
    std::function<int(int)> mGetKey;
    std::function<int(int)> mGetMouseButton;
    std::function<void(float&,float&)> mGetCursorPos;

    // assign by core module
    std::function<void(double, double)> mScrollCallback;

    void reset() {
        mScrollCallback = nullptr;
    }
};

#endif // _CallbackPointer_H_
