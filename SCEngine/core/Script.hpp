#ifndef _SCRIPT_H_
#define _SCRIPT_H_

#include <memory>

// Predefine GameObject here to solve circular reference problem
class GameObject;

// Script is a script attached to a GameObject to implement game logic
class Script {
public:
	Script(std::shared_ptr<GameObject>& gameObject) : mGameObject(gameObject) { }
	virtual void onCreate() { }  // called when the GameObject is created
	virtual void onUpdate() { }  // called every frame
	virtual void onDestroy() { } // called when the GameObject is destroyed

	// the GameObject this Script attached to.
	// Note: there is circular reference between Script and GameObject!
	std::shared_ptr<GameObject> mGameObject;
};

#endif // _SCRIPT_H_
