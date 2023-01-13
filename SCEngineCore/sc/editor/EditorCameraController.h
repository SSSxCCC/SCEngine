#ifndef _EditorCameraController_H_
#define _EditorCameraController_H_

#include <memory>
#include "sc/core/Script.h"
#include "sc/core/Camera.h"
#include "sc/utility/Window.h"
#include "sc/input/Input.h"

namespace sc {

class EditorCameraController : public Script {
public:
	SCRIPT_BODY(EditorCameraController)

	void onCreate() override;

	void zoomIn();
	void zoomOut();
	void reset();

	void onUpdate() override;
	void onDestroy() override;

	void setData(const ScriptData& data) override;
	ScriptData getData() override;

	float mMoveSpeed = 1000.0f;
private:
	std::shared_ptr<Camera> mCamera;
	Input* mEditorInput;
	float mLastWorldX, mLastWorldY;
	bool mDragging = false;
};

REGISTER_SCRIPT(EditorCameraController)

} // namespace sc

#endif // _EditorCameraController_H_
