#ifndef _SCEngine_H_
#define _SCEngine_H_

#include "common/OpenGLPointer.h"
#include "common/CallbackPointer.h"
#include "data/GameWorldData.h"

extern "C" __declspec(dllexport) void init(OpenGLPointer& openGLPointer, CallbackPointer& callbackPointer);

extern "C" __declspec(dllexport) GameWorldData& doFrame(bool editorMode);

extern "C" __declspec(dllexport) void doEditorFrame(bool focus, int with, int height, float cursorOffsetX, float cursorOffsetY);

extern "C" __declspec(dllexport) void doGameFrame(bool focus, int with, int height, float cursorOffsetX, float cursorOffsetY);

extern "C" __declspec(dllexport) void runGame();

extern "C" __declspec(dllexport) void stopGame();

extern "C" __declspec(dllexport) void save();

extern "C" __declspec(dllexport) void load();

extern "C" __declspec(dllexport) void close();

#endif // _SCEngine_H_
