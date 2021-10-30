#include "MainMenuScene.h"
#include "globalHeaders.h"

GLuint shaderProgram;
MainMenuScene::MainMenuScene() : Scene()
{
	Setup();
}
Shader mainMenuShader;
std::vector<Entity> renderingEntities = std::vector<Entity>();
entt::registry sceneECS = entt::registry();
void MainMenuScene::Update(float deltaTime)
{
	entt::entity sceneCam = getMainCamera();
	sceneECS.get<Camera>(sceneCam).calculateProjection();
	sceneECS.get<Camera>(sceneCam).calculateView();

	Camera c = sceneECS.get<Camera>(sceneCam);

	for each (Entity e in renderingEntities) {
		e.getVAO().Draw(c.getProjection(),c.getViewProjection(),e.getProjection(),mainMenuShader);
	}
	
	

}

void MainMenuScene::Setup()
{
	//Shader vs = Shader("vertex_shader.glsl", GL_VERTEX_SHADER);
	//Shader fs = Shader("frag_shader.glsl", GL_FRAGMENT_SHADER);
	mainMenuShader = Shader("vertex_shader.glsl", "frag_shader.glsl");

	VAO monkeyVAO = VAO();
	monkeyVAO.loadData("Chicken.obj");
	Entity monkey = Entity(sceneECS.create(), monkeyVAO);
	monkey.setPosition(glm::vec3(1));
	monkey.setScale(glm::vec3(0.05f, 0.05f, 0.05f));
	renderingEntities.push_back(monkey);

	Entity cam = Entity(sceneECS.create());

	sceneECS.emplace<Camera>(cam.getEntity());
	setMainCamera(cam.getEntity());
	
	
	sceneECS.get<Camera>(cam.getEntity()).setPosition(glm::vec3(0, 0, 2));
	sceneECS.get<Camera>(cam.getEntity()).lookAt(glm::vec3(0, 0, 0));

}

void MainMenuScene::setMainCamera(entt::entity e)
{
	sceneCamera = e;
}

entt::entity MainMenuScene::getMainCamera()
{
	return sceneCamera;
}
