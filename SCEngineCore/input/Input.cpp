#include "input/Input.h"

Input gInput, gEditorInput;

std::function<int(int)> Input::sGetKey;
std::function<int(int)> Input::sGetMouseButton;
std::function<void(float&,float&)> Input::sGetCursorPos;
