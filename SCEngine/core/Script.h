#ifndef _Script_H_
#define _Script_H_

#include <memory>

// Predefine GameObject here to solve circular reference problem
class GameObject;



// Script is a script attached to a GameObject to implement game logic
class Script : public std::enable_shared_from_this<Script> {
public:
	virtual void onCreate() { }  // called when the GameObject is created
	virtual void onStart() { }   // called before the first onUpdate call
	virtual void onUpdate() { }  // called every frame
	virtual void onDraw() { }    // called every frame after onUpdate
	virtual void onDestroy() { } // called when the GameObject is destroyed

	virtual std::shared_ptr<Script> clone() = 0; // create a copy of this Script

	// the GameObject this Script attached to.
	// Note: there is circular reference between Script and GameObject!
	std::shared_ptr<GameObject> mGameObject;

	bool mStarted = false; // have onStart called?
};

#endif // _Script_H_
