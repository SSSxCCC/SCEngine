#include "sc/core/Engine.h"
#include "sc/core/Camera.h"
#include "sc/core/Transform2D.h"
#include "sc/core/GameObject.h"
#include "sc/input/Input.h"
#include "sc/editor/DebugDrawPhysics2D.h"
#include "sc/editor/EditorCameraController.h"
#include "sc/physics2d/PhysicsWorld2D.h"
#include "sc/physics2d/RectangleCollider2D.h"
#include "sc/physics2d/RigidBody2D.h"
#include "sc/renderer2d/RectangleRenderer2D.h"
#include <iostream>

namespace sc {

Engine::Engine(Platform* platform, VulkanManager* vulkanManager, CallbackPointer& callbackPointer, const fs::path& assetDir) :
        mPlatform(platform), mVulkanManager(vulkanManager), mCallbackPointer(callbackPointer),
        mAssetManager(new AssetManager(mPlatform, assetDir)), mInputManager(new InputManager(callbackPointer)) {
    std::cout << "Box2D Version " << b2_version.major << "." << b2_version.minor << "." << b2_version.revision << std::endl;
}

Engine::~Engine() {
    mCallbackPointer.reset();
    mScene->destroy();
    mScene = nullptr;
    delete mAssetManager;
    delete mInputManager;
}

void Engine::createEmptyGame() {
    if (mScene) {
        mScene->destroy();
    }

    // Add some GameObject to Scene
    mScene = std::make_shared<Scene>(this);

    auto physicsObject = std::make_shared<GameObject>("PhysicsWorld2D");
    physicsObject->addScript(std::make_shared<PhysicsWorld2D>());
    physicsObject->addScript(std::make_shared<DebugDrawPhysics2D>());
    mScene->addGameObject(physicsObject);

    auto cameraObject = std::make_shared<GameObject>("EditorCamera");
    cameraObject->addScript(std::make_shared<Transform2D>());
    cameraObject->addScript(std::make_shared<Camera>());
    cameraObject->addScript(std::make_shared<EditorCameraController>());
    mScene->addGameObject(cameraObject);

    auto camera2Object = std::make_shared<GameObject>("GameCamera");
    camera2Object->addScript(std::make_shared<Transform2D>());
    camera2Object->addScript(std::make_shared<Camera>());
    mScene->addGameObject(camera2Object);

    auto gameObject = std::make_shared<GameObject>("Box1");
    gameObject->addScript(std::make_shared<Transform2D>());
    auto transform2D = gameObject->getScript<Transform2D>();
    transform2D->mScale.x = 10.f;
    transform2D->mScale.y = 10.f;
    transform2D->mPosition.x = 0.0f;
    transform2D->mPosition.y = 100.0f;
    gameObject->addScript(std::make_shared<RectangleRenderer2D>());
    gameObject->addScript(std::make_shared<RigidBody2D>());
    gameObject->getScript<RigidBody2D>()->mBodyDef.type = b2_dynamicBody;
    gameObject->addScript(std::make_shared<RectangleCollider2D>());
    mScene->addGameObject(gameObject);

    auto gameObject2 = gameObject->clone();
    gameObject2->mName = "Box2";
    transform2D = gameObject2->getScript<Transform2D>();
    transform2D->mPosition.x = 5.0f;
    transform2D->mPosition.y = 120.0f;
    mScene->addGameObject(gameObject2);

    auto groundObject = gameObject->clone();
    groundObject->mName = "Ground";
    groundObject->getScript<RigidBody2D>()->mBodyDef.type = b2_staticBody;
    transform2D = groundObject->getScript<Transform2D>();
    transform2D->mPosition.x = 0.0f;
    transform2D->mPosition.y = -100.0f;
    transform2D->mScale.x = 400.0f;
    transform2D->mScale.y = 80.f;
    mScene->addGameObject(groundObject);

    mScene->create();
    mStartTime = std::chrono::steady_clock::now();
    mSceneData = mScene->getData();
}

void Engine::reloadGame() {
    if (mScene) {
        mScene->destroy();
    }
    mScene = Scene::create(mSceneData, this);
    mScene->create();
    mStartTime = std::chrono::steady_clock::now();
}

SceneData& Engine::update(bool editorMode) {
    if (editorMode) {
        // reloadGame();
        // std::cout << "editType=" << static_cast<typename std::underlying_type<EditType>::type>(sceneData.editType) << std::endl;
        if (mSceneData.editType == EditType::Modify) {
            mScene->setData(mSceneData);
        }  // TODO: hanle other edit type
    } else {
        if (mTempSceneData.editType == EditType::Modify) {
            mScene->setData(mTempSceneData);
        }  // TODO: hanle other edit type
    }

    float currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - mStartTime).count() / 1000000000.0f;
    mScene->mDeltaTime = currentTime - mScene->mCurrentTime;
    mScene->mCurrentTime = currentTime;
    // std::cout << "editorMode=" << editorMode << ", currentTime=" << scene->mCurrentTime << ", deltaTime=" << scene->mDeltaTime << std::endl;

    if (editorMode) {
        mScene->mEditorCamera->mGameObject->update();  // only update EditorCamera in editor mode
    } else {
        mScene->update();
    }

    if (editorMode) {
        mSceneData = mScene->getData();
        return mSceneData;
    } else {
        mTempSceneData = mScene->getData();
        return mTempSceneData;
    }
}

void Engine::draw(bool focus, uint32_t width, uint32_t height, float cursorOffsetX, float cursorOffsetY, VkCommandBuffer commandBuffer, bool forEditor) {
    if (forEditor) {
        mInputManager->mEditorInput.setFocus(focus);
        mInputManager->mEditorInput.setCursorOffset(cursorOffsetX, cursorOffsetY);
        mScene->mEditorCamera->setSize(width, height);
        glm::mat4 projectionView = mScene->mEditorCamera->buildProjectionMatrix();
        mScene->draw({true, commandBuffer, projectionView, {width, height}});
        mInputManager->mEditorInput.reset();
    } else {
        mInputManager->mInput.setFocus(focus);
        mInputManager->mInput.setCursorOffset(cursorOffsetX, cursorOffsetY);
        mScene->mMainCamera->setSize(width, height);
        glm::mat4 projectionView = mScene->mMainCamera->buildProjectionMatrix();
        mScene->draw({false, commandBuffer, projectionView, {width, height}});
        mInputManager->mInput.reset();
    }
}

void Engine::runGame() {
    mSceneData = mScene->getData();
    reloadGame();
}

void Engine::stopGame() {
    reloadGame();
}

nlohmann::json Engine::save() {
    return mScene->getData();
}

void Engine::load(const nlohmann::json& j) {
    if (j.empty()) {
        createEmptyGame();
    } else {
        mSceneData = j;
        reloadGame();
    }
}

} // namespace sc
