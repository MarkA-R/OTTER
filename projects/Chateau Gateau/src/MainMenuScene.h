#pragma once
#include "Scene.h"
#include "globalHeaders.h"
class MainMenuScene :
    public Scene
{
protected:
     entt::entity sceneCamera;

public:
    MainMenuScene();
    void Update(float);
    void Setup();
    void setMainCamera(entt::entity);
    entt::entity getMainCamera();
};

