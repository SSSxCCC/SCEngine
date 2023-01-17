#ifndef _SceneEditor_H_
#define _SceneEditor_H_

#include "sc/data/SceneData.h"

namespace sc {

struct WindowInfo {
	int width;
	int height;
	float menuBarHeight;
};

class SceneEditor {
public:
	void doFrame(SceneData& sceneData, const WindowInfo& windowInfo);
};

} // namespace sc

#endif // _SceneEditor_H_
