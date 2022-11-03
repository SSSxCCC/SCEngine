#ifndef _CallbackPointer_H_
#define _CallbackPointer_H_

class CallbackPointer {
public:
    // call by core module
    using GetKey_fn = int(*)(int);
    using GetMouseButton_fn = int(*)(int);
    using GetCursorPos_fn = void(*)(float&,float&);
    GetKey_fn mGetKey;
    GetMouseButton_fn mGetMouseButton;
    GetCursorPos_fn mGetCursorPos;

    // assign by core module
    using ScrollCallback_fn = void(*)(double,double);
    ScrollCallback_fn mScrollCallback;

    void reset() {
        mScrollCallback = nullptr;
    }
};

#endif // _CallbackPointer_H_
