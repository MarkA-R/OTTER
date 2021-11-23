

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
#include "Register.h"
#include "TrashCan.h"
#include "DrinkMachine.h"
#include "Drink.h"
#include <algorithm>
#include <math.h>
#include <ctime>


#include <iostream>

#include "imgui.h"

#include <memory>
#include "OrderBubble.h"
#include "Register.h"

using namespace nou;

// Templated LERP function
template<typename T>
T Lerp(const T& p0, const T& p1, float t)
{
	return (1.0f - t) * p0 + t * p1;
}

template<typename T>
T Catmull(const T& p0, const T& p1, const T& p2, const T& p3, float t)
{

	return 0.5f * (2.f * p1 + t * (-p0 + p2)
		+ t * t * (2.f * p0 - 5.f * p1 + 4.f * p2 - p3)
		+ t * t * t * (-p0 + 3.f * p1 - 3.f * p2 + p3));
}

template<typename T>
T keepInBounds(T current, T lower, T upper) {
	if (current < lower) {
		current = upper - (lower - current);
	}
	if (current >= lower && current <= upper) {

	}
	if (current > upper) {
		current = (current + lower) % upper;
	}
	return current;
}


float easeInOut(float x) {//the easeInOutQuint function from easings.net (https://easings.net/#easeInOutQuint)
	if (x < 0.5) {
		return 16 * x * x * x * x * x;
	}
	else
	{
		return 1 - pow(-2 * x + 2, 5) / 2;
	}
}

float easeOut(float x) {
	return 1 - (1 - x) * (1 - x);
}

float easeIn(float x) {
	return 1 - cos((x * 3.1415926) / 2);
}


float tempA = 0.f;
float tempB = 0.f;
float tempC = 0.f;
float tempD = 0.f;
std::unique_ptr<ShaderProgram> prog_texLit, prog_lit, prog_unlit, prog_morph, prog_particles;
std::unique_ptr<Material>  mat_unselected, mat_selected, mat_line;
glm::vec3 cameraPos = glm::vec3(-1.f, -0.5f, -0.0f);
glm::vec3 menuCameraPos = glm::vec3(-0.7f, -1.2f, -10.7f);
glm::vec3 insidePos = glm::vec3(cameraPos.x - 0.3, cameraPos.y, cameraPos.z);
glm::vec3 outsidePos = glm::vec3(menuCameraPos.x - 0.3, (cameraPos.y + menuCameraPos.y)/2, menuCameraPos.z + 0.6);
std::vector<glm::vec3> cameraKeys = std::vector<glm::vec3>();


int ovenFrame = -1;
std::vector<Mesh*> currentOvenFrames;
std::vector<Mesh*> ovenDoorOneOpen;
std::vector<Mesh*> ovenDoorTwoOpen;
std::vector<Mesh*> ovenDoorThreeOpen;
std::vector<Mesh*> ovenDoorFourOpen;

float receiptT = 0.0f;
float receiptStopT = 0.0f;
glm::vec3 receptEndPos;
glm::vec3 receptBeginPos;

glm::quat cameraQuat = glm::quat();
glm::quat lastCameraQuat;

double xPos, yPos;
double cameraX = 0, cameraY = 0;
double lastCameraX = 0, lastCameraY = 0;
double sensitivity = -0.1;
bool isInMainMenu = true;
bool isInPauseMenu = false;
bool isInContinueMenu = false;
int selectedOption = 0;
glm::quat getCameraRotation();
glm::quat menuCameraQuat;
glm::quat standardCameraQuat;
glm::quat currentCameraQuat;
glm::quat wantedCameraQuat;

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
bool isClickingUp = false;
bool isClickingDown = false;
bool isClickingLeft = false;
bool isClickingRight = false;
bool isClickingEnter = false;
bool isClickingSpace = false;

//Mouse State
bool firstMouse = true;
int width = 1280;
int height = 720;
float lastX = width / 2;
float lastY = height / 2;
float scrollX;
float scrollY;
float cameraT = 0;
int currentCameraPoint = 0;
bool isCameraMoving = false;

std::vector<Entity*> renderingEntities = std::vector<Entity*>();
Transform traySlot[4] = {};

//std::unique_ptr<Entity> trayPastry[4] = {nullptr, nullptr, nullptr, nullptr};
std::vector<Order> orders; //new Mithunan
std::vector<OrderBubble*> orderBubbles;
std::vector<Transform*> orderBubbleTransform;
std::vector<OvenTimer*> orderBubbleTimers;
std::vector<int> orderBubblesToRemove;
Entity* trayPastry[4] = {nullptr, nullptr, nullptr, nullptr};
std::vector<Mesh*> fillingFrames = std::vector<Mesh*>();
std::vector<Material*> signFrames = std::vector<Material*>();
std::vector<Mesh*> drinkFrames = std::vector<Mesh*>();



// Keep our main cleaner
void LoadDefaultResources();
void getCursorData(GLFWwindow* window, double x, double y);
void outputCameraFacing(glm::vec3 cf);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
int getFirstTraySlot();
bool isSlotEmpty(int i);
void setMachinePastryMesh(Entity* e, bakeryUtils::pastryType type);
void setTrayPastryMesh(Entity* e, bakeryUtils::pastryType type);
void setPastryFilling(Entity* e, bakeryUtils::fillType type);
void setPastryTopping(Entity* e, bakeryUtils::toppingType type);
int getWantedSlot();
float getTrayRaise(bakeryUtils::pastryType type);
void moveCamera(int);
int getSignSelection(int max, bool reset);
bool isInRendering(Entity* e);
void setDrinkMesh(Entity* e, bakeryUtils::drinkType type);
// Function to handle user inputs
void GetInput();
void getKeyInput();


MaterialCreator crossantMat = MaterialCreator();
MaterialCreator doughMat = MaterialCreator();
MaterialCreator cookieMat = MaterialCreator();
MaterialCreator cupcakeMat = MaterialCreator();
MaterialCreator cakeMat = MaterialCreator();
MaterialCreator burntMat = MaterialCreator();


MaterialCreator playSignMat = MaterialCreator();
MaterialCreator settingsSignMat = MaterialCreator();
MaterialCreator exitSignMat = MaterialCreator();
MaterialCreator pauseSignMat = MaterialCreator();

MaterialCreator croissantTile = MaterialCreator();
MaterialCreator doughTile = MaterialCreator();
MaterialCreator cookieTile = MaterialCreator();
MaterialCreator cupcakeTile = MaterialCreator();
MaterialCreator cakeTile = MaterialCreator();
MaterialCreator nothingTile = MaterialCreator();
MaterialCreator burntTile = MaterialCreator();

MaterialCreator coffeeTile = MaterialCreator();
MaterialCreator teaTile = MaterialCreator();
MaterialCreator milkshakeTile = MaterialCreator();


MaterialCreator coffeeMat = MaterialCreator();
MaterialCreator teaMat = MaterialCreator();
MaterialCreator milkshakeMat = MaterialCreator();




MaterialCreator custardFilling = MaterialCreator();
MaterialCreator nutellaFilling = MaterialCreator();
MaterialCreator strawberryFilling = MaterialCreator();

MaterialCreator pecanTopping = MaterialCreator();
MaterialCreator sprinkleTopping = MaterialCreator();
MaterialCreator stawberryTopping = MaterialCreator();

MaterialCreator plusTile = MaterialCreator();
MaterialCreator bubbleTile = MaterialCreator();

std::unique_ptr<Material> pecanParticle;
std::unique_ptr<Material> sprinkleParticle;
std::unique_ptr<Material> strawberryParticle;


Transform customerBubbleLocation;
Transform upurrBubbleLocation1;
Transform upurrBubbleLocation2;

//burnt tile???

MaterialCreator* getPastryTile(bakeryUtils::pastryType x);
MaterialCreator* getFillingTile(bakeryUtils::fillType x);
MaterialCreator* getToppingTile(bakeryUtils::toppingType x);
MaterialCreator* getDrinkTile(bakeryUtils::drinkType x);
void removeFromRendering(Entity* e);
void resetBubble(int i);
glm::vec3 trayScale;
glm::vec3 cursorScale;
void createNewOrder(int i, bool addDifficulty, bool remove = true);

float currentGameTime = 0;
int difficulty = 1;
std::vector<Order> currentOrders = std::vector<Order>();

void log(std::string s) {
	std::cout << s << std::endl;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

bool isPaused = false;
int main()
{
	cameraKeys.push_back(cameraPos);
	cameraKeys.push_back(insidePos);
	cameraKeys.push_back(outsidePos);
	cameraKeys.push_back(menuCameraPos);

	srand(static_cast<unsigned int>(time(0)));
	// Create window and set clear color
	App::Init("Chateau Gateau", width, height);
	App::SetClearColor(glm::vec4(1, 0.964, 0.929,1.f));
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
	trayMat.createMaterial("bakery/models/tray.gltf", "bakery/textures/tray.png", *prog_texLit);

	MaterialCreator fridgeMat = MaterialCreator();
	fridgeMat.createMaterial("bakery/models/fridge.gltf", "bakery/textures/fridge.png", *prog_texLit);

	MaterialCreator binMat = MaterialCreator();
	binMat.createMaterial("bakery/models/trash.gltf", "bakery/textures/trash.png", *prog_texLit);

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

	MaterialCreator barMat1 = MaterialCreator();
	barMat1.createMaterial("bakery/models/bar1.gltf", "bakery/textures/bar.png", *prog_morph);
	MaterialCreator barMat2 = MaterialCreator();
	barMat2.createMaterial("bakery/models/bar2.gltf", "bakery/textures/bar.png", *prog_morph);



	MaterialCreator drinkMat1 = MaterialCreator();//for morphs
	drinkMat1.createMaterial("bakery/models/drinkMachine1.gltf", "bakery/textures/drinkMachine.png", *prog_morph);

	MaterialCreator drinkMat2 = MaterialCreator();//for morphs
	drinkMat2.createMaterial("bakery/models/drinkMachine2.gltf", "bakery/textures/drinkMachine.png", *prog_morph);

	drinkFrames.push_back(drinkMat1.getMesh().get());
	drinkFrames.push_back(drinkMat2.getMesh().get());


	//oven morph targets
	for (int i = 1; i <= 4; i++) {

	}


	MaterialCreator timerMat = MaterialCreator();
	timerMat.createMaterial("bakery/models/timer.gltf", "bakery/textures/timer.png", *prog_texLit);
	
	MaterialCreator arrowMat = MaterialCreator();
	arrowMat.createMaterial("bakery/models/arrow.gltf", "bakery/textures/arrow.png", *prog_texLit);
	
	MaterialCreator bakeryMat = MaterialCreator();
	bakeryMat.createMaterial("bakery/models/bakery.gltf", "bakery/textures/bakeryTexture.png", *prog_texLit);

	MaterialCreator receiptMat = MaterialCreator();
	receiptMat.createMaterial("bakery/models/tile.gltf", "UI/textures/Receipt.png", *prog_texLit);


	std::unique_ptr<Texture2D> particleTex = std::make_unique<Texture2D>("bakery/textures/particle.png");
	std::unique_ptr<Material> particleMat = std::make_unique<Material>(*prog_particles);
	particleMat->AddTexture("albedo", *particleTex);

	doughMat.createMaterial("bakery/models/dough.gltf", "bakery/textures/dough.png", *prog_texLit);
	crossantMat.createMaterial("bakery/models/croissant.gltf", "bakery/textures/croissant.png", *prog_texLit);
	cookieMat.createMaterial("bakery/models/cookie.gltf", "bakery/textures/cookie.png", *prog_texLit);
	cupcakeMat.createMaterial("bakery/models/cupcake.gltf", "bakery/textures/cupcake.png", *prog_texLit);
	cakeMat.createMaterial("bakery/models/cake.gltf", "bakery/textures/cake.png", *prog_texLit);
	burntMat.createMaterial("bakery/models/burnt.gltf", "bakery/textures/burnt.png", *prog_texLit);

	coffeeTile.createMaterial("bakery/models/tile.gltf", "bakery/textures/coffeeTile.png", *prog_texLit);
	teaTile.createMaterial("bakery/models/tile.gltf", "bakery/textures/teaTile.png", *prog_texLit);
	milkshakeTile.createMaterial("bakery/models/tile.gltf", "bakery/textures/milkshakeTile.png", *prog_texLit);

	coffeeMat.createMaterial("bakery/models/coffee.gltf", "bakery/textures/coffee.png", *prog_texLit);
	teaMat.createMaterial("bakery/models/tea.gltf", "bakery/textures/tea.png", *prog_texLit);
	milkshakeMat.createMaterial("bakery/models/milkshake.gltf", "bakery/textures/milkshake.png", *prog_texLit);
	
	playSignMat.createMaterial("UI/models/Chalkboard.gltf", "UI/textures/playSign.png", *prog_texLit);
	settingsSignMat.createMaterial("UI/models/Chalkboard.gltf", "UI/textures/settingsSign.png", *prog_texLit);
	exitSignMat.createMaterial("UI/models/Chalkboard.gltf", "UI/textures/exitSign.png", *prog_texLit);
	pauseSignMat.createMaterial("UI/models/Chalkboard.gltf", "UI/textures/pauseSign.png", *prog_texLit);
	signFrames.push_back(playSignMat.getMaterial().get());
	signFrames.push_back(settingsSignMat.getMaterial().get());
	signFrames.push_back(exitSignMat.getMaterial().get());
	signFrames.push_back(pauseSignMat.getMaterial().get());


	croissantTile.createMaterial("bakery/models/tile.gltf", "bakery/textures/croissantTile.png", *prog_texLit);
	doughTile.createMaterial("bakery/models/tile.gltf", "bakery/textures/doughTile.png", *prog_texLit);
	cookieTile.createMaterial("bakery/models/tile.gltf", "bakery/textures/cookieTile.png", *prog_texLit);
	cupcakeTile.createMaterial("bakery/models/tile.gltf", "bakery/textures/cupcakeTile.png", *prog_texLit);
	cakeTile.createMaterial("bakery/models/tile.gltf", "bakery/textures/cakeTile.png", *prog_texLit);
	nothingTile.createMaterial("bakery/models/tile.gltf", "bakery/textures/nothingTile.png", *prog_texLit);
	burntTile.createMaterial("bakery/models/tile.gltf", "bakery/textures/burntTile.png", *prog_texLit);

	custardFilling.createMaterial("bakery/models/tile.gltf", "bakery/textures/custardFilling.png", *prog_texLit);
	nutellaFilling.createMaterial("bakery/models/tile.gltf", "bakery/textures/nutellaFilling.png", *prog_texLit);
	strawberryFilling.createMaterial("bakery/models/tile.gltf", "bakery/textures/strawberryFilling.png", *prog_texLit);

	pecanTopping.createMaterial("bakery/models/tile.gltf", "bakery/textures/pecanTopping.png", *prog_texLit);
	stawberryTopping.createMaterial("bakery/models/tile.gltf", "bakery/textures/strawberryTopping.png", *prog_texLit);
	sprinkleTopping.createMaterial("bakery/models/tile.gltf", "bakery/textures/sprinkleTopping.png", *prog_texLit);
	
	plusTile.createMaterial("bakery/models/tile.gltf", "bakery/textures/plusTile.png", *prog_texLit);
	bubbleTile.createMaterial("bakery/models/tile.gltf", "bakery/textures/bubbleTile.png", *prog_texLit);

	
	std::unique_ptr<Texture2D> pecanTex = std::make_unique<Texture2D>("bakery/textures/pecanParticle.png");
	pecanParticle = std::make_unique<Material>(*prog_particles);
	pecanParticle->AddTexture("albedo", *pecanTex);

	std::unique_ptr<Texture2D> strawberryTex = std::make_unique<Texture2D>("bakery/textures/strawberryParticle.png");
	strawberryParticle = std::make_unique<Material>(*prog_particles);
	strawberryParticle->AddTexture("albedo", *strawberryTex);

	std::unique_ptr<Texture2D> sprinkleTex = std::make_unique<Texture2D>("bakery/textures/sprinkleParticle.png");
	sprinkleParticle = std::make_unique<Material>(*prog_particles);
	sprinkleParticle->AddTexture("albedo", *sprinkleTex);

	
	

	Entity bakery = Entity::Create();
	bakery.Add<CMeshRenderer>(bakery, *bakeryMat.getMesh(), *bakeryMat.getMaterial());
	bakery.transform.m_scale = glm::vec3(0.7, 0.7, 0.7);
	bakery.transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f));
	bakery.transform.m_pos = glm::vec3(-2.4, -1.9, -1.2);
	renderingEntities.push_back(&bakery);

	Entity sign = Entity::Create();
	sign.Add<CMeshRenderer>(sign, *playSignMat.getMesh(), *playSignMat.getMaterial());
	sign.transform.m_scale = glm::vec3(0.15, 0.2, 0.15);
	sign.transform.m_rotation = glm::angleAxis(glm::radians(270.f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec3 signPos = menuCameraPos;
	sign.transform.m_pos = glm::vec3(signPos.x - 0.5, -1.55f, signPos.z +0.2);
	
	renderingEntities.push_back(&sign);


	Entity cursor = Entity::Create();
	cursor.Add<CMeshRenderer>(cursor, *cursorMat.getMesh(), *cursorMat.getMaterial());
	cursorScale = glm::vec3(0.001f, 0.001f, 0.001f);
	cursor.transform.m_scale = cursorScale;
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
	
	Entity receipt = Entity::Create();
	receipt.Add<CMeshRenderer>(receipt, *receiptMat.getMesh(), *receiptMat.getMaterial());
	receipt.transform.m_scale = glm::vec3(0.15, 0.15, 0.15);
	receipt.transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f))
		* glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 0.0f, 1.0f));//0
	
	
	
		
		//Creating Cash Register Entity
		Entity ent_register = Entity::Create();
		ent_register.Add<CMeshRenderer>(ent_register, *registerMaterial.getMesh(), *registerMaterial.getMaterial());
		ent_register.Add<Register>();
		ent_register.transform.m_scale = glm::vec3(0.4f, 0.4f, 0.4f);
		ent_register.transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
		ent_register.transform.m_pos = glm::vec3(-1.f, -2.4, -2.29f);
		ent_register.Add<BoundingBox>(glm::vec3(0.5, 2.3, 0.06), ent_register);//TODO: REMOVE THIS WHEN CUSTOMERS ARE IN
		renderingEntities.push_back(&ent_register);

		customerBubbleLocation = ent_register.transform;
		customerBubbleLocation.m_pos.x -= 1.75;
		customerBubbleLocation.m_pos.y += 2.0;
		customerBubbleLocation.m_pos.z -= 1.0;


		upurrBubbleLocation1 = ent_register.transform;
		upurrBubbleLocation1.m_pos.x -= 1.75;
		upurrBubbleLocation1.m_pos.y += 2.4;
		upurrBubbleLocation1.m_pos.z -= 1.0;


		upurrBubbleLocation2 = ent_register.transform;
		upurrBubbleLocation2.m_pos.x -= 1.75;
		upurrBubbleLocation2.m_pos.y += 2.8;
		upurrBubbleLocation2.m_pos.z -= 1.0;


		Entity counter = Entity::Create();
		counter.Add<CMeshRenderer>(counter, *counterMat.getMesh(), *counterMat.getMaterial());
		counter.transform.m_scale = glm::vec3(1.f, 0.4f, 0.4f);
		counter.transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
		counter.transform.m_pos = glm::vec3(-1.f, -2.4, -2.29f);
		renderingEntities.push_back(&counter);


		Entity bin = Entity::Create();
		bin.Add<CMeshRenderer>(bin, *binMat.getMesh(), *binMat.getMaterial());
		bin.Add<Machine>();
		bin.Add<TrashCan>();
		bin.transform.m_scale = glm::vec3(0.2f, 0.5f, 0.2f);
		bin.transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f));
		bin.transform.m_pos = glm::vec3(-3.f, -1.3, -1.3);
		bin.Add<BoundingBox>(glm::vec3(0.2, 1, 0.3), bin);
		renderingEntities.push_back(&bin);
	//	std::cout << bin.Get<BoundingBox>().getOrigin().x << " " << bin.Get<BoundingBox>().getOrigin().y <<
		//	" " << bin.Get<BoundingBox>().getOrigin().z << std::endl;
		
		Entity fridge = Entity::Create();
		fridge.Add<CMeshRenderer>(fridge, *fridgeMat.getMesh(), *fridgeMat.getMaterial());
		
		fridge.Add<Machine>();
		fridge.Add<Fridge>();
		fridge.transform.m_scale = glm::vec3(0.5f, 1.f, .5f);
		fridge.transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f));
		fridge.transform.m_pos = glm::vec3(-3.f, -1.f, 0.4f);
		fridge.Add<BoundingBox>(glm::vec3(0.67, 4, 0.3), fridge);
		renderingEntities.push_back(&fridge);

		Entity oven = Entity::Create();
		oven.Add<CMeshRenderer>(oven, *ovenMat.getMesh(), *ovenMat.getMaterial());
		
		oven.Add<Machine>();
		oven.Add<Oven>();
		oven.transform.m_scale = glm::vec3(0.4f, 0.4f, 0.4f);
		oven.transform.m_rotation = glm::angleAxis(glm::radians(270.f), glm::vec3(0.0f, 1.0f, 0.0f));
		oven.transform.m_pos = glm::vec3(1.f, -1.5f, 0.5f);
		oven.Add<BoundingBox>(glm::vec3(0.51, 2, 0.25), oven);
		renderingEntities.push_back(&oven);
	
		Entity filling = Entity::Create();
		filling.Add<CMorphMeshRenderer>(filling, *fillingMat1.getMesh(), *fillingMat1.getMaterial());
		
		filling.Add<Machine>();
		filling.Add<FillingMachine>();
		filling.transform.m_scale = glm::vec3(0.3f, 1.f, 0.3f);
		filling.transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
		filling.transform.m_pos = glm::vec3(0.3f, -1.0f, 2.0f);
		filling.Add<BoundingBox>(glm::vec3(0.72, 2, 0.35), filling);
		glm::vec3 fillingPos = filling.transform.m_pos;
		filling.Get<BoundingBox>().setOrigin(glm::vec3(fillingPos.x + 0.4, fillingPos.y, fillingPos.z));
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
		

		Entity drink = Entity::Create();
		drink.Add<CMorphMeshRenderer>(drink, *drinkMat1.getMesh(), *drinkMat1.getMaterial());

		drink.Add<Machine>();
		drink.Add<DrinkMachine>();
		drink.transform.m_scale = glm::vec3(0.3f, 1.f, 0.3f);
		drink.transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
		drink.transform.m_pos = glm::vec3(-0.9f, -1.0f, 2.0f);
		drink.Add<BoundingBox>(glm::vec3(0.32, 2, 0.35), filling);
		glm::vec3 drinkPos = drink.transform.m_pos;
		drink.Get<BoundingBox>().setOrigin(glm::vec3(drinkPos.x - 0.0, drinkPos.y, drinkPos.z));
		renderingEntities.push_back(&drink);

		auto& animatordrink = drink.Add<CMorphAnimator>(drink);
		animatordrink.SetFrameTime(1.0f);
		animatordrink.SetFrames(drinkFrames);

		Transform drinkBarPos = drink.transform;
		drinkBarPos.m_pos.z -= 0.65;
		drinkBarPos.m_pos.y += 0.7;
		drinkBarPos.m_pos.x += 0.1;
		drinkBarPos.m_scale = glm::vec3(0.05,0.5,0.1);
		//drinkBarPos.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f));

		FillBar drinkFill;
		drinkFill.setup(barMat1, barMat2, drinkBarPos);
		Transform outDrinkTrans = drink.transform;
		outDrinkTrans.m_pos.z -= 0.3;
		outDrinkTrans.m_pos.y -= 0.05;
		Transform inDrinkTrans = drink.transform;
		inDrinkTrans.m_pos.y -= 0.05;
		drink.Get<DrinkMachine>().setTransform(inDrinkTrans, outDrinkTrans);
		drink.Get<DrinkMachine>().setup(&coffeeTile, &milkshakeTile, &teaTile, &drinkFill);
		renderingEntities.push_back(drinkFill.getEntity());
		

		//filling.Get<FillingMachine>().setup(&custardFilling, &nutellaFilling, &strawberryFilling);

		Entity drinkPlane = Entity::Create();
		drinkPlane.Add<CMeshRenderer>(drinkPlane, *coffeeTile.getMesh(), *coffeeTile.getMaterial());
		drinkPlane.transform.m_scale = glm::vec3(0.24f, 0.24f, 0.24f);
		drinkPlane.transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::angleAxis(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f));;
		glm::vec3 drinkTilePos = drink.transform.m_pos;
		drinkPlane.transform.m_pos = glm::vec3(drinkTilePos.x - 0.1, drinkTilePos.y + 0.8, drinkTilePos.z - 0.5);
		renderingEntities.push_back(&drinkPlane);
		drink.Get<DrinkMachine>().setDrinkPlane(&drinkPlane);

		


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
		topping.transform.m_pos = glm::vec3(-2.0, -1.0f, 2.0f);
		topping.Add<BoundingBox>(glm::vec3(0.4f, 2, 0.5), topping);
		
		glm::vec3 tempTopPos = topping.transform.m_pos;
		//tempTopPos.x += 0.0;
		//topping.Get<BoundingBox>().setOrigin(tempTopPos);
		
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


	
	Entity tray = Entity::Create();
	tray.Add<CMeshRenderer>(tray, *trayMat.getMesh(), *trayMat.getMaterial());
	trayScale = glm::vec3(0.053f, 0.035f, 0.037f);
	tray.transform.m_scale = trayScale;
	tray.transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
	tray.transform.m_pos = glm::vec3(cameraPos.x + 0.92, cameraPos.y + 0.430, cameraPos.z + -0.147);// 0.552
	
	//renderingEntities.push_back(&tray);
	traySlot[0] = Transform();
	traySlot[0].m_pos = tray.transform.m_pos;
	traySlot[0].m_pos.x = tray.transform.m_pos.x - 0.017;
	traySlot[0].m_pos.z = tray.transform.m_pos.z - 0.017;
	traySlot[0].SetParent(&tray.transform);
	traySlot[1] = Transform();
	traySlot[1].m_pos = tray.transform.m_pos;
	traySlot[1].m_pos.x = tray.transform.m_pos.x + 0.017;
	traySlot[1].m_pos.z = tray.transform.m_pos.z - 0.017;
	traySlot[1].SetParent(&tray.transform);
	traySlot[2] = Transform();
	traySlot[2].m_pos = tray.transform.m_pos;
	traySlot[2].m_pos.x = tray.transform.m_pos.x - 0.017;
	traySlot[2].m_pos.z = tray.transform.m_pos.z + 0.017;
	traySlot[2].SetParent(&tray.transform);
	traySlot[3] = Transform();
	traySlot[3].m_pos = tray.transform.m_pos;
	traySlot[3].m_pos.x = tray.transform.m_pos.x + 0.017;
	traySlot[3].m_pos.z = tray.transform.m_pos.z + 0.017;
	traySlot[3].SetParent(&tray.transform);

	

	Transform slot1Transform = oven.transform;
	//1.f, -1.5f, 0.5f
	slot1Transform.m_pos.x += -0.39;
	slot1Transform.m_pos.y += 0.53;
	slot1Transform.m_pos.z -= 0.440;
	OvenTimer slot1 = OvenTimer(nothingTile,arrowMat, timerMat, slot1Transform);
	renderingEntities.push_back(slot1.getArrow());
	renderingEntities.push_back(slot1.getCircle());
	renderingEntities.push_back(slot1.getTile());

	Transform slot2Transform = oven.transform;
	slot2Transform.m_pos.x += -0.27;
	slot2Transform.m_pos.y += 0.53;
	slot2Transform.m_pos.z -= 0.140;
	OvenTimer slot2 = OvenTimer(nothingTile, arrowMat, timerMat, slot2Transform);
	renderingEntities.push_back(slot2.getArrow());
	renderingEntities.push_back(slot2.getCircle());
	renderingEntities.push_back(slot2.getTile());


	Transform slot3Transform = oven.transform;
	slot3Transform.m_pos.x += -0.39;
	slot3Transform.m_pos.y += 0.27;
	slot3Transform.m_pos.z -= 0.440;
	OvenTimer slot3 = OvenTimer(nothingTile, arrowMat, timerMat, slot3Transform);
	renderingEntities.push_back(slot3.getArrow());
	renderingEntities.push_back(slot3.getCircle());
	renderingEntities.push_back(slot3.getTile());

	Transform slot4Transform = oven.transform;
	slot4Transform.m_pos.x += -0.27;
	slot4Transform.m_pos.y += 0.27;
	slot4Transform.m_pos.z -= 0.140;
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
	tiles.push_back(&burntTile);

	oven.Get<Oven>().setup(&slot1, &slot2, &slot3, &slot4, &oven.transform);
	oven.Get<Oven>().setMaterials(tiles);
	App::Tick();


	Oven* ovenScript = &oven.Get<Oven>();
	glm::vec3 lastPoint = glm::vec3(-999,-999,-999);
	glm::vec3 currentPoint = glm::vec3(0);
	bool raycastHit = false;
	Entity* hitEntity = nullptr;
	currentOrders.push_back(Order());
	currentOrders.back().createOrder(bakeryUtils::getDifficulty());//bakeryUtils::getDifficulty()
	//currentOrders.back().startOrder();
	
	OvenTimer upurrTimer1 = OvenTimer(nothingTile, arrowMat, timerMat, customerBubbleLocation, 0.2);
	OvenTimer upurrTimer2 = OvenTimer(nothingTile, arrowMat, timerMat, customerBubbleLocation, 0.2);
	OvenTimer customerTimer = OvenTimer(nothingTile, arrowMat, timerMat, customerBubbleLocation,0.2);

	OrderBubble uprrBubble1(&upurrTimer1);
	OrderBubble uprrBubble2(&upurrTimer2);

	orderBubbleTimers.push_back(&customerTimer);
	orderBubbleTimers.push_back(&upurrTimer1);
	orderBubbleTimers.push_back(&upurrTimer2);


	OrderBubble firstOrder(&customerTimer);
	//firstOrder.setTransform(customerBubbleLocation);
	firstOrder.setTiles(getPastryTile(currentOrders.back().type), getFillingTile(currentOrders.back().filling), getToppingTile(currentOrders.back().topping), getDrinkTile(currentOrders.back().drink));
	firstOrder.setup(&bubbleTile, &plusTile);
	firstOrder.create(currentOrders.back());

	orderBubbles.push_back(&firstOrder);
	orderBubbles.push_back(&uprrBubble1);
	orderBubbles.push_back(&uprrBubble2);

	orderBubbleTransform.push_back(&customerBubbleLocation);
	orderBubbleTransform.push_back(&upurrBubbleLocation1);
	orderBubbleTransform.push_back(&upurrBubbleLocation2);
	
	
	
	standardCameraQuat = getCameraRotation();
	wantedCameraQuat = standardCameraQuat;
	cameraX = 90.f;
	cameraY = 0.f;
	menuCameraQuat = getCameraRotation();
	currentCameraQuat = menuCameraQuat;
	
	//REMOVE WHEN YOU WANT TO TEST MENUS OR SHIP THE FINAL GAME OR DO A DEMO! #################################
	
	bool skipMenu = true;
	if(skipMenu) {
	cameraEntity.transform.m_pos = cameraPos;
	globalCameraEntity->transform.m_pos = cameraPos;
	cameraX = lastCameraX;
	cameraY = lastCameraY;
	isCameraMoving = false;
	isInMainMenu = false;
	isPaused = false;
	tray.transform.SetParent(&cameraEntity.transform);


	if (!isInRendering(&tray)) {
		renderingEntities.push_back(&tray);
	}
	if (!isInRendering(&cursor)) {
		renderingEntities.push_back(&cursor);
	}

	for (int i = 0; i < orderBubbles.size(); i++) {
		OrderBubble* ob = orderBubbles[i];
		for each (Entity * ent in ob->returnRenderingEntities()) {
			renderingEntities.push_back(ent);
		}
	}
	currentOrders.back().startOrder();
	}
	
//up to here
	
	DrinkMachine& drinkScript = drink.Get<DrinkMachine>();
	//renderingEntities.push_back(&receipt);
	while (!App::IsClosing() && !Input::GetKeyDown(GLFW_KEY_ESCAPE))
	{
		
		
		bool keepCheckingRaycast = true;
		isClicking = false;
		isRightClicking = false;
		int addedSlot = -1;
		raycastHit = false;
		App::FrameStart();
		float deltaTime = App::GetDeltaTime();
		getKeyInput();

		

		if (Input::GetKeyDown(GLFW_KEY_ENTER)) {//put this in the lose spot
			receipt.transform.m_pos = cursor.transform.m_pos;
			receipt.transform.m_rotation = cursor.transform.m_rotation * glm::angleAxis(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f)) *
				glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f))
				* glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 0.0f, 1.0f));
			removeFromRendering(&cursor);
			removeFromRendering(&tray);
			for each (Entity * trayEntity in trayPastry) {
				removeFromRendering(trayEntity);
			}
			renderingEntities.push_back(&receipt);
			receiptT = 0;
			isInContinueMenu = true;
		}

		if (isInMainMenu) {
			if (!isCameraMoving) {
				
				cameraEntity.transform.m_pos = menuCameraPos;

				cameraEntity.transform.m_rotation = menuCameraQuat;

				cameraEntity.Get<CCamera>().Update();
				int mainMenuChosen = getSignSelection(3,false);
				
				sign.Get<CMeshRenderer>().SetMaterial(*signFrames[selectedOption]);
				
				if (mainMenuChosen >= 0) {	
					if (mainMenuChosen == 0) {//PLAY	
						
						isCameraMoving = true;
					}

					if (mainMenuChosen == 2) {
						break;
					}

				}
			}
			else
			{
				if (cameraT == 0.f && currentCameraPoint == 3) {
					if (!isInContinueMenu) {
						cameraEntity.transform.m_pos = cameraPos;
						globalCameraEntity->transform.m_pos = cameraPos;
						cameraX = lastCameraX;
						cameraY = lastCameraY;
					}
					
					isCameraMoving = false;
					isInMainMenu = false;
					isPaused = false;
					tray.transform.SetParent(&cameraEntity.transform);
					

					if (!isInContinueMenu) {
						if (!isInRendering(&tray)) {
							renderingEntities.push_back(&tray);
						}
						if (!isInRendering(&cursor)) {
							renderingEntities.push_back(&cursor);
						}
						

						for (int i = 0; i < orderBubbles.size(); i++) {
							OrderBubble* ob = orderBubbles[i];
							for each (Entity * ent in ob->returnRenderingEntities()) {
								renderingEntities.push_back(ent);
							}
						}
						currentOrders.back().startOrder();
					}
					if (isInContinueMenu) {
						isInContinueMenu = false;
						isInMainMenu = true;
						selectedOption = 0;
						std::reverse(cameraKeys.begin(), cameraKeys.end());
						currentCameraPoint = 0;
						cameraT = 0.f;
						lastCameraX = 0;
						lastCameraY = 0;
						//standardCameraQuat = getCameraRotation();
						wantedCameraQuat = standardCameraQuat;
						cameraX = 90.f;
						cameraY = 0.f;
						//menuCameraQuat = getCameraRotation();
						currentCameraQuat = menuCameraQuat;
						lastCameraQuat = standardCameraQuat;
						
					}
					
				}
				else
				{
					moveCamera(1);
				}
				
				
				
			}
			
			for each (Entity * e in renderingEntities) {

				e->transform.RecomputeGlobal();


				if (e->Has<CMeshRenderer>()) {
					e->Get<CMeshRenderer>().Draw();
				}

				if (e->Has<CMorphMeshRenderer>()) {
					e->Get<CMorphMeshRenderer>().Draw();
				}
				
			}
			
			App::SwapBuffers();
			continue;

		}
		if (isInPauseMenu) {
			if (!isCameraMoving) {
				cameraEntity.transform.m_pos = menuCameraPos;

				cameraEntity.transform.m_rotation = menuCameraQuat;

				cameraEntity.Get<CCamera>().Update();
				int mainMenuChosen = getSignSelection(0, false) ;

				sign.Get<CMeshRenderer>().SetMaterial(*signFrames[selectedOption + 3]);
				
				if (mainMenuChosen >= 0) {
					if (mainMenuChosen == 0) {//CONTINUE						
						isCameraMoving = true;
						isInPauseMenu = false;
						isInMainMenu = true;
						
					
						
					}

				}
			}
			else
			{

				if (cameraT == 0.f && currentCameraPoint == 3) {
					isCameraMoving = false;
					selectedOption = 0;
					std::reverse(cameraKeys.begin(), cameraKeys.end());
					
					currentCameraQuat = menuCameraQuat;
					wantedCameraQuat = lastCameraQuat;
					currentCameraPoint = 0;
					
					

				}
				else
				{
					moveCamera(1);
					
				}
			}
			for each (Entity * e in renderingEntities) {

				e->transform.RecomputeGlobal();


				if (e->Has<CMeshRenderer>()) {
					e->Get<CMeshRenderer>().Draw();
				}

				if (e->Has<CMorphMeshRenderer>()) {
					e->Get<CMorphMeshRenderer>().Draw();
				}

			}

			App::SwapBuffers();
			continue;
		}
		 
		if (isInContinueMenu) {
			//std::cout << static_cast<int>(floor(static_cast<float>(receiptT * 100.f))) << std::endl;
			if ((static_cast<int>(floor(static_cast<float>(receiptT * 100.f))) % 2 == 0) && receiptStopT < 1
				&& receiptT >=0.8 && receiptT <= 0.9) {
				receiptStopT += deltaTime * 4;
			}
			else
			{
				receiptT += deltaTime;
				receiptStopT = 0;
			}
			
			
			if (receiptT > 1) {
				receiptT = 1;
			}
			
			receipt.transform.m_pos = Lerp(receptBeginPos, receptEndPos,receiptT);

			if (isClickingSpace) {
				if (isInRendering(&receipt)) {
					removeFromRendering(&receipt);
				}
				std::reverse(cameraKeys.begin(), cameraKeys.end());
				selectedOption = -1;
				sign.Get<CMeshRenderer>().SetMaterial(*signFrames[0]);
				currentCameraPoint = 0;
				cameraT = 0.f;
				isCameraMoving = true;
				isInPauseMenu = false;
				isInMainMenu = true;
				isInContinueMenu = true;
				lastCameraX = cameraX;
				lastCameraY = cameraY;
				wantedCameraQuat = menuCameraQuat;
				currentCameraQuat = cameraQuat;
				lastCameraQuat = cameraQuat;
				
				for (int i = 0; i < orderBubbles.size(); i++) {
					OrderBubble* ob = orderBubbles[i];
					for each (Entity * ent in ob->returnRenderingEntities()) {
						removeFromRendering(ent);
					}
				}
			}
			for each (Entity * e in renderingEntities) {
				

				e->transform.RecomputeGlobal();


				if (e->Has<CMeshRenderer>()) {
					e->Get<CMeshRenderer>().Draw();
				}

				if (e->Has<CMorphMeshRenderer>()) {
					e->Get<CMorphMeshRenderer>().Draw();
				}

			}

			App::SwapBuffers();
			continue;
		}
		

		
		if (isClickingSpace && !isCameraMoving) {
			isPaused = !isPaused;

			if (isPaused) {
				removeFromRendering(&tray);
				removeFromRendering(&cursor);
				std::reverse(cameraKeys.begin(), cameraKeys.end());
				selectedOption = -1;
				sign.Get<CMeshRenderer>().SetMaterial(*signFrames[3]);
				currentCameraPoint = 0;
				cameraT = 0.f;
				isCameraMoving = true;
				isInPauseMenu = true;
				lastCameraX = cameraX;
				lastCameraY = cameraY;
				wantedCameraQuat = menuCameraQuat;
				currentCameraQuat = cameraQuat;
				lastCameraQuat = cameraQuat;
				for (int i = 0; i < orderBubbles.size(); i++) {
					OrderBubble* ob = orderBubbles[i];
					for each (Entity * ent in ob->returnRenderingEntities()) {
						removeFromRendering(ent);
					}
				}
				//App::setCursorVisible(true);
			}
			
			
		}
		if (!isPaused) {
			//std::cout << isPaused << std::endl;
			GetInput();
			
			
			// Update our LERP timers
		
			bakeryUtils::addToGameTime(deltaTime);
			ovenScript->update(deltaTime);

			
			for (int i = 0; i < currentOrders.size(); i++) {
				OrderBubble* ob = orderBubbles[i];
				ob->addFill(deltaTime);
				//std::cout << bakeryUtils::getTime() << " > " << ob->isOrderExpired() << std::endl;
				if (!currentOrders[i].isOnTime()) {
					//std::cout << "START" << std::endl;
					createNewOrder(i, false,false);
					bakeryUtils::addToFailed(1);
					if (bakeryUtils::getOrdersFailed() == 3) {
						//std::cout << "END" << std::endl;
						receipt.transform.m_pos = cursor.transform.m_pos;
						removeFromRendering(&cursor);
						removeFromRendering(&tray);
						for each (Entity * trayEntity in trayPastry) {
							removeFromRendering(trayEntity);
						}
						renderingEntities.push_back(&receipt);
						
						isInContinueMenu = true;
						break;
					}


					for each (Entity * remover in orderBubbles[i]->returnRenderingEntities()) {
						renderingEntities.erase(std::remove(renderingEntities.begin(), renderingEntities.end(), remover), renderingEntities.end());
						//std::cout << "r" << std::endl;
					}

					resetBubble(i);

					for each (Entity * foe in orderBubbles[i]->returnRenderingEntities()) {
						renderingEntities.push_back(foe);
						//std::cout << "d" << std::endl;
					}
					
				}
			}

			cameraEntity.Get<CCamera>().Update();
		}
		

		
		
	
		
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
		

		receipt.transform.m_pos = cursor.transform.m_pos;
		receptEndPos = cursor.transform.m_pos;
		receipt.transform.m_rotation = cursor.transform.m_rotation * glm::angleAxis(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f)) *
			glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f))
			* glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 0.0f, 1.0f));//glm::inverse(cursor.transform.m_rotation);
		
		/*
		//receipt.transform.SetParent(&cameraEntity.transform);
		//receipt.transform.m_pos = cursor.transform.m_pos;

		
		ImGui::SetNextWindowPos(ImVec2(0, 800), ImGuiCond_FirstUseEver);
		App::StartImgui();
		ImGui::DragFloat("X", &(tempA), 0.1f);
		ImGui::DragFloat("Y", &(tempB), 0.1f);
		ImGui::DragFloat("Z", &(tempC), 0.1f);

		

		//ImGui::DragFloat("Scale", &(sc), 0.1f);
		//ImGui::SetWindowPos(0,0);

		App::EndImgui();
		*/
		receptBeginPos = cursor.transform.m_pos + glm::cross(glm::cross(cameraFacingVector, glm::vec3(0, 1, 0)), cameraFacingVector) * -1.8f;
		receipt.transform.m_pos = receptEndPos;
			
		//receipt.transform.m_rotation = cameraQuat;
		
		
		hitEntity = nullptr;
		for each (Entity* e in renderingEntities) {
			
			e->transform.RecomputeGlobal();

			if (true) {

			}
			if (e->Has<CMeshRenderer>()) {
				e->Get<CMeshRenderer>().Draw();
			}

			if (e->Has<CMorphMeshRenderer>()) {
				e->Get<CMorphMeshRenderer>().Draw();
			}
			
			
			
			if (keepCheckingRaycast && !isPaused) {
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

		if (drinkScript.isOpening || drinkScript.isClosing) {
			float multiplier = 1;
			if (drinkScript.isClosing) {
				multiplier = -1;
			}
			drinkScript.addToT(deltaTime * multiplier);
			drink.Get<CMorphAnimator>().setFrameAndTime(0, 1, drinkScript.getT());
			if (drinkScript.isClosing) {
				drinkScript.addFill(deltaTime* multiplier);
				drinkScript.getFillBar()->setT(drinkScript.getFill());
				drinkScript.getFillBar()->updateArrow();
			}
			if (drinkScript.isOpening) {
				drinkScript.moveDrink(drinkScript.getT());

			}
			if (drinkScript.getT() >= 1.f) {
				drinkScript.setT(1.f);
				drinkScript.isOpening = false;
			}
			if (drinkScript.getT() <= 0.f) {
				drinkScript.setT(0.f);
				
				drinkScript.isClosing = false;
				drinkScript.isOpening = false;
			}
		}

		

		if (raycastHit && hitEntity != nullptr) {
			Entity* e = hitEntity;
			if (e->Has<Machine>()) {//check for fridge tomorrow
				
				if (e->Has<Fridge>()) {
					//log("B");
					//std::cout << "A" << std::endl;
					int wantedSlot = getWantedSlot();
					if (isClicking) {
						wantedSlot = getFirstTraySlot();
					}
					if (wantedSlot >= 0) {
						int slot = wantedSlot;
						
							trayPastry[slot] = Entity::Allocate().release();



							trayPastry[slot]->Add<CMeshRenderer>(*trayPastry[slot], *doughMat.getMesh(), *doughMat.getMaterial());
							trayPastry[slot]->Add<Pastry>();

							trayPastry[slot]->transform.m_scale = glm::vec3(0.009f, 0.009f, 0.009f);
							trayPastry[slot]->transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));

							//std::cout << traySlot[slot].m_pos.x << " " << traySlot[slot].m_pos.y << " " << traySlot[slot].m_pos.z << std::endl;

							trayPastry[slot]->transform.m_pos = traySlot[slot].m_pos;
							trayPastry[slot]->transform.SetParent(&globalCameraEntity->transform);
							addedSlot = slot;
							//std::cout << "B" << std::endl;

						
						
					}
						


					
				}
				else if (e->Has<Oven>()) {
					//log("A");
					//std::cout << "OVEN" << std::endl;
					int wantedSlot = getWantedSlot();
					int addingSlot = wantedSlot;
					if (wantedSlot >= 0) {
						//std::cout << "A" << std::endl;

						bool putInOven = false;
						if (trayPastry[wantedSlot] != nullptr) {
							if (!ovenScript->isSlotFull(wantedSlot)) {
								if (trayPastry[wantedSlot]->Has<Pastry>()) {
									if (trayPastry[wantedSlot]->Get<Pastry>().getPastryType() == bakeryUtils::pastryType::DOUGH)
									{
										if (!trayPastry[wantedSlot]->Get<Pastry>().isInOven()) {
											putInOven = true;
										}
									}

									}
								
							}
							else
							{
								int firstEmpty = ovenScript->getFirstEmpty();
								if (firstEmpty >= 0) {
									if (trayPastry[wantedSlot]->Has<Pastry>()) {
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
								setTrayPastryMesh(trayPastry[newSlot], trayPastry[newSlot]->Get<Pastry>().getPastryType());

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
					//std::cout << "FILLING" << std::endl;
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
									setTrayPastryMesh(trayPastry[wantedSlot], trayPastry[wantedSlot]->Get<Pastry>().getPastryType());

									trayPastry[wantedSlot]->Get<Pastry>().setInFilling(false);
									fillingScript.removeFromFilling();
									//UPDATE FILLING HERE
									setPastryFilling(trayPastry[wantedSlot], trayPastry[wantedSlot]->Get<Pastry>().getPastryFilling());
									//std::cout << bakeryUtils::getFillingName(trayPastry[wantedSlot]->Get<Pastry>().getPastryFilling()) << std::endl;
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
									if (trayPastry[wantedSlot]->Has<Pastry>()) {
										if (trayPastry[wantedSlot]->Get<Pastry>().getPastryType() != bakeryUtils::pastryType::DOUGH
											&& trayPastry[wantedSlot]->Get<Pastry>().getPastryFilling() == bakeryUtils::fillType::NONE)
										{
											if (!trayPastry[wantedSlot]->Get<Pastry>().isInFilling()) {
												putinFill = true;
											}
										}
									}
									
								}
							}
							if (putinFill) {

								//std::cout << "B" << std::endl;
								//ovenScript->canAdd(trayPastry[wantedSlot], wantedSlot);
								fillingScript.putInFilling(trayPastry[wantedSlot]);
								trayPastry[wantedSlot]->Get<Pastry>().setInFilling(true);
								setMachinePastryMesh(trayPastry[wantedSlot], trayPastry[wantedSlot]->Get<Pastry>().getPastryType());

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
									setTrayPastryMesh(trayPastry[wantedSlot], trayPastry[wantedSlot]->Get<Pastry>().getPastryType());

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
									setTrayPastryMesh(trayPastry[wantedSlot], trayPastry[wantedSlot]->Get<Pastry>().getPastryType());

									trayPastry[wantedSlot]->Get<Pastry>().setInTopping(false);
									toppingScript.removeFromTopping();
									//UPDATE FILLING HERE
									setPastryTopping(trayPastry[wantedSlot], trayPastry[wantedSlot]->Get<Pastry>().getPastryTopping());
									//std::cout << bakeryUtils::getToppingName(trayPastry[wantedSlot]->Get<Pastry>().getPastryTopping()) << std::endl;

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
									if (trayPastry[wantedSlot]->Has<Pastry>()) {
										if (trayPastry[wantedSlot]->Get<Pastry>().getPastryType() != bakeryUtils::pastryType::DOUGH
											&& trayPastry[wantedSlot]->Get<Pastry>().getPastryTopping() == bakeryUtils::toppingType::NONE)
										{
											if (!trayPastry[wantedSlot]->Get<Pastry>().isInTopping()) {
												putinTop = true;
											}
										}
									}
									
								}
							}
							if (putinTop) {

								//std::cout << "B" << std::endl;
								//ovenScript->canAdd(trayPastry[wantedSlot], wantedSlot);
								toppingScript.putInTopping(trayPastry[wantedSlot]);
								trayPastry[wantedSlot]->Get<Pastry>().setInTopping(true);
								setMachinePastryMesh(trayPastry[wantedSlot], trayPastry[wantedSlot]->Get<Pastry>().getPastryType());

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
									setTrayPastryMesh(trayPastry[wantedSlot], trayPastry[wantedSlot]->Get<Pastry>().getPastryType());

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
				else if (e->Has<DrinkMachine>()) {
				if (!drinkScript.isOpening && !drinkScript.isClosing && !drinkScript.isDrinkFull()) {
					if (isLeftButtonHeld) {
						drinkScript.addFill(deltaTime / bakeryUtils::getDrinkFillAmount());
						drinkScript.getFillBar()->setT(drinkScript.getFill());
						drinkScript.getFillBar()->updateArrow();
						
					}
					else if (scrollY != 0) {
						drinkScript.addDrinkNum(scrollY);
						drinkScript.updatePlane();
					}
					else 
					{
						 if (drinkScript.getFill() > 0.9f && drinkScript.getFill() < 1.05f
							&& (!drinkScript.isOpening || !drinkScript.isClosing)) {
							drinkScript.isOpening = true;
							drinkScript.isClosing = false;
							if (!drinkScript.isDrinkFull()) {
								drinkScript.createDrink();
								setDrinkMesh(drinkScript.getFromDrink(), drinkScript.getDrink());
								drinkScript.getFromDrink()->transform.m_scale = glm::vec3(0.2);
								drinkScript.getFromDrink()->Add<Drink>();
								drinkScript.getFromDrink()->Get<Drink>().setDrinkType(drinkScript.getDrink());
								renderingEntities.push_back(drinkScript.getFromDrink());
							}
							

							//create drink entity here
						}
						else
						{
							drinkScript.setFill(0.f);
							drinkScript.getFillBar()->setT(drinkScript.getFill());
							drinkScript.getFillBar()->updateArrow();

						}
						
					}
				}
				else if (drinkScript.isDrinkFull()) {
					if (getWantedSlot() != -1) {
						//put in tray
						//std::cout << getWantedSlot() << std::endl;
						int wantedSlot = getWantedSlot();
						if (trayPastry[wantedSlot] == nullptr) {
							trayPastry[wantedSlot] = drinkScript.releaseFromDrink();
							//setDrinkMesh(trayPastry[wantedSlot], trayPastry[wantedSlot]->Get<Drink>().getDrinkType());
							trayPastry[wantedSlot]->transform.m_pos = traySlot[wantedSlot].m_pos;
							trayPastry[wantedSlot]->transform.m_scale /= 5;

							trayPastry[wantedSlot]->transform.SetParent(&globalCameraEntity->transform);

							//trayPastry[wantedSlot]->transform.m_pos.y += getTrayRaise(trayPastry[wantedSlot]->Get<Pastry>().getPastryType());

							drinkScript.removeFromDrink();

							drinkScript.isClosing = true;
							drinkScript.isOpening = false;
						}
						
						

					}
					
				}
				
				}
				else if (e->Has<TrashCan>()) {
				//std::cout << "HERE" << std::endl;
				int wantedSlot = getWantedSlot();
				if (wantedSlot >= 0) {
					if (trayPastry[wantedSlot] != nullptr) {
						removeFromRendering(trayPastry[wantedSlot]);
						trayPastry[wantedSlot] = nullptr;
					}

				}
			}
				

			


			}
			else if (e->Has<Register>()) {
				//check if order is complete here
				if (isClicking) {
					for (int u = 0; u < currentOrders.size(); u++) {
						Order& o = currentOrders[u];
						int pastryIndex = -1;
						int drinkIndex = -1;
						for (int i = 0; i < std::size(trayPastry); i++) {
							Entity* tray = trayPastry[i];
							if (tray != nullptr) {
								if (tray->Has<Pastry>())
								{
									if (o.validateOrder(tray->Get<Pastry>())) {//for drinks just have a check here for if it has a pastry or not and to check if an order is completed, have an internal counter or something
										o.setPastryValidated(true);
										pastryIndex = i;
									}
								}
								
								if (tray->Has<Drink>()) {
									if (o.validateDrink(tray->Get<Drink>())) {
										o.setDrinkValidated(true);
										drinkIndex = i;
									}
								}
								
								if (o.returnSatisfied()) {
									if (pastryIndex != -1) {
										renderingEntities.erase(std::remove(renderingEntities.begin(), renderingEntities.end(), trayPastry[pastryIndex]), renderingEntities.end());

										trayPastry[pastryIndex] = nullptr;

									}
									if (drinkIndex != -1) {
										renderingEntities.erase(std::remove(renderingEntities.begin(), renderingEntities.end(), trayPastry[drinkIndex]), renderingEntities.end());

										trayPastry[drinkIndex] = nullptr;

									}
									
									bakeryUtils::addToRounds(1);
									std::cout << "HERE" << std::endl;
									createNewOrder(u, true);
									for each (Entity * remover in orderBubbles[u]->returnRenderingEntities()) {
										renderingEntities.erase(std::remove(renderingEntities.begin(), renderingEntities.end(), remover), renderingEntities.end());

									}

									resetBubble(u);

									for each (Entity * foe in orderBubbles[u]->returnRenderingEntities()) {
										renderingEntities.push_back(foe);
									}
								}
							}
							
							
						}
					}
				}
			}


		}
		

		//std::cout << bakeryUtils::returnBakeTime(bakeryUtils::pastryType::CROISSANT) << std::endl;
	
		if (addedSlot >= 0) {
			renderingEntities.push_back(trayPastry[addedSlot]);
		}

		if (orderBubblesToRemove.size() > 0) {

			for (int i = 0; i < orderBubblesToRemove.size(); i++) {//orderBubblesToRemove.size()
				
				for each (Entity * remover in orderBubbles[orderBubblesToRemove[i]]->returnRenderingEntities()) {
					renderingEntities.erase(std::remove(renderingEntities.begin(), renderingEntities.end(), remover), renderingEntities.end());
					
				}
				
				resetBubble(orderBubblesToRemove[i]);

				for each (Entity * foe in orderBubbles[orderBubblesToRemove[i]]->returnRenderingEntities()) {
					renderingEntities.push_back(foe);
				}
			}


			if ((bakeryUtils::getRoundsLasted() >= 4 && bakeryUtils::getDifficulty() >= 3 && currentOrders.size() == 1)
				) {//|| currentOrders.size() == 1
			
				//std::cout << "JJJ" << std::endl;
				createNewOrder(1, false,false);
				//orderBubbleTimers.push_back(&upurrTimer1);
				resetBubble(1);
				
				
				for each (Entity * foe in orderBubbles[1]->returnRenderingEntities()) {
					renderingEntities.push_back(foe);
				}
			}


			//if (bakeryUtils::getRoundsLasted() == 6 && bakeryUtils::getDifficulty() >= 3) {
			//std::cout << bakeryUtils::getRoundsLasted() << " " << bakeryUtils::getDifficulty() << std::endl;
			if ((bakeryUtils::getRoundsLasted() >= 7 && bakeryUtils::getDifficulty() >= 3 && currentOrders.size() == 2)
				) {//|| currentOrders.size() == 2
				//std::cout << "JJJ" << std::endl;
				createNewOrder(2, false, false);
				//orderBubbleTimers.push_back(&upurrTimer1);

				resetBubble(2);


				for each (Entity * foe in orderBubbles[2]->returnRenderingEntities()) {
					renderingEntities.push_back(foe);
				}
			}

		}
		orderBubblesToRemove.clear();
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

void removeFromRendering(Entity* e) {
	renderingEntities.erase(std::remove(renderingEntities.begin(), renderingEntities.end(), e), renderingEntities.end());

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
	
	if (cameraY <= -80) {
		cameraY = -79.9;
		deltaY = 0;
	}
	if (cameraY >= 80) {
		cameraY = 79.9;
		deltaY = 0;
	}
	deltaX += cameraX;
	deltaY += cameraY;
	

	//BELOW FROM https://gamedev.stackexchange.com/questions/13436/glm-euler-angles-to-quaternion
	
	cameraQuat = getCameraRotation();
	
	if (!isPaused && !isInMainMenu && !isInContinueMenu) {
		globalCameraEntity->transform.m_rotation = cameraQuat;
		globalCameraEntity->transform.m_pos = cameraPos;

		glfwSetCursorPos(gameWindow, width / 2, height / 2);
		cameraX = deltaX;
		cameraY = deltaY;
	}
	

	
	
	
	
}

void GetInput()
{
	
	double xRot, yRot;

	//glfwSetInputMode(gameWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	if (!isPaused) {
		glfwGetCursorPos(gameWindow, &xPos, &yPos);
		glfwSetCursorPosCallback(gameWindow, getCursorData);
	}

	

}
void getKeyInput() {
	if (Input::GetKeyDown(GLFW_KEY_UP))
	{
		isClickingUp = true;
		//std::cout << "YES" << std::endl;
	}
	else
	{
		isClickingUp = false;
	}

	if (Input::GetKeyDown(GLFW_KEY_DOWN))
	{
		isClickingDown = true;
	}
	else
	{
		isClickingDown = false;
	}

	if (Input::GetKeyDown(GLFW_KEY_LEFT))
	{
		isClickingLeft = true;
	}
	else
	{
		isClickingLeft = false;
	}

	if (Input::GetKeyDown(GLFW_KEY_RIGHT))
	{
		isClickingRight = true;
	}
	else
	{
		isClickingRight = false;
	}

	if (Input::GetKeyDown(GLFW_KEY_ENTER))
	{
		isClickingEnter = true;
	}
	else
	{
		isClickingEnter = false;
	}

	if (Input::GetKeyDown(GLFW_KEY_SPACE))
	{
		isClickingSpace = true;
	}
	else
	{
		isClickingSpace = false;
	}
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

void setTrayPastryMesh(Entity* e, bakeryUtils::pastryType type) {
	if (type == bakeryUtils::pastryType::CROISSANT) {
		e->Remove<CMeshRenderer>();
		e->Add<CMeshRenderer>(*e, *crossantMat.getMesh(), *crossantMat.getMaterial());
		e->transform.m_scale = glm::vec3(0.02f, 0.02f, 0.02f);
	}
	if (type == bakeryUtils::pastryType::COOKIE) {
		e->Remove<CMeshRenderer>();
		e->Add<CMeshRenderer>(*e, *cookieMat.getMesh(), *cookieMat.getMaterial());
		e->transform.m_scale = glm::vec3(0.02f, 0.02f, 0.02f);
	}
	if (type == bakeryUtils::pastryType::CUPCAKE) {
		e->Remove<CMeshRenderer>();
		e->Add<CMeshRenderer>(*e, *cupcakeMat.getMesh(), *cupcakeMat.getMaterial());
		e->transform.m_scale = glm::vec3(0.015f, 0.015f, 0.015f);
	}
	if (type == bakeryUtils::pastryType::BURNT) {
		e->Remove<CMeshRenderer>();
		e->Add<CMeshRenderer>(*e, *burntMat.getMesh(), *burntMat.getMaterial());
		e->transform.m_scale = glm::vec3(0.015f, 0.015f, 0.015f);
	}
	if (type == bakeryUtils::pastryType::CAKE) {
		e->Remove<CMeshRenderer>();
		e->Add<CMeshRenderer>(*e, *cakeMat.getMesh(), *cakeMat.getMaterial());
		e->transform.m_scale = glm::vec3(0.01f, 0.01f, 0.01f);
	}
}

void setDrinkMesh(Entity* e, bakeryUtils::drinkType type) {
	//std::cout << "A" << (int) type << std::endl;
	if (type == bakeryUtils::drinkType::COFFEE) {
		if (e->Has<CMeshRenderer>()) {
			e->Remove<CMeshRenderer>();
		}
		
		e->Add<CMeshRenderer>(*e, *coffeeMat.getMesh(), *coffeeMat.getMaterial());
		e->transform.m_scale = glm::vec3(0.02f, 0.02f, 0.02f);
	}
	if (type == bakeryUtils::drinkType::TEA) {
		if (e->Has<CMeshRenderer>()) {
			e->Remove<CMeshRenderer>();
		}
		e->Add<CMeshRenderer>(*e, *teaMat.getMesh(), *teaMat.getMaterial());
		e->transform.m_scale = glm::vec3(0.02f, 0.02f, 0.02f);
	}
	if (type == bakeryUtils::drinkType::MILKSHAKE) {
		if (e->Has<CMeshRenderer>()) {
			e->Remove<CMeshRenderer>();
		}
		e->Add<CMeshRenderer>(*e, *milkshakeMat.getMesh(), *milkshakeMat.getMaterial());
		e->transform.m_scale = glm::vec3(0.02f, 0.02f, 0.02f);
	}
}

void setMachinePastryMesh(Entity* e, bakeryUtils::pastryType type) {
	e->transform.m_scale *= 10;
}
float getTrayRaise(bakeryUtils::pastryType type) {
	float raise = 0;
	if (type == bakeryUtils::pastryType::CUPCAKE) {
		raise = 0.013;
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

MaterialCreator* getPastryTile(bakeryUtils::pastryType x) {
	if (x == bakeryUtils::pastryType::CROISSANT) {
		return &croissantTile;
	}
	if (x == bakeryUtils::pastryType::COOKIE) {
		return &cookieTile;
	}
	if (x == bakeryUtils::pastryType::CUPCAKE) {
		return &cupcakeTile;
	}
	if (x == bakeryUtils::pastryType::CAKE) {
		return &cakeTile;
	}
	if (x == bakeryUtils::pastryType::DOUGH) {
		return &doughTile;
	}
	return nullptr;
	
}

MaterialCreator* getFillingTile(bakeryUtils::fillType x) {
	if (x == bakeryUtils::fillType::CHOCOLATE) {
		return &nutellaFilling;
	}
	if (x == bakeryUtils::fillType::CUSTARD) {
		return &custardFilling;
	}
	if (x == bakeryUtils::fillType::JAM) {
		return &strawberryFilling;
	}
	
	return nullptr;

}

MaterialCreator* getToppingTile(bakeryUtils::toppingType x) {
	if (x == bakeryUtils::toppingType::PECAN) {
		return &pecanTopping;
	}
	if (x == bakeryUtils::toppingType::SPRINKLE) {
		return &sprinkleTopping;
	}
	if (x == bakeryUtils::toppingType::STRAWBERRY) {
		return &stawberryTopping;
	}

	return nullptr;

}

MaterialCreator* getDrinkTile(bakeryUtils::drinkType x) {
	if (x == bakeryUtils::drinkType::COFFEE) {
		return &coffeeTile;
	}
	if (x == bakeryUtils::drinkType::MILKSHAKE) {
		return &milkshakeTile;
	}
	if (x == bakeryUtils::drinkType::TEA) {
		return &teaTile;
	}

	return nullptr;
}

bool isInRendering(Entity* e) {
	if (std::find(renderingEntities.begin(), renderingEntities.end(), e) != renderingEntities.end()) {
		return true;
	}
	return false;
}

void createNewOrder(int i, bool addDifficulty, bool remove) {
	if (addDifficulty) {
		bakeryUtils::addToDifficulty(1);
	}
	
	if (i >= currentOrders.size()) {
		currentOrders.push_back(Order());
	}
	currentOrders[i] = Order();
	currentOrders[i].createOrder(bakeryUtils::getDifficulty());
	currentOrders[i].startOrder();
	
	
	if (remove) {
		orderBubblesToRemove.push_back(i);
	}
	

}

void resetBubble(int i) {
	orderBubbles[i]->clearRenderingEntities();
	orderBubbles[i]->setTransform(*orderBubbleTransform[i]);
	orderBubbleTimers[i]->setFill(0);
	orderBubbleTimers[i]->updateArrow();
	orderBubbles[i]->setupTimer(orderBubbleTimers[i]);
	orderBubbles[i]->setTiles(getPastryTile(currentOrders[i].type), getFillingTile(currentOrders[i].filling), getToppingTile(currentOrders[i].topping), getDrinkTile(currentOrders[i].drink));
	orderBubbles[i]->setup(&bubbleTile, &plusTile);
	orderBubbles[i]->create(currentOrders[i]);
}

glm::quat getCameraRotation() {
	glm::quat QuatAroundX = glm::angleAxis(glm::radians((float)cameraY), glm::vec3(1.0, 0.0, 0.0));
	glm::quat QuatAroundY = glm::angleAxis(glm::radians((float)cameraX), glm::vec3(0.0, 1.0, 0.0));
	glm::quat QuatAroundZ = glm::angleAxis(glm::radians(0.f), glm::vec3(0.0, 0.0, 1.0));
	glm::quat finalOrientation = QuatAroundY * QuatAroundX * QuatAroundZ;
	return finalOrientation;
}


void moveCamera(int direction) {
	float deltaTime = App::GetDeltaTime();

	cameraT += deltaTime;
	
	if (cameraT >= 1) {
		cameraT = 0.f;
		currentCameraPoint += direction;
	}

	int index = keepInBounds(currentCameraPoint, 0, 3);
	
	
	globalCameraEntity->transform.m_pos = Catmull(cameraKeys[keepInBounds(4 - currentCameraPoint, 0, 3)]
		, cameraKeys[keepInBounds(4 - (currentCameraPoint + 1), 0, 3)],
		cameraKeys[keepInBounds(4 - (currentCameraPoint + 2), 0, 3)],
		cameraKeys[keepInBounds(4 - (currentCameraPoint + 3), 0, 3)], cameraT);
	globalCameraEntity->transform.m_rotation = glm::slerp(currentCameraQuat, wantedCameraQuat, (cameraT + currentCameraPoint) / 3);
	globalCameraEntity->Get<CCamera>().Update();
}

int getSignSelection(int max, bool reset) {
	if (reset) {
		selectedOption = 0;
	}
	if (max != 0) {
		if (isClickingUp) {
			selectedOption += (max - 1);
			selectedOption = selectedOption % max;
		}
		if (isClickingDown) {
			selectedOption++;
			selectedOption = selectedOption % max;
		}
	}
	

	

	if (isClickingSpace) {
		
		return selectedOption;

	}
	return -1;
}





