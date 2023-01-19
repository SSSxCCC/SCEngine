#include <jni.h>
#include "SCEngineClient.h"

#include <game-activity/GameActivity.cpp>
#include <game-text-input/gametextinput.cpp>

extern "C" {

#include <game-activity/native_app_glue/android_native_app_glue.c>

/*!
 * Handles commands sent to this Android application
 * @param app the app the commands are coming from
 * @param cmd the command to handle
 */
static void handleCmd(android_app *app, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW: {
            // A new window is created, associate a renderer with it. You may replace this with a
            // "game" class if that suits your needs. Remember to change all instances of userData
            // if you change the class here as a reinterpret_cast is dangerous this in the
            // android_main function and the APP_CMD_TERM_WINDOW handler case.
            app->userData = new sc::SCEngineClient(app);
            break;
        }
        case APP_CMD_WINDOW_RESIZED: {
            auto* client = reinterpret_cast<sc::SCEngineClient*>(app->userData);
            client->framebufferResized();
            break;
        }
        case APP_CMD_TERM_WINDOW: {
            // The window is being destroyed. Use this to clean up your userData to avoid leaking
            // resources.
            if (app->userData) {
                auto* client = reinterpret_cast<sc::SCEngineClient*>(app->userData);
                app->userData = nullptr;
                delete client;
            }
            break;
        }
        default: break;
    }
}

/*!
 * This the main entry point for a native activity
 */
void android_main(struct android_app *app) {
    // Can be removed, useful to ensure your code is running
    aout << "Welcome to android_main" << std::endl;

    // register an event handler for Android events
    app->onAppCmd = handleCmd;

    // This sets up a typical game/event loop. It will run until the app is destroyed.
    int events;
    android_poll_source *pollSource;
    do {
        // Process all pending events before running game logic.
        if (ALooper_pollAll(0, nullptr, &events, (void **) &pollSource) >= 0) {
            if (pollSource) {
                pollSource->process(app, pollSource);
            }
        }

        // Check if any user data is associated. This is assigned in handleCmd
        if (app->userData) {
            auto* client = reinterpret_cast<sc::SCEngineClient*>(app->userData);
            client->doFrame();
        }
    } while (!app->destroyRequested);
}

} // extern "C"
