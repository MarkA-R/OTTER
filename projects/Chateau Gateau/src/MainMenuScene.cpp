#include "MainMenuScene.h"
#include "globalHeaders.h"

MainMenuScene::MainMenuScene()
{
	Setup();
}

void MainMenuScene::Update(float deltaTime)
{
	Entity cam = Entity(entt::registry().create());
	

	entt::registry().emplace<Camera>(cam.getEntity());
	setMainCamera(cam.getEntity());
	
	VAO monkey = VAO();
	monkey.loadData("monkey.obj");
	monkey.Draw();

}

void MainMenuScene::Setup()
{

}

void MainMenuScene::setMainCamera(entt::entity e)
{
	sceneCamera = e;
}

entt::entity MainMenuScene::getMainCamera()
{
	return sceneCamera;
}
