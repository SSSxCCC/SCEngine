#ifndef _EditorCameraController_H_
#define _EditorCameraController_H_

#include <memory>
#include "core/Script.h"
#include "core/Camera.h"
#include "utility/Window.h"

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
	float mLastWorldX, mLastWorldY;
	bool mDragging = false;
};

REGISTER_SCRIPT(EditorCameraController)

#endif // _EditorCameraController_H_
