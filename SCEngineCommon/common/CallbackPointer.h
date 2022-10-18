#ifndef _CallbackPointer_H_
#define _CallbackPointer_H_

#include <functional>

class CallbackPointer {
public:
    // assign by core module
    std::function<void(double, double)> mScrollCallback;

    // call by core module
    std::function<int(int)> mGetKey;
    std::function<int(int)> mGetMouseButton;
    std::function<void(float&,float&)> mGetCursorPos;
};

#endif // _CallbackPointer_H_
