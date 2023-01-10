#include "input/Input.h"

namespace sc {

Input gInput, gEditorInput;

std::function<int(int)> Input::sGetKey;
std::function<int(int)> Input::sGetMouseButton;
std::function<void(float&,float&)> Input::sGetCursorPos;

} // namespace sc
