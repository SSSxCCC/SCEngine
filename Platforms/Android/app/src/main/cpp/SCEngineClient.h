#ifndef _SCEngineClient_H_
#define _SCEngineClient_H_

#include "AndroidOut.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include "SCEngine.h"
#include "sc_platform/PlatformImpl.h"
#include "sc/graphics/VulkanManager.h"
#include "sc/common/CallbackPointer.h"
#include "sc/input/InputConstants.h"

namespace sc {

    class SCEngineClient {
    public:
        SCEngineClient(android_app* androidApp): mAndroidApp(androidApp) {
            initCallbackPointer();
            initVulkan();
            initEngine();
        }

        ~SCEngineClient() {
            delete mEngine;
            delete mVulkanManager;
            delete mPlatform;
        }

        void doFrame() {
            // Process game input
            handleInput();

            int width, height;
            mPlatform->getSurfaceSize(width, height);

            mEngine->update(false);
            auto commandBuffer = mVulkanManager->preDrawFrame();
            mEngine->draw(true, static_cast<uint32_t>(width), static_cast<uint32_t>(height), 0, 0, commandBuffer, false);
            mVulkanManager->postDrawFrame();
        }

        void framebufferResized() {
            mVulkanManager->framebufferResized();
        }

    private:
        android_app* mAndroidApp;
        Platform* mPlatform;
        VulkanManager* mVulkanManager;
        CallbackPointer mCallbackPointer;
        IEngine* mEngine;

        void initCallbackPointer() {
            mCallbackPointer.mGetKey = [=](int key) {
                return RELEASE;
            };
            mCallbackPointer.mGetMouseButton = [=](int button) {
                return RELEASE;
            };
            mCallbackPointer.mGetCursorPos = [=](float& x, float& y) {
                x = 0; y = 0;
            };
        }

        void initVulkan() {
            mPlatform = PlatformImpl::build(mAndroidApp);
            mVulkanManager = new VulkanManager(mPlatform, false);
        }

        struct membuf : std::streambuf {
            membuf(char* begin, char* end) {
                this->setg(begin, begin, end);
            }
        };

        void initEngine() {
            mEngine = scCreate(mPlatform, mVulkanManager, mCallbackPointer, "");
            auto scene = mPlatform->readFile("Scenes/Scene.json");
            membuf scenebuf(scene.data(), scene.data() + sizeof(char) * scene.size());
            std::istream i(&scenebuf);

            nlohmann::json j;
            i >> j;

            mEngine->load(j);
        }

        void handleInput() {
            // handle all queued inputs
            for (auto i = 0; i < mAndroidApp->motionEventsCount; i++) {

                // cache the current event
                auto &motionEvent = mAndroidApp->motionEvents[i];

                // cache the current action
                auto action = motionEvent.action;

                // Find the pointer index, mask and bitshift to turn it into a readable value
                auto pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)
                        >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
                aout << "Pointer " << pointerIndex << ":";

                // get the x and y position of this event
                auto &pointer = motionEvent.pointers[pointerIndex];
                auto x = GameActivityPointerAxes_getX(&pointer);
                auto y = GameActivityPointerAxes_getY(&pointer);
                aout << "(" << x << ", " << y << ") ";

                // Only consider touchscreen events, like touches
                auto actionMasked = action & AINPUT_SOURCE_TOUCHSCREEN;

                // determine the kind of event it is
                switch (actionMasked) {
                    case AMOTION_EVENT_ACTION_DOWN:
                    case AMOTION_EVENT_ACTION_POINTER_DOWN:
                        aout << "Pointer Down";
                        break;

                    case AMOTION_EVENT_ACTION_UP:
                    case AMOTION_EVENT_ACTION_POINTER_UP:
                        aout << "Pointer Up";
                        break;

                    default:
                        aout << "Pointer Move";
                }

                aout << std::endl;
            }

            // clear inputs, be careful as this will clear it for anyone listening to these events
            android_app_clear_motion_events(mAndroidApp);
        }
    };

} // namespace sc

#endif // _SCEngineClient_H_
