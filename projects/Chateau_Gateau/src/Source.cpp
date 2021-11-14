

#include "NOU/App.h"
#include "NOU/Input.h"
#include "NOU/Entity.h"
#include "NOU/CCamera.h"
#include "NOU/CMeshRenderer.h"
#include "NOU/Shader.h"
#include "NOU/GLTFLoader.h"
#include "CParticleSystem.h"
#include "MaterialCreator.h"
#include "Raycast.h"
#include "BoundingBox.h"
#include "Machine.h"
#include "Fridge.h"
#include "Oven.h"
#include "bakeryUtils.h"
#include "Order.h"
#include "OvenTimer.h"
#include "CMorphMeshRenderer.h"
#include "CMorphAnimator.h"
#include "FillingMachine.h"
#include "ToppingMachine.h"
#include <ctime>


#include <iostream>

#include "imgui.h"

#include <memory>

using namespace nou;

//std::vector<Scene*> gameScenes = std::vector<Scene*>();
int activeScene = 0;

std::unique_ptr<ShaderProgram> prog_texLit, prog_lit, prog_unlit, prog_morph, prog_particles;
std::unique_ptr<Material>  mat_unselected, mat_selected, mat_line;
glm::vec3 cameraPos = glm::vec3(-1.f, -0.5f, -0.7f);
glm::quat cameraQuat = glm::quat();

double xPos, yPos;
double cameraX = 0, cameraY = 0;
double sensitivity = -0.1;

Entity* globalCameraEntity;
GLFWwindow* gameWindow;
bool isClicking = false;
bool isRightClicking = false;
bool isLeftButtonHeld = false;
bool isRightButtonHeld = false;
bool isClickingOne = false;
bool isClickingTwo = false;
bool isClickingThree = false;
bool isClickingFour = false;

//Mouse State
bool firstMouse = true;
int width = 1280;
int height = 720;
float lastX = width / 2;
float lastY = height / 2;
float scrollX;
float scrollY;
std::vector<Entity*> renderingEntities = std::vector<Entity*>();
Transform traySlot[4] = {};

//std::unique_ptr<Entity> trayPastry[4] = {nullptr, nullptr, nullptr, nullptr};
std::vector<Order> orders; //new Mithunan
Entity* trayPastry[4] = {nullptr, nullptr, nullptr, nullptr};
std::vector<Mesh*> fillingFrames = std::vector<Mesh*>();



// Keep our main cleaner
void LoadDefaultResources();
void getCursorData(GLFWwindow* window, double x, double y);
void outputCameraFacing(glm::vec3 cf);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
int getFirstTraySlot();
bool isSlotEmpty(int i);
void setPastryMesh(Entity* e, bakeryUtils::pastryType type);
void setPastryFilling(Entity* e, bakeryUtils::fillType type);
void setPastryTopping(Entity* e, bakeryUtils::toppingType type);
int getWantedSlot();
float getTrayRaise(bakeryUtils::pastryType type);
// Function to handle user inputs
void GetInput();


MaterialCreator crossantMat = MaterialCreator();
MaterialCreator doughMat = MaterialCreator();
MaterialCreator cookieMat = MaterialCreator();
MaterialCreator cupcakeMat = MaterialCreator();

MaterialCreator croissantTile = MaterialCreator();
MaterialCreator doughTile = MaterialCreator();
MaterialCreator cookieTile = MaterialCreator();
MaterialCreator cupcakeTile = MaterialCreator();
MaterialCreator cakeTile = MaterialCreator();
MaterialCreator nothingTile = MaterialCreator();


MaterialCreator custardFilling = MaterialCreator();
MaterialCreator nutellaFilling = MaterialCreator();
MaterialCreator strawberryFilling = MaterialCreator();

MaterialCreator pecanTopping = MaterialCreator();
MaterialCreator sprinkleTopping = MaterialCreator();
MaterialCreator stawberryTopping = MaterialCreator();

std::unique_ptr<Material> pecanParticle;
std::unique_ptr<Material> sprinkleParticle;
std::unique_ptr<Material> strawberryParticle;
//burnt tile???



void log(std::string s) {
	std::cout << s << std::endl;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


int main()
{
	srand(static_cast<unsigned int>(time(0)));
	// Create window and set clear color
	App::Init("Chateau Gateau", width, height);
	App::SetClearColor(glm::vec4(0.0f, 0.27f, 0.4f, 1.0f));
	App::setCursorVisible(false);
	gameWindow = glfwGetCurrentContext();
	//glfwSetInputMode(gameWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	// Initialize ImGui
	App::InitImgui();
	//glfwSetInputMode(gameWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	// Load in our model/texture resources
	LoadDefaultResources();
	glfwSetMouseButtonCallback(gameWindow, mouse_button_callback);
	glfwSetCursorPosCallback(gameWindow, getCursorData);
	glfwSetScrollCallback(gameWindow, scroll_callback);

	//gameScenes.push_back(&GameScene());
	//gameScenes[activeScene]->Setup();

	//glfwSetKeyCallback(gameWindow, key_callback);

	MaterialCreator cursorMat = MaterialCreator();
	cursorMat.createMaterial("UI/cursor.gltf", "UI/cursor.png", *prog_unlit);

	MaterialCreator registerMaterial = MaterialCreator();
	registerMaterial.createMaterial("bakery/models/cashregister.gltf", "bakery/textures/cashregister.png", *prog_texLit);
	
	MaterialCreator counterMat = MaterialCreator();
	counterMat.createMaterial("bakery/models/counter.gltf", "bakery/textures/counter.png", *prog_texLit);

	MaterialCreator trayMat = MaterialCreator();
	trayMat.createMaterial("bakery/models/tray.gltf", "bakery/textures/wood.png", *prog_texLit);

	MaterialCreator fridgeMat = MaterialCreator();
	fridgeMat.createMaterial("bakery/models/fridge.gltf", "bakery/textures/fridge.png", *prog_texLit);

	MaterialCreator ovenMat = MaterialCreator();
	ovenMat.createMaterial("bakery/models/oven.gltf", "bakery/textures/oven.png", *prog_texLit);

	MaterialCreator toppingMat = MaterialCreator();
	toppingMat.createMaterial("bakery/models/topping.gltf", "bakery/textures/topping.png", *prog_texLit);

	MaterialCreator fillingMat1 = MaterialCreator();
	fillingMat1.createMaterial("bakery/models/fillingMachine1.gltf", "bakery/textures/fillingMachine.png", *prog_morph);
	
	MaterialCreator fillingMat2 = MaterialCreator();//for morphs
	fillingMat2.createMaterial("bakery/models/fillingMachine2.gltf", "bakery/textures/fillingMachine.png", *prog_morph);
	fillingFrames.push_back(fillingMat1.getMesh().get());
	fillingFrames.push_back(fillingMat2.getMesh().get());
	
	
	MaterialCreator timerMat = MaterialCreator();
	timerMat.createMaterial("bakery/models/timer.gltf", "bakery/textures/timer.png", *prog_texLit);
	
	MaterialCreator arrowMat = MaterialCreator();
	arrowMat.createMaterial("bakery/models/arrow.gltf", "bakery/textures/arrow.png", *prog_texLit);

	std::unique_ptr<Texture2D> particleTex = std::make_unique<Texture2D>("bakery/textures/particle.png");
	std::unique_ptr<Material> particleMat = std::make_unique<Material>(*prog_particles);
	particleMat->AddTexture("albedo", *particleTex);

	doughMat.createMaterial("bakery/models/dough.gltf", "bakery/textures/dough.png", *prog_texLit);
	crossantMat.createMaterial("bakery/models/croissant.gltf", "bakery/textures/croissant.png", *prog_texLit);
	cookieMat.createMaterial("bakery/models/cookie.gltf", "bakery/textures/cookie.png", *prog_texLit);
	cupcakeMat.createMaterial("bakery/models/cupcake.gltf", "bakery/textures/cupcake.png", *prog_texLit);

	croissantTile.createMaterial("bakery/models/tile.gltf", "bakery/textures/croissantTile.png", *prog_texLit);
	doughTile.createMaterial("bakery/models/tile.gltf", "bakery/textures/doughTile.png", *prog_texLit);
	cookieTile.createMaterial("bakery/models/tile.gltf", "bakery/textures/cookieTile.png", *prog_texLit);
	cupcakeTile.createMaterial("bakery/models/tile.gltf", "bakery/textures/cupcakeTile.png", *prog_texLit);
	cakeTile.createMaterial("bakery/models/tile.gltf", "bakery/textures/cakeTile.png", *prog_texLit);
	nothingTile.createMaterial("bakery/models/tile.gltf", "bakery/textures/nothingTile.png", *prog_texLit);

	custardFilling.createMaterial("bakery/models/tile.gltf", "bakery/textures/custardFilling.png", *prog_texLit);
	nutellaFilling.createMaterial("bakery/models/tile.gltf", "bakery/textures/nutellaFilling.png", *prog_texLit);
	strawberryFilling.createMaterial("bakery/models/tile.gltf", "bakery/textures/strawberryFilling.png", *prog_texLit);

	pecanTopping.createMaterial("bakery/models/tile.gltf", "bakery/textures/pecanTopping.png", *prog_texLit);
	stawberryTopping.createMaterial("bakery/models/tile.gltf", "bakery/textures/strawberryTopping.png", *prog_texLit);
	sprinkleTopping.createMaterial("bakery/models/tile.gltf", "bakery/textures/sprinkleTopping.png", *prog_texLit);


	std::unique_ptr<Texture2D> pecanTex = std::make_unique<Texture2D>("bakery/textures/pecanParticle.png");
	pecanParticle = std::make_unique<Material>(*prog_particles);
	pecanParticle->AddTexture("albedo", *pecanTex);

	std::unique_ptr<Texture2D> strawberryTex = std::make_unique<Texture2D>("bakery/textures/strawberryParticle.png");
	strawberryParticle = std::make_unique<Material>(*prog_particles);
	strawberryParticle->AddTexture("albedo", *strawberryTex);

	std::unique_ptr<Texture2D> sprinkleTex = std::make_unique<Texture2D>("bakery/textures/sprinkleParticle.png");
	sprinkleParticle = std::make_unique<Material>(*prog_particles);
	sprinkleParticle->AddTexture("albedo", *sprinkleTex);

	


	Entity cursor = Entity::Create();
	cursor.Add<CMeshRenderer>(cursor, *cursorMat.getMesh(), *cursorMat.getMaterial());
	cursor.transform.m_scale = glm::vec3(0.001f, 0.001f, 0.001f);
	cursor.transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
	cursor.transform.m_pos = glm::vec3(-1.f, -3.f, -3.0f);
	renderingEntities.push_back(&cursor);

	// Create and set up camera
	Entity cameraEntity = Entity::Create();
	CCamera& cam = cameraEntity.Add<CCamera>(cameraEntity);
	cam.Perspective(60.0f, (float) width/height, 0.1f, 100.0f);
	//cam.Perspective(100.f, (float) width/height, 0.1f, 100.0f);
	cameraEntity.transform.m_pos = cameraPos;
	cameraEntity.transform.m_rotation = glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f));
	globalCameraEntity = &cameraEntity;
	
	
	
		//Creating Cash Register Entity
		Entity ent_register = Entity::Create();
		ent_register.Add<CMeshRenderer>(ent_register, *registerMaterial.getMesh(), *registerMaterial.getMaterial());
		ent_register.transform.m_scale = glm::vec3(0.5f, 0.5f, 0.5f);
		

		
		ent_register.transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
		ent_register.transform.m_pos = glm::vec3(-1.f, -3.f, -3.0f);
		ent_register.Add<BoundingBox>(glm::vec3(0.5, 2.3, 0.06), ent_register);
		renderingEntities.push_back(&ent_register);

		Entity fridge = Entity::Create();
		fridge.Add<CMeshRenderer>(fridge, *fridgeMat.getMesh(), *fridgeMat.getMaterial());
		
		fridge.Add<Machine>();
		fridge.Add<Fridge>();
		fridge.transform.m_scale = glm::vec3(0.5f, 1.f, 0.5f);
		fridge.transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f));
		fridge.transform.m_pos = glm::vec3(-3.f, -1.f, 2.0f);
		fridge.Add<BoundingBox>(glm::vec3(0.67, 4, 0.5), fridge);
		renderingEntities.push_back(&fridge);

		Entity oven = Entity::Create();
		oven.Add<CMeshRenderer>(oven, *ovenMat.getMesh(), *ovenMat.getMaterial());
		
		oven.Add<Machine>();
		oven.Add<Oven>();
		oven.transform.m_scale = glm::vec3(0.4f, 0.4f, 0.4f);
		oven.transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
		oven.transform.m_pos = glm::vec3(1.f, -1.5f, 2.0f);
		oven.Add<BoundingBox>(glm::vec3(0.67, 2, 0.5), oven);
		renderingEntities.push_back(&oven);
	
		Entity filling = Entity::Create();
		filling.Add<CMorphMeshRenderer>(filling, *fillingMat1.getMesh(), *fillingMat1.getMaterial());
		
		filling.Add<Machine>();
		filling.Add<FillingMachine>();
		filling.transform.m_scale = glm::vec3(0.3f, 1.f, 0.3f);
		filling.transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
		filling.transform.m_pos = glm::vec3(0.f, -1.0f, 2.0f);
		filling.Add<BoundingBox>(glm::vec3(0.5, 2, 1), filling);
		renderingEntities.push_back(&filling);

		auto& animator = filling.Add<CMorphAnimator>(filling);
		animator.SetFrameTime(1.0f);
		animator.SetFrames(fillingFrames);


		
		filling.Get<FillingMachine>().setup(&custardFilling,&nutellaFilling,&strawberryFilling);

		Entity fillingPlane = Entity::Create();
		fillingPlane.Add<CMeshRenderer>(fillingPlane, *custardFilling.getMesh(), *custardFilling.getMaterial());
		fillingPlane.transform.m_scale = glm::vec3(0.24f, 0.24f, 0.24f);
		fillingPlane.transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::angleAxis(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f));;
		glm::vec3 fillPos = filling.transform.m_pos;
		fillingPlane.transform.m_pos = glm::vec3(fillPos.x - 0.1 , fillPos.y +0.8, fillPos.z - 0.5);
		renderingEntities.push_back(&fillingPlane);
		filling.Get<FillingMachine>().setFillingPlane(&fillingPlane);

		Transform fillTransform = filling.transform;
		fillTransform.m_pos.y -= 0.2;
		fillTransform.m_pos.x += 0;
		//fillTransform.m_pos.z += 0.3;
		filling.Get<FillingMachine>().setTransform(fillTransform);
		

		//Setting up our particle system.
		ParticleParam particleData;
		particleData.lifetime = 0.8f;
		particleData.startColor = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		particleData.endColor = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
		particleData.startSize = 0.1f;
		particleData.maxParticles = 200;
		particleData.emissionRate = 10.0f;
		particleData.tanTheta = glm::tan(glm::radians(25.0f));



		


		Entity topping = Entity::Create();
		topping.Add<CMeshRenderer>(topping, *toppingMat.getMesh(), *toppingMat.getMaterial());
		
		topping.Add<Machine>();
		topping.Add<ToppingMachine>();
		topping.transform.m_scale = glm::vec3(0.4f, 1.f, 0.4f);
		topping.transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
		topping.transform.m_pos = glm::vec3(-1.2, -1.0f, 2.0f);
		topping.Add<BoundingBox>(glm::vec3(0.4f, 2, 0.5), topping);
		std::cout << topping.Get<BoundingBox>().getOrigin().x << " " <<
			topping.Get<BoundingBox>().getOrigin().y << " " <<
			topping.Get<BoundingBox>().getOrigin().z << std::endl;
		glm::vec3 tempTopPos = topping.transform.m_pos;
		//tempTopPos.x += 0.0;
		//topping.Get<BoundingBox>().setOrigin(tempTopPos);
		std::cout << topping.Get<BoundingBox>().getOrigin().x << " " <<
			topping.Get<BoundingBox>().getOrigin().y << " " <<
			topping.Get<BoundingBox>().getOrigin().z << std::endl;
		renderingEntities.push_back(&topping);

		Transform topParticleTransform = topping.transform;
		topParticleTransform.m_pos.y += 0.11;
		//topParticleTransform.m_pos.x -= 0.05;
		//topParticleTransform.m_pos.z += 1.0;
		topping.Get<ToppingMachine>().setParticleTransform(topParticleTransform);

		Transform toppingTL = topping.transform;
		toppingTL.m_pos.y -= 0.5;
		toppingTL.m_pos.x -= 0.2;

		Transform toppingTR = topping.transform;
		toppingTR.m_pos.y -= 0.5;
		toppingTR.m_pos.x += 0.2;

		topping.Get<ToppingMachine>().setup(&pecanTopping, &sprinkleTopping, &stawberryTopping);
		topping.Get<ToppingMachine>().setTransform(toppingTL, toppingTR);
		glm::vec3 pecanColour = glm::vec3(0.811, 0.647, 0.235);
		glm::vec3 sprinkleColour = glm::vec3(0.654, 0.858, 0.078);
		glm::vec3 strawberryColour = glm::vec3(0.945, 0.254, 0.333);
		topping.Get<ToppingMachine>().setupParticleColours(pecanColour, sprinkleColour, strawberryColour);
		topping.Get<ToppingMachine>().setupParticles(pecanParticle.get(), sprinkleParticle.get(), strawberryParticle.get());
		
			int selectedt = topping.Get<ToppingMachine>().getSelectedNumber();
			particleData.startColor = glm::vec4(topping.Get<ToppingMachine>().getParticleColour(selectedt), 1.f);
			particleData.endColor = glm::vec4(topping.Get<ToppingMachine>().getParticleColour(selectedt), 0.f);;
			
			
		
		
		Entity particleEntity = Entity::Create();
		particleEntity.transform.m_pos = topParticleTransform.m_pos;
		particleEntity.Add<CParticleSystem>(particleEntity, *topping.Get<ToppingMachine>().getParticleMaterial(selectedt), particleData);
		renderingEntities.push_back(&particleEntity);
		

		Entity toppingPlane = Entity::Create();
		toppingPlane.Add<CMeshRenderer>(toppingPlane, *pecanTopping.getMesh(), *pecanTopping.getMaterial());
		toppingPlane.transform.m_scale = glm::vec3(0.24f, 0.24f, 0.24f);
		toppingPlane.transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::angleAxis(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f));;
		glm::vec3 topPos = topping.transform.m_pos;
		toppingPlane.transform.m_pos = glm::vec3(topPos.x - 0.1, topPos.y + 0.8, topPos.z - 0.5);
		renderingEntities.push_back(&toppingPlane);
		topping.Get<ToppingMachine>().setTopPlane(&toppingPlane);


	Entity counter = Entity::Create();
	counter.Add<CMeshRenderer>(counter, *counterMat.getMesh(), *counterMat.getMaterial());
	counter.transform.m_scale = glm::vec3(1.f, 0.5f, 0.5f);
	counter.transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
	counter.transform.m_pos = glm::vec3(-1.f, -3.f, -3.0f);
	renderingEntities.push_back(&counter);

	Entity tray = Entity::Create();
	tray.Add<CMeshRenderer>(tray, *trayMat.getMesh(), *trayMat.getMaterial());
	tray.transform.m_scale = glm::vec3(0.53f, 0.35f, 0.35f);
	tray.transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
	tray.transform.m_pos = glm::vec3(cameraPos.x + 0.2, cameraPos.y, cameraPos.z - 0.45);
	tray.transform.SetParent(&cameraEntity.transform);
	renderingEntities.push_back(&tray);
	traySlot[0] = Transform();
	traySlot[0].m_pos = tray.transform.m_pos;
	traySlot[0].m_pos.x = tray.transform.m_pos.x - 0.15;
	traySlot[0].m_pos.z = tray.transform.m_pos.z - 0.15;
	traySlot[0].SetParent(&tray.transform);
	traySlot[1] = Transform();
	traySlot[1].m_pos = tray.transform.m_pos;
	traySlot[1].m_pos.x = tray.transform.m_pos.x + 0.15;
	traySlot[1].m_pos.z = tray.transform.m_pos.z - 0.15;
	traySlot[1].SetParent(&tray.transform);
	traySlot[2] = Transform();
	traySlot[2].m_pos = tray.transform.m_pos;
	traySlot[2].m_pos.x = tray.transform.m_pos.x - 0.15;
	traySlot[2].m_pos.z = tray.transform.m_pos.z + 0.15;
	traySlot[2].SetParent(&tray.transform);
	traySlot[3] = Transform();
	traySlot[3].m_pos = tray.transform.m_pos;
	traySlot[3].m_pos.x = tray.transform.m_pos.x + 0.15;
	traySlot[3].m_pos.z = tray.transform.m_pos.z + 0.15;
	traySlot[3].SetParent(&tray.transform);

	

	Transform slot1Transform = oven.transform;
	slot1Transform.m_pos.x += 0.95;
	slot1Transform.m_pos.y += 0.55;
	slot1Transform.m_pos.z += 0.6;
	OvenTimer slot1 = OvenTimer(nothingTile,arrowMat, timerMat, slot1Transform);
	renderingEntities.push_back(slot1.getArrow());
	renderingEntities.push_back(slot1.getCircle());
	renderingEntities.push_back(slot1.getTile());

	Transform slot2Transform = oven.transform;
	slot2Transform.m_pos.x += 0.35;
	slot2Transform.m_pos.y += 0.55;
	slot2Transform.m_pos.z += 0.6;
	OvenTimer slot2 = OvenTimer(nothingTile, arrowMat, timerMat, slot2Transform);
	renderingEntities.push_back(slot2.getArrow());
	renderingEntities.push_back(slot2.getCircle());
	renderingEntities.push_back(slot2.getTile());


	Transform slot3Transform = oven.transform;
	slot3Transform.m_pos.x += 0.95;
	slot3Transform.m_pos.y += 0.30;
	slot3Transform.m_pos.z += 0.6;
	OvenTimer slot3 = OvenTimer(nothingTile, arrowMat, timerMat, slot3Transform);
	renderingEntities.push_back(slot3.getArrow());
	renderingEntities.push_back(slot3.getCircle());
	renderingEntities.push_back(slot3.getTile());

	Transform slot4Transform = oven.transform;
	slot4Transform.m_pos.x += 0.35;
	slot4Transform.m_pos.y += 0.30;
	slot4Transform.m_pos.z += 0.6;
	OvenTimer slot4 = OvenTimer(nothingTile, arrowMat, timerMat, slot4Transform);
	renderingEntities.push_back(slot4.getArrow());
	renderingEntities.push_back(slot4.getCircle());
	renderingEntities.push_back(slot4.getTile());


	std::vector<MaterialCreator*> tiles = std::vector<MaterialCreator*>();
	tiles.push_back(&nothingTile);
	tiles.push_back(&doughTile);
	tiles.push_back(&croissantTile);
	tiles.push_back(&cookieTile);
	tiles.push_back(&cupcakeTile);
	tiles.push_back(&cakeTile);

	oven.Get<Oven>().setup(&slot1, &slot2, &slot3, &slot4, &oven.transform);
	oven.Get<Oven>().setMaterials(tiles);
	App::Tick();

	float angle = 0;
	Oven* ovenScript = &oven.Get<Oven>();
	glm::vec3 lastPoint = glm::vec3(-999,-999,-999);
	glm::vec3 currentPoint = glm::vec3(0);
	bool raycastHit = false;
	Entity* hitEntity = nullptr;
	// Main loop
	while (!App::IsClosing() && !Input::GetKeyDown(GLFW_KEY_ESCAPE))
	{
		bool keepCheckingRaycast = true;
		 isClicking = false;
		 isRightClicking = false;
		 int addedSlot = -1;
		 raycastHit = false;
		 
		App::FrameStart();
		
		
		
		GetInput();
		
		

		// Update our LERP timers
		float deltaTime = App::GetDeltaTime();

		ovenScript->update(deltaTime);
		//cameraEntity.transform.m_rotation = glm::angleAxis(glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
		//angle++;
		//std::cout << angle << std::endl;
		//angle += 0.05;
		//slot1.addFill(0.01);
		//slot1.updateArrow();
		// Update camera
		/*
		ImGui::SetNextWindowPos(ImVec2(0, 800), ImGuiCond_FirstUseEver);
		App::StartImgui();
		ImGui::DragFloat("X", &(slot1.getCircle()->transform.m_pos.x), 0.1f);
		ImGui::DragFloat("Y", &(slot1.getCircle()->transform.m_pos.y), 0.1f);
		ImGui::DragFloat("Z", &(slot1.getCircle()->transform.m_pos.z), 0.1f);
		//ImGui::SetWindowPos(0,0);
		
		App::EndImgui();
		*/
		cameraEntity.Get<CCamera>().Update();
		glm::quat cameraRotEuler = cameraQuat;
		glm::vec3 cameraFacingVector = glm::vec3(0);
		glm::vec3 up = glm::vec3(0);
	
		glm::vec3 left = glm::vec3(0);
		
		//https://www.gamedev.net/forums/topic/56471-extracting-direction-vectors-from-quaternion/

		left.x = 1 - 2 * (cameraRotEuler.y * cameraRotEuler.y + cameraRotEuler.z * cameraRotEuler.z);
		left.y = 2 * (cameraRotEuler.x * cameraRotEuler.y + cameraRotEuler.w * cameraRotEuler.z);
		left.z = 2 * (cameraRotEuler.x * cameraRotEuler.z - cameraRotEuler.w * cameraRotEuler.y);
	
		
		up.x = 2 * (cameraRotEuler.x * cameraRotEuler.y - cameraRotEuler.w * cameraRotEuler.z);
		up.y = 1 - 2 * (cameraRotEuler.x * cameraRotEuler.x + cameraRotEuler.z * cameraRotEuler.z);
		up.z = 2 * (cameraRotEuler.y * cameraRotEuler.z + cameraRotEuler.w * cameraRotEuler.x);
	
		cameraFacingVector = glm::cross(left, -up);
		
		//outputCameraFacing(cameraFacingVector); //DEBUG OPTION
		Raycast r = Raycast(cameraEntity.transform.m_pos, cameraFacingVector,10.f);//needs to be after GetInput so cameraQuat is initialized;
		std::vector<glm::vec3> raycastPoints = r.crossedPoints();
		//std::cout << cameraRotEuler.x << " " << cameraRotEuler.y << " " << cameraRotEuler.z << std::endl;
		cursor.transform.m_pos = raycastPoints[5];
		cursor.transform.m_rotation = cameraEntity.transform.m_rotation;
		
		hitEntity = nullptr;
		for each (Entity* e in renderingEntities) {
			
			e->transform.RecomputeGlobal();


			if (e->Has<CMeshRenderer>()) {
				e->Get<CMeshRenderer>().Draw();
			}

			if (e->Has<CMorphMeshRenderer>()) {
				e->Get<CMorphMeshRenderer>().Draw();
			}
			
			
			
			if (keepCheckingRaycast) {
				if (e->Has<BoundingBox>()) {

					for each (glm::vec3	pos in raycastPoints) {

						if (e->Get<BoundingBox>().isColliding(pos)) {
							raycastHit = true;
							hitEntity = e;
							currentPoint = pos;
							if (lastPoint.x == -999) {
								lastPoint = pos;
							}
							keepCheckingRaycast = false;
							break;
						}

					}
				}
			}
					
		}

		if (raycastHit && hitEntity != nullptr) {
			Entity* e = hitEntity;
			if (e->Has<Machine>()) {//check for fridge tomorrow
				if (e->Has<Fridge>()) {
					//log("B");
					//std::cout << "A" << std::endl;
					if (isClicking) {
						int slot = getFirstTraySlot();
						if (slot >= 0) {
							trayPastry[slot] = Entity::Allocate().release();



							trayPastry[slot]->Add<CMeshRenderer>(*trayPastry[slot], *doughMat.getMesh(), *doughMat.getMaterial());
							trayPastry[slot]->Add<Pastry>();

							trayPastry[slot]->transform.m_scale = glm::vec3(0.09f, 0.09f, 0.09f);
							trayPastry[slot]->transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));

							//std::cout << traySlot[slot].m_pos.x << " " << traySlot[slot].m_pos.y << " " << traySlot[slot].m_pos.z << std::endl;

							trayPastry[slot]->transform.m_pos = traySlot[slot].m_pos;
							trayPastry[slot]->transform.SetParent(&globalCameraEntity->transform);
							addedSlot = slot;
							//std::cout << "B" << std::endl;

						}
						else
						{
							std::cout << "Tray full!" << std::endl;
						}


					}
				}
				else if (e->Has<Oven>()) {
					//log("A");
					int wantedSlot = getWantedSlot();
					int addingSlot = wantedSlot;
					if (wantedSlot >= 0) {
						//std::cout << "A" << std::endl;

						bool putInOven = false;
						if (trayPastry[wantedSlot] != nullptr) {
							if (!ovenScript->isSlotFull(wantedSlot)) {
								if (trayPastry[wantedSlot]->Get<Pastry>().getPastryType() == bakeryUtils::pastryType::DOUGH)
								{
									if (!trayPastry[wantedSlot]->Get<Pastry>().isInOven()) {
										putInOven = true;
									}
								}
							}
							else
							{
								int firstEmpty = ovenScript->getFirstEmpty();
								if (firstEmpty >= 0) {
									
									if (trayPastry[wantedSlot]->Get<Pastry>().getPastryType() == bakeryUtils::pastryType::DOUGH)
									{
										if (!trayPastry[wantedSlot]->Get<Pastry>().isInOven()) {
											putInOven = true;
											
											addingSlot = firstEmpty;
										}
									}
								}
								
							}
						}
						if (putInOven) {

							//std::cout << "B" << std::endl;
							ovenScript->canAdd(trayPastry[wantedSlot], addingSlot);

							trayPastry[wantedSlot]->Get<Pastry>().setInOven(true);
							trayPastry[wantedSlot] = nullptr;

						}
						else
						{
							//std::cout << "C" << std::endl;
							int newSlot = getFirstTraySlot();
							//std::cout << newSlot << std::endl;
							
							if (ovenScript->canRemove(wantedSlot) && newSlot >= 0) {
								//std::cout << newSlot << std::endl;
								trayPastry[newSlot] = &ovenScript->getEntity(wantedSlot);

								trayPastry[newSlot]->transform.m_pos = traySlot[newSlot].m_pos;
								trayPastry[newSlot]->transform.m_pos.y += getTrayRaise(trayPastry[newSlot]->Get<Pastry>().getPastryType());
								trayPastry[newSlot]->transform.SetParent(&globalCameraEntity->transform);
								trayPastry[newSlot]->Get<Pastry>().setInOven(false);
								setPastryMesh(trayPastry[newSlot], trayPastry[newSlot]->Get<Pastry>().getPastryType());

								/*
								if (trayPastry[newSlot]->Get<Pastry>().getPastryType() == bakeryUtils::pastryType::DOUGH)
								{
									trayPastry[newSlot]->Get<Pastry>().setCookedSeconds(wantedSlot);//should it reset seconds if not completed?
								}
								*/
								ovenScript->removeFromSlot(wantedSlot);
							}
						}

					}
				}
				else if (e->Has<FillingMachine>()) {
					FillingMachine& fillingScript = e->Get<FillingMachine>();
					if (isLeftButtonHeld) {
						if (fillingScript.isFillingFull()) {
							float yChange = (currentPoint.y - lastPoint.y) * -1.25;//the 1.25 is the shortness scale
							fillingScript.setT(abs(fillingScript.getT() + yChange));
							filling.Get<CMorphAnimator>().setFrameAndTime(0, 1, fillingScript.getT());
							filling.transform.RecomputeGlobal();
							filling.Get<CMorphMeshRenderer>().Draw();

							if (fillingScript.getT() >= 1.0f) {//in case 1.0 wont work lol
								fillingScript.getFromFilling()->Get<Pastry>().setFilling(fillingScript.getFilling());
								//put into tray now
								int wantedSlot = getFirstTraySlot();

								if (wantedSlot >= 0) {
									trayPastry[wantedSlot] = fillingScript.getFromFilling();
									trayPastry[wantedSlot]->transform.m_pos = traySlot[wantedSlot].m_pos;
									trayPastry[wantedSlot]->transform.m_pos.y += getTrayRaise(trayPastry[wantedSlot]->Get<Pastry>().getPastryType());
									trayPastry[wantedSlot]->transform.SetParent(&globalCameraEntity->transform);
									trayPastry[wantedSlot]->Get<Pastry>().setInFilling(false);
									fillingScript.removeFromFilling();
									//UPDATE FILLING HERE
									setPastryFilling(trayPastry[wantedSlot], trayPastry[wantedSlot]->Get<Pastry>().getPastryFilling());
								}
								
							}
							//std::cout << (int)fillingScript.getFromFilling()->Get<Pastry>().getPastryFilling() << std::endl;
						}
						
						
						
						//std::cout << fillingScript.getT() << std::endl;
						
					}
					else if (scrollY != 0) {
						//std::cout << "SCROLLY: " << scrollY << std::endl;
						fillingScript.setT(0);
						filling.Get<CMorphAnimator>().setFrameAndTime(0, 1, 0);
						filling.transform.RecomputeGlobal();
						filling.Get<CMorphMeshRenderer>().Draw();
						fillingScript.addFillNum(scrollY);
						fillingScript.updatePlane();

					}
					else
					{
						int wantedSlot = getWantedSlot();
						if (wantedSlot >= 0) {
							bool putinFill = false;
							if (trayPastry[wantedSlot] != nullptr) {
								if (!fillingScript.isFillingFull()) {
									if (trayPastry[wantedSlot]->Get<Pastry>().getPastryType() != bakeryUtils::pastryType::DOUGH
										&& trayPastry[wantedSlot]->Get<Pastry>().getPastryFilling() == bakeryUtils::fillType::NONE)
									{
										if (!trayPastry[wantedSlot]->Get<Pastry>().isInFilling()) {
											putinFill = true;
										}
									}
								}
							}
							if (putinFill) {

								//std::cout << "B" << std::endl;
								//ovenScript->canAdd(trayPastry[wantedSlot], wantedSlot);
								fillingScript.putInFilling(trayPastry[wantedSlot]);
								trayPastry[wantedSlot]->Get<Pastry>().setInFilling(true);
								trayPastry[wantedSlot] = nullptr;

							}
							else
							{
								//put in tray slot
								//std::cout << "C" << std::endl;
								//int newSlot = wantedSlot();
								//std::cout << newSlot << std::endl;
								if (fillingScript.isFillingFull() && wantedSlot >= 0 && trayPastry[wantedSlot] == nullptr) {
									//std::cout << newSlot << std::endl;
									trayPastry[wantedSlot] = fillingScript.getFromFilling();
									trayPastry[wantedSlot]->transform.m_pos = traySlot[wantedSlot].m_pos;
									trayPastry[wantedSlot]->transform.m_pos.y += getTrayRaise(trayPastry[wantedSlot]->Get<Pastry>().getPastryType());
									trayPastry[wantedSlot]->transform.SetParent(&globalCameraEntity->transform);
									trayPastry[wantedSlot]->Get<Pastry>().setInFilling(false);
									fillingScript.removeFromFilling();
									//dont set texture here cause they just removed it
									
									
								}
							}
						}
						else
						{
							fillingScript.setT(0);
							filling.Get<CMorphAnimator>().setFrameAndTime(0, 1, 0);
							filling.transform.RecomputeGlobal();
							filling.Get<CMorphMeshRenderer>().Draw();
						}
						

					}
					

				}
				else if (e->Has<ToppingMachine>()) {
				ToppingMachine& toppingScript = e->Get<ToppingMachine>();
				
					if (isLeftButtonHeld) {
						if (toppingScript.isToppingFull()) {
							float xChange = (currentPoint.x - lastPoint.x) * 1.5;//the 1.25 is the shortness scale
							toppingScript.setT(abs(toppingScript.getT() + xChange));
							toppingScript.moveTopping(toppingScript.getT());
							toppingScript.addDistance(xChange);
							//std::cout << toppingScript.getDistance() << std::endl;
							if (toppingScript.getDistance() >= 5.0f) {//in case 1.0 wont work lol
								toppingScript.getFromTopping()->Get<Pastry>().setTopping(toppingScript.getTopping());
								//put into tray now
								int wantedSlot = getFirstTraySlot();

								if (wantedSlot >= 0) {
									trayPastry[wantedSlot] = toppingScript.getFromTopping();
									trayPastry[wantedSlot]->transform.m_pos = traySlot[wantedSlot].m_pos;
									trayPastry[wantedSlot]->transform.SetParent(&globalCameraEntity->transform);
									trayPastry[wantedSlot]->transform.m_pos.y += getTrayRaise(trayPastry[wantedSlot]->Get<Pastry>().getPastryType());

									trayPastry[wantedSlot]->Get<Pastry>().setInTopping(false);
									toppingScript.removeFromTopping();
									//UPDATE FILLING HERE
									setPastryTopping(trayPastry[wantedSlot], trayPastry[wantedSlot]->Get<Pastry>().getPastryTopping());
								}

							}
							//std::cout << (int)fillingScript.getFromFilling()->Get<Pastry>().getPastryFilling() << std::endl;
						}
					}
					else if (scrollY != 0) {
						toppingScript.addTopNum(scrollY);
						toppingScript.updatePlane();
						
						int selected = toppingScript.getSelectedNumber();
						particleData.startColor = glm::vec4(toppingScript.getParticleColour(selected), 1.f);
						particleData.endColor = glm::vec4(toppingScript.getParticleColour(selected), 0.f);;
						//particleEntity
						particleEntity.Remove<CParticleSystem>();
						particleEntity.Add<CParticleSystem>(particleEntity, *toppingScript.getParticleMaterial(selected), particleData);
					}
					else
					{
						int wantedSlot = getWantedSlot();
						if (wantedSlot >= 0) {
							bool putinTop = false;
							if (trayPastry[wantedSlot] != nullptr) {
								if (!toppingScript.isToppingFull()) {
									if (trayPastry[wantedSlot]->Get<Pastry>().getPastryType() != bakeryUtils::pastryType::DOUGH
										&& trayPastry[wantedSlot]->Get<Pastry>().getPastryTopping() == bakeryUtils::toppingType::NONE)
									{
										if (!trayPastry[wantedSlot]->Get<Pastry>().isInTopping()) {
											putinTop = true;
										}
									}
								}
							}
							if (putinTop) {

								//std::cout << "B" << std::endl;
								//ovenScript->canAdd(trayPastry[wantedSlot], wantedSlot);
								toppingScript.putInTopping(trayPastry[wantedSlot]);
								trayPastry[wantedSlot]->Get<Pastry>().setInTopping(true);
								trayPastry[wantedSlot] = nullptr;

							}
							else
							{
								//put in tray slot
								//std::cout << "C" << std::endl;
								//int newSlot = wantedSlot();
								//std::cout << newSlot << std::endl;
								if (toppingScript.isToppingFull() && wantedSlot >= 0 && trayPastry[wantedSlot] == nullptr) {
									//std::cout << newSlot << std::endl;
									trayPastry[wantedSlot] = toppingScript.getFromTopping();
									trayPastry[wantedSlot]->transform.m_pos = traySlot[wantedSlot].m_pos;
									trayPastry[wantedSlot]->transform.m_pos.y += getTrayRaise(trayPastry[wantedSlot]->Get<Pastry>().getPastryType());

									trayPastry[wantedSlot]->transform.SetParent(&globalCameraEntity->transform);
									trayPastry[wantedSlot]->Get<Pastry>().setInTopping(false);
									toppingScript.removeFromTopping();
									//dont set texture here cause they just removed it


								}
							}
						}
					}
					if (toppingScript.isToppingFull())
					{
						particleEntity.transform.RecomputeGlobal();
						particleEntity.Get<CParticleSystem>().Update(deltaTime);

						glDisable(GL_DEPTH_TEST);
						particleEntity.Get<CParticleSystem>().Draw();
						glEnable(GL_DEPTH_TEST);
					}
				
				}




			}
		}
		

		//std::cout << bakeryUtils::returnBakeTime(bakeryUtils::pastryType::CROISSANT) << std::endl;
	
		if (addedSlot >= 0) {
			renderingEntities.push_back(trayPastry[addedSlot]);
		}


		//std::cout << (currentPoint.x - lastPoint.x) << " " << (currentPoint.y - lastPoint.y) << " " << (currentPoint.z - lastPoint.z) << std::endl;
		lastPoint = currentPoint;
		scrollX = 0;
		scrollY = 0;
		// Draw everything we queued up to the screen
		App::SwapBuffers();
	}
	std::fill(std::begin(trayPastry), std::end(trayPastry), nullptr);
	//std::fill(std::begin(trayPastry), std::end(trayPastry), nullptr);
	// Destroy window
	App::Cleanup();

	return 0;
}

void LoadDefaultResources()
{
	// Load in the shaders we will need and activate them
	// Textured lit shader
	std::unique_ptr vs_texLitShader = std::make_unique<Shader>("shaders/texturedlit.vert", GL_VERTEX_SHADER);
	std::unique_ptr fs_texLitShader = std::make_unique<Shader>("shaders/texturedlit.frag", GL_FRAGMENT_SHADER);
	std::vector<Shader*> texLit = { vs_texLitShader.get(), fs_texLitShader.get() };
	prog_texLit = std::make_unique<ShaderProgram>(texLit);

	// Untextured lit shader
	std::unique_ptr vs_litShader = std::make_unique<Shader>("shaders/lit.vert", GL_VERTEX_SHADER);
	std::unique_ptr fs_litShader = std::make_unique<Shader>("shaders/lit.frag", GL_FRAGMENT_SHADER);
	std::vector<Shader*> lit = { vs_litShader.get(), fs_litShader.get() };
	prog_lit = std::make_unique<ShaderProgram>(lit);

	// Untextured unlit shader
	std::unique_ptr vs_unlitShader = std::make_unique<Shader>("shaders/unlit.vert", GL_VERTEX_SHADER);
	std::unique_ptr fs_unlitShader = std::make_unique<Shader>("shaders/unlit.frag", GL_FRAGMENT_SHADER);
	std::vector<Shader*> unlit = { vs_unlitShader.get(), vs_unlitShader.get() };
	prog_unlit = std::make_unique<ShaderProgram>(unlit);

	auto v_morph = std::make_unique<Shader>("shaders/morph.vert", GL_VERTEX_SHADER);
	auto f_lit = std::make_unique<Shader>("shaders/lit.frag", GL_FRAGMENT_SHADER);
	
	//Billboarded particles shader program.
	auto v_particles = std::make_unique<Shader>("shaders/particles.vert", GL_VERTEX_SHADER);
	auto g_particles = std::make_unique<Shader>("shaders/particles.geom", GL_GEOMETRY_SHADER);
	auto f_particles = std::make_unique<Shader>("shaders/particles.frag", GL_FRAGMENT_SHADER);

	std::vector<Shader*> particles = { v_particles.get(), g_particles.get(), f_particles.get() };
	prog_particles = std::make_unique<ShaderProgram>(particles);



	std::vector<Shader*> morph = { v_morph.get(), f_lit.get() };
	prog_morph = std::make_unique<ShaderProgram>(morph);

	// Set up point and line materials
	mat_unselected = std::make_unique<Material>(*prog_lit);
	mat_unselected->m_color = glm::vec3(0.5f, 0.5f, 0.5f);

	mat_selected = std::make_unique<Material>(*prog_lit);
	mat_selected->m_color = glm::vec3(1.0f, 0.0f, 0.0f);

	mat_line = std::make_unique<Material>(*prog_unlit);
	mat_line->m_color = glm::vec3(1.0f, 1.0f, 1.0f);




}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		isClickingOne = true;
	}
	else
	{
		isClickingOne = false;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		isClicking = true;
		isLeftButtonHeld = true;
	}
	else
	{
		isLeftButtonHeld = false;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		isRightClicking = true;
		isRightButtonHeld = true;
	}
	else
	{
		isRightButtonHeld = false;
	}
	
}

void getCursorData(GLFWwindow* window, double x, double y) {

	//globalCameraEntity->transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f));;
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	double deltaX = (x - xPos) * sensitivity;
	double deltaY = (y - yPos) * sensitivity;
	deltaX += cameraX;
	deltaY += cameraY;
	

	//BELOW FROM https://gamedev.stackexchange.com/questions/13436/glm-euler-angles-to-quaternion
	glm::quat QuatAroundX = glm::angleAxis(glm::radians((float)deltaY), glm::vec3(1.0, 0.0, 0.0));
	glm::quat QuatAroundY = glm::angleAxis(glm::radians((float)deltaX), glm::vec3(0.0, 1.0, 0.0));
	glm::quat QuatAroundZ = glm::angleAxis(glm::radians(0.f), glm::vec3(0.0, 0.0, 1.0));
	glm::quat finalOrientation = QuatAroundY * QuatAroundX * QuatAroundZ;
	cameraQuat = finalOrientation;
	globalCameraEntity->transform.m_rotation = finalOrientation;

	globalCameraEntity->transform.m_pos = cameraPos;


	glfwSetCursorPos(gameWindow, width/2, height/2);
	cameraX = deltaX;
	cameraY = deltaY;
	
}

void GetInput()
{
	
	double xRot, yRot;

	//glfwSetInputMode(gameWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	
		glfwGetCursorPos(gameWindow, &xPos, &yPos);
		glfwSetCursorPosCallback(gameWindow, getCursorData);
	
	
	//glfwGetCursorPos(gameWindow, &xPos, &yPos);
	
	//std::cout << xPos << " " << yPos << std::endl;

	

	//std::cout << globalCameraEntity->transform.m_rotation.x << " , " << globalCameraEntity->transform.m_rotation.y << " , " << globalCameraEntity->transform.m_rotation.z << "\n";

}
int getWantedSlot() {
	int wantedSlot = -1;
	if (Input::GetKeyDown(GLFW_KEY_1)) {
		wantedSlot = 0;
	}
	if (Input::GetKeyDown(GLFW_KEY_2)) {
		wantedSlot = 1;
	}
	if (Input::GetKeyDown(GLFW_KEY_3)) {
		wantedSlot = 2;
	}
	if (Input::GetKeyDown(GLFW_KEY_4)) {
		wantedSlot = 3;
	}
	return wantedSlot;
}
void outputCameraFacing(glm::vec3 cf) {
	glm::vec3 cameraFacingVector = glm::vec3(0);
	cameraFacingVector = cf;
	Entity* cameraEntity = globalCameraEntity;
	glm::vec3 pos = cameraEntity->transform.m_pos;
	float deltaX = 0;
	float deltaY = 0;
	float deltaZ = 0;
	deltaX = pos.x - (pos.x + cameraFacingVector.x);
	deltaY = pos.y - (pos.y + cameraFacingVector.y);
	deltaZ = pos.z - (pos.z + cameraFacingVector.z);

	if (deltaX < -0.5) {
		std::cout << "Facing: Positive X" << std::endl;
	}
	else if (deltaX >= 0.5) {
		std::cout << "Facing: Negative X" << std::endl;
	}
	if (deltaY < -0.5) {
		std::cout << "Facing: Positive Y" << std::endl;
	}
	else if (deltaY >= 0.5) {
		std::cout << "Facing: Negative Y" << std::endl;
	}
	if (deltaZ < -0.5) {
		std::cout << "Facing: Positive Z" << std::endl;
	}
	else if (deltaZ >= 0.5) {
		std::cout << "Facing: Negative Z" << std::endl;
	}
}

bool isSlotEmpty(int i) {
	if (trayPastry[i] == nullptr) {
		//std::cout << i << std::endl;
		return true;
	}
	return false;
}

int getFirstTraySlot() {

	for (int i = 0; i < std::size(trayPastry); i++) {
		
		if (isSlotEmpty(i)) {
			return i;
		}
	}
	return -1;
}

void setPastryMesh(Entity* e, bakeryUtils::pastryType type) {
	if (type == bakeryUtils::pastryType::CROISSANT) {
		e->Remove<CMeshRenderer>();
		e->Add<CMeshRenderer>(*e, *crossantMat.getMesh(), *crossantMat.getMaterial());
		e->transform.m_scale = glm::vec3(0.2f, 0.2f, 0.2f);
	}
	if (type == bakeryUtils::pastryType::COOKIE) {
		e->Remove<CMeshRenderer>();
		e->Add<CMeshRenderer>(*e, *cookieMat.getMesh(), *cookieMat.getMaterial());
		e->transform.m_scale = glm::vec3(0.2f, 0.2f, 0.2f);
	}
	if (type == bakeryUtils::pastryType::CUPCAKE) {
		e->Remove<CMeshRenderer>();
		e->Add<CMeshRenderer>(*e, *cupcakeMat.getMesh(), *cupcakeMat.getMaterial());
		e->transform.m_scale = glm::vec3(0.15f, 0.15f, 0.15f);
	}
}
float getTrayRaise(bakeryUtils::pastryType type) {
	float raise = 0;
	if (type == bakeryUtils::pastryType::CUPCAKE) {
		raise = 0.13;
	}
	return raise;
}

void setPastryFilling(Entity* e, bakeryUtils::fillType type)
{
}

void setPastryTopping(Entity* e, bakeryUtils::toppingType type)
{
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	scrollX = xoffset;
	scrollY = yoffset;
	//std::cout << scrollX << " " << scrollY << std::endl;
}






