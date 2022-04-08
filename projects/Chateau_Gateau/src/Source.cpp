

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
#include "CharacterController.h" 
#include "MorphAnimation.h" 
#include "CPathAnimator.h" 
#include "Light.h" 
#include "Transparency.h" 
#include "PictureSelector.h" 
#include "MusicPlayer.h"

#include <algorithm> 
#include <math.h> 
#include <ctime> 


#include <iostream> 
#include <fstream> 
#include <cereal/archives/json.hpp> 

#include "imgui.h" 

#include <memory> 
#include "OrderBubble.h" 
#include "Register.h" 

#include <unordered_map> 
#include<chrono> 

#include "Audio.h"
#include "TutorialStep.h"

#include "ToneFire.h"
#include "Music.h"

#include <windows.h> 

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

// TODO: Templated Bezier function 
template<typename T>
T Bezier(const T& p0, const T& p1, const T& p2, const T& p3, float t)
{
	glm::vec3 l10 = Lerp(p1, p0, t);
	glm::vec3 l32 = Lerp(p3, p2, t);
	glm::vec3 l03 = Lerp(p0, p3, t);

	glm::vec3 l10_l03 = Lerp(l10, l03, t);
	glm::vec3 l10_l32 = Lerp(l10, l32, t);

	glm::vec3 finalL = Lerp(l10_l03, l10_l32, t);

	return finalL;
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


template<typename T, typename X>
bool isInVector(std::vector<T> a, X e) {
	if (std::find(a.begin(), a.end(), e) != a.end()) {
		return true;
	}
	return false;
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
float tempD = 0.f; //(-0.5, -1.22, -2.29f 

float lineY = -1.89;//-1.1f; 
std::unique_ptr<ShaderProgram> prog_texLit, prog_lit, prog_unlit, prog_morph, prog_particles, prog_transparent, prog_UI,
prog_allLights, prog_RB;
std::unique_ptr<Material>  mat_unselected, mat_selected, mat_line;
glm::vec3 cameraPos = glm::vec3(-1.f, -0.5f, -0.0f);
glm::vec3 menuCameraPos = glm::vec3(-0.7f, -1.2f, -10.7f);
glm::vec3 insidePos = glm::vec3(cameraPos.x - 0.8, cameraPos.y, cameraPos.z);
glm::vec3 outsidePos = glm::vec3(menuCameraPos.x - 0.90, (cameraPos.y + menuCameraPos.y) / 2, menuCameraPos.z + 0.6);
std::vector<glm::vec3> cameraKeys = std::vector<glm::vec3>();

std::vector<MaterialCreator*> registerImages;
Entity* ent_register;
Entity* tablet;

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
float sensitivity = -0.15;
bool isInMainMenu = true;
bool isInPauseMenu = false;
bool isInContinueMenu = false;
bool isInOptionsMenu = false;
bool isInTutorialMenu = false;
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
bool isClickingEscape = false;
bool isScrollingUp = false;
bool isScrollingDown = false;

//Mouse State 
bool firstMouse = true;
int width = 1600;//1600�~900 
int height = 900;
float lastX = width / 2;
float lastY = height / 2;
float scrollX;
float scrollY;
float cameraT = 0;
int currentCameraPoint = 0;
bool isCameraMoving = false;

std::vector<Entity*> renderingEntities = std::vector<Entity*>();
std::vector<Entity*> UIEntities = std::vector<Entity*>();
Transform traySlot[4] = {};

//std::unique_ptr<Entity> trayPastry[4] = {nullptr, nullptr, nullptr, nullptr}; 
std::vector<Order> orders; //new Mithunan 
std::vector<OrderBubble*> orderBubbles;
std::vector<Transform*> orderBubbleTransform;
std::vector<OvenTimer*> orderBubbleTimers;
std::vector<int> orderBubblesToRemove;
Entity* trayPastry[4] = { nullptr, nullptr, nullptr, nullptr };
std::vector<Mesh*> fillingFrames = std::vector<Mesh*>();
std::vector<Material*> signFrames = std::vector<Material*>();
std::vector<Mesh*> drinkFrames = std::vector<Mesh*>();
std::vector<glm::vec2> mouseMovements;


GLuint trayKeys[4] = { GLFW_KEY_1 ,GLFW_KEY_2 ,GLFW_KEY_3 ,GLFW_KEY_4 };
float soundVolume = 50;
float musicVolume = 50;
float UIScale = 0.95;//1.35 

std::unordered_map<GLuint, int> alphanumeric;
std::vector<MaterialCreator> alphanumericMat;
std::vector<MaterialCreator> sliderMat;
std::vector<MaterialCreator> booleanMat;
std::vector<MaterialCreator> resoloutionMat;

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
bool isInUI(Entity* e);
void setDrinkMesh(Entity* e, bakeryUtils::drinkType type);
void loadAnimationData(std::vector<Mesh*>& toModify, std::string prefix, int count);
int placeInLineToIndex(int linePlace);
int indexToPlaceInLine(int index);
void setTutorialChange(int i);
void UpdateTutorial();
void loadSettings();
void loadNumberHashMap();
GLuint pictureIndexToGLuint(int i);
int GLuintToPictureIndex(GLuint);
int getWhichKeyPressed();
void saveSettings();
void loadSettings();
void applySettings();
int selectedOvenPosition(float x);
void loadMaterialCreatorData(std::vector<MaterialCreator*>& toModify, std::string meshName, std::string prefix, int count);
void loadMaterialCreatorData(std::vector<MaterialCreator*>& toModify, Mesh& meshName, std::string prefix, int count);
void nextStepTutorialIfNeeded();
int getHighscore();
glm::vec3 getTrayScale(bakeryUtils::pastryType type);
// Function to handle user inputs 
void GetInput();
void getKeyInput();
void applyResolution();


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
MaterialCreator restartSignMat = MaterialCreator();
MaterialCreator mainMenuSignMat = MaterialCreator();
MaterialCreator tutorialNoSignMat = MaterialCreator();
MaterialCreator tutorialYesSignMat = MaterialCreator();


MaterialCreator optionTraySignMat = MaterialCreator();
MaterialCreator optionSensitivitySignMat = MaterialCreator();
MaterialCreator optionMusicSignMat = MaterialCreator();
MaterialCreator optionSoundSignMat = MaterialCreator();
MaterialCreator optionEnlargeSignMat = MaterialCreator();
MaterialCreator optionFullscreenSignMat = MaterialCreator();
MaterialCreator optionExitSignMat = MaterialCreator();
MaterialCreator optionKeybindSignMat = MaterialCreator();
MaterialCreator optionKeybindDoneSignMat = MaterialCreator();

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

MaterialCreator carMat = MaterialCreator();

std::unique_ptr<Material> pecanParticle;
std::unique_ptr<Material> sprinkleParticle;
std::unique_ptr<Material> strawberryParticle;
std::unique_ptr<Material> confettiParticle;


Transform customerBubbleLocation;
Transform upurrBubbleLocation1;
Transform upurrBubbleLocation2;

Transform orderTut; //MJ 
Transform fridgeTut;
Transform ovenTut;
Transform customerTut;
std::vector<Transform> tutorialArray;
std::vector<MaterialCreator*> dialogueList;
std::unique_ptr<Entity> tutorialPlane;
std::vector<TutorialStep> tutorialSteps;
float tutorialT = 0.f;
int tutorialPos = 0;
int tutorialImage = 0;
std::vector<float> tutorialPeriods;
bool shouldShowTutorial = true;
float tutorialMultiplier = 1;


std::vector<std::vector<MaterialCreator*>> pastryMats;
/*0,0 = croissant plain (with topping enum 0)
* 0,1 = croissant filling 1 with topping enum 1
* 0,2 = croissant filling 2 with topping enum 2
* 1,0 = cookie plain
* ...
*/



MaterialCreator* getPastryTile(bakeryUtils::pastryType x);
MaterialCreator* getFillingTile(bakeryUtils::fillType x);
MaterialCreator* getToppingTile(bakeryUtils::toppingType x);
MaterialCreator* getDrinkTile(bakeryUtils::drinkType x);
Mesh& createPastryMat(Entity* e);
Texture2D& createItemTex(Entity* e);
void removeFromRendering(Entity* e);
void removeFromUI(Entity* e);
void resetBubble(int i, bool create = true);
glm::vec3 trayScale;
glm::vec3 cursorScale;
void createNewOrder(int i, bool addDifficulty, bool remove = true);
std::vector<glm::vec3> line;
int lineStart = 4;
glm::vec3 firstCarPos = glm::vec3(5, -1.9, 10);
glm::vec3 lastCarPos = glm::vec3(-15, -1.9, 10);
Entity* customerLine[3];
std::vector<Entity*> customers;
float lastActionTime = 0.f;
std::vector<MaterialCreator*> tabletMats;

float currentGameTime = 0;
int difficulty = 1;
std::vector<Order> currentOrders = std::vector<Order>();
std::vector<glm::vec3> beginingNumberPos;
std::vector<glm::vec3> endNumberPos;

bool isCarMoving = false;
float carT = 0.f;
float dayT = 0.0f;
float dayBright = 0.9f;
float dayDark = 0.2;

MaterialCreator copyMaterials[4];

Transform accessStart[5];
Transform accessTray[10];
Entity* accessEntities[5];
int accessButtonPressed = -1;
int accessSettings[9] = { 1,2,3,4,3,3,3,0,0 };
bool isInOption = false;
glm::vec3 accessScale = glm::vec3(0.01f, 0.01f, 0.01f);
glm::vec2 resoloutions[3] = {glm::vec2(1280,720),glm::vec2(1600,900), glm::vec2(1920,1080)};

std::vector<MaterialCreator> numberTiles;
std::vector<Entity*> numberEntities;

glm::vec3 numberScale;
void setScores(int totalOrders, int highscore);
int saveHighscore(int);
void restartGame();
Entity* ovenEntites[4];
float ovenHeights[4];
std::vector<Mesh*> allOvenFrames;
std::vector<Mesh*> fridgeFrames;

void log(std::string s) {
	std::cout << s << std::endl;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

bool isPaused = false;

Mesh signMesh;
Mesh tileMesh;
Mesh registerMesh;

Transform normalTrayPos;
Transform lowTrayPos;
float trayT = 0;
float trayMultiplier = 0;

ToneFire::StudioSound tick;
ToneFire::StudioSound click;
void playSound(ToneFire::StudioSound* sound, std::string name);
void playMusic(ToneFire::StudioSound* music, std::string name);
float currentConfusion = 0.f;
float confusionThreshold = 7.f;
float modifiedConfusionThreshold = 7.f;

unsigned int textureColorbuffer;
unsigned int framebuffer;
unsigned int rbo;
glm::vec2 screenRes;
float blurT = 0.f;
float blurTime = 1.f;
float blurMulti = 0;
glm::vec2 blurBounds = glm::vec2(0, 10);
bool shouldUseGraphics = true;



void addToRendering(Entity*);

std::vector<Music*> songList = std::vector<Music*>();
void chooseAndPlaySong();
//std::vector<glm::vec4> confettiColours = std::vector<glm::vec4>();
//std::vector<std::unique_ptr<Entity>> confettiEmitters = std::vector<std::unique_ptr<Entity>>();
bool confettiEmission = false;
float confettiEmissionTime = 2.f;
float currentConfettiEmission = 0.f;
int main()
{
	
	auto startTime = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < 6; i++) {
		endNumberPos.push_back(glm::vec3(0));
		beginingNumberPos.push_back(glm::vec3(0));
	}

	cameraKeys.push_back(cameraPos);
	cameraKeys.push_back(insidePos);
	cameraKeys.push_back(outsidePos);
	cameraKeys.push_back(menuCameraPos);
	line.push_back(glm::vec3(3.5, lineY, -11.4));
	line.push_back(glm::vec3(0.f, lineY, -11.4));
	line.push_back(glm::vec3(-2.1, lineY, -8.8));
	line.push_back(glm::vec3(-1.1, lineY, -7.7));
	line.push_back(glm::vec3(-1.1, lineY, -5.3));
	line.push_back(glm::vec3(-1.1, lineY, -3.8));
	line.push_back(glm::vec3(-1.2, lineY, -3.5));
	line.push_back(glm::vec3(-1.9, lineY, -3.2));
	line.push_back(glm::vec3(-2.9, lineY, -4.8));
	line.push_back(glm::vec3(-2.9, lineY, -10.5));
	line.push_back(glm::vec3(-7.6, lineY, -11.4));
	PathSampler::Lerp = Lerp<glm::vec3>;
	PathSampler::Catmull = Catmull<glm::vec3>;
	PathSampler::Bezier = Bezier<glm::vec3>;
	srand(static_cast<unsigned int>(time(0)));
	loadSettings();
	loadNumberHashMap();
	// Create window and set clear color 
	App::Init("Chateau Gateau", width, height);
	App::SetClearColor(glm::vec4(1, 0.964, 0.929, 1.f));
	//App::SetClearColor(glm::vec4(1, 0, 1, 1.f)); 
	
	App::setCursorVisible(false);
	gameWindow = glfwGetCurrentContext();

	// Initialize ImGui 
	App::InitImgui();

	//const GLubyte* vend = glGetString(GL_VENDOR); // Returns the vendor
	const GLubyte* rend = glGetString(GL_RENDERER); // Returns a hint to the model
	
	std::string render = std::string((char*) rend);
	
	//render = "Intel (R) Epic Graphics";
	std::cout << render << std::endl;
	if (render.find("Intel") != std::string::npos) {
		shouldUseGraphics = false;
	}
	if (render.find("Integrated") != std::string::npos) {
		shouldUseGraphics = false;
	}
	if (render.find("APU") != std::string::npos) {
		shouldUseGraphics = false;
	}

	try
	{
		// Load in our model/texture resources 
		LoadDefaultResources();
	}
	catch (const std::exception& e) {
		system("Colour 6");
		std::cout << "ERROR LOADING SHADERS: " << e.what() << std::endl;
		return 1;
	}
	if (prog_UI == nullptr || prog_RB == nullptr || prog_morph == nullptr || prog_texLit == nullptr) {
		system("Colour 6");
		std::cout << "One or more shaders could not load. Check that your computer can run OpenGL version 430 core or later." << std::endl;
		return 1;
	}
	
	

	//this is to save loading time, not memory!
	//cant really save memory without reworking OTTER itself lol

	GLTF::LoadMesh("bakery/models/tile.gltf", tileMesh);
	GLTF::LoadMesh("UI/models/Chalkboard.gltf", signMesh);
	GLTF::LoadMesh("bakery/models/cashregister.gltf", registerMesh);

	glfwSetMouseButtonCallback(gameWindow, mouse_button_callback);
	glfwSetCursorPosCallback(gameWindow, getCursorData);
	glfwSetScrollCallback(gameWindow, scroll_callback);
	glfwSwapInterval(1);
	//App::Tick(); 

	//Audio audioEngine;
	//audioEngine.Init();
	//audioEngine.loadSound("ambient1", "audio/Duel of the Fates.wav",true,true,false);
	//ToneFire::FMODStudio::FMODStudio(512, "./audio/") ;
	ToneFire::Listener listener = ToneFire::Listener();
	ToneFire::FMODStudio studio = ToneFire::FMODStudio(512, "./audio/", listener);
	//ToneFire::FMODStudio::FMODStudio(512, "./audio/");
	//ToneFire::FMODStudio::FMODStudio(512,"./audio/") studio;
	
	studio.LoadBank("Master.bank");
	studio.LoadBank("Master.strings.bank");
	ToneFire::StudioSound menuBGM;
	menuBGM.LoadEvent("event:/TitleMusic");
	ToneFire::StudioSound pauseBGM;
	pauseBGM.LoadEvent("event:/PauseMusic");
	ToneFire::StudioSound toppingS;
	toppingS.LoadEvent("event:/Topping");
	ToneFire::StudioSound pop;
	pop.LoadEvent("event:/DoughTray");
	ToneFire::StudioSound ding;
	ding.LoadEvent("event:/CashRegister");
	ToneFire::StudioSound printShortS;
	printShortS.LoadEvent("event:/printTick");
	ToneFire::StudioSound printLongS;
	printLongS.LoadEvent("event:/printLong");
	ToneFire::StudioSound machineS;
	machineS.LoadEvent("event:/machineOn");
	ToneFire::StudioSound machineST;
	machineST.LoadEvent("event:/machineOn2");
	ToneFire::StudioSound fridgeDS;
	fridgeDS.LoadEvent("event:/FridgeDoor");
	ToneFire::StudioSound fridgeS;
	fridgeS.LoadEvent("event:/fridgeHum");
	ToneFire::StudioSound ovenS;
	ovenS.LoadEvent("event:/oven");
	ToneFire::StudioSound drinkUpS;
	drinkUpS.LoadEvent("event:/DrinkMachine");
	ToneFire::StudioSound carS;
	carS.LoadEvent("event:/Honk");
	ToneFire::StudioSound closeCallS;
	closeCallS.LoadEvent("event:/ticking");
	ToneFire::StudioSound musicOneS;
	musicOneS.LoadEvent("event:/Song1");
	


	tick.LoadEvent("event:/Tick");
	
	click.LoadEvent("event:/TickTick");
	Music titleMusic = Music(menuBGM, "TitleMusic", 38.f);
	Music pauseMusic = Music(pauseBGM, "PauseMusic", 41.f);
	Music toppingSound = Music(toppingS, "Topping", 20.f, true);
	Music machineSound = Music(machineS, "machineOn", 13.f, true);
	Music machineSound2 = Music(machineST, "machineOn2", 13.f, true);
	Music printShort = Music(printShortS, "printTick", 0.25, true);
	Music printLong = Music(printLongS, "printLong", 0.39, true);
	Music fridgeOpenSound = Music(fridgeS, "FridgeDoor", 0.4, true);
	Music fridgeHumSound = Music(fridgeDS, "fridgeHum", 9.2, true);
	Music ovenSound = Music(ovenS, "oven", 21.1, true);
	Music drinkUpSound = Music(drinkUpS, "DrinkMachine", 10.6, true);
	Music closeCallSound = Music(closeCallS, "ticking", 4.32, true);
	Music carSound = Music(carS, "Honk", 0.45, true);
	Music musicOne = Music(musicOneS, "Song1", 46.83);


	
	songList.push_back(&musicOne);
	songList.push_back(&titleMusic);

	//drinkUpSound.fadeIn(0.3);
	//drinkUpSound.update(0);
	
	// Create and set up camera 
	Entity cameraEntity = Entity::Create();
	CCamera& cam = cameraEntity.Add<CCamera>(cameraEntity);
	cam.Perspective(60.0f, (float)width / height, 0.001f, 100.0f);
	//cam.Perspective(100.f, (float) width/height, 0.1f, 100.0f); 
	cameraEntity.transform.m_pos = cameraPos;
	cameraEntity.transform.m_rotation = glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f));
	globalCameraEntity = &cameraEntity;
	cameraEntity.transform.m_pos = menuCameraPos;


	standardCameraQuat = getCameraRotation();
	wantedCameraQuat = standardCameraQuat;
	cameraX = 90.f;
	cameraY = 0.f;
	menuCameraQuat = getCameraRotation();
	currentCameraQuat = menuCameraQuat;

	cameraEntity.transform.m_rotation = menuCameraQuat;
	cameraEntity.transform.RecomputeGlobal();
	cameraEntity.Get<CCamera>().Update();

	applyResolution();

	MaterialCreator loading = MaterialCreator();
	loading.createMaterial(tileMesh, "UI/textures/LoadingScreen.png", *prog_UI);
	
	Entity loadingEntity = Entity::Create();
	loadingEntity.Add<CMeshRenderer>(loadingEntity, *loading.getMesh(), *loading.getMaterial());
	loadingEntity.transform.m_scale = glm::vec3(0.41, 0.41, 0.41);
	loadingEntity.transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f))
		* glm::angleAxis(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f))
		* glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 0.0f, 1.0f));
	loadingEntity.transform.m_pos = glm::vec3(-1.1, -1.2f, -10.7);
	loadingEntity.transform.RecomputeGlobal();
	App::FrameStart();
	App::SwapBuffers();
	loadingEntity.Get<CMeshRenderer>().Draw();
	App::SwapBuffers();
	//addToRendering(&loadingEntity); 
	//loadingEntity.Get<CMeshRenderer>().Draw(); 


	MaterialCreator cursorMat = MaterialCreator();
	cursorMat.createMaterial("UI/cursor.gltf", "UI/cursor.png", *prog_texLit);//there isnt supposed to be an image on purpose

	

	MaterialCreator registerMaterial = MaterialCreator();
	registerMaterial.createMaterial(registerMesh, "bakery/textures/cregister0.png", *prog_texLit);
	registerImages.push_back(&registerMaterial);
	MaterialCreator registerMaterial1 = MaterialCreator();
	registerMaterial1.createMaterial(registerMesh, "bakery/textures/cregister1.png", *prog_texLit);
	registerImages.push_back(&registerMaterial1);
	MaterialCreator registerMaterial2 = MaterialCreator();
	registerMaterial2.createMaterial(registerMesh, "bakery/textures/cregister2.png", *prog_texLit);
	registerImages.push_back(&registerMaterial2);
	MaterialCreator registerMaterial3 = MaterialCreator();
	registerMaterial3.createMaterial(registerMesh, "bakery/textures/cregister3.png", *prog_texLit);
	registerImages.push_back(&registerMaterial3);
	//loadMaterialCreatorData(registerImages, registerMesh, "bakery/textures/register",3, *prog_allLights); 

	MaterialCreator counterMat = MaterialCreator();
	counterMat.createMaterial("bakery/models/counter.gltf", "bakery/textures/counter.png", *prog_texLit);

	MaterialCreator fridgePosterMat = MaterialCreator();
	fridgePosterMat.createMaterial("bakery/models/poster.gltf", "bakery/textures/fridge poster.png", *prog_texLit);

	MaterialCreator ovenPosterMat = MaterialCreator();
	ovenPosterMat.createMaterial("bakery/models/poster.gltf", "bakery/textures/oven poster.png", *prog_texLit);

	MaterialCreator paintingMat = MaterialCreator();
	paintingMat.createMaterial("bakery/models/Painting.gltf", "bakery/textures/Painting.png", *prog_texLit);

	MaterialCreator furnitureMat = MaterialCreator();
	furnitureMat.createMaterial("bakery/models/interiorFurniture.gltf", "bakery/textures/furnitureTex.png", *prog_texLit);
/*
	MaterialCreator boothMat = MaterialCreator();
	boothMat.createMaterial("bakery/models/booth.gltf", "bakery/textures/boothTableTex.png", *prog_texLit);
	*/
	MaterialCreator tabletMat0 = MaterialCreator();
	tabletMat0.createMaterial("bakery/models/tablet.gltf", "bakery/textures/tablet0.png", *prog_texLit);
	MaterialCreator tabletMat1 = MaterialCreator();
	tabletMat1.createMaterial("bakery/models/tablet.gltf", "bakery/textures/tablet1.png", *prog_texLit);
	MaterialCreator tabletMat2 = MaterialCreator();
	tabletMat2.createMaterial("bakery/models/tablet.gltf", "bakery/textures/tablet2.png", *prog_texLit);
	tabletMats.push_back(&tabletMat0);
	tabletMats.push_back(&tabletMat1);
	tabletMats.push_back(&tabletMat2);
	MaterialCreator trayMat = MaterialCreator();
	trayMat.createMaterial("bakery/models/tray.gltf", "bakery/textures/tray.png", *prog_texLit);
	
	MaterialCreator fridgeClosedMat = MaterialCreator();
	fridgeClosedMat.createMaterial("bakery/models/closedFridge.gltf", "bakery/textures/fridgeTexture.png", *prog_morph);

	MaterialCreator fridgeOpenMat = MaterialCreator();
	fridgeOpenMat.createMaterial("bakery/models/openFridge.gltf", "bakery/textures/fridgeTexture.png", *prog_morph);

	fridgeFrames.push_back(fridgeClosedMat.getMesh().get());
	fridgeFrames.push_back(fridgeOpenMat.getMesh().get());

	MaterialCreator binMat = MaterialCreator();
	binMat.createMaterial("bakery/models/trash.gltf", "bakery/textures/trash.png", *prog_texLit);

	MaterialCreator ovenMat = MaterialCreator();
	ovenMat.createMaterial("bakery/models/legs.gltf", "bakery/textures/ovenTexture.png", *prog_texLit);

	MaterialCreator toppingMat = MaterialCreator();
	toppingMat.createMaterial("bakery/models/toppingMachine.gltf", "bakery/textures/topping.png", *prog_texLit);

	MaterialCreator fillingMat1 = MaterialCreator();
	fillingMat1.createMaterial("bakery/models/fillingMachine1.gltf", "bakery/textures/fillingMachine.png", *prog_morph);

	MaterialCreator fillingMat2 = MaterialCreator();//for morphs 
	fillingMat2.createMaterial("bakery/models/fillingMachine2.gltf", "bakery/textures/fillingMachine.png", *prog_morph);
	fillingFrames.push_back(fillingMat1.getMesh().get());
	fillingFrames.push_back(fillingMat2.getMesh().get());

	MaterialCreator barMat1 = MaterialCreator();
	barMat1.createMaterial("bakery/models/fillbar1.gltf", "bakery/textures/drinkMachine.png", *prog_morph);
	MaterialCreator barMat2 = MaterialCreator();
	barMat2.createMaterial("bakery/models/fillbar2.gltf", "bakery/textures/drinkMachine.png", *prog_morph);



	MaterialCreator drinkMat1 = MaterialCreator();//for morphs 
	drinkMat1.createMaterial("bakery/models/drinkMachineClosed.gltf", "bakery/textures/drinkMachine.png", *prog_morph);

	MaterialCreator drinkMat2 = MaterialCreator();//for morphs 
	drinkMat2.createMaterial("bakery/models/drinkMachineOpen.gltf", "bakery/textures/drinkMachine.png", *prog_morph);



	drinkFrames.push_back(drinkMat1.getMesh().get());
	drinkFrames.push_back(drinkMat2.getMesh().get());


	for (int i = 0; i < 10; i++) {
		numberTiles.push_back(MaterialCreator());
		std::string newNum = "UI/textures/Number_" + std::to_string(i) + ".png";
		numberTiles.back().createMaterial(tileMesh, newNum, *prog_UI);
	}
	std::string orderToCheck = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	for (char& c : orderToCheck) {
		alphanumericMat.push_back(MaterialCreator());
		std::string newString = "UI/textures/alphanumeric/Key" + std::string(1, c) + ".png";
		alphanumericMat.back().createMaterial(tileMesh, newString, *prog_texLit);

	}
	booleanMat.push_back(MaterialCreator());
	booleanMat.back().createMaterial(tileMesh, "UI/textures/symbols/typeCross.png", *prog_texLit);
	booleanMat.push_back(MaterialCreator());
	booleanMat.back().createMaterial(tileMesh, "UI/textures/symbols/typeCheck.png", *prog_texLit);

	sliderMat.push_back(MaterialCreator());
	sliderMat.back().createMaterial(tileMesh, "UI/textures/symbols/typeLow.png", *prog_texLit);
	sliderMat.push_back(MaterialCreator());
	sliderMat.back().createMaterial(tileMesh, "UI/textures/symbols/typeMidLow.png", *prog_texLit);
	sliderMat.push_back(MaterialCreator());
	sliderMat.back().createMaterial(tileMesh, "UI/textures/symbols/typeMiddle.png", *prog_texLit);
	sliderMat.push_back(MaterialCreator());
	sliderMat.back().createMaterial(tileMesh, "UI/textures/symbols/typeMidHigh.png", *prog_texLit);
	sliderMat.push_back(MaterialCreator());
	sliderMat.back().createMaterial(tileMesh, "UI/textures/symbols/typeHigh.png", *prog_texLit);

	resoloutionMat.push_back(MaterialCreator());
	resoloutionMat.back().createMaterial(tileMesh, "UI/textures/symbols/resolution720.png", *prog_texLit);
	resoloutionMat.push_back(MaterialCreator());
	resoloutionMat.back().createMaterial(tileMesh, "UI/textures/symbols/resolution900.png", *prog_texLit);
	resoloutionMat.push_back(MaterialCreator());
	resoloutionMat.back().createMaterial(tileMesh, "UI/textures/symbols/resolution1080.png", *prog_texLit);
	resoloutionMat.push_back(MaterialCreator());
	resoloutionMat.back().createMaterial(tileMesh, "UI/textures/symbols/resolutionFullscreen.png", *prog_texLit);


	MaterialCreator keyDownMat = MaterialCreator();
	keyDownMat.createMaterial("bakery/models/keyPressed.gltf", "bakery/textures/plusTile.png", *prog_texLit);

	MaterialCreator keyUpMat = MaterialCreator();
	keyUpMat.createMaterial("bakery/models/keyUp.gltf", "bakery/textures/plusTile.png", *prog_texLit);


	MaterialCreator cityMat = MaterialCreator();
	cityMat.createMaterial("bakery/models/cityMesh.gltf", "bakery/textures/cityTexture.png", *prog_texLit);

	MaterialCreator bakeryTopMat = MaterialCreator();
	bakeryTopMat.createMaterial("bakery/models/bakeryTop.gltf", "bakery/textures/roofTexture.png", *prog_texLit);

	MaterialCreator timerMat = MaterialCreator();
	timerMat.createMaterial("bakery/models/timer.gltf", "bakery/textures/timer.png", *prog_texLit);

	MaterialCreator arrowMat = MaterialCreator();
	arrowMat.createMaterial("bakery/models/arrow.gltf", "bakery/textures/arrow.png", *prog_texLit);

	MaterialCreator ovenDial = MaterialCreator();
	ovenDial.createMaterial("bakery/models/dial.gltf", "bakery/textures/ovenTexture.png", *prog_texLit);




	MaterialCreator ovenOpenMat1 = MaterialCreator();
	ovenOpenMat1.createMaterial("bakery/models/ovenopen1.gltf", "bakery/textures/ovenTexture.png", *prog_morph);

	MaterialCreator ovenOpenMat2 = MaterialCreator();
	ovenOpenMat2.createMaterial("bakery/models/ovenopen2.gltf", "bakery/textures/ovenTexture.png", *prog_morph);

	MaterialCreator ovenOpenMat3 = MaterialCreator();
	ovenOpenMat3.createMaterial("bakery/models/ovenopen3.gltf", "bakery/textures/ovenTexture.png", *prog_morph);

	MaterialCreator ovenOpenMat4 = MaterialCreator();
	ovenOpenMat4.createMaterial("bakery/models/ovenopen4.gltf", "bakery/textures/ovenTexture.png", *prog_morph);

	MaterialCreator ovenClosedMat = MaterialCreator();
	ovenClosedMat.createMaterial("bakery/models/ovenclosed.gltf", "bakery/textures/ovenTexture.png", *prog_morph);

	allOvenFrames.push_back(ovenClosedMat.getMesh().get());
	allOvenFrames.push_back(ovenOpenMat1.getMesh().get());
	allOvenFrames.push_back(ovenOpenMat2.getMesh().get());
	allOvenFrames.push_back(ovenOpenMat3.getMesh().get());
	allOvenFrames.push_back(ovenOpenMat4.getMesh().get());



	MaterialCreator bakeryMat = MaterialCreator();
	bakeryMat.createMaterial("bakery/models/interiorJoined.gltf", "bakery/textures/bakeryTexture.png", *prog_texLit);

	MaterialCreator receiptMat = MaterialCreator();
	receiptMat.createMaterial(tileMesh, "UI/textures/Receipt.png", *prog_UI);

	MaterialCreator plexiMat = MaterialCreator();
	plexiMat.createMaterial("bakery/models/plexiGlass.gltf", "bakery/textures/plexiGlass.png", *prog_texLit);

	


	std::unique_ptr<Texture2D> particleTex = std::make_unique<Texture2D>("bakery/textures/particle.png");
	std::unique_ptr<Material> particleMat = std::make_unique<Material>(*prog_particles);
	particleMat->AddTexture("albedo", *particleTex);


	std::string toppingAddon[4] = { "Plain","Pecan","Sprinkle","Strawberry" };
	std::string fillingAddon[4] = { "Plain","Vanilla","Chocolate","Strawberry" };
	std::string pastryNames[4] = { "croissant","cookie","cupcake","cake" };
	for (int i = 0; i < 4; i++) {
		pastryMats.push_back(std::vector<MaterialCreator*>());

		for (int u = 0; u < 4; u++) {
			//MaterialCreator temp = MaterialCreator(); 
			std::string toppingName = "pastries/models/" + pastryNames[i] + toppingAddon[u] + ".gltf";
			std::string fillingName = "pastries/textures/" + pastryNames[i] + fillingAddon[u] + ".png";
			pastryMats[i].push_back(new MaterialCreator());
			pastryMats[i].back()->createMaterial(toppingName, fillingName, *prog_texLit);
			if (i == 1) {
				//pastryMats[i].back()->addTexture("Nmap", "bakery/textures/cookieNormal.png");
			}
			
			//temp.createMaterial(toppingName, toppingName, *prog_transparent); 
			//pastryMeshes[i].push_back(temp.getMesh().get()); 
			//pastryTextures[i].push_back(temp.getTexture().get()); 
			//std::cout << toppingName << std::endl; 
			//std::cout << fillingName << std::endl; 
		}


	}//cookie NMAP change
	//crossantMat = *pastryMats[0][0]; 
	crossantMat.createMaterial("pastries/models/croissantPlain.gltf", "pastries/textures/croissantPlain.png", *prog_texLit);
	doughMat.createMaterial("bakery/models/dough.gltf", "bakery/textures/dough.png", *prog_texLit);
	
	cookieMat.createMaterial("pastries/models/cookiePlain.gltf", "pastries/textures/cookiePlain.png", *prog_texLit);
	//cookieMat.addTexture("Nmap", "bakery/textures/cookieNormal.png");
	cupcakeMat.createMaterial("pastries/models/cupcakePlain.gltf", "pastries/textures/cupcakePlain.png", *prog_texLit);
	cakeMat.createMaterial("pastries/models/cakePlain.gltf", "pastries/textures/cakePlain.png", *prog_texLit);
	burntMat.createMaterial("bakery/models/dust.gltf", "bakery/textures/dust.png", *prog_texLit);
	
	coffeeTile.createMaterial(tileMesh, "bakery/textures/coffeeTile.png", *prog_texLit);
	teaTile.createMaterial(tileMesh, "bakery/textures/teaTile.png", *prog_texLit);
	milkshakeTile.createMaterial(tileMesh, "bakery/textures/milkshakeTile.png", *prog_texLit);

	coffeeMat.createMaterial("bakery/models/coffee.gltf", "bakery/textures/coffee.png", *prog_texLit);
	teaMat.createMaterial("bakery/models/bubbletea.gltf", "bakery/textures/tea.png", *prog_texLit);
	milkshakeMat.createMaterial("bakery/models/milkshake.gltf", "bakery/textures/milkshake.png", *prog_texLit);

	playSignMat.createMaterial(signMesh, "UI/textures/playSign.png", *prog_texLit);
	settingsSignMat.createMaterial(signMesh, "UI/textures/settingsSign.png", *prog_texLit);
	exitSignMat.createMaterial(signMesh, "UI/textures/exitSign.png", *prog_texLit);
	pauseSignMat.createMaterial(signMesh, "UI/textures/pauseSign.png", *prog_texLit);
	restartSignMat.createMaterial(signMesh, "UI/textures/restartSign.png", *prog_texLit);

	mainMenuSignMat.createMaterial(signMesh, "UI/textures/mainMenuSign.png", *prog_texLit);
	tutorialYesSignMat.createMaterial(signMesh, "UI/textures/tutorialYesSign.png", *prog_texLit);
	tutorialNoSignMat.createMaterial(signMesh, "UI/textures/tutorialNoSign.png", *prog_texLit);


	optionTraySignMat.createMaterial(signMesh, "UI/textures/optionTraySign.png", *prog_texLit);
	optionSensitivitySignMat.createMaterial(signMesh, "UI/textures/optionSensitivitySign.png", *prog_texLit);
	optionMusicSignMat.createMaterial(signMesh, "UI/textures/optionMusicSign.png", *prog_texLit);
	optionSoundSignMat.createMaterial(signMesh, "UI/textures/optionSoundSign.png", *prog_texLit);
	optionEnlargeSignMat.createMaterial(signMesh, "UI/textures/optionEnlargeSign.png", *prog_texLit);
	optionFullscreenSignMat.createMaterial(signMesh, "UI/textures/optionDisplaySign.png", *prog_texLit);
	optionExitSignMat.createMaterial(signMesh, "UI/textures/optionExitSign.png", *prog_texLit);

	optionKeybindSignMat.createMaterial(signMesh, "UI/textures/optionKeybindSign.png", *prog_texLit);
	optionKeybindDoneSignMat.createMaterial(signMesh, "UI/textures/optionKeybindDoneSign.png", *prog_texLit);

	signFrames.push_back(playSignMat.getMaterial().get());
	signFrames.push_back(settingsSignMat.getMaterial().get());
	signFrames.push_back(exitSignMat.getMaterial().get());
	signFrames.push_back(pauseSignMat.getMaterial().get());
	signFrames.push_back(restartSignMat.getMaterial().get());
	signFrames.push_back(mainMenuSignMat.getMaterial().get());

	signFrames.push_back(tutorialYesSignMat.getMaterial().get());
	signFrames.push_back(tutorialNoSignMat.getMaterial().get());


	signFrames.push_back(optionTraySignMat.getMaterial().get());
	signFrames.push_back(optionSensitivitySignMat.getMaterial().get());
	signFrames.push_back(optionMusicSignMat.getMaterial().get());
	signFrames.push_back(optionSoundSignMat.getMaterial().get());
	signFrames.push_back(optionEnlargeSignMat.getMaterial().get());
	signFrames.push_back(optionFullscreenSignMat.getMaterial().get());
	signFrames.push_back(optionExitSignMat.getMaterial().get());
	signFrames.push_back(optionKeybindSignMat.getMaterial().get());
	signFrames.push_back(optionKeybindDoneSignMat.getMaterial().get());


	croissantTile.createMaterial(tileMesh, "bakery/textures/croissantTile.png", *prog_texLit);
	doughTile.createMaterial(tileMesh, "bakery/textures/doughTile.png", *prog_texLit);
	cookieTile.createMaterial(tileMesh, "bakery/textures/cookieTile.png", *prog_texLit);
	cupcakeTile.createMaterial(tileMesh, "bakery/textures/cupcakeTile.png", *prog_texLit);
	cakeTile.createMaterial(tileMesh, "bakery/textures/cakeTile.png", *prog_texLit);
	nothingTile.createMaterial(tileMesh, "bakery/textures/nothingTile.png", *prog_texLit);
	burntTile.createMaterial(tileMesh, "bakery/textures/burntTile.png", *prog_texLit);

	custardFilling.createMaterial(tileMesh, "bakery/textures/custardFilling.png", *prog_texLit);
	nutellaFilling.createMaterial(tileMesh, "bakery/textures/nutellaFilling.png", *prog_texLit);
	strawberryFilling.createMaterial(tileMesh, "bakery/textures/strawberryFilling.png", *prog_texLit);

	pecanTopping.createMaterial(tileMesh, "bakery/textures/pecanTopping.png", *prog_texLit);
	stawberryTopping.createMaterial(tileMesh, "bakery/textures/strawberryTopping.png", *prog_texLit);
	sprinkleTopping.createMaterial(tileMesh, "bakery/textures/sprinkleTopping.png", *prog_texLit);

	plusTile.createMaterial(tileMesh, "bakery/textures/plusTile.png", *prog_texLit);
	bubbleTile.createMaterial(tileMesh, "bakery/textures/bubbleTile.png", *prog_texLit);

	carMat.createMaterial("bakery/models/car.gltf", "bakery/textures/car.png", *prog_texLit);


	std::unique_ptr<Texture2D> pecanTex = std::make_unique<Texture2D>("bakery/textures/pecanParticle.png");
	pecanParticle = std::make_unique<Material>(*prog_particles);
	pecanParticle->AddTexture("albedo", *pecanTex);

	std::unique_ptr<Texture2D> strawberryTex = std::make_unique<Texture2D>("bakery/textures/strawberryParticle.png");
	strawberryParticle = std::make_unique<Material>(*prog_particles);
	strawberryParticle->AddTexture("albedo", *strawberryTex);

	std::unique_ptr<Texture2D> sprinkleTex = std::make_unique<Texture2D>("bakery/textures/sprinkleParticle.png");
	sprinkleParticle = std::make_unique<Material>(*prog_particles);
	sprinkleParticle->AddTexture("albedo", *sprinkleTex);

	MaterialCreator confettiMat;
	confettiMat.createMaterial(tileMesh, "bakery/textures/particle.png", *prog_particles);
	



	Entity bakery = Entity::Create();
	bakery.Add<CMeshRenderer>(bakery, *bakeryMat.getMesh(), *bakeryMat.getMaterial());
	bakery.transform.m_scale = glm::vec3(0.7, 0.7, 0.7);
	bakery.transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f));
	bakery.transform.m_pos = glm::vec3(-2.4, -1.9, -4.3);
	addToRendering(&bakery);

	Entity city = Entity::Create();
	city.Add<CMeshRenderer>(city, *cityMat.getMesh(), *cityMat.getMaterial());
	city.transform.m_scale = glm::vec3(0.560);
	city.transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f));
	city.transform.m_pos = glm::vec3(-2.060, -1.710, -5.240);
	addToRendering(&city);

	Entity bakeryTop = Entity::Create();
	bakeryTop.Add<CMeshRenderer>(bakeryTop, *bakeryTopMat.getMesh(), *bakeryTopMat.getMaterial());
	bakeryTop.transform.m_scale = glm::vec3(0.8, 0.8, 0.8);
	bakeryTop.transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f));
	bakeryTop.transform.m_pos = glm::vec3(-3.1, 1.4, -9.1);
	addToRendering(&bakeryTop);

	Entity car = Entity::Create();
	car.Add<CMeshRenderer>(car, *carMat.getMesh(), *carMat.getMaterial());
	car.transform.m_scale = glm::vec3(0.55, 0.55, 0.55);
	car.transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec3 carPos = menuCameraPos;
	car.transform.m_pos = glm::vec3(carPos.x + 0.5, -1.0f, carPos.z - 2.2);

	addToRendering(&car);


	Entity sign = Entity::Create();
	sign.Add<CMeshRenderer>(sign, *playSignMat.getMesh(), *playSignMat.getMaterial());
	sign.transform.m_scale = glm::vec3(0.15, 0.2, 0.15);
	sign.transform.m_rotation = glm::angleAxis(glm::radians(270.f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec3 signPos = menuCameraPos;
	sign.transform.m_pos = glm::vec3(signPos.x - 0.5, -1.55f, signPos.z + 0.2);

	addToRendering(&sign);


	Entity cursor = Entity::Create();
	cursor.Add<CMeshRenderer>(cursor, *cursorMat.getMesh(), *cursorMat.getMaterial());
	cursorScale = glm::vec3(0.001f, 0.001f, 0.001f);
	cursor.transform.m_scale = cursorScale;
	cursor.transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
	cursor.transform.m_pos = glm::vec3(-1.f, -3.f, -3.0f);
	addToRendering(&cursor);



	Entity receipt = Entity::Create();
	receipt.Add<CMeshRenderer>(receipt, *receiptMat.getMesh(), *receiptMat.getMaterial());
	receipt.transform.m_scale = glm::vec3(0.15, 0.15, 0.15);
	receipt.transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f))
		* glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 0.0f, 1.0f));//0 



	Entity fridgePoster = Entity::Create();
	fridgePoster.Add<CMeshRenderer>(fridgePoster, *fridgePosterMat.getMesh(), *fridgePosterMat.getMaterial());
	fridgePoster.transform.m_scale = glm::vec3(1.270);
	fridgePoster.transform.m_pos = glm::vec3(-3.700, -0.12, -1.090);
	fridgePoster.transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f))
		* glm::angleAxis(glm::radians(-90.f), glm::vec3(0.0f, 0.0f, 1.0f));//0 
	addToRendering(&fridgePoster);

	Entity ovenPoster = Entity::Create();
	ovenPoster.Add<CMeshRenderer>(ovenPoster, *ovenPosterMat.getMesh(), *ovenPosterMat.getMaterial());
	ovenPoster.transform.m_scale = glm::vec3(1.270);
	ovenPoster.transform.m_pos = glm::vec3(1.700, -0.092, -1.090);
	ovenPoster.transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f))
		* glm::angleAxis(glm::radians(90.f), glm::vec3(0.0f, 0.0f, 1.0f));//0 
	addToRendering(&ovenPoster);


	Entity painting = Entity::Create();
	painting.Add<CMeshRenderer>(painting, *paintingMat.getMesh(), *paintingMat.getMaterial());
	painting.transform.m_scale = glm::vec3(0.510);
	painting.transform.m_pos = glm::vec3(1.660, -0.640, -4.110);
	painting.transform.m_rotation = glm::angleAxis(glm::radians(0.f), glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::angleAxis(glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f))
		* glm::angleAxis(glm::radians(00.f), glm::vec3(0.0f, 0.0f, 1.0f));//0 
	addToRendering(&painting);

	


		//Creating Cash Register Entity 
	ent_register = Entity::Allocate().release();
	ent_register->Add<CMeshRenderer>(*ent_register, *registerMaterial.getMesh(), *registerMaterial.getMaterial());
	ent_register->Add<Register>();
	ent_register->transform.m_scale = glm::vec3(0.45f, 0.45f, 0.45f);
	ent_register->transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
	ent_register->transform.m_pos = glm::vec3(-1.6, -2.59, -2.29f);
	ent_register->Add<BoundingBox>(glm::vec3(2.3, 3.3, 0.06), *ent_register);
	addToRendering(ent_register);

	customerBubbleLocation = ent_register->transform;
	customerBubbleLocation.m_scale = glm::vec3(0.4f);
	customerBubbleLocation.m_pos.x -= (1.75 + (((UIScale + 0.05) - 0.8)));
	customerBubbleLocation.m_pos.y += 2.0;
	customerBubbleLocation.m_pos.z -= 0.9;


	upurrBubbleLocation2 = ent_register->transform;
	upurrBubbleLocation2.m_scale = glm::vec3(0.4f);

	upurrBubbleLocation2.m_pos.x += 1.85;
	upurrBubbleLocation2.m_pos.y += 2.6;
	upurrBubbleLocation2.m_pos.z -= 1.0;

	upurrBubbleLocation1 = ent_register->transform;
	upurrBubbleLocation1.m_scale = glm::vec3(0.4f);

	upurrBubbleLocation1.m_pos.x += 1.85;
	upurrBubbleLocation1.m_pos.y += 2.0;
	upurrBubbleLocation1.m_pos.z -= 1.0;


	Entity chairs = Entity::Create();
	chairs.Add<CMeshRenderer>(chairs, *furnitureMat.getMesh(), *furnitureMat.getMaterial());
	chairs.transform = bakery.transform;
	addToRendering(&chairs);
	/*
	Entity booth = Entity::Create();
	booth.Add<CMeshRenderer>(booth, *boothMat.getMesh(), *boothMat.getMaterial());
	booth.transform.m_scale = glm::vec3(0.750);
	booth.transform.m_rotation = glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f));
	booth.transform.m_pos = glm::vec3(-0.020, -1.900, -7.370);

	//addToRendering(&booth);
	*/
	tablet = Entity::Allocate().release();
	tablet->Add<CMeshRenderer>(*tablet, *tabletMat0.getMesh(), *tabletMat0.getMaterial());
	tablet->transform.m_scale = glm::vec3(0.630);
	tablet->transform.m_rotation = glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f));
	tablet->transform.m_pos = glm::vec3(-0.750, -1.180, -2.190);

	addToRendering(tablet);

	Entity counter = Entity::Create();
	counter.Add<CMeshRenderer>(counter, *counterMat.getMesh(), *counterMat.getMaterial());
	counter.transform.m_scale = glm::vec3(0.492);
	counter.transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
	counter.transform.m_pos = glm::vec3(-1.000, -2.032, -2.388);
	addToRendering(&counter);



	Entity bin = Entity::Create();
	bin.Add<CMeshRenderer>(bin, *binMat.getMesh(), *binMat.getMaterial());
	bin.Add<Machine>();
	bin.Add<TrashCan>();
	bin.transform.m_scale = glm::vec3(0.320);
	bin.transform.m_rotation = glm::angleAxis(glm::radians(-28.200f), glm::vec3(0.0f, 1.0f, 0.0f));
	bin.transform.m_pos = glm::vec3(-3.f, -1.890, -1.100);
	bin.Add<BoundingBox>(glm::vec3(0.5, 1, 0.5), bin);
	glm::vec3 binOrigin = bin.transform.m_pos;
	bin.Get<BoundingBox>().setOrigin(glm::vec3(binOrigin.x, binOrigin.y, binOrigin.z-0.4));
	addToRendering(&bin);
	//	std::cout << bin.Get<BoundingBox>().getOrigin().x << " " << bin.Get<BoundingBox>().getOrigin().y << 
		//	" " << bin.Get<BoundingBox>().getOrigin().z << std::endl; 

	Entity fridge = Entity::Create();
	fridge.Add<CMorphMeshRenderer>(fridge, *fridgeClosedMat.getMesh(), *fridgeClosedMat.getMaterial());

	fridge.Add<Machine>();
	fridge.Add<Fridge>();
	fridge.transform.m_scale = glm::vec3(0.850);
	fridge.transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f));
	fridge.transform.m_pos = glm::vec3(-3.020, -1.870, 0.270);
	fridge.Add<BoundingBox>(glm::vec3(0.67, 4, 0.5), fridge);
	addToRendering(&fridge);
	auto& animatorFridge = fridge.Add<CMorphAnimator>(fridge);
	animatorFridge.SetFrameTime(1.0f);
	animatorFridge.SetFrames(fridgeFrames);

	Entity oven = Entity::Create();
	oven.Add<CMorphMeshRenderer>(oven, *ovenClosedMat.getMesh(), *ovenClosedMat.getMaterial());

	oven.Add<Machine>();
	oven.Add<Oven>();
	oven.transform.m_scale = glm::vec3(0.075f, 0.075f, 0.075f);
	oven.transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
	oven.transform.m_pos = glm::vec3(0.5f, -1.91f, 0.100f);
	oven.Add<BoundingBox>(glm::vec3(0.51, 2, 0.35), oven);
	addToRendering(&oven);
	auto& animatorOven = oven.Add<CMorphAnimator>(oven);
	animatorOven.SetFrameTime(1.0f);
	animatorOven.SetFrames(allOvenFrames);


	/*


	for (int i = 0; i < 4; i++) {
		ovenEntites[i] = Entity::Allocate().release();
		ovenEntites[i]->transform = oven.transform;
		ovenEntites[i]->transform.m_pos.y += 0.19 + (i * 0.3);
		ovenEntites[i]->transform.m_scale= glm::vec3(0.3);
		ovenEntites[i]->transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));

		ovenEntites[i]->Add<CMorphMeshRenderer>(*ovenEntites[i], *ovenClosedMat.getMesh(), *ovenClosedMat.getMaterial());
		ovenEntites[i]->Add<MorphAnimation>(closingFrames,0.3f,0);
		addToRendering(ovenEntites[i]);

		auto& animatordrink = ovenEntites[i]->Add<CMorphAnimator>(*ovenEntites[i]);
		animatordrink.SetFrameTime(1.0f);
		animatordrink.SetFrames(closingFrames);
	}

	*/
	Entity filling = Entity::Create();
	filling.Add<CMorphMeshRenderer>(filling, *fillingMat1.getMesh(), *fillingMat1.getMaterial());

	filling.Add<Machine>();
	filling.Add<FillingMachine>();
	filling.Add<Transparency>(filling);
	filling.transform.m_scale = glm::vec3(0.340f);
	filling.transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f));
	filling.transform.m_pos = glm::vec3(0.3f, -1.9f, 2.0f);
	filling.Add<BoundingBox>(glm::vec3(0.72, 2, 0.35), filling);
	glm::vec3 fillingPos = filling.transform.m_pos;
	filling.Get<BoundingBox>().setOrigin(glm::vec3(fillingPos.x + 0.4, fillingPos.y, fillingPos.z));
	addToRendering(&filling);

	auto& animator = filling.Add<CMorphAnimator>(filling);
	animator.SetFrameTime(1.0f);
	animator.SetFrames(fillingFrames);



	filling.Get<FillingMachine>().setup(&custardFilling, &nutellaFilling, &strawberryFilling);

	Entity fillingPlane = Entity::Create();
	
	fillingPlane.Add<CMeshRenderer>(fillingPlane, *custardFilling.getMesh(), *custardFilling.getMaterial());
	fillingPlane.Add<Transparency>(fillingPlane);
	fillingPlane.transform.m_scale = glm::vec3(0.16f);
	fillingPlane.transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::angleAxis(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f));;
	glm::vec3 fillPos = filling.transform.m_pos;
	fillingPlane.transform.m_pos = glm::vec3(0.308, 0.077, 1.663);
	addToRendering(&fillingPlane);
	filling.Get<FillingMachine>().setFillingPlane(&fillingPlane);

	Transform fillTransform = filling.transform;
	fillTransform.m_pos.y = filling.transform.m_pos.y + 1.02;//0.2
	fillTransform.m_pos.x += 0;
	//fillTransform.m_pos.z += 0.3; 
	filling.Get<FillingMachine>().setTransform(fillTransform);
	

	Entity tester = Entity::Create();
	tester.Add<CMeshRenderer>(tester, *cursorMat.getMesh(), *cursorMat.getMaterial());
	//cursorScale = glm::vec3(0.001f, 0.001f, 0.001f);
	tester.transform.m_scale = glm::vec3(0.1f, 0.1f, 0.1f);
	tester.transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
	tester.transform.m_pos = glm::vec3(-1.f, -3.f, -3.0f);
	//addToRendering(&tester);

	Entity drink = Entity::Create();
	drink.Add<CMorphMeshRenderer>(drink, *drinkMat1.getMesh(), *drinkMat1.getMaterial());

	drink.Add<Machine>();
	drink.Add<DrinkMachine>();
	drink.Add<Transparency>(drink);
	drink.transform.m_scale = glm::vec3(0.370);
	drink.transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f));
	drink.transform.m_pos = glm::vec3(-0.900, -1.900, 2.0f);
	drink.Add<BoundingBox>(glm::vec3(0.32, 2, 0.35), drink);//filling????
	glm::vec3 drinkPos = drink.transform.m_pos;
	drink.Get<BoundingBox>().setOrigin(glm::vec3(drinkPos.x - 0.0, drinkPos.y, drinkPos.z));
	addToRendering(&drink);

	auto& animatordrink = drink.Add<CMorphAnimator>(drink);
	animatordrink.SetFrameTime(1.0f);
	animatordrink.SetFrames(drinkFrames);

	Transform drinkBarPos = Transform();
	drinkBarPos.m_pos.z = 1.900;
	drinkBarPos.m_pos.y = -1.770;
	drinkBarPos.m_pos.x = -0.960;
	drinkBarPos.m_scale = glm::vec3(0.340);
	drinkBarPos.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f)); 

	FillBar drinkFill;
	drinkFill.setup(barMat1, barMat2, drinkBarPos);
	Transform outDrinkTrans = Transform();
	outDrinkTrans.m_pos.z = 1.700;
	outDrinkTrans.m_pos.y = -0.840;
	outDrinkTrans.m_pos.x = -1.000;
	Transform inDrinkTrans = Transform();
	inDrinkTrans.m_pos.z = 2.000;
	inDrinkTrans.m_pos.y = -0.840;
	inDrinkTrans.m_pos.x = -1.000;
	drink.Get<DrinkMachine>().setTransform(inDrinkTrans, outDrinkTrans);
	drink.Get<DrinkMachine>().setup(&coffeeTile, &milkshakeTile, &teaTile, &drinkFill);
	drinkFill.getEntity()->Add<Transparency>(*drinkFill.getEntity());
	addToRendering(drinkFill.getEntity());


	//filling.Get<FillingMachine>().setup(&custardFilling, &nutellaFilling, &strawberryFilling); 

	Entity drinkPlane = Entity::Create();
	drinkPlane.Add<CMeshRenderer>(drinkPlane, *coffeeTile.getMesh(), *coffeeTile.getMaterial());
	drinkPlane.Add<Transparency>(drinkPlane);
	drinkPlane.transform.m_scale = glm::vec3(0.14f);
	drinkPlane.transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::angleAxis(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f));;
	glm::vec3 drinkTilePos = drink.transform.m_pos;
	drinkPlane.transform.m_pos = glm::vec3(-1.010, -0.010, 1.500);
	addToRendering(&drinkPlane);
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
	topping.Add<Transparency>(topping);
	topping.Add<Machine>();
	topping.Add<ToppingMachine>();
	topping.transform.m_scale = glm::vec3(0.351);
	topping.transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f));
	topping.transform.m_pos = glm::vec3(-2.0, -1.900, 2.0f);
	topping.Add<BoundingBox>(glm::vec3(0.4f, 2, 0.5), topping);

	glm::vec3 tempTopPos = topping.transform.m_pos;
	//tempTopPos.x += 0.0; 
	//topping.Get<BoundingBox>().setOrigin(tempTopPos); 

	addToRendering(&topping);

	Transform topParticleTransform = topping.transform;
	//topParticleTransform.m_pos.y += 0.38;
	//topParticleTransform.m_pos.x -= 0.05; 
	topParticleTransform.m_pos = glm::vec3(-1.971, -0.704, 1.765);
	topping.Get<ToppingMachine>().setParticleTransform(topParticleTransform);

	Transform toppingTL = topping.transform;
	toppingTL.m_pos.y += 0.77;
	toppingTL.m_pos.x -= 0.22;
	toppingTL.m_pos.z -= 0.2;


	Transform toppingTR = topping.transform;
	toppingTR.m_pos.y += 0.77;
	toppingTR.m_pos.x += 0.22;
	toppingTR.m_pos.z -= 0.2;

	topping.Get<ToppingMachine>().setup(&pecanTopping, &sprinkleTopping, &stawberryTopping);
	topping.Get<ToppingMachine>().setTransform(toppingTL, toppingTR);
	glm::vec3 pecanColour = glm::vec3(0.59,0.29,0);
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
	addToRendering(&particleEntity);


	Entity toppingPlane = Entity::Create();
	toppingPlane.Add<CMeshRenderer>(toppingPlane, *pecanTopping.getMesh(), *pecanTopping.getMaterial());
	toppingPlane.Add<Transparency>(toppingPlane);
	toppingPlane.transform.m_scale = glm::vec3(0.22f);
	toppingPlane.transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::angleAxis(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f));;
	//glm::vec3 topPos = topping.transform.m_pos;
	toppingPlane.transform.m_pos = glm::vec3(-2.070, 0.160, 1.650);
	addToRendering(&toppingPlane);
	topping.Get<ToppingMachine>().setTopPlane(&toppingPlane);



	//Characters below 
	std::vector<MorphAnimation*> allMithunanFrames;

	std::vector<Mesh*> mithunanWalkFrames;
	loadAnimationData(mithunanWalkFrames, "characters/mithunan/walk/bearWalk", 16);
	MorphAnimation mithunanWalk = MorphAnimation(mithunanWalkFrames, 0.1, 0);
	allMithunanFrames.push_back(&mithunanWalk);

	std::vector<Mesh*> mithunanIdleFrames;
	loadAnimationData(mithunanIdleFrames, "characters/mithunan/idle/bearIdle", 4);
	MorphAnimation mithunanIdle = MorphAnimation(mithunanIdleFrames, 1, 0);
	allMithunanFrames.push_back(&mithunanIdle);

	std::unique_ptr<Texture2D> mithunanTex = std::make_unique<Texture2D>("characters/mithunan/mithunan.png");
	std::unique_ptr<Material> mithunanMat = std::make_unique<Material>(*prog_morph);
	mithunanMat->AddTexture("albedo", *mithunanTex);



	Entity mithunan = Entity::Create();
	{


		mithunan.Add<CMorphMeshRenderer>(mithunan, *mithunanWalkFrames[0], *mithunanMat.get());
		mithunan.Add<CPathAnimator>(mithunan);
		mithunan.Get<CPathAnimator>().SetMode(PathSampler::PathMode::CATMULL);
		mithunan.transform.m_scale = glm::vec3(0.44f, 0.44f, 0.44f);
		mithunan.transform.m_rotation = glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::angleAxis(glm::radians(00.f), glm::vec3(1.0f, 0.0f, 0.0f));
		mithunan.transform.m_pos = glm::vec3(-1.f, -5.5, -2.29f);
		mithunan.Add<CharacterController>(&mithunan, allMithunanFrames, line);
		mithunan.Get<CharacterController>().setStopSpot(placeInLineToIndex(1));

		mithunan.Get<CharacterController>().setDistance(placeInLineToIndex(2));
		mithunan.Get<CharacterController>().setCurrentSpot(placeInLineToIndex(2));
		//mithunan.Get<CharacterController>().updateDistance(0.0001, 1); 


		//mithunan.Get<CharacterController>().continueAnimation(false); 
		auto& mithunanAnimator = mithunan.Add<CMorphAnimator>(mithunan);
		mithunanAnimator.SetFrameTime(mithunanWalk.getFrameTime());
		mithunanAnimator.SetFrames(mithunanWalkFrames);
	}

	addToRendering(&mithunan);

	std::unique_ptr<Texture2D> kainatTex = std::make_unique<Texture2D>("characters/mithunan/kainat.png");
	std::unique_ptr<Material> kainatMat = std::make_unique<Material>(*prog_morph);
	kainatMat->AddTexture("albedo", *kainatTex);

	std::vector<MorphAnimation*> allKainatFrames;

	//std::vector<Mesh*> kainatWalkFrames;
	//loadAnimationData(kainatWalkFrames, "characters/kainat/walk/bearWalk", 16);
	MorphAnimation kainatWalk = MorphAnimation(mithunanWalkFrames, 0.1, 0);
	allKainatFrames.push_back(&kainatWalk);
	//std::vector<Mesh*> kainatIdleFrames;
	//loadAnimationData(kainatIdleFrames, "characters/kainat/idle/bearIdle", 4);
	MorphAnimation kainatIdle = MorphAnimation(mithunanIdleFrames, 0.5, 0);
	allKainatFrames.push_back(&kainatIdle);
	Entity kainat = Entity::Create();
	{


		kainat.Add<CMorphMeshRenderer>(kainat, *mithunanWalkFrames[0], *kainatMat.get());
		kainat.Add<CPathAnimator>(kainat);
		kainat.Get<CPathAnimator>().SetMode(PathSampler::PathMode::CATMULL);
		kainat.transform.m_scale = glm::vec3(0.44f, 0.44f, 0.44f);
		kainat.transform.m_rotation = glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::angleAxis(glm::radians(00.f), glm::vec3(1.0f, 0.0f, 0.0f));
		kainat.transform.m_pos = glm::vec3(-1.f, -5.5, -2.29f);
		kainat.Add<CharacterController>(&kainat, allKainatFrames, line);

		kainat.Get<CharacterController>().setStopSpot(placeInLineToIndex(2));

		kainat.Get<CharacterController>().setDistance(placeInLineToIndex(3));
		kainat.Get<CharacterController>().setCurrentSpot(placeInLineToIndex(3));
		//mithunan.Get<CharacterController>().continueAnimation(false); 
		auto& kainatAnimator = kainat.Add<CMorphAnimator>(kainat);
		kainatAnimator.SetFrameTime(kainatWalk.getFrameTime());
		kainatAnimator.SetFrames(mithunanWalkFrames);
	}
	addToRendering(&kainat);

	std::vector<MorphAnimation*> allmarkFrames;
	//std::vector<Mesh*> markWalkFrames;
	//loadAnimationData(markWalkFrames, "characters/mark/walk/bearWalk", 16);
	MorphAnimation markWalk = MorphAnimation(mithunanWalkFrames, 0.1, 0);
	allmarkFrames.push_back(&markWalk);
	//std::vector<Mesh*> markIdleFrames;
	//loadAnimationData(markIdleFrames, "characters/mark/idle/bearIdle", 4);
	MorphAnimation markIdle = MorphAnimation(mithunanIdleFrames, 0.5, 0);
	allmarkFrames.push_back(&markIdle);
	std::unique_ptr<Texture2D> markTex = std::make_unique<Texture2D>("characters/mithunan/mark.png");
	std::unique_ptr<Material> markMat = std::make_unique<Material>(*prog_morph);
	markMat->AddTexture("albedo", *markTex);
	Entity mark = Entity::Create();
	{


		mark.Add<CMorphMeshRenderer>(mark, *mithunanWalkFrames[0], *markMat.get());
		mark.Add<CPathAnimator>(mark);
		mark.Get<CPathAnimator>().SetMode(PathSampler::PathMode::CATMULL);
		mark.transform.m_scale = glm::vec3(0.44f, 0.44f, 0.44f);
		mark.transform.m_rotation = glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::angleAxis(glm::radians(00.f), glm::vec3(1.0f, 0.0f, 0.0f));
		mark.transform.m_pos = glm::vec3(-1.f, -5.5, -2.29f);
		mark.Add<CharacterController>(&mark, allmarkFrames, line);
		mark.Get<CharacterController>().setStopSpot(placeInLineToIndex(3));
		//mark.Get<CharacterController>().setDistance(placeInLineToIndex(4)); 

		//mithunan.Get<CharacterController>().continueAnimation(false); 
		auto& markAnimator = mark.Add<CMorphAnimator>(mark);
		markAnimator.SetFrameTime(markWalk.getFrameTime());
		markAnimator.SetFrames(mithunanWalkFrames);
	}
	addToRendering(&mark);

	std::vector<MorphAnimation*> allkyraFrames;
	//std::vector<Mesh*> kyraWalkFrames;
	//loadAnimationData(kyraWalkFrames, "characters/kyra/walk/bearWalk", 16);
	MorphAnimation kyraWalk = MorphAnimation(mithunanWalkFrames, 0.1, 0);
	allkyraFrames.push_back(&kyraWalk);
	//std::vector<Mesh*> kyraIdleFrames;
	//loadAnimationData(kyraIdleFrames, "characters/kyra/idle/bearIdle", 4);
	MorphAnimation kyraIdle = MorphAnimation(mithunanIdleFrames, 0.5, 0);
	allkyraFrames.push_back(&kyraIdle);
	std::unique_ptr<Texture2D> kyraTex = std::make_unique<Texture2D>("characters/mithunan/kyra.png");
	std::unique_ptr<Material> kyraMat = std::make_unique<Material>(*prog_morph);
	kyraMat->AddTexture("albedo", *kyraTex);
	Entity kyra = Entity::Create();
	{


		kyra.Add<CMorphMeshRenderer>(kyra, *mithunanWalkFrames[0], *kyraMat.get());
		kyra.Add<CPathAnimator>(kyra);
		kyra.Get<CPathAnimator>().SetMode(PathSampler::PathMode::CATMULL);
		kyra.transform.m_scale = glm::vec3(0.44f, 0.44f, 0.44f);
		kyra.transform.m_rotation = glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::angleAxis(glm::radians(00.f), glm::vec3(1.0f, 0.0f, 0.0f));
		kyra.transform.m_pos = glm::vec3(-1.f, -5.5, -2.29f);
		kyra.Add<CharacterController>(&kyra, allkyraFrames, line);
		kyra.Get<CharacterController>().setStopSpot(placeInLineToIndex(4));


		//mithunan.Get<CharacterController>().continueAnimation(false); 
		auto& kyraAnimator = kyra.Add<CMorphAnimator>(kyra);
		kyraAnimator.SetFrameTime(kyraWalk.getFrameTime());
		kyraAnimator.SetFrames(mithunanWalkFrames);
	}
	addToRendering(&kyra);

	std::vector<MorphAnimation*> allnathanFrames;
	//std::vector<Mesh*> nathanWalkFrames;
	//loadAnimationData(nathanWalkFrames, "characters/nathan/walk/bearWalk", 16);
	MorphAnimation nathanWalk = MorphAnimation(mithunanWalkFrames, 0.1, 0);
	allnathanFrames.push_back(&nathanWalk);
	//std::vector<Mesh*> nathanIdleFrames;
	//loadAnimationData(nathanIdleFrames, "characters/nathan/idle/bearIdle", 4);
	MorphAnimation nathanIdle = MorphAnimation(mithunanIdleFrames, 0.5, 0);
	allnathanFrames.push_back(&nathanIdle);
	std::unique_ptr<Texture2D> nathanTex = std::make_unique<Texture2D>("characters/mithunan/nathan.png");
	std::unique_ptr<Material> nathanMat = std::make_unique<Material>(*prog_morph);
	nathanMat->AddTexture("albedo", *nathanTex);
	Entity nathan = Entity::Create();
	{


		nathan.Add<CMorphMeshRenderer>(nathan, *mithunanWalkFrames[0], *nathanMat.get());
		nathan.Add<CPathAnimator>(nathan);
		nathan.Get<CPathAnimator>().SetMode(PathSampler::PathMode::CATMULL);
		nathan.transform.m_scale = glm::vec3(0.44f, 0.44f, 0.44f);
		nathan.transform.m_rotation = glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::angleAxis(glm::radians(00.f), glm::vec3(1.0f, 0.0f, 0.0f));
		nathan.transform.m_pos = glm::vec3(-1.f, -5.5, -2.29f);
		nathan.Add<CharacterController>(&nathan, allnathanFrames, line);
		nathan.Get<CharacterController>().setStopSpot(placeInLineToIndex(4));


		//mithunan.Get<CharacterController>().continueAnimation(false); 
		auto& nathanAnimator = nathan.Add<CMorphAnimator>(nathan);
		nathanAnimator.SetFrameTime(nathanWalk.getFrameTime());
		nathanAnimator.SetFrames(mithunanWalkFrames);
	}
	addToRendering(&nathan);

	customerLine[0] = &mithunan;
	customerLine[1] = &kainat;
	customerLine[2] = &mark;
	customers.push_back(&mithunan);
	customers.push_back(&kainat);
	customers.push_back(&mark);
	customers.push_back(&kyra);
	customers.push_back(&nathan);

	Entity tray = Entity::Create();
	tray.Add<CMeshRenderer>(tray, *trayMat.getMesh(), *trayMat.getMaterial());
	trayScale = glm::vec3(0.053f, 0.035f, 0.037f);
	tray.transform.m_scale = trayScale;
	tray.transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
	tray.transform.m_pos = glm::vec3(cameraPos.x + 0.92, cameraPos.y + 0.430, cameraPos.z + -0.147);// 0.552 


	normalTrayPos = tray.transform;
	lowTrayPos = tray.transform;
	lowTrayPos.m_pos = glm::vec3(cameraPos.x + 0.92, cameraPos.y + 0.427, cameraPos.z + -0.147);

	loadMaterialCreatorData(dialogueList, tileMesh, "UI/textures/dialogue/Dialogue",20);
	
	tutorialSteps.push_back(TutorialStep(dialogueList[0],true));
	tutorialSteps.push_back(TutorialStep(dialogueList[1],true));
	tutorialSteps.push_back(TutorialStep(dialogueList[2],false));
	tutorialSteps.push_back(TutorialStep(dialogueList[3],false));
	tutorialSteps.push_back(TutorialStep(dialogueList[4],true));
	tutorialSteps.push_back(TutorialStep(dialogueList[5], true));
	tutorialSteps.push_back(TutorialStep(dialogueList[6], false));
	tutorialSteps.push_back(TutorialStep(dialogueList[7], false));
	tutorialSteps.push_back(TutorialStep(dialogueList[8], false));
	tutorialSteps.push_back(TutorialStep(dialogueList[9], false));
	tutorialSteps.push_back(TutorialStep(dialogueList[10], true));
	tutorialSteps.push_back(TutorialStep(dialogueList[11], false));
	tutorialSteps.push_back(TutorialStep(dialogueList[12], true));
	tutorialSteps.push_back(TutorialStep(dialogueList[13], false));
	tutorialSteps.push_back(TutorialStep(dialogueList[14], false));
	tutorialSteps.push_back(TutorialStep(dialogueList[15], true));
	tutorialSteps.push_back(TutorialStep(dialogueList[16], true));
	tutorialSteps.push_back(TutorialStep(&nothingTile, false));//transparent, continues at second order
	tutorialSteps.push_back(TutorialStep(dialogueList[17], true));//upurr1
	tutorialSteps.push_back(TutorialStep(dialogueList[18], true));//lets get to work
	tutorialSteps.push_back(TutorialStep(&nothingTile, false));//transparent, continues at third order
	tutorialSteps.push_back(TutorialStep(dialogueList[19], true));//last upurr eats order, which can be hidden by a spacebar
	
	MaterialCreator tutorialConfusion;
	tutorialConfusion.createMaterial(tileMesh, "UI/textures/Keys1.png", *prog_UI);
	MaterialCreator optionConfusion;
	optionConfusion.createMaterial(tileMesh, "UI/textures/Keys2.png", *prog_UI);
	






	
																											  //addToRendering(&tray); 
	traySlot[0] = Transform();
	traySlot[0].m_pos = tray.transform.m_pos;
	traySlot[0].m_pos.x = tray.transform.m_pos.x - 0.0173;
	traySlot[0].m_pos.z = tray.transform.m_pos.z - 0.0173;
	traySlot[0].SetParent(&tray.transform);
	traySlot[1] = Transform();
	traySlot[1].m_pos = tray.transform.m_pos;
	traySlot[1].m_pos.x = tray.transform.m_pos.x + 0.0173;
	traySlot[1].m_pos.z = tray.transform.m_pos.z - 0.0173;
	traySlot[1].SetParent(&tray.transform);
	traySlot[2] = Transform();
	traySlot[2].m_pos = tray.transform.m_pos;
	traySlot[2].m_pos.x = tray.transform.m_pos.x - 0.0173;
	traySlot[2].m_pos.z = tray.transform.m_pos.z + 0.0173;
	traySlot[2].SetParent(&tray.transform);
	traySlot[3] = Transform();
	traySlot[3].m_pos = tray.transform.m_pos;
	traySlot[3].m_pos.x = tray.transform.m_pos.x + 0.0173;
	traySlot[3].m_pos.z = tray.transform.m_pos.z + 0.0173;
	traySlot[3].SetParent(&tray.transform);




	int currentOvenPos = -1;//selectedOvenPosition(currentPoint.y); 
	int lastOvenPos = -1;//selectedOvenPosition(lastPoint.y); 
	Transform slot1Transform = oven.transform;
	//1.f, -1.5f, 0.5f 
	slot1Transform.m_pos.x = 0.145;
	slot1Transform.m_pos.y = -1.218;
	slot1Transform.m_pos.z = -0.175;
	slot1Transform.m_rotation = glm::angleAxis(glm::radians(270.f), glm::vec3(0.0f, 1.0f, 0.0f));

	slot1Transform.m_scale = glm::vec3(0.25);
	OvenTimer slot4 = OvenTimer(nothingTile, ovenDial, timerMat, slot1Transform, 0.3, glm::angleAxis(glm::radians(270.f), glm::vec3(0, 1, 0)));
	addToRendering(slot4.getArrow());
	//addToRendering(slot1.getCircle()); 
	addToRendering(slot4.getTile());
	slot4.getArrow()->transform.m_scale = glm::vec3(1.4);
	//slot1.getArrow()->transform.m_rotation = glm::angleAxis(glm::radians(270.f), glm::vec3(0, 1, 0)); 
	slot4.getArrow()->transform.m_pos.x += 0.01;
	slot4.getArrow()->transform.m_pos.z += 0.03;
	slot4.getTile()->transform.m_pos.y = slot4.getTransform().m_pos.y + 0.569;

	//slot1.getArrow()->transform.m_rotation =  
	Transform slot2Transform = oven.transform;
	//slot2Transform.m_pos.x += -0.27; 

	//slot2Transform.m_pos.z -= 0.140; 

	slot2Transform.m_pos.x = 0.145;
	slot2Transform.m_pos.y = -0.981;
	slot2Transform.m_pos.z = -0.175;


	slot2Transform.m_rotation = glm::angleAxis(glm::radians(270.f), glm::vec3(0.0f, 1.0f, 0.0f));

	slot2Transform.m_scale = glm::vec3(0.25);
	OvenTimer slot3 = OvenTimer(nothingTile, ovenDial, timerMat, slot2Transform, 0.3, glm::angleAxis(glm::radians(270.f), glm::vec3(0, 1, 0)));
	addToRendering(slot3.getArrow());
	//addToRendering(slot2.getCircle()); 
	addToRendering(slot3.getTile());
	//slot2.getTile()->transform.m_pos = slot2Transform.m_pos; 
	slot3.getArrow()->transform.m_scale = glm::vec3(1.4);
	//slot2.getArrow()->transform.m_rotation = glm::angleAxis(glm::radians(270.f), glm::vec3(0, 1, 0)); 
	slot3.getArrow()->transform.m_pos.x += 0.01;
	slot3.getArrow()->transform.m_pos.z += 0.03;
	slot3.getTile()->transform.m_pos.y = slot3.getTransform().m_pos.y + 0.569;


	Transform slot3Transform = oven.transform;

	slot3Transform.m_pos.x = 0.145;
	slot3Transform.m_pos.y = -0.739;
	slot3Transform.m_pos.z = -0.175;



	slot3Transform.m_scale = glm::vec3(0.25);
	slot3Transform.m_rotation = glm::angleAxis(glm::radians(270.f), glm::vec3(0.0f, 1.0f, 0.0f));

	OvenTimer slot2 = OvenTimer(nothingTile, ovenDial, timerMat, slot3Transform, 0.3, glm::angleAxis(glm::radians(270.f), glm::vec3(0, 1, 0)));
	addToRendering(slot2.getArrow());
	//addToRendering(slot3.getCircle()); 
	addToRendering(slot2.getTile());
	//slot3.getTile()->transform.m_pos = slot3Transform.m_pos ; 
	slot2.getArrow()->transform.m_scale = glm::vec3(1.4);
	//slot3.getArrow()->transform.m_rotation = glm::angleAxis(glm::radians(270.f), glm::vec3(0, 1, 0)); 
	slot2.getArrow()->transform.m_pos.x += 0.01;
	slot2.getArrow()->transform.m_pos.z += 0.03;
	slot2.getTile()->transform.m_pos.y = slot2.getTransform().m_pos.y + 0.587;


	Transform slot4Transform = oven.transform;

	slot4Transform.m_pos.x = 0.145;
	slot4Transform.m_pos.y = -0.502;
	slot4Transform.m_pos.z = -0.175;


	slot4Transform.m_scale = glm::vec3(0.25);
	slot4Transform.m_rotation = glm::angleAxis(glm::radians(270.f), glm::vec3(0.0f, 1.0f, 0.0f));

	OvenTimer slot1 = OvenTimer(nothingTile, ovenDial, timerMat, slot4Transform, 0.3, glm::angleAxis(glm::radians(270.f), glm::vec3(0, 1, 0)));
	addToRendering(slot1.getArrow());
	//addToRendering(slot4.getCircle()); 
	addToRendering(slot1.getTile());
	//slot4.getTile()->transform.m_pos = slot4Transform.m_pos; 
	slot1.getArrow()->transform.m_scale = glm::vec3(1.4);
	//slot4.getArrow()->transform.m_rotation = glm::angleAxis(glm::radians(270.f), glm::vec3(0, 1, 0)); 
	slot1.getArrow()->transform.m_pos.x += 0.01;
	slot1.getArrow()->transform.m_pos.z += 0.03;
	slot1.getTile()->transform.m_pos.y = slot1.getTransform().m_pos.y + 0.586;

	ovenHeights[0] = slot1Transform.m_pos.y - 0.25;
	ovenHeights[1] = slot2Transform.m_pos.y - 0.20;
	ovenHeights[2] = slot3Transform.m_pos.y - 0.20;
	ovenHeights[3] = slot4Transform.m_pos.y - 0.15;

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
	glm::vec3 lastPoint = glm::vec3(-999, -999, -999);
	glm::vec3 currentPoint = glm::vec3(0);
	bool raycastHit = false;
	Entity* hitEntity = nullptr;
	currentOrders.push_back(Order());
	currentOrders.back().createOrder(bakeryUtils::getDifficulty());//bakeryUtils::getDifficulty() 
	//currentOrders.back().startOrder(); 

	OvenTimer upurrTimer1 = OvenTimer(nothingTile, arrowMat, timerMat, customerBubbleLocation, 0.2, glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f)));
	OvenTimer upurrTimer2 = OvenTimer(nothingTile, arrowMat, timerMat, customerBubbleLocation, 0.2, glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f)));
	OvenTimer customerTimer = OvenTimer(nothingTile, arrowMat, timerMat, customerBubbleLocation, 0.2, glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f)));

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





	//glm::vec3 carLight = glm::vec3(-0.0f, -0.0f, 1.0f); 
	//glm::vec3 carLightColour = glm::vec3(1, 0, 0); 
	//float strength = 1.f; 
	Light carLight;
	carLight.colour = glm::vec3(1, 1, 0);
	carLight.pos = glm::vec3(-0.0f, -0.0f, 1.0f);
	carLight.strength = 0.f;
	car.transform.m_pos = glm::vec3(-10, -10, 10);
	//REMOVE WHEN YOU WANT TO TEST MENUS OR SHIP THE FINAL GAME OR DO A DEMO! ################################# 
	

	

	//up to here 

	DrinkMachine& drinkScript = drink.Get<DrinkMachine>();
	//addToRendering(&receipt); 
	GLfloat seeThrough = 0.5f;




	//addToRendering(&receipt); 
	for (int i = 0; i < 6; i++) {
		numberEntities.push_back(Entity::Allocate().release());
		//numberEntities.back()->transform.SetParent(&receipt.transform); 
		numberScale = glm::vec3(0.007, 0.007, 0.007);
		if (i < 3) {

			numberEntities.back()->Add<CMeshRenderer>(*numberEntities.back(), *numberTiles[i].getMesh(), *numberTiles[i].getMaterial());
			numberEntities.back()->transform.m_scale = glm::vec3(0.007, 0.007, 0.007);
			numberEntities.back()->transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f)) *
				glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f))
				* glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 0.0f, 1.0f));//0 
			numberEntities.back()->transform.m_pos = receipt.transform.m_pos;
			//numberEntities.back()->transform.m_pos.z -= 0.001; 



		}
		else
		{
			numberEntities.back()->Add<CMeshRenderer>(*numberEntities.back(), *numberTiles[i].getMesh(), *numberTiles[i].getMaterial());
			numberEntities.back()->transform.m_scale = glm::vec3(0.007, 0.007, 0.007);
			numberEntities.back()->transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f)) *
				glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f))
				* glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 0.0f, 1.0f));//0 
			numberEntities.back()->transform.m_pos = receipt.transform.m_pos;
			numberEntities.back()->transform.m_pos.z += 0.001;
			numberEntities.back()->transform.m_pos.y -= 0.1;

		}
		//addToRendering(numberEntities.back()); 
	}

	std::vector<MaterialCreator*> alphanumericPointer;
	std::vector<MaterialCreator*> sliderPointer;
	std::vector<MaterialCreator*> booleanPointer;
	std::vector<MaterialCreator*> resoloutionPointer;
	for (int i = 0; i < alphanumericMat.size(); i++) {
		alphanumericPointer.push_back(&alphanumericMat[i]);
	}
	for (int i = 0; i < sliderMat.size(); i++) {
		sliderPointer.push_back(&sliderMat[i]);
	}
	for (int i = 0; i < booleanMat.size(); i++) {
		booleanPointer.push_back(&booleanMat[i]);
	}
	for (int i = 0; i < resoloutionMat.size(); i++) {
		resoloutionPointer.push_back(&resoloutionMat[i]);
	}
	loadSettings();
	applySettings();
	for (int i = 0; i < 5; i++) {
		accessEntities[i] = Entity::Allocate().release();//make into other entity for accessibility and re use them for tray abd sign board 
		accessEntities[i]->Add<CMeshRenderer>(*accessEntities[i], *alphanumericMat[i].getMesh(), *alphanumericMat[i].getMaterial());
		accessEntities[i]->transform.m_scale = accessScale;
		accessEntities[i]->transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f)) *
			glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f))
			* glm::angleAxis(glm::radians(270.f), glm::vec3(0.0f, 0.0f, 1.0f));
		accessEntities[i]->Add<PictureSelector>(accessEntities[i]);
		accessEntities[i]->Get<PictureSelector>().setPictures(alphanumericPointer);
	}
	{//accessEntities stuff 
		//accessEntities[0]->transform.m_pos = accessTray[0].m_pos; 
		accessEntities[0]->Get<PictureSelector>().setPictures(sliderPointer);
		accessEntities[1]->Get<PictureSelector>().setPictures(sliderPointer);
		accessEntities[2]->Get<PictureSelector>().setPictures(sliderPointer);
		accessEntities[3]->Get<PictureSelector>().setPictures(booleanPointer);
		accessEntities[4]->Get<PictureSelector>().setPictures(resoloutionPointer);




		tray.transform.m_pos = glm::vec3(menuCameraPos.x - 0.1, menuCameraPos.y - 0.040, menuCameraPos.z);// 0.552 
		tray.transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f));
		accessTray[0].m_pos = glm::vec3(-0.81, -1.223, -10.68);
		accessTray[1].m_pos = glm::vec3(-0.81, -1.223, -10.725);
		accessTray[2].m_pos = glm::vec3(-0.78, -1.223, -10.68);
		accessTray[3].m_pos = glm::vec3(-0.78, -1.223, -10.725);
		accessTray[4].m_pos = glm::vec3(-0.78, -4.223, -10.725);
		accessTray[5].m_pos = glm::vec3(-1, -1.168, -10.620);
		accessTray[6].m_pos = glm::vec3(-1, -1.201, -10.620);
		accessTray[7].m_pos = glm::vec3(-1, -1.234, -10.620);
		accessTray[8].m_pos = glm::vec3(-1, -1.269, -10.620);
		accessTray[9].m_pos = glm::vec3(-1, -1.307, -10.620);

	}

	filling.Get<Transparency>().setTransparency(0.5);
	fillingPlane.Get<Transparency>().setTransparency(0.5);
	topping.Get<Transparency>().setTransparency(0.5);
	toppingPlane.Get<Transparency>().setTransparency(0.5);
	drink.Get<Transparency>().setTransparency(0.5);
	drinkPlane.Get<Transparency>().setTransparency(0.5);
	drinkFill.getEntity()->Get<Transparency>().setTransparency(0.5);

	//has to be defined after the settings get applied!
	tutorialPlane = Entity::Allocate();
	tutorialPlane->Add<CMeshRenderer>(*tutorialPlane, *dialogueList[0]->getMesh(), *dialogueList[0]->getMaterial());
	tutorialPlane->transform.m_scale = glm::vec3(0.003 * (UIScale + 0.05));
	tutorialPlane->transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f))
		* glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 0.0f, 1.0f));//0 	//tutorialPlane->transform.m_pos = glm::vec3(cameraPos.x - 0.92, cameraPos.y + 0.430, cameraPos.z + -0.147);// 0.552
	tutorialPlane->transform.m_pos = glm::vec3(cameraPos.x + 1.006, cameraPos.y + 0.497, cameraPos.z - 0.010);// 0.552 
	
	UIEntities.push_back(tutorialPlane.get());


	//has to be defined after the settings get applied!
	Entity confusionPlane = Entity::Create();
	confusionPlane.Add<CMeshRenderer>(confusionPlane, *tutorialConfusion.getMesh(), *tutorialConfusion.getMaterial());
	confusionPlane.transform.m_scale = glm::vec3(0.15 * (UIScale + 0.05));
	confusionPlane.transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f))
		* glm::angleAxis(glm::radians(-90.f), glm::vec3(0.0f, 0.0f, 1.0f));//0 	//tutorialPlane->transform.m_pos = glm::vec3(cameraPos.x - 0.92, cameraPos.y + 0.430, cameraPos.z + -0.147);// 0.552
	confusionPlane.transform.m_pos = glm::vec3(-1.200, -1.280, -10.950);// 0.552 
	//renderingEntities.insert(renderingEntities.begin() + 2, &confusionPlane);

	//confusionPlane.transform.SetParent(&cameraEntity.transform);
	//UIEntities.push_back(tutorialPlane.get());
	addToRendering(&confusionPlane);
	/*
	confettiColours.push_back(glm::vec4(1.f, 0.f, 0.f, 1.f));
	confettiColours.push_back(glm::vec4(0.f, 1.f, 0.f, 1.f));
	confettiColours.push_back(glm::vec4(0.f, 0.f, 1.f, 1.f));
	
	for each (glm::vec4 c in confettiColours) {
		
		//Setting up our particle system. 
		ParticleParam newParticle;
		newParticle.lifetime = 0.8f;
		newParticle.startColor = c;
		newParticle.endColor = c;
		newParticle.startSize = 0.1f;
		newParticle.maxParticles = 10;
		newParticle.emissionRate = 10.f;
		newParticle.tanTheta = glm::tan(glm::radians(40.0f));

		confettiEmitters.push_back(Entity::Allocate());
		confettiEmitters.back()->transform.m_pos = ent_register->transform.m_pos;
		confettiEmitters.back()->Add<CParticleSystem>(*confettiEmitters.back().get(), *confettiMat.getMaterial(), newParticle);
		addToRendering(confettiEmitters.back().get());
		
		//particleEntity.Remove<CParticleSystem>();
		//particleEntity.Add<CParticleSystem>(particleEntity, *toppingScript.getParticleMaterial(selected), particleData);
	
	}

	*/

	ParticleParam newParticle;
	newParticle.lifetime = 5.f;
	newParticle.startColor = glm::vec4(1,0,0,1.f);
	newParticle.endColor = glm::vec4(1, 0, 0, 0.f);
	newParticle.startSize = 0.1f;
	newParticle.maxParticles = 100;
	newParticle.emissionRate = 10.f;
	newParticle.tanTheta = glm::tan(glm::radians(45.0f));
	

	Entity confettiEmitter = Entity::Create();
	confettiEmitter.transform.m_pos = glm::vec3(-1.134, 0.644, -2.459);
	confettiEmitter.Add<CParticleSystem>(confettiEmitter, *confettiMat.getMaterial(), newParticle);
	//confettiEmitter.Get<CParticleSystem>().setYSpeed(1);
	addToRendering(&confettiEmitter);

	auto endTime = std::chrono::high_resolution_clock::now();//measure end time 
	auto timeTook = endTime - startTime;//calculate elapsed time 
	float tutorialOneSecondTick = 0.f;
	bool tutorialIsKeyUp = true;
	float timeSinceClickedSpace = 0.f;
	//audioEngine.playSound("ambient1");
	//shouldShowTutorial = false;
	float fridgeMultiplier = 4;
	//addToRendering(&tester);
	//menuBGM.PlayEvent("event:/BackgroundMusic");
	
	float traySpeed = 8;
	
	if (getHighscore() == 0) {
		modifiedConfusionThreshold = confusionThreshold / 5;
	}
	int selected = 0;
	titleMusic.fadeIn(3);
	titleMusic.setLoop(true);
	pauseMusic.setLoop(true);
	toppingSound.setLoop(true);
	machineSound.setLoop(true);
	machineSound2.setLoop(true);
	fridgeHumSound.setLoop(true);
	ovenSound.setLoop(true);
	drinkUpSound.setLoop(true);
	
	//toppingSound.fadeIn(0.4);
	bool lookingAtFridge = false, lookingAtOven = false, lookingAtFilling = false, lookingAtDrink = false, lookingAtTopping = false;
	float fridgeLookTime = 0.f;
	bool canCheat = true;
	GLuint albedoUniform = prog_RB->GetUniformLoc("albedo");
	GLuint quadVAO;
	

	if (shouldUseGraphics) {
	
	glm::vec2 res = resoloutions[accessSettings[8]];
	if (accessSettings[8] == 3) {
		res = App::getScreenSize();
	}

	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// generate texture
	
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, res.x, res.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, res.x, res.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);


	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, res.x, res.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {

	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	float quadVertices[] = {
		// positions  
		-1.0f,  1.0f,0.f,
		-1.0f, -1.0f,0.f,
		 1.0f, -1.0f ,0.f,

		-1.0f,  1.0f,0.f,
		 1.0f, -1.0f,0.f,
		 1.0f,  1.0f,0.f
	};
	float quadUV[] = { 0,  1.0f ,0, 0, 1.0f, 0,
	0,  1.0f, 1.0f, 0,1.0f,  1.0f };
	
	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);
	GLuint pos;
	GLuint uv;
	glGenBuffers(1, &pos);
	glBindBuffer(GL_ARRAY_BUFFER, pos);
	glBufferData(GL_ARRAY_BUFFER, std::size(quadVertices) * sizeof(float), &quadVertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);//uv

	//std::cout << std::size(quadUV) << std::endl;
	glGenBuffers(1, &uv);
	glBindBuffer(GL_ARRAY_BUFFER, uv);
	glBufferData(GL_ARRAY_BUFFER, std::size(quadUV) * sizeof(float), &quadUV[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);//uv
	
	}//RB stuff
	
	float totalGameSeconds = 0;
	
	std::cout << "Loaded game in: " << (timeTook / std::chrono::seconds(1)) << " seconds" << std::endl;//output elapsed time 
	if (!shouldUseGraphics) {
		system("Color 6");
		std::cout << "NOTICE: You are using integrated graphics. As such, the game has turned off all post-processing effects. Change your settings in order to render with a dedicated graphics card and restart the game to turn on post-processing." << std::endl;
		//system("Color 7");
	}

	
	
	
	
	//currentConfettiEmission = 100.f;
	
	while (!App::IsClosing() && !Input::GetKeyDown(GLFW_KEY_BACKSPACE))
	{
		

		
			
		
		//audioEngine.Update();
		studio.Update();
		
		//playMusic(&menuBGM, "BackgroundMusic");
		//menuBGM.SetEventParameter("event:/BackgroundMusic", "parameter:/musicVolume", musicVolume);//an exception for the music, if we cant tell if an event is playing or not
		
		//tutorialPlane.get()->transform.m_rotation = glm::angleAxis(glm::radians(tempA ), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::angleAxis(glm::radians(tempB), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::angleAxis(glm::radians(tempC), glm::vec3(1.0f, 0.0f, 0.0f)); 
		
		prog_RB->Bind();
		prog_RB.get()->SetUniform("shouldBuffer", (int) true);
		prog_RB.get()->SetUniform("passedTime", totalGameSeconds);
		prog_RB.get()->SetUniform("filmGrainStrength", 16.5f);
		prog_RB.get()->SetUniform("blurStr",(int) floor(Lerp(blurBounds.x,blurBounds.y,blurT)));
		prog_RB.get()->SetUniform("screenRes", screenRes);
		//std::cout << screenRes.x << " " << screenRes.y << std::endl;
		prog_texLit->Bind();
		prog_texLit.get()->SetUniform("lightDir2", carLight.pos);
		prog_texLit.get()->SetUniform("lightColor2", carLight.colour);
		prog_texLit.get()->SetUniform("strength", carLight.strength);
		prog_texLit.get()->SetUniform("camPos", globalCameraEntity->transform.m_pos);

		

		
		//prog_texLit->Bind();
		prog_texLit.get()->SetUniform("lightColor", glm::vec3(Lerp(dayBright, dayDark, dayT)));
		prog_texLit.get()->SetUniform("ambientPower", 0.4f);
		
		//prog_transparent->Bind();
		//prog_transparent.get()->SetUniform("lightColor", glm::vec3(Lerp(dayBright, dayDark, dayT)));
		//prog_transparent.get()->SetUniform("ambientPower", 0.4f);
	
		prog_morph->Bind();
		prog_morph.get()->SetUniform("lightDir2", carLight.pos);
		prog_morph.get()->SetUniform("lightColor2", carLight.colour);
		prog_morph.get()->SetUniform("strength", carLight.strength);
		//prog_texLit->Bind();
		prog_morph.get()->SetUniform("lightColor", glm::vec3(Lerp(dayBright, dayDark, dayT)));
		prog_morph.get()->SetUniform("ambientPower", 0.4f);
		
		//prog_UI->Bind();
		//prog_UI.get()->SetUniform("brightness", tempA);

		
		//App::StartImgui();
		
		//ImGui::DragInt("X", &(intA), 0.1f);
		//ImGui::DragFloat("Y", &(tempB), 0.01f);
		//ImGui::DragFloat("Z", &(tempC), 0.01f);
		//ImGui::DragFloat("R", &(tempA), 0.01f);
		//ImGui::DragFloat("R", &(tempA), 0.01f);
		//ImGui::SliderInt("T", &tempA, 0, 4);
		//ImGui::DragFloat("S", &(tempD), 0.001f);
		/*
		App::StartImgui();
		ImGui::SetNextWindowPos(ImVec2(0, 800), ImGuiCond_FirstUseEver);
		ImGui::DragFloat("A", &(confettiEmitter.transform.m_pos.x), 0.001f);
		ImGui::DragFloat("B", &(confettiEmitter.transform.m_pos.y), 0.001f);
		ImGui::DragFloat("C", &(confettiEmitter.transform.m_pos.z), 0.001f);
		//ImGui::DragFloat("S", &(tempD), 0.001f);


		//ImGui::DragFloat("Scale", &(sc), 0.1f);
		//ImGui::SetWindowPos(0,0);

		App::EndImgui();
		*/
		//painting.transform.m_rotation = glm::angleAxis(glm::radians(tempA), glm::vec3(1.0f, 0.0f, 0.0f)) *
		//	glm::angleAxis(glm::radians(tempB), glm::vec3(0.0f, 1.0f, 0.0f))
		//	* glm::angleAxis(glm::radians(tempC), glm::vec3(0.0f, 0.0f, 1.0f));//0
		//counter.transform.m_scale = glm::vec3(tempA);
		//topping.transform.m_rotation = glm::angleAxis(glm::radians(tempC), glm::vec3(0.0f, 1.0f, 0.0f));
		
		
		

		bool keepCheckingRaycast = true;
		isClicking = false;
		isRightClicking = false;
		int addedSlot = -1;
		raycastHit = false;
		App::FrameStart();
		float deltaTime = App::GetDeltaTime();
		titleMusic.update(deltaTime);
		pauseMusic.update(deltaTime);
		printLong.update(deltaTime);
		toppingSound.update(deltaTime);
		machineSound.update(deltaTime);
		machineSound2.update(deltaTime);
		printShort.update(deltaTime);
		fridgeHumSound.update(deltaTime);
		fridgeOpenSound.update(deltaTime);
		ovenSound.update(deltaTime);
		drinkUpSound.update(deltaTime);
		closeCallSound.update(deltaTime);
		carSound.update(deltaTime);
		musicOne.update(deltaTime);
		totalGameSeconds += deltaTime;
		getKeyInput();

		currentConfettiEmission -= deltaTime;
		if (currentConfettiEmission < 0) {
			confettiEmission = false;
			currentConfettiEmission = 0.f;
		}
		else
		{
			confettiEmission = true;

		}

		if (blurMulti != 0.f) {
			blurT += (deltaTime / blurTime) * blurMulti;
			if (blurT > 1) {
				blurT = 1;
				blurMulti = 0;
			}
			if (blurT <0) {
				blurT = 0;
				blurMulti = 0;
			}
		}

		
		//UpdateTutorial(deltaTime);
		if (isInMainMenu || isInOptionsMenu) {
			currentConfusion += deltaTime;
			
			if (currentConfusion >= modifiedConfusionThreshold) {
				confusionPlane.transform.m_scale = glm::vec3((0.15 * (UIScale + 0.05)) * 1);
				//confusionPlane.Get<CMeshRenderer>().Draw();
				//std::cout << "DONE" << std::endl;
			}
			else
			{
				confusionPlane.transform.m_scale = glm::vec3(0);
				//std::cout << "NO" << std::endl;
			}
		}
		else
		{
			currentConfusion = 0;
			//std::cout << "NOT YET" << std::endl;
			modifiedConfusionThreshold = confusionThreshold;
		}

		int roundsLasted = bakeryUtils::getRoundsLasted();
		{//change transparencies 
			if (roundsLasted > 0) {
				filling.Get<Transparency>().setTransparency(0.f);
				fillingPlane.Get<Transparency>().setTransparency(0.f);
			}
			if (roundsLasted > 1) {
				topping.Get<Transparency>().setTransparency(0.f);
				toppingPlane.Get<Transparency>().setTransparency(0.f);

			}
			if (roundsLasted > 2) {
				drink.Get<Transparency>().setTransparency(0.f);
				drinkPlane.Get<Transparency>().setTransparency(00.f);

				drinkFill.getEntity()->Get<Transparency>().setTransparency(0.f);
			}
		}

		if (canCheat) {
			//std::cout << "GGGG" << std::endl; 
			
			if (Input::GetKeyDown(GLFW_KEY_ENTER)) {//put this in the lose spot 
				blurMulti = 1;
				int highScore = saveHighscore(bakeryUtils::getRoundsLasted());
				setScores(bakeryUtils::getRoundsLasted(), highScore);
				if (cameraX == 0 && cameraY == 0) {
					cameraX = 1;
					cameraY = 1;
				}
				receipt.transform.m_pos = cursor.transform.m_pos;
				receipt.transform.m_rotation = cursor.transform.m_rotation * glm::angleAxis(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f)) *
					glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f))
					* glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 0.0f, 1.0f));
				removeFromRendering(&cursor);
				removeFromRendering(&tray);
				tray.transform.SetParent(nullptr);
				for each (Entity * trayEntity in trayPastry) {
					removeFromRendering(trayEntity);
				}
				UIEntities.push_back(&receipt);
				for (int i = 0; i < 6; i++)
				{
					UIEntities.push_back(numberEntities[i]);

				}
				for (int i = 0; i < 4; i++) {
					accessEntities[i]->transform.SetParent(nullptr);
					removeFromRendering(accessEntities[i]);
				}

				receiptT = 0;
				isInContinueMenu = true;
				continue;
				
			}
			if (Input::GetKeyDown(GLFW_KEY_G)) {//put this in the lose spot 
				std::cout << "------------------" << std::endl;
			}
			

			

			if (Input::GetKey(GLFW_KEY_V)) {//put this in the lose spot 
				dayT += deltaTime;
				if (dayT > 1) {
					dayT = 1;
				}
			}
			if (Input::GetKey(GLFW_KEY_C)) {//put this in the lose spot 
				dayT -= deltaTime;
				if (dayT < 0) {
					dayT = 0;
				}
			}

			if (Input::GetKeyDown(GLFW_KEY_E)) {//put this in the lose spot 
				for (int i = 0; i < 3; i++) {
					if (customerLine[i] != nullptr) {
						customerLine[i]->Get<CharacterController>().setStopSpot(line.size());
						customerLine[i] = nullptr;
						break;
					}
				}

			}

			if (Input::GetKey(GLFW_KEY_F)) {//put this in the lose spot 
				isCarMoving = true;
			}
		}

		//mithunan.Get<CharacterController>().updatePosition(deltaTime, 0.5); 
		if (isCarMoving == true && !isPaused) {


			
			//carSound.getSound().SetEventPosition("event:/Honk", pos);
			if (carT == 0.f) {
				carSound.fadeIn(0.1);
			}

			carT += deltaTime / 3;
			if (carT > 1) {
				carT = 0.f;
				isCarMoving = false;
			}
			
			carLight.pos = Lerp(lastCarPos, glm::vec3(0, -1.9, 10), carT);
			carLight.strength = sin(carT * 3.1415926) / 2;
			car.transform.m_pos = Lerp(firstCarPos, lastCarPos, carT);
			car.transform.m_pos.z = -15;
			
			
			//car.transform.m_pos.y = -1.7;
		}
		if (isCarMoving == false) {
			car.transform.m_pos = Lerp(firstCarPos, lastCarPos, 0);
			carT = 0;
			carLight.pos = Lerp(lastCarPos, glm::vec3(0, -1.9, 10), 0);
			carLight.strength = 0;
		}
		if (isInTutorialMenu) {
			int tutorialMenuChosen = getSignSelection(2, false);

			sign.Get<CMeshRenderer>().SetMaterial(*signFrames[selectedOption + 6]);
			if (tutorialMenuChosen >= 0) {
				if (tutorialMenuChosen == 0) {//YES
					shouldShowTutorial = true;
					isInMainMenu = true;
					isInTutorialMenu = false;
					bakeryUtils::setRoundsLasted(0);
					int slot = 0;
					trayPastry[slot] = Entity::Allocate().release();
					trayPastry[slot]->Add<CMeshRenderer>(*trayPastry[slot], *burntMat.getMesh(), *burntMat.getMaterial());
					trayPastry[slot]->Add<Pastry>();
					trayPastry[slot]->Get<Pastry>().setType(bakeryUtils::pastryType::BURNT);
					trayPastry[slot]->Get<Pastry>().setCookedSeconds(bakeryUtils::returnBurnTime());
					trayPastry[slot]->Add<Transparency>(*trayPastry[slot]);
					trayPastry[slot]->Get<Transparency>().setTransparency(1.f);
					trayPastry[slot]->Get<Transparency>().setWantedTransparency(0.f);
					trayPastry[slot]->Get<Transparency>().setTime(0.2);


					trayPastry[slot]->transform.m_scale = glm::vec3(0.009f, 0.009f, 0.009f);
					trayPastry[slot]->transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));

					//std::cout << traySlot[slot].m_pos.x << " " << traySlot[slot].m_pos.y << " " << traySlot[slot].m_pos.z << std::endl; 

					trayPastry[slot]->transform.m_pos = glm::vec3(-20);
					trayPastry[slot]->Get<Pastry>().setInOven(true);
					ovenScript->canAdd(trayPastry[0], 0);
					

					trayPastry[0] = nullptr;
					//std::cout << traySlot[slot].m_pos.x << " " << traySlot[slot].m_pos.y << " " << traySlot[slot].m_pos.z << std::endl; 
					tutorialMultiplier = 1;
				}
				else if (tutorialMenuChosen == 1) {//NO
					shouldShowTutorial = false;
					isInMainMenu = true;
					isInTutorialMenu = false;
					bakeryUtils::setRoundsLasted(0);
					tutorialMultiplier = 0;
					//bakeryUtils::setDifficulty(4);

				}
				titleMusic.fadeOut(3);
				//menuBGM.StopEvent("event:/BackgroundMusic");
				orderBubbles[0]->updateScale(UIScale);
				tray.transform.m_scale = trayScale;
				tray.transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
				tray.transform.m_pos = glm::vec3(cameraPos.x + 0.92, cameraPos.y + 0.430, cameraPos.z + -0.147);// 0.552 
				//tutorialPlane->transform.m_scale = glm::vec3(0.07 * (UIScale + 0.05));
				if (isInRendering(tutorialPlane.get())) {
					removeFromRendering(tutorialPlane.get());
				}
				isCameraMoving = true;
				isInMainMenu = true;
				lastCameraX = 0;
				lastCameraY = 0;
				//standardCameraQuat = getCameraRotation(); 
				wantedCameraQuat = standardCameraQuat;
				cameraX = 90.f;
				cameraY = 0.f;
				xPos = 0;
				yPos = 0;
				
			}

			if (shouldUseGraphics) {
				// first pass
				glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
				glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
				//glEnable(GL_DEPTH_TEST);
			}
			
			for each (Entity * e in renderingEntities) {
				prog_texLit->Bind();
				prog_texLit.get()->SetUniform("transparency", 0.f);
				e->transform.RecomputeGlobal();


				if (e->Has<CMeshRenderer>()) {
					e->Get<CMeshRenderer>().Draw();
				}

				if (e->Has<CMorphMeshRenderer>()) {
					e->Get<CMorphMeshRenderer>().Draw();
				}

			}
			confusionPlane.transform.RecomputeGlobal();
			confusionPlane.Get<CMeshRenderer>().Draw();
			isScrollingDown = false;
			isScrollingUp = false;


			if (shouldUseGraphics) {

			
			// second pass
			glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
			glCullFace(GL_CW);
			prog_RB->Bind();



			glDisable(GL_DEPTH_TEST);
			//glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

			glBindTexture(GL_TEXTURE_2D, 0);

			glUniform1i(albedoUniform, 0);
			glActiveTexture(GL_TEXTURE0 + 0);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

			glBindVertexArray(quadVAO);

			glDrawArrays(GL_TRIANGLES, 0, 6);
			//glViewport(0, 0, resoloutions[accessSettings[8]].x, resoloutions[accessSettings[8]].y);
			}
			App::SwapBuffers();
			//glDeleteFramebuffers(1, &framebuffer);
			if (shouldUseGraphics) {
				glEnable(GL_DEPTH_TEST);
			}
			
			//glDeleteTextures(1, &textureColorbuffer);
			
			continue;
		}
		if (isInOptionsMenu) {
			
			
			if (isClickingSpace || isClickingEnter) {
				modifiedConfusionThreshold = confusionThreshold;
				currentConfusion = 0;
				isInOption = !isInOption;
				playSound(&click, "tickTick");
				
				if (!isInOption && selectedOption == 0) {
					isInOption = true;
				}
				else
				{
					if (isInOption) {
						if (selectedOption == 0) {
							sign.Get<CMeshRenderer>().SetMaterial(*signFrames[15]);
							if (!isInRendering(&tray)) {
								addToRendering(&tray);
								
							}
							tray.transform.m_pos = glm::vec3(menuCameraPos.x - 0.1, menuCameraPos.y - 0.040, menuCameraPos.z);// 0.552 
							tray.transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f));

							for (int i = 0; i < 4; i++) {
								accessEntities[i]->Get<PictureSelector>().setPictures(alphanumericPointer);
								accessEntities[i]->Get<PictureSelector>().setIndex(accessSettings[i]);
								accessEntities[i]->Get<PictureSelector>().updatePicture();
								accessEntities[i]->transform.m_pos = accessTray[i].m_pos;
								accessEntities[i]->transform.m_scale = accessScale * (UIScale + 0.05f);

							}
							for (int i = 0; i < 5; i++) {
								if (isInRendering(accessEntities[i])) {
									removeFromRendering(accessEntities[i]);
								}
							}
							accessButtonPressed = 0;
						}
						else if (selectedOption == 6) {
							isInMainMenu = true;
							isInOptionsMenu = false;
							isInOption = false;
							confusionPlane.Remove<CMeshRenderer>();
							confusionPlane.Add<CMeshRenderer>(confusionPlane, *tutorialConfusion.getMesh(), *tutorialConfusion.getMaterial());

							sign.Get<CMeshRenderer>().SetMaterial(*signFrames[1]);
							selectedOption = 1;
							for (int i = 0; i < 5; i++) {
								if (isInRendering(accessEntities[i])) {
									removeFromRendering(accessEntities[i]);
								}
							}
							
						}
						else
						{
							for (int i = 0; i < 5; i++) {
								if (selectedOption - 1 != i) {
									if (isInRendering(accessEntities[i])) {
										removeFromRendering(accessEntities[i]);
									}
								}
							}
						}

					}
					else
					{
						for (int i = 0; i < 5; i++) {
							accessSettings[i + 4] = accessEntities[i]->Get<PictureSelector>().getIndex();
						}
						saveSettings();
						applySettings();
						for (int i = 0; i < 5; i++) {
							if (isInRendering(accessEntities[i])) {
								removeFromRendering(accessEntities[i]);
							}
						}
						for (int i = 4; i >= 0; i--) {
							if (!isInRendering(accessEntities[i])) {
								addToRendering(accessEntities[i]);
							}
						}
						for (int i = 0; i < 5; i++) {

							
							if (i < 3) {
								accessEntities[i]->transform.m_scale = accessScale * ((UIScale + 0.05f) * 2);
							}
							else if (i == 4) {
								accessEntities[i]->transform.m_scale = accessScale * ((UIScale + 0.05f) * 1.5f);
							}
							else
							{
								accessEntities[i]->transform.m_scale = accessScale * (UIScale + 0.05f);
							}
							accessEntities[i]->transform.m_pos = accessTray[i + 5].m_pos;
						}

					}
				}



			}

			if (!isInOption) {
				int mainMenuChosen = getSignSelection(7, false);

				sign.Get<CMeshRenderer>().SetMaterial(*signFrames[selectedOption + 8]);
			}
			else
			{
				if (selectedOption == 0) {
					currentConfusion = 0;
					if (accessButtonPressed >= 0) {
						for (int i = 0; i < 4; i++) {
							accessEntities[i]->transform.m_pos = accessTray[i].m_pos;
						}

						//show tray and stuff 
						if (accessButtonPressed < 4) {
							for (int i = 0; i < accessButtonPressed + 1; i++) {
								if (!isInRendering(accessEntities[i])) {
									addToRendering(accessEntities[i]);
								}
							}
						}

						if (getWhichKeyPressed() != -1) {
							if (accessButtonPressed >= 4) {
								accessButtonPressed = -1;
								//accessButtonPressed = 0; 

								//isInMainMenu = true; 
								//isInOptionsMenu = false; 
								isInOption = false;
								if (isInRendering(&tray)) {
									removeFromRendering(&tray);
								}
								accessEntities[0]->Get<PictureSelector>().setPictures(sliderPointer);
								accessEntities[1]->Get<PictureSelector>().setPictures(sliderPointer);
								accessEntities[2]->Get<PictureSelector>().setPictures(sliderPointer);
								accessEntities[3]->Get<PictureSelector>().setPictures(booleanPointer);
								accessEntities[4]->Get<PictureSelector>().setPictures(resoloutionPointer);
								for (int i = 4; i >= 0; i--) {
									if (isInRendering(accessEntities[i])) {
										removeFromRendering(accessEntities[i]);
									}
									
								}
								for (int i = 4; i >= 0; i--) {
									if (!isInRendering(accessEntities[i])) {
										addToRendering(accessEntities[i]);
									}
								}
								for (int i = 0; i < 5; i++) {
									accessEntities[i]->Get<PictureSelector>().setIndex(accessSettings[i + 4]);
									accessEntities[i]->Get<PictureSelector>().updatePicture();
									accessEntities[i]->transform.m_pos = accessTray[i + 5].m_pos;
									if (i < 3) {
										accessEntities[i]->transform.m_scale = accessScale * ((UIScale + 0.05f) * 2);
									}
									else if (i == 4) {
										accessEntities[i]->transform.m_scale = accessScale * ((UIScale + 0.05f) * 1.5f);
									}
									else
									{
										accessEntities[i]->transform.m_scale = accessScale * (UIScale + 0.05f);
									}
								}
								saveSettings();
								applySettings();
							}
							else
							{
								if (getWhichKeyPressed() >= 0) {
									addToRendering(accessEntities[accessButtonPressed]);
									int keyPressed = getWhichKeyPressed();
									bool alreadyUsed = false;
									for (int i = 0; i < accessButtonPressed; i++) {
										if (accessEntities[i]->Get<PictureSelector>().getIndex() == keyPressed) {
											alreadyUsed = true;
										}
									}
									if (!alreadyUsed) {

										trayKeys[accessButtonPressed] = pictureIndexToGLuint(keyPressed);
										accessEntities[accessButtonPressed]->Get<PictureSelector>().setIndex(keyPressed);
										accessEntities[accessButtonPressed]->Get<PictureSelector>().updatePicture();
										accessSettings[accessButtonPressed] = keyPressed;
										accessButtonPressed++;
										if (accessButtonPressed == 4) {
											sign.Get<CMeshRenderer>().SetMaterial(*signFrames[16]);
										}
									}
								}



							}

						}

					}
					else
					{

						//accessButtonPressed = 0; 
					}
				}
				else if (selectedOption != 6) {
					if (isClickingUp || isClickingRight || isScrollingUp) {
						accessEntities[selectedOption - 1]->Get<PictureSelector>().addToIndex(1);
						accessEntities[selectedOption - 1]->Get<PictureSelector>().updatePicture();
						currentConfusion = 0;
					}
					else if (isClickingDown || isClickingLeft || isScrollingDown) {
						accessEntities[selectedOption - 1]->Get<PictureSelector>().addToIndex(-1);
						accessEntities[selectedOption - 1]->Get<PictureSelector>().updatePicture();
						currentConfusion = 0;

					}
				}

			}









			
		
			if (shouldUseGraphics) {
				// first pass
				glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
				glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
				//glEnable(GL_DEPTH_TEST);
			}

			for each (Entity * e in renderingEntities) {
				prog_texLit->Bind();
				prog_texLit.get()->SetUniform("transparency", 0.f);
				e->transform.RecomputeGlobal();


				if (e->Has<CMeshRenderer>()) {
					e->Get<CMeshRenderer>().Draw();
				}

				if (e->Has<CMorphMeshRenderer>()) {
					e->Get<CMorphMeshRenderer>().Draw();
				}

			}
			confusionPlane.transform.RecomputeGlobal();
			confusionPlane.Get<CMeshRenderer>().Draw();
			isScrollingDown = false;
			isScrollingUp = false;


			if (shouldUseGraphics) {


				// second pass
				glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);
				glEnable(GL_DEPTH_TEST);
				glCullFace(GL_CW);
				prog_RB->Bind();



				glDisable(GL_DEPTH_TEST);
				//glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

				glBindTexture(GL_TEXTURE_2D, 0);

				glUniform1i(albedoUniform, 0);
				glActiveTexture(GL_TEXTURE0 + 0);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

				glBindVertexArray(quadVAO);

				glDrawArrays(GL_TRIANGLES, 0, 6);
				//glViewport(0, 0, resoloutions[accessSettings[8]].x, resoloutions[accessSettings[8]].y);
			}
			App::SwapBuffers();
			//glDeleteFramebuffers(1, &framebuffer);
			if (shouldUseGraphics) {
				glEnable(GL_DEPTH_TEST);
			}
			continue;
		}
		if (isInMainMenu) {
			if (!isCameraMoving) {

				cameraEntity.transform.m_pos = menuCameraPos;

				cameraEntity.transform.m_rotation = menuCameraQuat;
				
				
				cameraEntity.Get<CCamera>().Update();
				int mainMenuChosen = getSignSelection(3, false);

				sign.Get<CMeshRenderer>().SetMaterial(*signFrames[selectedOption]);

				if (mainMenuChosen >= 0) {
					//nextStepTutorialIfNeeded(1);
					
					if (mainMenuChosen == 0) {//PLAY	 

						isInTutorialMenu = true;
						isInMainMenu = false;
						tray.transform.SetParent(&cameraEntity.transform);
						tutorialPlane->transform.SetParent(&cameraEntity.transform);
						normalTrayPos.SetParent(&cameraEntity.transform);
						lowTrayPos.SetParent(&cameraEntity.transform);
						tutorialPlane->Remove<CMeshRenderer>();
						tutorialPlane->Add<CMeshRenderer>(*tutorialPlane, *tutorialSteps[0].getMaterialCreator()->getMesh(), *tutorialSteps[0].getMaterialCreator()->getMaterial());
						
						

					}
					if (mainMenuChosen == 1) {
						
						confusionPlane.Remove<CMeshRenderer>();
						confusionPlane.Add<CMeshRenderer>(confusionPlane, *optionConfusion.getMesh(), *optionConfusion.getMaterial());

						isInOptionsMenu = true;
						isInMainMenu = false;
						tray.transform.SetParent(nullptr);
						tray.transform.m_pos = glm::vec3(menuCameraPos.x - 0.1, menuCameraPos.y - 0.040, menuCameraPos.z);// 0.552 
						tray.transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f));
						
						//addToRendering(accessEntities[0]); 
						for (int i = 4; i >= 0; i--) {
							if (!isInRendering(accessEntities[i])) {
								addToRendering(accessEntities[i]);
							}
						}
						
						for (int i = 0; i < 5; i++) {
							
							accessEntities[i]->transform.SetParent(nullptr);
							//std::cout << "AE" << i << std::endl;
							accessEntities[i]->Remove<CMeshRenderer>();
							accessEntities[i]->Add<CMeshRenderer>(*accessEntities[i], *coffeeTile.getMesh(), *coffeeTile.getMaterial());
							
							accessEntities[i]->transform.m_pos = accessTray[i + 5].m_pos;
							if (i < 3) {
								accessEntities[i]->transform.m_scale = accessScale * ((UIScale + 0.05f) * 2);
							}
							else if (i == 4) {
								accessEntities[i]->transform.m_scale = accessScale * ((UIScale + 0.05f) * 1.5f);
							}
							else
							{
								accessEntities[i]->transform.m_scale = accessScale * (UIScale + 0.05f);
							}
							accessEntities[i]->transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f)) *
								glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f))
								* glm::angleAxis(glm::radians(270.f), glm::vec3(0.0f, 0.0f, 1.0f));
							accessEntities[i]->Get<PictureSelector>().setIndex(accessSettings[i + 4]);
							accessEntities[i]->Get<PictureSelector>().updatePicture();
						}
						accessButtonPressed = 0;
						selectedOption = 0;
						isInOption = false;
						
					}
					if (mainMenuChosen == 2) {
						break;
					}
					tutorialPlane->transform.m_scale = glm::vec3(0);
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
					
					if (!isInRendering(&car)) {
						addToRendering(&car);
					}
					carLight.strength = 0;
					carLight.pos = Lerp(firstCarPos, lastCarPos, 0);
					isCarMoving = false;
					car.transform.m_pos = firstCarPos;
					
					
					if (!isInRendering(&cursor)) {
					//	addToRendering(&cursor);
						renderingEntities.insert(renderingEntities.begin() + 1, &cursor);
					}
					if (!isInContinueMenu) {
						tray.transform.SetParent(&cameraEntity.transform);
						//tutorialPlane->transform.SetParent(&cameraEntity.transform);
						for each (Entity * cust in customers) {
							if (!isInRendering(cust)) {
								addToRendering(cust);
							}
						}

						if (!isInRendering(&tray)) {
							addToRendering(&tray);
						}
						if (shouldShowTutorial) {

						
						for (int i = 0; i < 4; i++) {
							accessEntities[i]->transform.m_scale = glm::vec3(0.009);
							accessEntities[i]->transform.m_pos = traySlot[i].m_pos;
							accessEntities[i]->transform.m_pos.y += 0.002;
							accessEntities[i]->transform.m_rotation = glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f))
								* glm::angleAxis(glm::radians(0.f), glm::vec3(1.0f, 0.0f, 0.0f))
								* glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 0.0f, 1.0f));
							accessEntities[i]->transform.SetParent(&globalCameraEntity->transform);
							accessEntities[i]->transform.RecomputeGlobal();

							//accessEntities[i]->Get<PictureSelector>().setPictures(alphanumericPointer);
							//accessEntities[i]->Get<PictureSelector>().setIndex(accessSettings[i]);
							//accessEntities[i]->Get<PictureSelector>().updatePicture();
							accessEntities[i]->Remove<CMeshRenderer>();
							accessEntities[i]->Add<CMeshRenderer>(*accessEntities[i], *keyUpMat.getMesh(), *alphanumericPointer[accessSettings[i]]->getMaterial());

							addToRendering(accessEntities[i]);
						}
						
						}

						
						for (int i = 0; i < currentOrders.size(); i++) {
							OrderBubble* ob = orderBubbles[i];
							for each (Entity * ent in ob->returnRenderingEntities()) {

								removeFromRendering(ent);


							}
						}
						for (int i = 0; i < currentOrders.size(); i++) {
							OrderBubble* ob = orderBubbles[i];
							for each (Entity * ent in ob->returnRenderingEntities()) {
								if (!isInRendering(ent)) {
									addToRendering(ent);
								}

							}
						}


						for each (Entity * en in trayPastry) {
							if (en != nullptr) {
								addToRendering(en);
							}

						}

						currentOrders.back().startOrder();
						if (isInRendering(tutorialPlane.get())) {
							removeFromRendering(tutorialPlane.get());
						}
						

					}
					//tutorialPlane->transform.m_pos = glm::vec3(-20);
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
					continue;

				}
				else
				{
					moveCamera(1);
				}


				
			}


			if (shouldUseGraphics) {
				// first pass
				glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
				glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
				//glEnable(GL_DEPTH_TEST);
			}
			for each (Entity * e in renderingEntities) {
				prog_texLit->Bind();
				prog_texLit.get()->SetUniform("transparency", 0.f);
				e->transform.RecomputeGlobal();


				if (e->Has<CMeshRenderer>()) {
					e->Get<CMeshRenderer>().Draw();
				}

				if (e->Has<CMorphMeshRenderer>()) {
					e->Get<CMorphMeshRenderer>().Draw();
				}

			}
			confusionPlane.transform.RecomputeGlobal();
			confusionPlane.Get<CMeshRenderer>().Draw();
			isScrollingDown = false;
			isScrollingUp = false;
			
			

			if (shouldUseGraphics) {


				// second pass
				glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);
				glEnable(GL_DEPTH_TEST);
				glCullFace(GL_CW);
				prog_RB->Bind();



				glDisable(GL_DEPTH_TEST);
				//glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

				glBindTexture(GL_TEXTURE_2D, 0);

				glUniform1i(albedoUniform, 0);
				glActiveTexture(GL_TEXTURE0 + 0);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

				glBindVertexArray(quadVAO);

				glDrawArrays(GL_TRIANGLES, 0, 6);
				//glViewport(0, 0, resoloutions[accessSettings[8]].x, resoloutions[accessSettings[8]].y);
			}
			App::SwapBuffers();
			//glDeleteFramebuffers(1, &framebuffer);
			if (shouldUseGraphics) {
				glEnable(GL_DEPTH_TEST);
			}
			continue;

		}
		if (isInPauseMenu) {
			if (!isCameraMoving) {
				cameraEntity.transform.m_pos = menuCameraPos;

				cameraEntity.transform.m_rotation = menuCameraQuat;

				cameraEntity.Get<CCamera>().Update();
				int mainMenuChosen = getSignSelection(3, false);

				sign.Get<CMeshRenderer>().SetMaterial(*signFrames[selectedOption + 3]);
				//std::cout << selectedOption << std::endl; 
				if (mainMenuChosen >= 0) {
					
					pauseMusic.fadeOut(3);
					for each (Entity * cust in customers) {
						if (isInRendering(cust)) {
							removeFromRendering(cust);
						}
					}
					//tutorialPlane->transform.m_scale = glm::vec3(0);

					if (mainMenuChosen == 0) {//CONTINUE						 
						isCameraMoving = true;
						isInPauseMenu = false;
						isInMainMenu = true;
						//std::cout << "ZERO" << std::endl; 


					}
					else if (mainMenuChosen == 1) {
						//std::cout << "ONE" << std::endl; 
						restartGame();


						//createNewOrder(0, false, false); 
						for (int i = 0; i < 4; i++) {
							if (ovenScript->canRemove(i)) {
								ovenScript->removeFromSlot(i);
							}

						}
						if (isInRendering(drinkScript.getFromDrink())) {
							removeFromRendering(drinkScript.getFromDrink());
						}
						if (isInRendering(&car)) {
							removeFromRendering(&car);
						}
						if (drinkScript.isDrinkFull()) {
							drinkScript.moveDrink(0);
						}
						/*
						customerLine[0] = &mithunan;
						customerLine[1] = &kainat;
						customerLine[2] = &mark;
						mithunan.Get<CharacterController>().setCurrentSpot(placeInLineToIndex(1));
						kainat.Get<CharacterController>().setCurrentSpot(placeInLineToIndex(2));
						mark.Get<CharacterController>().setCurrentSpot(placeInLineToIndex(3));
						mithunan.Get<CharacterController>().setStopSpot(placeInLineToIndex(1));
						kainat.Get<CharacterController>().setStopSpot(placeInLineToIndex(2));
						mark.Get<CharacterController>().setStopSpot(placeInLineToIndex(3));
						mithunan.Get<CharacterController>().updateDistance(deltaTime,1);
						kainat.Get<CharacterController>().updateDistance(deltaTime, 1);
						mark.Get<CharacterController>().updateDistance(deltaTime, 1);
						*/

						drinkScript.setT(0);
						drink.Get<CMorphAnimator>().setFrameAndTime(0, 1, 0);
						drinkScript.isClosing = false;
						drinkScript.isOpening = false;
						drinkScript.releaseFromDrink();
						drinkScript.removeFromDrink();
						drinkScript.setDrinkNum(0);
						drinkScript.updatePlane();
						drinkScript.setFill(0);

						filling.Get<FillingMachine>().setFillNum(0);
						filling.Get<FillingMachine>().updatePlane();
						if (isInRendering(filling.Get<FillingMachine>().getFromFilling())) {
							removeFromRendering(filling.Get<FillingMachine>().getFromFilling());
						}
						filling.Get<FillingMachine>().removeFromFilling();

						topping.Get<ToppingMachine>().setTopNum(0);
						topping.Get<ToppingMachine>().updatePlane();
						if (isInRendering(topping.Get<ToppingMachine>().getFromTopping())) {
							removeFromRendering(topping.Get<ToppingMachine>().getFromTopping());
						}
						topping.Get<ToppingMachine>().removeFromTopping();
						drink.Get<DrinkMachine>().setDrinkNum(0);
						drink.Get<DrinkMachine>().updatePlane();
						filling.Get<Transparency>().setTransparency(0.5);
						fillingPlane.Get<Transparency>().setTransparency(0.5);
						topping.Get<Transparency>().setTransparency(0.5);
						toppingPlane.Get<Transparency>().setTransparency(0.5);
						drink.Get<Transparency>().setTransparency(0.5);
						drinkPlane.Get<Transparency>().setTransparency(0.5);
						drinkFill.getEntity()->Get<Transparency>().setTransparency(0.5);

						receipt.transform.m_pos = beginingNumberPos[0];
						for (int i = 0; i < 6; i++) {
							numberEntities[i]->transform.m_pos = beginingNumberPos[i];
						}
						if (shouldShowTutorial) {//YES
							shouldShowTutorial = true;
							
							bakeryUtils::setRoundsLasted(0);
							int slot = 0;
							trayPastry[slot] = Entity::Allocate().release();
							trayPastry[slot]->Add<CMeshRenderer>(*trayPastry[slot], *burntMat.getMesh(), *burntMat.getMaterial());
							trayPastry[slot]->Add<Pastry>();
							trayPastry[slot]->Get<Pastry>().setType(bakeryUtils::pastryType::BURNT);
							trayPastry[slot]->Get<Pastry>().setCookedSeconds(bakeryUtils::returnBurnTime());
							trayPastry[slot]->Add<Transparency>(*trayPastry[slot]);
							trayPastry[slot]->Get<Transparency>().setTransparency(1.f);
							trayPastry[slot]->Get<Transparency>().setWantedTransparency(0.f);
							trayPastry[slot]->Get<Transparency>().setTime(0.2);


							trayPastry[slot]->transform.m_scale = glm::vec3(0.009f, 0.009f, 0.009f);
							trayPastry[slot]->transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));

							//std::cout << traySlot[slot].m_pos.x << " " << traySlot[slot].m_pos.y << " " << traySlot[slot].m_pos.z << std::endl; 

							trayPastry[slot]->transform.m_pos = glm::vec3(-20);
							trayPastry[slot]->Get<Pastry>().setInOven(true);
							ovenScript->canAdd(trayPastry[0], 0);


							trayPastry[0] = nullptr;
							//std::cout << traySlot[slot].m_pos.x << " " << traySlot[slot].m_pos.y << " " << traySlot[slot].m_pos.z << std::endl; 

						}
						else {//NO
							
							bakeryUtils::setRoundsLasted(0);
							//bakeryUtils::setDifficulty(4);

						}

						isCameraMoving = true;
						isInPauseMenu = false;
						isInMainMenu = true;
					}
					else if (mainMenuChosen == 2) {
					titleMusic.fadeIn(3);
						//std::cout << "ONE" << std::endl; 
						restartGame();


						//createNewOrder(0, false, false); 
						for (int i = 0; i < 4; i++) {
							if (ovenScript->canRemove(i)) {
								ovenScript->removeFromSlot(i);
							}

						}
						if (isInRendering(drinkScript.getFromDrink())) {
							removeFromRendering(drinkScript.getFromDrink());
						}
						if (isInRendering(&car)) {
							removeFromRendering(&car);
						}
						if (drinkScript.isDrinkFull()) {
							drinkScript.moveDrink(0);
						}

						drinkScript.setT(0);
						drink.Get<CMorphAnimator>().setFrameAndTime(0, 1, 0);
						drinkScript.isClosing = false;
						drinkScript.isOpening = false;
						drinkScript.releaseFromDrink();
						drinkScript.removeFromDrink();
						drinkScript.setDrinkNum(0);
						drinkScript.updatePlane();
						drinkScript.setFill(0);

						filling.Get<FillingMachine>().setFillNum(0);
						filling.Get<FillingMachine>().updatePlane();
						if (isInRendering(filling.Get<FillingMachine>().getFromFilling())) {
							removeFromRendering(filling.Get<FillingMachine>().getFromFilling());
						}
						filling.Get<FillingMachine>().removeFromFilling();

						topping.Get<ToppingMachine>().setTopNum(0);
						topping.Get<ToppingMachine>().updatePlane();
						if (isInRendering(topping.Get<ToppingMachine>().getFromTopping())) {
							removeFromRendering(topping.Get<ToppingMachine>().getFromTopping());
						}
						topping.Get<ToppingMachine>().removeFromTopping();
						drink.Get<DrinkMachine>().setDrinkNum(0);
						drink.Get<DrinkMachine>().updatePlane();
						filling.Get<Transparency>().setTransparency(0.5);
						fillingPlane.Get<Transparency>().setTransparency(0.5);
						topping.Get<Transparency>().setTransparency(0.5);
						toppingPlane.Get<Transparency>().setTransparency(0.5);
						drink.Get<Transparency>().setTransparency(0.5);
						drinkPlane.Get<Transparency>().setTransparency(0.5);
						drinkFill.getEntity()->Get<Transparency>().setTransparency(0.5);

						receipt.transform.m_pos = beginingNumberPos[0];
						for (int i = 0; i < 6; i++) {
							numberEntities[i]->transform.m_pos = beginingNumberPos[i];
						}


						
						isInPauseMenu = false;
						isInMainMenu = true;
						selectedOption = 0;
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
					if (ovenScript->getActivePastries() > 0) {
						ovenSound.fadeIn(0.3);
					}


				}
				else
				{
					moveCamera(1);

				}
			}
			if (shouldUseGraphics) {
				// first pass
				glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
				glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
				//glEnable(GL_DEPTH_TEST);
			}

			for each (Entity * e in renderingEntities) {
				prog_texLit->Bind();
				prog_texLit.get()->SetUniform("transparency", 0.f);
				e->transform.RecomputeGlobal();


				if (e->Has<CMeshRenderer>()) {
					e->Get<CMeshRenderer>().Draw();
				}

				if (e->Has<CMorphMeshRenderer>()) {
					e->Get<CMorphMeshRenderer>().Draw();
				}

			}
			
			isScrollingDown = false;
			isScrollingUp = false;



			if (shouldUseGraphics) {


				// second pass
				glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);
				glEnable(GL_DEPTH_TEST);
				glCullFace(GL_CW);
				prog_RB->Bind();



				glDisable(GL_DEPTH_TEST);
				//glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

				glBindTexture(GL_TEXTURE_2D, 0);

				glUniform1i(albedoUniform, 0);
				glActiveTexture(GL_TEXTURE0 + 0);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

				glBindVertexArray(quadVAO);

				glDrawArrays(GL_TRIANGLES, 0, 6);
				//glViewport(0, 0, resoloutions[accessSettings[8]].x, resoloutions[accessSettings[8]].y);
			}
			App::SwapBuffers();
			//glDeleteFramebuffers(1, &framebuffer);
			if (shouldUseGraphics) {
				glEnable(GL_DEPTH_TEST);
			}
			continue;
		}

		if (isInContinueMenu) {
			//tutorialPlane->transform.m_pos = glm::vec3(-20);
			tutorialMultiplier = 0;
			//other blurmulti is in !ispaused place when isincontinue gets set to true
			ovenSound.fadeOut(0.3);
		//tutorialPlane->transform.m_scale = glm::vec3(0.07 * (UIScale + 0.05));
		tutorialPlane->transform.m_scale = glm::vec3((0.003 * (UIScale + 0.05)) * tutorialMultiplier);
		
			
			if (isInRendering(tutorialPlane.get())) {
				removeFromRendering(tutorialPlane.get());
			}
			//std::cout << static_cast<int>(floor(static_cast<float>(receiptT * 100.f))) << std::endl; 
			if ((static_cast<int>(floor(static_cast<float>(receiptT * 100.f))) % 2 == 0) && receiptStopT < 1
				&& receiptT >= 0.8 && receiptT <= 0.9) {
				if (receiptStopT == 0) {
					printShort.fadeIn(0.001);
				}
				receiptStopT += deltaTime * 4;
				
				
			}
			else
			{
				receiptT += deltaTime;
				receiptStopT = 0;
				
				
				
			}
			int receiptInt = static_cast<int>(floor(static_cast<float>(receiptT * 100.f)));
			
			if (receiptInt > 97 && receiptInt < 100) {
				printLong.fadeIn(0.001);
				//std::cout << "A" << std::endl;
			}
			if (receiptT > 1) {
				receiptT = 1;
				
				
			}

			receipt.transform.m_pos = Lerp(receptBeginPos, receptEndPos, receiptT);
			for (int i = 0; i < 6; i++)
			{
				numberEntities[i]->transform.m_pos = Lerp(beginingNumberPos[i], endNumberPos[i], receiptT);
			}
			if (isClickingSpace || isClickingEscape || isClickingEnter) {
				blurMulti = -1;
				printShort.fadeIn(0.001);
				titleMusic.fadeIn(3);
				receiptT = 0;
				receiptStopT = 0;
				if (isInUI(&receipt)) {
					removeFromUI(&receipt);
				}
				for (int i = 0; i < 6; i++)
				{
					if (isInUI(numberEntities[i])) {
						removeFromUI(numberEntities[i]);
					}
				}

				for each (Entity * cust in customers) {
					if (isInRendering(cust)) {
						removeFromRendering(cust);
					}
				}
				if (isInRendering(&car)) {
					removeFromRendering(&car);
				}
				std::reverse(cameraKeys.begin(), cameraKeys.end());
				selectedOption = -1;
				sign.Get<CMeshRenderer>().SetMaterial(*signFrames[0]);
				restartGame();
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

				for (int i = 0; i < currentOrders.size(); i++) {
					OrderBubble* ob = orderBubbles[i];
					for each (Entity * ent in ob->returnRenderingEntities()) {
						removeFromRendering(ent);
					}
				}
			
				createNewOrder(0, false, false);
				for (int i = 0; i < 4; i++) {
					if (ovenScript->canRemove(i)) {
						ovenScript->removeFromSlot(i);
					}

				}
				if (isInRendering(drinkScript.getFromDrink())) {
					removeFromRendering(drinkScript.getFromDrink());
				}
				if (drinkScript.isDrinkFull()) {
					drinkScript.moveDrink(0);
				}

				drinkScript.setT(0);
				drink.Get<CMorphAnimator>().setFrameAndTime(0, 1, 0);
				drinkScript.isClosing = false;
				drinkScript.isOpening = false;
				drinkScript.releaseFromDrink();
				drinkScript.removeFromDrink();
				drinkScript.setDrinkNum(0);
				drinkScript.setFill(0);
				drinkScript.updatePlane();

				filling.Get<FillingMachine>().setFillNum(0);
				filling.Get<FillingMachine>().updatePlane();
				if (isInRendering(filling.Get<FillingMachine>().getFromFilling())) {
					removeFromRendering(filling.Get<FillingMachine>().getFromFilling());
				}
				filling.Get<FillingMachine>().removeFromFilling();

				topping.Get<ToppingMachine>().setTopNum(0);
				topping.Get<ToppingMachine>().updatePlane();
				if (isInRendering(topping.Get<ToppingMachine>().getFromTopping())) {
					removeFromRendering(topping.Get<ToppingMachine>().getFromTopping());
				}
				topping.Get<ToppingMachine>().removeFromTopping();
				filling.Get<Transparency>().setTransparency(0.5);
				fillingPlane.Get<Transparency>().setTransparency(0.5);
				topping.Get<Transparency>().setTransparency(0.5);
				toppingPlane.Get<Transparency>().setTransparency(0.5);
				drink.Get<Transparency>().setTransparency(0.5);
				drinkPlane.Get<Transparency>().setTransparency(0.5);
				drinkFill.getEntity()->Get<Transparency>().setTransparency(0.5);

				receipt.transform.m_pos = beginingNumberPos[0];
				for (int i = 0; i < 6; i++) {
					numberEntities[i]->transform.m_pos = beginingNumberPos[i];
				}
				for each (Entity * ent in orderBubbles[1]->returnRenderingEntities()) {
					if (isInRendering(ent)) {
						removeFromRendering(ent);
					}

					//addToRendering(ent); 
				}
				orderBubbles[1]->getOrder()->setStarted(false);
				orderBubbles[2]->getOrder()->setStarted(false);
				for each (Entity * ent in orderBubbles[2]->returnRenderingEntities()) {
					if (isInRendering(ent)) {
						removeFromRendering(ent);
					}
					//addToRendering(ent); 
				}
			}
			tutorialMultiplier = 0;

			tutorialPlane->transform.m_scale = glm::vec3((0.003 * (UIScale + 0.05)) * tutorialMultiplier);
			if (shouldUseGraphics) {
				// first pass
				glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
				glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
				//glEnable(GL_DEPTH_TEST);
			}
			for each (Entity * e in renderingEntities) {
				prog_texLit->Bind();
				prog_texLit.get()->SetUniform("transparency", 0.f);

				e->transform.RecomputeGlobal();


				if (e->Has<CMeshRenderer>()) {
					e->Get<CMeshRenderer>().Draw();
				}

				if (e->Has<CMorphMeshRenderer>()) {
					e->Get<CMorphMeshRenderer>().Draw();
				}

			}
			isScrollingDown = false;
			isScrollingUp = false;

			if (shouldUseGraphics) {


				// second pass
				glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);
				glEnable(GL_DEPTH_TEST);
				glCullFace(GL_CW);
				prog_RB->Bind();



				glDisable(GL_DEPTH_TEST);
				//glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

				glBindTexture(GL_TEXTURE_2D, 0);

				glUniform1i(albedoUniform, 0);
				glActiveTexture(GL_TEXTURE0 + 0);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

				glBindVertexArray(quadVAO);

				glDrawArrays(GL_TRIANGLES, 0, 6);
				//glViewport(0, 0, resoloutions[accessSettings[8]].x, resoloutions[accessSettings[8]].y);
			}
			//App::SwapBuffers();
			//glDeleteFramebuffers(1, &framebuffer);
			if (shouldUseGraphics) {
				glEnable(GL_DEPTH_TEST);
			}
			for each (Entity * e in UIEntities) {
				if (e->Has<Transparency>()) {
					if (e->Get<Transparency>().getWantedTransparency() > -1) {

						e->Get<Transparency>().updateTransparency(deltaTime);
					}
					e->transform.RecomputeGlobal();
					//prog_transparent->Bind();
					//prog_transparent.get()->SetUniform("transparency", e->Get<Transparency>().getTransparency());
					//std::cout << e->Get<Transparency>().getTransparency() << std::endl; 

					if (e->Has<CMeshRenderer>()) {
						e->Get<CMeshRenderer>().Draw();
					}

				}
				else
				{
					e->transform.RecomputeGlobal();
					if (e->Has<CMeshRenderer>()) {
						//prog_transparent->Bind();
						//prog_transparent.get()->SetUniform("transparency", 0.f);
						e->Get<CMeshRenderer>().Draw();
					}


				}
			}
			if (shouldUseGraphics) {
				glDisable(GL_DEPTH_TEST);
			}
			
		//	glViewport(0, 0, resoloutions[accessSettings[8]].x, resoloutions[accessSettings[8]].y);
			App::SwapBuffers();
			//glDeleteFramebuffers(1, &framebuffer);
			if (shouldUseGraphics) {
				glEnable(GL_DEPTH_TEST);
			}
			
			//glDeleteTextures(1, &textureColorbuffer);
			//glViewport(0, 0, resoloutions[accessSettings[8]].x, resoloutions[accessSettings[8]].y);
			
			continue;
		}



		if (isClickingEscape && !isCameraMoving && !isInContinueMenu) {
			
			isPaused = !isPaused;
			
			if (isPaused) {
				ovenSound.fadeOut(0.3);
				pauseMusic.fadeIn(3);
				removeFromRendering(&tray);
				removeFromRendering(tutorialPlane.get());
				//tutorialPlane->transform.m_pos = glm::vec3(-20);
				removeFromRendering(&cursor);
				for (int i = 0; i < 4; i++) {
					removeFromRendering(accessEntities[i]);
				}
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
				for (int i = 0; i < currentOrders.size(); i++) {
					OrderBubble* ob = orderBubbles[i];
					for each (Entity * ent in ob->returnRenderingEntities()) {
						removeFromRendering(ent);
					}
				}
				for each (Entity * en in trayPastry) {
					removeFromRendering(en);
				}
				for each (Entity * ch in customers) {
					if (isInRendering(ch)) {
						removeFromRendering(ch);
					}
				}
				if (isInRendering(tutorialPlane.get())) {
					removeFromRendering(tutorialPlane.get());
				}
				//App::setCursorVisible(true); 
			}


		}
		if (!isPaused) {
			//std::cout << isPaused << std::endl; 
			//tutorialPlane->transform.m_pos = glm::vec3(-20);
			timeSinceClickedSpace += deltaTime;
			if (isClickingSpace && timeSinceClickedSpace >= 0.25) {
				
				nextStepTutorialIfNeeded();
				UpdateTutorial();
				timeSinceClickedSpace = 0;
			}
			GetInput();
			tutorialOneSecondTick += deltaTime;
			
			if (tutorialOneSecondTick >= 0.75) {
				tutorialIsKeyUp = !tutorialIsKeyUp;
				for (int i = 0; i < 4; i++) {
					
					if (tutorialIsKeyUp) {
						accessEntities[i]->Remove<CMeshRenderer>();
						accessEntities[i]->Add<CMeshRenderer>(*accessEntities[i], *keyUpMat.getMesh(), *alphanumericPointer[accessSettings[i]]->getMaterial());

					}
					else
					{
						accessEntities[i]->Remove<CMeshRenderer>();
						accessEntities[i]->Add<CMeshRenderer>(*accessEntities[i], *keyDownMat.getMesh(), *alphanumericPointer[accessSettings[i]]->getMaterial());

					}

				}
				tutorialOneSecondTick = 0;
			}
			//mithunan.Get<CharacterController>().updateAnimation(deltaTime); 
			for (int u = 0; u < customers.size(); u++)
			{
				Entity* customer = customers[u];
				if (customer->Get<CharacterController>().isDoneMoving()) {

					customer->Get<CharacterController>().setStopSpot(0);
					customer->Get<CharacterController>().resetPosition();
				}
				if (customer->Get<CharacterController>().hasStopped()) {
					customer->Get<CharacterController>().queueAnimation(1);//make idle 
				}
				else
				{
					customer->Get<CharacterController>().queueAnimation(0);//make walk 

				}
				float speed = 1;
				if (customer->Get<CharacterController>().getStopSpot() > placeInLineToIndex(1)) {
					speed = 0.8;
				}
				customer->Get<CharacterController>().updateDistance(deltaTime, speed);
				customer->Get<CharacterController>().updateAnimation(deltaTime);


				if (customer->Get<CharacterController>().getStopSpot() <= placeInLineToIndex(1)) {
					int inLine = -1;
					bool isInLine = false;
					bool alreadyMoved = false;
					for (int i = 2; i >= 0; i--) {
						if (customerLine[i] == customer) {
							isInLine = true;
							inLine = i;
							break;
						}
					}
					for (int i = 2; i >= 0; i--) {
						if (!alreadyMoved) {

							if (customerLine[i] == nullptr && (i + 1) < indexToPlaceInLine(customer->Get<CharacterController>().getStopSpot())) {
								//std::cout << (i + 1) - indexToPlaceInLine(customer->Get<CharacterController>().getStopSpot()) << std::endl; 




								bool canKeepMoving = true;
								if (i < 2 && !isInLine) {
									canKeepMoving = false;
									//std::cout << "HERE2" << std::endl; 
								}
								if ((i + 1) - indexToPlaceInLine(customer->Get<CharacterController>().getStopSpot()) < -lineStart) {
									canKeepMoving = false;
									//std::cout << (i + 1) - indexToPlaceInLine(customer->Get<CharacterController>().getStopSpot()) << std::endl; 
								}
								if ((i + 1) - indexToPlaceInLine(customer->Get<CharacterController>().getStopSpot()) < -1 && isInLine) {
									//std::cout << "HERE1" << std::endl; 
									canKeepMoving = false;
								}
								if (canKeepMoving) {
									//std::cout << u << " " <<  placeInLineToIndex(i + 1) << std::endl; 
									customer->Get<CharacterController>().setStopSpot(placeInLineToIndex(i + 1));
									customerLine[i] = customer;
									alreadyMoved = true;
									if (inLine >= 0) {
										customerLine[inLine] = nullptr;
									}
								}





							}
						}

					}

				}




			}

			oven.Get<CMorphAnimator>().addToT(deltaTime);
			if (oven.Get<CMorphAnimator>().getT() < 1) {
				oven.Get<CMorphAnimator>().setMeshAndTime(allOvenFrames[1 + currentOvenPos], allOvenFrames[1 + lastOvenPos], oven.Get<CMorphAnimator>().getT());
				//std::cout << "T" << oven.Get<CMorphAnimator>().getT() << std::endl; 
				//oven.Get<CMorphMeshRenderer>().UpdateData(*allOvenFrames[1 + currentOvenPos], *allOvenFrames[1 + lastOvenPos], oven.Get<CMorphAnimator>().getT()); 

				//oven.Get<CMorphMeshRenderer>().Draw(); 

			}
			else
			{
				oven.Get<CMorphAnimator>().setMeshAndTime(allOvenFrames[1 + currentOvenPos], allOvenFrames[1 + lastOvenPos], 1);
				//std::cout << "T1" << std::endl; 
			}
			
			fridge.Get<CMorphAnimator>().addToTLessThanOne(deltaTime* fridgeMultiplier);
			fridge.Get<CMorphAnimator>().setMeshAndTime(fridgeClosedMat.getMesh().get(), fridgeOpenMat.getMesh().get(), fridge.Get<CMorphAnimator>().getT());

			

			
			

			bakeryUtils::addToGameTime(deltaTime);
			ovenScript->update(deltaTime);
			
			if (bakeryUtils::getRoundsLasted() >= 4 || !shouldShowTutorial) {

			
			for (int i = 0; i < currentOrders.size(); i++) {//pausing 
				OrderBubble* ob = orderBubbles[i];
				ob->addFill(deltaTime);
				orderBubbles[i]->getTimer().getArrow()->Get<Transparency>().setTransparency(0);
				orderBubbles[i]->getTimer().getTile()->Get<Transparency>().setTransparency(0);
				orderBubbles[i]->getTimer().getCircle()->Get<Transparency>().setTransparency(0);
				//std::cout << bakeryUtils::getTime() << " > " << currentOrders[i].maxEndTime << std::endl; 
				if (!currentOrders[i].isOnTime()) {//HERE CHANGE HERE XXX 
					//std::cout << "START" << std::endl; 
					createNewOrder(i, false, false);
					bakeryUtils::addToFailed(1);
					int failed = bakeryUtils::getOrdersFailed();
					if (failed > 3) {
						failed = 3;
					}
					ent_register->Get<CMeshRenderer>().SetMaterial(*registerImages[failed]->getMaterial());

					if (bakeryUtils::getOrdersFailed() == 3) {
						blurMulti = 1;
						int highScore = saveHighscore(bakeryUtils::getRoundsLasted());
						setScores(bakeryUtils::getRoundsLasted(), highScore);
						if (cameraX == 0 && cameraY == 0) {
							cameraX = 1;
							cameraY = 1;
						}
						receipt.transform.m_pos = cursor.transform.m_pos;
						receipt.transform.m_rotation = cursor.transform.m_rotation * glm::angleAxis(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f)) *
							glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f))
							* glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 0.0f, 1.0f));
						removeFromRendering(&cursor);
						removeFromRendering(&tray);
						tray.transform.SetParent(nullptr);
						for each (Entity * trayEntity in trayPastry) {
							removeFromRendering(trayEntity);
						}
						UIEntities.push_back(&receipt);
						for (int i = 0; i < 6; i++)
						{
							UIEntities.push_back(numberEntities[i]);

						}
						for (int i = 0; i < 4; i++) {
							accessEntities[i]->transform.SetParent(nullptr);
							removeFromRendering(accessEntities[i]);
						}
						
						receiptT = 0;
						isInContinueMenu = true;
						break;
					}
					else
					{
						for (int i = 0; i < 3; i++) {
							if (customerLine[i] != nullptr) {
								customerLine[i]->Get<CharacterController>().setStopSpot(line.size());
								customerLine[i] = nullptr;
								break;
							}
						}
						isCarMoving = true;
					}


					for each (Entity * remover in orderBubbles[i]->returnRenderingEntities()) {
						renderingEntities.erase(std::remove(renderingEntities.begin(), renderingEntities.end(), remover), renderingEntities.end());
						//std::cout << "r" << std::endl; 
					}

					resetBubble(i);

					for each (Entity * foe in orderBubbles[i]->returnRenderingEntities()) {
						addToRendering(foe);
						//std::cout << "d" << std::endl; 
					}

				}
			}
			}
			else
			{
				orderBubbles[0]->getTimer().getArrow()->Get<Transparency>().setTransparency(0.5);
				orderBubbles[0]->getTimer().getTile()->Get<Transparency>().setTransparency(0.5);
				orderBubbles[0]->getTimer().getCircle()->Get<Transparency>().setTransparency(0.5);
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
		glm::vec3 normForward = glm::normalize(cameraFacingVector);
		glm::vec3 normUp = glm::normalize(up);
		carS.forward = { normForward.x, normForward.y, normForward.z };
		carS.up = { normUp.x, normUp.y, normUp.z };
		carSound.getSound().SetEventPosition("event:/Honk", { -normForward.x, normForward.y, -normForward.z });
		//carS.up = { normForward.x, normForward.y, normForward.z };
		//listener.setForward(cameraFacingVector.x, cameraFacingVector.y, cameraFacingVector.z);
		//outputCameraFacing(cameraFacingVector); //DEBUG OPTION 
		Raycast r = Raycast(cameraEntity.transform.m_pos, cameraFacingVector, 10.f);//needs to be after GetInput so cameraQuat is initialized; 
		std::vector<glm::vec3> raycastPoints = r.crossedPoints();
		//std::cout << cameraRotEuler.x << " " << cameraRotEuler.y << " " << cameraRotEuler.z << std::endl; 
		cursor.transform.m_pos = raycastPoints[5];

		cursor.transform.m_rotation = cameraEntity.transform.m_rotation;



		receipt.transform.m_pos = cursor.transform.m_pos;
		receptEndPos = cursor.transform.m_pos;
		receipt.transform.m_rotation = cursor.transform.m_rotation * glm::angleAxis(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f)) *
			glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f))
			* glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 0.0f, 1.0f));//glm::inverse(cursor.transform.m_rotation); 


		//receipt.transform.SetParent(&cameraEntity.transform); 
		//receipt.transform.m_pos = cursor.transform.m_pos; 





		receptBeginPos = cursor.transform.m_pos + glm::cross(glm::cross(cameraFacingVector, glm::vec3(0, 1, 0)), cameraFacingVector) * -1.8f;
		//receipt.transform.m_pos = receptBeginPos; 
		receipt.transform.m_pos = receptBeginPos;
		//glm::vec3 hundredTopPos = raycastPoints[4] + (left * 0.01f) + (up * 0.03f); 

		for (int i = 0; i < 6; i++) {

			if (i < 3) {
				endNumberPos[i] = raycastPoints[4] + (left * (0.02f + (0.01f * i))) + (up * 0.03f);
			}
			else
			{
				endNumberPos[i] = raycastPoints[4] + (left * (0.02f + (0.01f * (i - 3)))) + (up * (-0.003f));
			}
			beginingNumberPos[i] = endNumberPos[i] + glm::cross(glm::cross(cameraFacingVector, glm::vec3(0, 1, 0)), cameraFacingVector) * -1.8f;

		}
		for (int i = 0; i < numberEntities.size(); i++) {
			numberEntities[i]->transform.m_rotation = receipt.transform.m_rotation;
			numberEntities[i]->transform.m_pos = beginingNumberPos[i];
			numberEntities[i]->transform.m_scale = numberScale;
		}
	
		
		if (shouldShowTutorial && bakeryUtils::getRoundsLasted() < 2 && !isPaused) {
			for (int i = 0; i < 4; i++) {

				if (trayPastry[i] != nullptr) {
					if (isInRendering(accessEntities[i])) {
						removeFromRendering(accessEntities[i]);
						//accessEntities[i]->transform.SetParent(nullptr);
					}
					
				}
				else
				{
					if (!isInRendering(accessEntities[i])) {
						addToRendering(accessEntities[i]);
						//accessEntities[i]->transform.SetParent(nullptr);
					}
				}
				

			}
		}
		else
		{
			for (int i = 0; i < 4; i++) {
				accessEntities[i]->transform.m_scale = glm::vec3(0);
			}
		}
		
		
		trayT += trayMultiplier * deltaTime * traySpeed;
		if (trayT >= 1) {
			trayMultiplier = -1;
		}
		if (trayT <= 0) {
			trayMultiplier = 0;
		}

		tray.transform.m_pos = Lerp(normalTrayPos.m_pos, lowTrayPos.m_pos, trayT);//tray movement here
		for (int i = 0; i < 4; i++) {
			if (trayPastry[i] != nullptr) {
				if (trayPastry[i]->Has<Transparency>()) {
					if (trayPastry[i]->Get<Transparency>().isFadingIn()) {
						trayPastry[i]->transform.m_pos.y = Lerp(normalTrayPos.m_pos.y, lowTrayPos.m_pos.y, trayT);
					}
				}
					
			}
			accessEntities[i]->transform.m_pos.y = Lerp(normalTrayPos.m_pos.y, lowTrayPos.m_pos.y, trayT);
			traySlot[i].m_pos.y = Lerp(normalTrayPos.m_pos.y, lowTrayPos.m_pos.y, trayT);
		}

		lastActionTime += deltaTime;
		hitEntity = nullptr;


		if (shouldUseGraphics) {
			// first pass
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
			//glEnable(GL_DEPTH_TEST);
		}

		for each (Entity * e in renderingEntities) {



			if (e->Has<Transparency>()) {
				if (e->Get<Transparency>().getWantedTransparency() > -1) {

					e->Get<Transparency>().updateTransparency(deltaTime);
				}
				e->transform.RecomputeGlobal();
				prog_texLit->Bind();
				prog_texLit.get()->SetUniform("transparency", e->Get<Transparency>().getTransparency());
				//std::cout << e->Get<Transparency>().getTransparency() << std::endl; 

				if (e->Has<CMeshRenderer>()) {
					e->Get<CMeshRenderer>().Draw();
				}
				else if (e->Has<CMorphMeshRenderer>()) {
					prog_morph->Bind();
					prog_morph.get()->SetUniform("transparency", e->Get<Transparency>().getTransparency());
					e->Get<CMorphMeshRenderer>().Draw();
					//std::cout << e->Get<Transparency>().getTransparency() << std::endl; 
				}
				
				
				//prog_transparent.get()->SetUniform("transparency", 0.f); 
			}
			else
			{
				e->transform.RecomputeGlobal();
				if (e->Has<CMeshRenderer>()) {
					prog_texLit->Bind();
					prog_texLit.get()->SetUniform("transparency", 0.f);
					e->Get<CMeshRenderer>().Draw();
				}

				if (e->Has<CMorphMeshRenderer>()) {
					prog_morph->Bind();
					prog_morph.get()->SetUniform("transparency", 0.f);
					e->Get<CMorphMeshRenderer>().Draw();
				}
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

		

		//sneaking this here since its important to only do it when the game is running
		tutorialPlane->transform.m_scale = glm::vec3((0.003 * (UIScale + 0.05))*tutorialMultiplier);

		
		


		if ((drinkScript.isOpening || drinkScript.isClosing) && !isPaused) {
			float multiplier = 1;
			if (drinkScript.isClosing) {
				multiplier = -1;
			}
			drinkScript.addToT(deltaTime * multiplier);
			drink.Get<CMorphAnimator>().setFrameAndTime(0, 1, drinkScript.getT());
			if (drinkScript.isClosing) {
				drinkScript.addFill(deltaTime * multiplier);
				drinkScript.getFillBar()->setT(drinkScript.getFill());
				drinkScript.getFillBar()->updateArrow();
			}
			if (drinkScript.isOpening) {
				drinkScript.moveDrink(drinkScript.getT());

			}
			if (drinkScript.getT() >= 1.f) {
				drinkScript.setT(1.f);
				drinkScript.isOpening = false;
				machineSound.fadeOut(0.3);
			}
			if (drinkScript.getT() <= 0.f) {
				drinkScript.setT(0.f);

				drinkScript.isClosing = false;
				drinkScript.isOpening = false;
				machineSound.fadeOut(0.3);
			}
		}

		
		
		if (lookingAtFridge) {
			fridgeLookTime += deltaTime;
		}
		else
		{
			fridgeLookTime = 0;
		}
		if (fridgeLookTime == deltaTime && lookingAtFridge) {
			fridgeHumSound.fadeIn(0.2);
			fridgeOpenSound.fadeIn(0.001);
			//std::cout << "OPEN" << std::endl;
		}
		if (fridgeLookTime == 0 && !lookingAtFridge) {
			fridgeHumSound.fadeOut(0.2);
			//std::cout << "CLOSED" << std::endl;

		}
		if (lastActionTime > deltaTime) {
			lookingAtDrink = false;
			lookingAtFilling = false;
			lookingAtFridge = false;
			lookingAtOven = false;
			lookingAtTopping = false;
		}
		if (raycastHit && hitEntity != nullptr && lastActionTime > deltaTime) {
			Entity* e = hitEntity;
			
			if (e->Has<Machine>()) {//check for fridge tomorrow 
				
				if (e->Has<Fridge>()) {
					lookingAtFridge = true;
					//log("B"); 
					//std::cout << "A" << std::endl; 
					//fridge.Get<CMorphAnimator>().setT(0);
					fridgeMultiplier = 4;
					int wantedSlot = getWantedSlot();
					if (isClicking) {
						wantedSlot = getFirstTraySlot();
					}
					if (wantedSlot >= 0 && trayPastry[wantedSlot] == nullptr) {
						trayMultiplier = 1;
						setTutorialChange(6);
						UpdateTutorial();
						int slot = wantedSlot;
						lastActionTime = 0;
						trayPastry[slot] = Entity::Allocate().release();



						trayPastry[slot]->Add<CMeshRenderer>(*trayPastry[slot], *doughMat.getMesh(), *doughMat.getMaterial());
						trayPastry[slot]->Add<Pastry>();
						trayPastry[slot]->Add<Transparency>(*trayPastry[slot]);
						trayPastry[slot]->Get<Transparency>().setTransparency(1.f);
						trayPastry[slot]->Get<Transparency>().setWantedTransparency(0.f);
						trayPastry[slot]->Get<Transparency>().setTime(0.2);


						trayPastry[slot]->transform.m_scale = glm::vec3(0.009f, 0.009f, 0.009f);
						trayPastry[slot]->transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));

						//std::cout << traySlot[slot].m_pos.x << " " << traySlot[slot].m_pos.y << " " << traySlot[slot].m_pos.z << std::endl; 

						trayPastry[slot]->transform.m_pos = traySlot[slot].m_pos;
						trayPastry[slot]->transform.SetParent(&globalCameraEntity->transform);
						addedSlot = slot;
						//std::cout << "B" << std::endl; 

						//nextStepTutorialIfNeeded(2);
						//if (shouldShowTutorial) {
							
						//}
						//pop.SetEventParameter();
						
						playSound(&pop, "DoughTray");
					}




				}
				else if (e->Has<Oven>()) {
					lookingAtOven = true;
					currentOvenPos = selectedOvenPosition(currentPoint.y);
					lastOvenPos = selectedOvenPosition(lastPoint.y);
					if (currentOvenPos >= 0 && currentOvenPos != lastOvenPos) {
						oven.Get<CMorphAnimator>().setMeshAndTime(allOvenFrames[1 + currentOvenPos], allOvenFrames[1 + lastOvenPos], 0);
						oven.Get<CMorphAnimator>().setT(0);
						//std::cout << currentOvenPos << std::endl; 



						//ovenEntites[currentOvenPos]->Get<MorphAnimation>().reverseFrames(ovenEntites[currentOvenPos]); 


					}
					//log("A"); 
					//std::cout << "OVEN" << std::endl; 
					int pressedSlot = getWantedSlot();

					int lookingSlot = pressedSlot;
					if (currentOvenPos >= 0) {
						lookingSlot = 3 - currentOvenPos;
					}
					int affectedTraySlot = -1;//position on tray to use 
					int affectedOvenSlot = -1;//position in oven to use 

					if (pressedSlot >= 0) {
						//std::cout << "A" << std::endl; 

						bool putInOven = false;
						if (lookingSlot >= 0) {
							if (ovenScript->isSlotFull(lookingSlot)) {
								if (trayPastry[pressedSlot] != nullptr) {
									if (trayPastry[pressedSlot]->Has<Pastry>()) {
										if (trayPastry[pressedSlot]->Get<Pastry>().getPastryType() == bakeryUtils::pastryType::DOUGH) {
											if (!trayPastry[pressedSlot]->Get<Pastry>().isInOven()) {
												//its dough 
												affectedTraySlot = pressedSlot;
												affectedOvenSlot = ovenScript->getFirstEmpty();
												putInOven = true;
											}

										}
										else
										{
											affectedTraySlot = getFirstTraySlot();
											affectedOvenSlot = lookingSlot;
											putInOven = false;
										}
									}
									else
									{
										//its a drink, dont do anything 
									}
								}
								else
								{
									//its for sure empty 
									affectedOvenSlot = lookingSlot;
									affectedTraySlot = pressedSlot;
									putInOven = false;
								}
							}
							else
							{//slot not full 
								if (trayPastry[pressedSlot] != nullptr) {
									if (trayPastry[pressedSlot]->Has<Pastry>()) {
										if (trayPastry[pressedSlot]->Get<Pastry>().getPastryType() == bakeryUtils::pastryType::DOUGH) {
											affectedTraySlot = pressedSlot;
											affectedOvenSlot = lookingSlot;
											putInOven = true;
										}
									}
								}

							}
						}



						if (putInOven && affectedTraySlot >= 0 && affectedOvenSlot >= 0) {
							playSound(&pop, "DoughTray");
							if (ovenScript->getActivePastries() == 0) {
								ovenSound.fadeIn(0.3);
							}
							trayMultiplier = 1;
							setTutorialChange(7);
							UpdateTutorial();
							lastActionTime = 0;
							//std::cout << "B" << std::endl; 
							ovenScript->canAdd(trayPastry[affectedTraySlot], affectedOvenSlot);
							trayPastry[affectedTraySlot]->Get<Transparency>().setTransparency(0.f);
							trayPastry[affectedTraySlot]->Get<Transparency>().setNextPosition(ovenScript->getInsideOven()->m_pos, nullptr);
							trayPastry[affectedTraySlot]->Get<Transparency>().setWantedTransparency(1);
							trayPastry[affectedTraySlot]->Get<Transparency>().setTime(0.15);
							trayPastry[affectedTraySlot]->Get<Pastry>().setInOven(true);
							trayPastry[affectedTraySlot] = nullptr;
							//nextStepTutorialIfNeeded(3);
						}
						else if (!putInOven && affectedTraySlot >= 0 && affectedOvenSlot >= 0)
						{
							playSound(&pop, "DoughTray");
							
							trayMultiplier = 1;
							lastActionTime = 0;
							//std::cout << "C" << std::endl; 
							int newSlot = affectedTraySlot;
							//std::cout << newSlot << std::endl; 

							if (ovenScript->canRemove(affectedOvenSlot) && newSlot >= 0) {
								//std::cout << newSlot << std::endl; 
								if (ovenScript->getActivePastries() == 1) {
									ovenSound.fadeOut(0.3);
								}
								trayPastry[newSlot] = &ovenScript->getEntity(affectedOvenSlot);

								glm::vec3 finalPos = traySlot[newSlot].m_pos;
								finalPos.y += getTrayRaise(trayPastry[newSlot]->Get<Pastry>().getPastryType());

								trayPastry[newSlot]->transform.m_pos = traySlot[newSlot].m_pos;
								trayPastry[newSlot]->transform.m_pos.y += getTrayRaise(trayPastry[newSlot]->Get<Pastry>().getPastryType());
								trayPastry[newSlot]->transform.SetParent(&globalCameraEntity->transform);
								setTrayPastryMesh(trayPastry[newSlot], trayPastry[newSlot]->Get<Pastry>().getPastryType());
								trayPastry[newSlot]->Get<Pastry>().setInOven(false);
								trayPastry[newSlot]->Get<Transparency>().setTransparency(1.f);
								//trayPastry[newSlot]->Get<Transparency>().setNextPosition(finalPos, &globalCameraEntity->transform);
								trayPastry[newSlot]->Get<Transparency>().setNextTransformPos(&traySlot[newSlot], &globalCameraEntity->transform);
								trayPastry[newSlot]->Get<Transparency>().setWantedTransparency(0.f);
								trayPastry[newSlot]->Get<Transparency>().setTime(0.15);

								if (!isInRendering(trayPastry[newSlot])) {
									addToRendering(trayPastry[newSlot]);
								}
								/*
								if (trayPastry[newSlot]->Get<Pastry>().getPastryType() == bakeryUtils::pastryType::DOUGH)
								{
									trayPastry[newSlot]->Get<Pastry>().setCookedSeconds(wantedSlot);//should it reset seconds if not completed?
								}
								*/
								ovenScript->removeFromSlot(affectedOvenSlot);
								if (trayPastry[newSlot]->Get<Pastry>().getPastryType() == bakeryUtils::pastryType::BURNT) {
									
									//nextStepTutorialIfNeeded(4);
									//nextStepTutorialIfNeeded();
									setTutorialChange(2);
									UpdateTutorial();
								}
								else if (trayPastry[newSlot]->Get<Pastry>().getPastryType() == bakeryUtils::pastryType::CROISSANT) {
									setTutorialChange(8);
									UpdateTutorial();
								}
								
							}
						}

					}
				}
				else if (e->Has<FillingMachine>() && roundsLasted > 0) {
				lookingAtFilling = true;
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
								lastActionTime = 0;
								fillingScript.getFromFilling()->Get<Pastry>().setFilling(fillingScript.getFilling());
								//put into tray now 
								int wantedSlot = getFirstTraySlot();

								if (wantedSlot >= 0) {
									trayMultiplier = 1;
									trayPastry[wantedSlot] = fillingScript.getFromFilling();
									float currentT, wantedT, time;
									time = 0.15;
									currentT = 0.f;
									wantedT = 1.f;
									glm::vec3 finalPos = traySlot[wantedSlot].m_pos;
									finalPos.y += getTrayRaise(trayPastry[wantedSlot]->Get<Pastry>().getPastryType());

									trayPastry[wantedSlot]->Get<Transparency>().setTransparency(currentT);
									//trayPastry[wantedSlot]->Get<Transparency>().setNextPosition(finalPos, &globalCameraEntity->transform, trayPastry[wantedSlot]->transform.m_scale);
									trayPastry[wantedSlot]->Get<Transparency>().setNextTransformPos(&traySlot[wantedSlot], &globalCameraEntity->transform, trayPastry[wantedSlot]->transform.m_scale);

									trayPastry[wantedSlot]->Get<Transparency>().setWantedTransparency(wantedT);
									trayPastry[wantedSlot]->Get<Transparency>().setTime(time);
									trayPastry[wantedSlot]->Get<Transparency>().setNextWantedTransparency(currentT);
									trayPastry[wantedSlot]->Get<Transparency>().setNextWantedTime(time);
									trayPastry[wantedSlot]->Get<Transparency>().setNextWantedScale(trayPastry[wantedSlot]->transform.m_scale * 0.1f);

									trayPastry[wantedSlot]->Get<Pastry>().setInFilling(false);
									fillingScript.removeFromFilling();
									//UPDATE FILLING HERE 
									setPastryFilling(trayPastry[wantedSlot], trayPastry[wantedSlot]->Get<Pastry>().getPastryFilling());
									//std::cout << bakeryUtils::getFillingName(trayPastry[wantedSlot]->Get<Pastry>().getPastryFilling()) << std::endl; 
									machineSound2.fadeOut(0.3);
									playSound(&pop, "DoughTray");
								}

							}
							//std::cout << (int)fillingScript.getFromFilling()->Get<Pastry>().getPastryFilling() << std::endl; 

						}



						//std::cout << fillingScript.getT() << std::endl; 

					}
					else if (scrollY != 0) {
						lastActionTime = 0;
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
											&& trayPastry[wantedSlot]->Get<Pastry>().getPastryType() != bakeryUtils::pastryType::BURNT
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
								playSound(&pop, "DoughTray");
								machineSound2.fadeIn(0.3);
								trayMultiplier = 1;
								lastActionTime = 0;
								//std::cout << "B" << std::endl; 
								//ovenScript->canAdd(trayPastry[wantedSlot], wantedSlot); 


								float currentT, wantedT, time;
								time = 0.15;
								currentT = 0.f;
								wantedT = 1.f;

								trayPastry[wantedSlot]->Get<Transparency>().setTransparency(currentT);
								trayPastry[wantedSlot]->Get<Transparency>().setNextPosition(fillingScript.getFillingPosition(), nullptr, trayPastry[wantedSlot]->transform.m_scale);
								trayPastry[wantedSlot]->Get<Transparency>().setWantedTransparency(wantedT);
								trayPastry[wantedSlot]->Get<Transparency>().setTime(time);
								trayPastry[wantedSlot]->Get<Transparency>().setNextWantedTransparency(currentT);
								trayPastry[wantedSlot]->Get<Transparency>().setNextWantedTime(time);
								trayPastry[wantedSlot]->Get<Transparency>().setNextWantedScale(trayPastry[wantedSlot]->transform.m_scale * 10.f);


								fillingScript.putInFilling(trayPastry[wantedSlot]);

								//


								trayPastry[wantedSlot]->Get<Pastry>().setInFilling(true);
								//setMachinePastryMesh(trayPastry[wantedSlot], trayPastry[wantedSlot]->Get<Pastry>().getPastryType()); 


								trayPastry[wantedSlot] = nullptr;

							}
							else
							{

								lastActionTime = 0;
								//put in tray slot 
								//std::cout << "C" << std::endl; 
								//int newSlot = wantedSlot(); 
								//std::cout << newSlot << std::endl; 
								if (fillingScript.isFillingFull() && wantedSlot >= 0 && trayPastry[wantedSlot] == nullptr) {
									//std::cout << newSlot << std::endl; 
									playSound(&pop, "DoughTray");
									trayMultiplier = 1;
									trayPastry[wantedSlot] = fillingScript.getFromFilling();
									float currentT, wantedT, time;
									time = 0.15;
									currentT = 0.f;
									wantedT = 1.f;
									glm::vec3 finalPos = traySlot[wantedSlot].m_pos;
									finalPos.y += getTrayRaise(trayPastry[wantedSlot]->Get<Pastry>().getPastryType());

									trayPastry[wantedSlot]->Get<Transparency>().setTransparency(currentT);
									//trayPastry[wantedSlot]->Get<Transparency>().setNextPosition(finalPos, &globalCameraEntity->transform, trayPastry[wantedSlot]->transform.m_scale);
									trayPastry[wantedSlot]->Get<Transparency>().setNextTransformPos(&traySlot[wantedSlot], &globalCameraEntity->transform, trayPastry[wantedSlot]->transform.m_scale);

									trayPastry[wantedSlot]->Get<Transparency>().setWantedTransparency(wantedT);
									trayPastry[wantedSlot]->Get<Transparency>().setTime(time);
									trayPastry[wantedSlot]->Get<Transparency>().setNextWantedTransparency(currentT);
									trayPastry[wantedSlot]->Get<Transparency>().setNextWantedTime(time);
									trayPastry[wantedSlot]->Get<Transparency>().setNextWantedScale(trayPastry[wantedSlot]->transform.m_scale * 0.1f);

									//trayPastry[wantedSlot]->transform.m_pos = traySlot[wantedSlot].m_pos; 
									//trayPastry[wantedSlot]->transform.m_pos.y += getTrayRaise(trayPastry[wantedSlot]->Get<Pastry>().getPastryType()); 
									//trayPastry[wantedSlot]->transform.SetParent(&globalCameraEntity->transform); 
									trayPastry[wantedSlot]->Get<Pastry>().setInFilling(false);
									//setTrayPastryMesh(trayPastry[wantedSlot], trayPastry[wantedSlot]->Get<Pastry>().getPastryType()); 

									fillingScript.removeFromFilling();
									//dont set texture here cause they just removed it 
									machineSound2.fadeOut(0.3);

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
				else if (e->Has<ToppingMachine>() && roundsLasted > 1) {
				lookingAtTopping = true;
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
									trayMultiplier = 1;
									lastActionTime = 0;
									trayPastry[wantedSlot] = toppingScript.getFromTopping();
									float currentT, wantedT, time;
									time = 0.15;
									currentT = 0.f;
									wantedT = 1.f;
									glm::vec3 finalPos = traySlot[wantedSlot].m_pos;
									finalPos.y += getTrayRaise(trayPastry[wantedSlot]->Get<Pastry>().getPastryType());

									trayPastry[wantedSlot]->Get<Transparency>().setTransparency(currentT);
									//trayPastry[wantedSlot]->Get<Transparency>().setNextPosition(finalPos, &globalCameraEntity->transform, trayPastry[wantedSlot]->transform.m_scale);
									trayPastry[wantedSlot]->Get<Transparency>().setNextTransformPos(&traySlot[wantedSlot], &globalCameraEntity->transform, trayPastry[wantedSlot]->transform.m_scale);
									trayPastry[wantedSlot]->Get<Transparency>().setWantedTransparency(wantedT);
									trayPastry[wantedSlot]->Get<Transparency>().setTime(time);
									trayPastry[wantedSlot]->Get<Transparency>().setNextWantedTransparency(currentT);
									trayPastry[wantedSlot]->Get<Transparency>().setNextWantedTime(time);
									trayPastry[wantedSlot]->Get<Transparency>().setNextWantedScale(getTrayScale(trayPastry[wantedSlot]->Get<Pastry>().getPastryType()));

									trayPastry[wantedSlot]->Get<Pastry>().setInTopping(false);
									toppingScript.removeFromTopping();
									//UPDATE FILLING HERE 
									setPastryTopping(trayPastry[wantedSlot], trayPastry[wantedSlot]->Get<Pastry>().getPastryTopping());
									//trayPastry[wantedSlot]->transform.m_pos = toppingScript.getToppingPosition();

									//std::cout << bakeryUtils::getToppingName(trayPastry[wantedSlot]->Get<Pastry>().getPastryTopping()) << std::endl; 
									toppingSound.fadeOut(0.3);
									playSound(&pop, "DoughTray");
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
											&& trayPastry[wantedSlot]->Get<Pastry>().getPastryType() != bakeryUtils::pastryType::BURNT
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
								trayMultiplier = 1;
								toppingSound.fadeIn(0.3);
								playSound(&pop, "DoughTray");
								//std::cout << "B" << std::endl; 
								//ovenScript->canAdd(trayPastry[wantedSlot], wantedSlot); 
								toppingScript.putInTopping(trayPastry[wantedSlot]);
								float currentT, wantedT, time;
								time = 0.15;
								currentT = 0.f;
								wantedT = 1.f;

								trayPastry[wantedSlot]->Get<Transparency>().setTransparency(currentT);
								trayPastry[wantedSlot]->Get<Transparency>().setNextPosition(toppingScript.getToppingPosition(), nullptr, trayPastry[wantedSlot]->transform.m_scale);
								trayPastry[wantedSlot]->Get<Transparency>().setWantedTransparency(wantedT);
								trayPastry[wantedSlot]->Get<Transparency>().setTime(time);
								trayPastry[wantedSlot]->Get<Transparency>().setNextWantedTransparency(currentT);
								trayPastry[wantedSlot]->Get<Transparency>().setNextWantedTime(time);
								trayPastry[wantedSlot]->Get<Transparency>().setNextWantedScale(getTrayScale(trayPastry[wantedSlot]->Get<Pastry>().getPastryType()) * 10.f);

								trayPastry[wantedSlot]->Get<Pastry>().setInTopping(true);
								//setMachinePastryMesh(trayPastry[wantedSlot], trayPastry[wantedSlot]->Get<Pastry>().getPastryType()); 

								trayPastry[wantedSlot] = nullptr;
								//toppingSound.fadeOut(0.1);
							}
							else
							{
								//put in tray slot 
								//std::cout << "C" << std::endl; 
								//int newSlot = wantedSlot(); 
								//std::cout << newSlot << std::endl; 
								if (toppingScript.isToppingFull() && wantedSlot >= 0 && trayPastry[wantedSlot] == nullptr) {
									//std::cout << newSlot << std::endl; 
									playSound(&pop, "DoughTray");
									trayMultiplier = 1;
									trayPastry[wantedSlot] = toppingScript.getFromTopping();
									float currentT, wantedT, time;
									time = 0.15;
									currentT = 0.f;
									wantedT = 1.f;
									glm::vec3 finalPos = traySlot[wantedSlot].m_pos;
									finalPos.y += getTrayRaise(trayPastry[wantedSlot]->Get<Pastry>().getPastryType());
								
									trayPastry[wantedSlot]->Get<Transparency>().setTransparency(currentT);
								//	trayPastry[wantedSlot]->Get<Transparency>().setOutput(true);
									//trayPastry[wantedSlot]->Get<Transparency>().setNextPosition(finalPos, &globalCameraEntity->transform, trayPastry[wantedSlot]->transform.m_scale);
									trayPastry[wantedSlot]->Get<Transparency>().setNextTransformPos(&traySlot[wantedSlot], &globalCameraEntity->transform, trayPastry[wantedSlot]->transform.m_scale);
									trayPastry[wantedSlot]->Get<Transparency>().setWantedTransparency(wantedT);
									trayPastry[wantedSlot]->Get<Transparency>().setTime(time);
									trayPastry[wantedSlot]->Get<Transparency>().setNextWantedTransparency(currentT);
									trayPastry[wantedSlot]->Get<Transparency>().setNextWantedTime(time);
									trayPastry[wantedSlot]->Get<Transparency>().setNextWantedScale(getTrayScale(trayPastry[wantedSlot]->Get<Pastry>().getPastryType()));

									trayPastry[wantedSlot]->Get<Pastry>().setInTopping(false);
									toppingScript.removeFromTopping();
									//trayPastry[wantedSlot]->transform.m_pos = toppingScript.getToppingPosition();
									//dont set texture here cause they just removed it 
									std::cout << "BEFORE ->" <<trayPastry[wantedSlot]->transform.m_pos.x << " "
										<< trayPastry[wantedSlot]->transform.m_pos.y << " "
										<< trayPastry[wantedSlot]->transform.m_pos.z << " " << std::endl;
									toppingSound.fadeOut(0.3);
									

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
				else if (e->Has<DrinkMachine>() && roundsLasted > 2) {
				lookingAtDrink = true;
					if (!drinkScript.isOpening && !drinkScript.isClosing && !drinkScript.isDrinkFull()) {
						if (isLeftButtonHeld) {
							if (drinkScript.getFill() <= deltaTime) {
								drinkUpSound.fadeIn(0.001);
							}
							
							lastActionTime = 0;
							drinkScript.addFill((deltaTime / bakeryUtils::getDrinkFillAmount()) * 2);//*2 because it only goes every other frame cause of the last action time check up top 
							drinkScript.getFillBar()->setT(drinkScript.getFill());
							drinkScript.getFillBar()->updateArrow();
							//std::cout << drinkScript.getFill() << std::endl;
						}
						else if (scrollY != 0) {
							lastActionTime = 0;
							drinkScript.addDrinkNum(scrollY);
							drinkScript.updatePlane();
						}
						else
						{
							drinkUpSound.fadeOut(0.3);
							if (drinkScript.getFill() > 0.80f && drinkScript.getFill() < 1.05f
								&& (!drinkScript.isOpening || !drinkScript.isClosing)) {
								machineSound.fadeIn(0.3);
								drinkScript.isOpening = true;
								drinkScript.isClosing = false;
								if (!drinkScript.isDrinkFull()) {
									//lastActionTime = 0; 
									drinkScript.createDrink();
									setDrinkMesh(drinkScript.getFromDrink(), drinkScript.getDrink());
									//drinkScript.getFromDrink()->transform.m_scale = glm::vec3(0.002); 
									drinkScript.getFromDrink()->Add<Drink>();
									drinkScript.getFromDrink()->Get<Drink>().setDrinkType(drinkScript.getDrink());
									addToRendering(drinkScript.getFromDrink());

								}


								//create drink entity here 
							}
							else
							{
								//machineSound.fadeOut(0.3);
								drinkScript.setFill(0.f);
								drinkScript.getFillBar()->setT(drinkScript.getFill());
								drinkScript.getFillBar()->updateArrow();

							}

						}
					}
					else if (drinkScript.isDrinkFull() && lastActionTime >= 0.3) {
						if (getWantedSlot() != -1) {
							lastActionTime = 0;
							//put in tray 
							//std::cout << getWantedSlot() << std::endl; 
							int wantedSlot = getWantedSlot();
							if (trayPastry[wantedSlot] == nullptr) {
								trayMultiplier = 1;
								trayPastry[wantedSlot] = drinkScript.releaseFromDrink();
								//setDrinkMesh(trayPastry[wantedSlot], trayPastry[wantedSlot]->Get<Drink>().getDrinkType()); 
								//trayPastry[wantedSlot]->transform.m_pos = traySlot[wantedSlot].m_pos; 
								//trayPastry[wantedSlot]->transform.m_scale /= 5; 
								glm::vec3 finalPos = traySlot[wantedSlot].m_pos;
								//finalPos.y += getTrayRaise(trayPastry[wantedSlot]->Get<Pastry>().getPastryType()); 
								float currentT, wantedT, time;
								time = 0.3;
								currentT = 0.f;
								wantedT = 1.f;

								trayPastry[wantedSlot]->Get<Transparency>().setTransparency(currentT);
								//trayPastry[wantedSlot]->Get<Transparency>().setNextPosition(finalPos, &globalCameraEntity->transform, trayPastry[wantedSlot]->transform.m_scale);
								trayPastry[wantedSlot]->Get<Transparency>().setNextTransformPos(&traySlot[wantedSlot], &globalCameraEntity->transform, trayPastry[wantedSlot]->transform.m_scale);
								trayPastry[wantedSlot]->Get<Transparency>().setWantedTransparency(wantedT);
								trayPastry[wantedSlot]->Get<Transparency>().setTime(0.1);
								trayPastry[wantedSlot]->Get<Transparency>().setNextWantedTransparency(currentT);
								trayPastry[wantedSlot]->Get<Transparency>().setNextWantedTime(time);
								trayPastry[wantedSlot]->Get<Transparency>().setNextWantedScale(trayPastry[wantedSlot]->transform.m_scale * (0.2f));


								//trayPastry[wantedSlot]->transform.m_pos.y += getTrayRaise(trayPastry[wantedSlot]->Get<Pastry>().getPastryType()); 

								drinkScript.removeFromDrink();

								drinkScript.isClosing = true;
								drinkScript.isOpening = false;
								machineSound.fadeIn(0.3);
								playSound(&pop, "DoughTray");
							}



						}

					}

				}
				else if (e->Has<TrashCan>()) {
					//std::cout << "HERE" << std::endl; 
					int wantedSlot = getWantedSlot();
					if (wantedSlot >= 0) {
						if (trayPastry[wantedSlot] != nullptr) {
							playSound(&pop, "DoughTray");
							trayMultiplier = 1;
							setTutorialChange(3);
							UpdateTutorial();
							lastActionTime = 0;
							removeFromRendering(trayPastry[wantedSlot]);
							trayPastry[wantedSlot] = nullptr;
						}

					}
				}





			}
			else if (e->Has<Register>()) {
				//check if order is complete here 
				if (isClicking || getWantedSlot() >= 0) {
					for (int u = 0; u < currentOrders.size(); u++) {
						Order& searchingOrder = currentOrders[u];
						int validatedPastryIndex = -1;
						int validatedDrinkIndex = -1;
						if (searchingOrder.drink == bakeryUtils::drinkType::NONE) {
							searchingOrder.setDrinkValidated(true);
							validatedDrinkIndex = -2;
						}
						for (int i = 0; i < 4; i++) {
							Entity* currentItem = trayPastry[i];
							if (currentItem != nullptr) {
								if (currentItem->Has<Pastry>()) {
									Pastry& p = currentItem->Get<Pastry>();
									if (searchingOrder.validateOrder(p) && validatedPastryIndex == -1) {
										validatedPastryIndex = i;
										searchingOrder.setPastryValidated(true);
									}
								}
								else if (currentItem->Has<Drink>()) {
									Drink& p = currentItem->Get<Drink>();
									if (searchingOrder.validateDrink(p) && validatedDrinkIndex == -1) {
										validatedDrinkIndex = i;
										searchingOrder.setDrinkValidated(true);
									}
								}
							}


						}

						if (searchingOrder.returnSatisfied()) {
							trayMultiplier = 1;
							//nextStepTutorialIfNeeded(7);
							if (validatedPastryIndex >= 0) {

								//renderingEntities.erase(std::remove(renderingEntities.begin(), renderingEntities.end(), trayPastry[pastryIndex]), renderingEntities.end()); 
								removeFromRendering(trayPastry[validatedPastryIndex]);
								trayPastry[validatedPastryIndex] = nullptr;
								//	std::cout << "NOT PASTRY -1" << std::endl; 

							}
							else
							{
								//std::cout << "IS PASTRY -1" << std::endl; 

							}
							if (validatedDrinkIndex >= 0) {
								//renderingEntities.erase(std::remove(renderingEntities.begin(), renderingEntities.end(), trayPastry[drinkIndex]), renderingEntities.end()); 
								removeFromRendering(trayPastry[validatedDrinkIndex]);
								trayPastry[validatedDrinkIndex] = nullptr;
								//std::cout << "NOT DRINK -1" << std::endl; 
							}
							else
							{
								//int a = 1; 
								//std::cout << "IS DRINK -1" << std::endl; 
							}
							if (bakeryUtils::getRoundsLasted() == 0) {
								setTutorialChange(9);
								UpdateTutorial();
							}
							else if (bakeryUtils::getRoundsLasted() == 1) {
								for (int i = 0; i < 4; i++) {
									if (isInRendering(accessEntities[i])) {
										removeFromRendering(accessEntities[i]);
										accessEntities[i]->transform.SetParent(nullptr);
									}
								}
								setTutorialChange(11);
								UpdateTutorial();
							}
							else if (bakeryUtils::getRoundsLasted() == 2) {
								setTutorialChange(13);
								UpdateTutorial();
							}
							else if (bakeryUtils::getRoundsLasted() == 3) {
								setTutorialChange(14);
								UpdateTutorial();
								
							}
							chooseAndPlaySong();
							bakeryUtils::addToRounds(1);
							//std::cout << "HERE" << std::endl; 
							createNewOrder(u, true);
							for each (Entity * remover in orderBubbles[u]->returnRenderingEntities()) {
								//renderingEntities.erase(std::remove(renderingEntities.begin(), renderingEntities.end(), remover), renderingEntities.end()); 
								removeFromRendering(remover);
							}

							resetBubble(u);
							currentConfettiEmission = confettiEmissionTime;
							//confettiEmission = true;
							
							
							playSound(&ding, "CashRegister");
							for each (Entity * foe in orderBubbles[u]->returnRenderingEntities()) {
								addToRendering(foe);
							}

							for (int f = 0; f < 3; f++) {
								if (customerLine[f] != nullptr) {
									customerLine[f]->Get<CharacterController>().setStopSpot(line.size());
									customerLine[f] = nullptr;
									break;
								}
							}

						}
						else
						{
							searchingOrder.setDrinkValidated(false);
							searchingOrder.setPastryValidated(false);
						}
					}
				}
			}
			
			
			

		}
		else
		{
		if (!raycastHit || hitEntity == nullptr)
		{
			fridgeMultiplier = -4;
		}
		else if(hitEntity != nullptr)
		{
			Entity* e = hitEntity;
			if (!e->Has<Fridge>()) {
				fridgeMultiplier = -4;
			}
		}
		
		
		}

		
		
			confettiEmitter.transform.RecomputeGlobal();
			
			confettiEmitter.Get<CParticleSystem>().Update(deltaTime,confettiEmission,true);

			glDisable(GL_DEPTH_TEST);
			confettiEmitter.Get<CParticleSystem>().Draw();
			glEnable(GL_DEPTH_TEST);
		

		if (shouldUseGraphics) {

		
		// second pass
		glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_CW);
		prog_RB->Bind();



		glDisable(GL_DEPTH_TEST);
		//glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

		glBindTexture(GL_TEXTURE_2D, 0);

		glUniform1i(albedoUniform, 0);
		glActiveTexture(GL_TEXTURE0 + 0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

		glBindVertexArray(quadVAO);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glEnable(GL_DEPTH_TEST);
		}
		for each (Entity * e in UIEntities) {
			if (e->Has<Transparency>()) {
				if (e->Get<Transparency>().getWantedTransparency() > -1) {

					e->Get<Transparency>().updateTransparency(deltaTime);
				}
				e->transform.RecomputeGlobal();
				//prog_transparent->Bind();
				//prog_transparent.get()->SetUniform("transparency", e->Get<Transparency>().getTransparency());
				//std::cout << e->Get<Transparency>().getTransparency() << std::endl; 

				if (e->Has<CMeshRenderer>()) {
					e->Get<CMeshRenderer>().Draw();
				}

			}
			else
			{
				e->transform.RecomputeGlobal();
				if (e->Has<CMeshRenderer>()) {
					//prog_transparent->Bind();
				//	prog_transparent.get()->SetUniform("transparency", 0.f);
					e->Get<CMeshRenderer>().Draw();
				}


			}
		}

		if (addedSlot >= 0) {
			addToRendering(trayPastry[addedSlot]);
		}
		
		if (orderBubblesToRemove.size() > 0) {


			for (int i = 0; i < orderBubblesToRemove.size(); i++) {//orderBubblesToRemove.size() 

				for each (Entity * remover in orderBubbles[orderBubblesToRemove[i]]->returnRenderingEntities()) {
					renderingEntities.erase(std::remove(renderingEntities.begin(), renderingEntities.end(), remover), renderingEntities.end());

				}

				resetBubble(orderBubblesToRemove[i]);

				for each (Entity * foe in orderBubbles[orderBubblesToRemove[i]]->returnRenderingEntities()) {
					addToRendering(foe);
				}
			}


			if ((bakeryUtils::getRoundsLasted() >= 6 && bakeryUtils::getDifficulty() >= 3 && currentOrders.size() == 1)
				) {//|| currentOrders.size() == 1 
				setTutorialChange(17);
				UpdateTutorial();
				tablet->Remove<CMeshRenderer>();
				tablet->Add<CMeshRenderer>(*tablet, *tabletMats[1]->getMesh(), *tabletMats[1]->getMaterial());

				//std::cout << "JJJ" << std::endl; 
				createNewOrder(1, false, false);
				//orderBubbleTimers.push_back(&upurrTimer1); 
				resetBubble(1);


				for each (Entity * foe in orderBubbles[1]->returnRenderingEntities()) {
					addToRendering(foe);
				}
			}


			//if (bakeryUtils::getRoundsLasted() == 6 && bakeryUtils::getDifficulty() >= 3) { 
			//std::cout << bakeryUtils::getRoundsLasted() << " " << bakeryUtils::getDifficulty() << std::endl; 
			if ((bakeryUtils::getRoundsLasted() >= 10 && bakeryUtils::getDifficulty() >= 3 && currentOrders.size() == 2)
				) {//|| currentOrders.size() == 2 
				tablet->Remove<CMeshRenderer>();
				tablet->Add<CMeshRenderer>(*tablet, *tabletMats[2]->getMesh(), *tabletMats[2]->getMaterial());
				setTutorialChange(20);
				UpdateTutorial();
				//std::cout << "JJJ" << std::endl; 
				createNewOrder(2, false, false);
				//orderBubbleTimers.push_back(&upurrTimer1); 

				resetBubble(2);


				for each (Entity * foe in orderBubbles[2]->returnRenderingEntities()) {
					addToRendering(foe);
				}
			}
			
			
		}
		orderBubblesToRemove.clear();
		//std::cout << (currentPoint.x - lastPoint.x) << " " << (currentPoint.y - lastPoint.y) << " " << (currentPoint.z - lastPoint.z) << std::endl; 
		lastPoint = currentPoint;
		scrollX = 0;
		scrollY = 0;
		isScrollingDown = false;
		isScrollingUp = false;
		// Draw everything we queued up to the screen 
		App::SwapBuffers();
	}
	std::fill(std::begin(trayPastry), std::end(trayPastry), nullptr);
	//std::fill(std::begin(tutorialMasterList), std::end(tutorialMasterList), nullptr); 
	dialogueList.clear();
	tutorialPlane.release();
	//tutorialPlane = nullptr; 
	//audioEngine.Quit();
	//std::fill(std::begin(trayPastry), std::end(trayPastry), nullptr); 
	// Destroy window 
	App::Cleanup();

	return 0;
}



void LoadDefaultResources()
{
	try
	{
		// Load in the shaders we will need and activate them 
	// Textured lit shader 
		std::unique_ptr vs_texLitShader = std::make_unique<Shader>("shaders/texturedlit.vert", GL_VERTEX_SHADER);
		//std::unique_ptr fs_texLitShader = std::make_unique<Shader>("shaders/texturedlit.frag", GL_FRAGMENT_SHADER);
		std::unique_ptr fs_texLitShader = std::make_unique<Shader>("shaders/stippling.frag", GL_FRAGMENT_SHADER);
		std::vector<Shader*> texLit = { vs_texLitShader.get(), fs_texLitShader.get() };
		prog_texLit = std::make_unique<ShaderProgram>(texLit);
	}
	catch (const std::exception& e) {
		std::cout << "TEXLIT SHADER ERROR: " << e.what() << std::endl;
	}
	

	if (shouldUseGraphics) {
		try
		{
			std::unique_ptr vs_RB = std::make_unique<Shader>("shaders/RB.vert", GL_VERTEX_SHADER);
			//std::unique_ptr fs_texLitShader = std::make_unique<Shader>("shaders/texturedlit.frag", GL_FRAGMENT_SHADER);
			std::unique_ptr fs_RB = std::make_unique<Shader>("shaders/RB.frag", GL_FRAGMENT_SHADER);
			std::vector<Shader*> RB = { vs_RB.get(), fs_RB.get() };
			prog_RB = std::make_unique<ShaderProgram>(RB);
		}
		catch (const std::exception& e) {
			std::cout << "RENDERBUFFER ERROR: " << e.what() << std::endl;
		}
		
	}
	

	try
	{
		std::unique_ptr vs_UIShader = std::make_unique<Shader>("shaders/texturedlit.vert", GL_VERTEX_SHADER);
		//std::unique_ptr fs_texLitShader = std::make_unique<Shader>("shaders/texturedlit.frag", GL_FRAGMENT_SHADER);
		std::unique_ptr fs_UIShader = std::make_unique<Shader>("shaders/UI.frag", GL_FRAGMENT_SHADER);
		std::vector<Shader*> UIShader = { vs_UIShader.get(), fs_UIShader.get() };
		prog_UI = std::make_unique<ShaderProgram>(UIShader);
	}
	catch (const std::exception& e) {
		std::cout << "UI SHADER ERROR: " << e.what() << std::endl;
	}
	

	

	
	auto v_morph = std::make_unique<Shader>("shaders/morph.vert", GL_VERTEX_SHADER);
	//auto f_lit = std::make_unique<Shader>("shaders/lit.frag", GL_FRAGMENT_SHADER);
	auto f_stipple = std::make_unique<Shader>("shaders/stippling.frag", GL_FRAGMENT_SHADER);


	//Billboarded particles shader program. 
	auto v_particles = std::make_unique<Shader>("shaders/particles.vert", GL_VERTEX_SHADER);
	auto g_particles = std::make_unique<Shader>("shaders/particles.geom", GL_GEOMETRY_SHADER);
	auto f_particles = std::make_unique<Shader>("shaders/particles.frag", GL_FRAGMENT_SHADER);

	try
	{
		std::vector<Shader*> particles = { v_particles.get(), g_particles.get(), f_particles.get() };
		prog_particles = std::make_unique<ShaderProgram>(particles);
	}
	catch (const std::exception& e) {
		std::cout << "PARTICLE SHADER ERROR: " << e.what() << std::endl;
	}
	

	try
	{
		std::vector<Shader*> morph = { v_morph.get(), f_stipple.get() };
		prog_morph = std::make_unique<ShaderProgram>(morph);
	}
	catch (const std::exception& e) {
		std::cout << "MORPH SHADER ERROR: " << e.what() << std::endl;
	}

	

	



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

void calculateStar() {

	if (mouseMovements.size() >= 5) {
		int backIndex = mouseMovements.size() - 1;
		int correctCount = 0;
		for (int i = 0; i < mouseMovements.size(); i++) {
			if (mouseMovements[i] == glm::vec2(1, -1)) {
				correctCount++;

			}
			if (mouseMovements[i] == glm::vec2(1, 1)) {
				correctCount++;
			}

			if (mouseMovements[i] == glm::vec2(-1, -1)) {
				correctCount++;
			}

			if (mouseMovements[i].x == 1) {
				correctCount++;
			}

			if (mouseMovements[i] == glm::vec2(-1, 1)) {
				correctCount++;
			}
		}
		if (correctCount >= 5) {
			std::cout << "MAID MODE ACTIVATED" << std::endl;
		}
	}
}

void getCursorData(GLFWwindow* window, double x, double y) {

	//globalCameraEntity->transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f));; 
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); 
	if (!isPaused && !isInMainMenu && !isInContinueMenu && !isInOptionsMenu) {
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


		/*
		int xMovement = 1;
		int yMovement = 1;
		if ((x - xPos) < 0) {
			xMovement = -1;
		}
		if ((y - yPos) < 0) {
			yMovement = -1;
		}
		glm::vec2 toAdd = glm::vec2(xMovement, yMovement);
		if (mouseMovements.size() >= 1) {
			std::cout << toAdd.x << " " << toAdd.y << std::endl;

			if (mouseMovements.back() != toAdd) {
				mouseMovements.push_back(toAdd);
			}
			while (mouseMovements.size() > 7) {
				mouseMovements.erase(mouseMovements.begin());
			}
		}
		else
		{
			mouseMovements.push_back(toAdd);
			std::cout << toAdd.x << " " << toAdd.y << std::endl;
		}

		calculateStar();
		*/
		globalCameraEntity->transform.m_rotation = cameraQuat;
		globalCameraEntity->transform.m_pos = cameraPos;

		glfwSetCursorPos(gameWindow, width / 2, height / 2);
		cameraX = deltaX;
		cameraY = deltaY;
	}
	else
	{
		xPos = x;
		yPos = y;
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

	if (Input::GetKeyDown(GLFW_KEY_ESCAPE))
	{
		isClickingEscape = true;
	}
	else
	{
		isClickingEscape = false;
	}
}
int getWantedSlot() {
	int wantedSlot = -1;
	if (Input::GetKeyDown(trayKeys[0])) {
		wantedSlot = 0;
	}
	if (Input::GetKeyDown(trayKeys[1])) {
		wantedSlot = 1;
	}
	if (Input::GetKeyDown(trayKeys[2])) {
		wantedSlot = 2;
	}
	if (Input::GetKeyDown(trayKeys[3])) {
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
//these will have to change when specific croissant textures are added 
Texture2D& createItemTex(Entity* e) {
	Texture2D* tex;
	if (e->Has<Pastry>()) {
		MaterialCreator* creator;
		bakeryUtils::pastryType type = e->Get<Pastry>().getPastryType();
		if (type == bakeryUtils::pastryType::CROISSANT) {
			return *crossantMat.getTexture().get();
		}
		if (type == bakeryUtils::pastryType::COOKIE) {
			return *cookieMat.getTexture().get();
		}
		if (type == bakeryUtils::pastryType::CUPCAKE) {
			return *cupcakeMat.getTexture().get();
		}
		if (type == bakeryUtils::pastryType::BURNT) {
			return *burntMat.getTexture().get();
		}
		if (type == bakeryUtils::pastryType::CAKE) {
			return *cakeMat.getTexture().get();
		}

	}
	if (e->Has<Drink>()) {
		MaterialCreator* creator;
		bakeryUtils::drinkType type = e->Get<Drink>().getDrinkType();
		if (type == bakeryUtils::drinkType::COFFEE) {
			return *coffeeMat.getTexture().get();
		}
		if (type == bakeryUtils::drinkType::TEA) {
			return *teaMat.getTexture().get();
		}
		if (type == bakeryUtils::drinkType::MILKSHAKE) {
			return *milkshakeMat.getTexture().get();
		}


	}
}
Mesh& createPastryMat(Entity* e) {
	Mesh* mesh;

	if (e->Has<Pastry>()) {
		MaterialCreator* creator;
		bakeryUtils::pastryType type = e->Get<Pastry>().getPastryType();
		if (type == bakeryUtils::pastryType::CROISSANT) {
			return *pastryMats[0][0]->getMesh().get();
		}
		if (type == bakeryUtils::pastryType::COOKIE) {
			return *pastryMats[1][0]->getMesh().get();
		}
		if (type == bakeryUtils::pastryType::CUPCAKE) {
			return *pastryMats[2][0]->getMesh().get();
		}
		if (type == bakeryUtils::pastryType::BURNT) {
			return *burntMat.getMesh().get();
		}
		if (type == bakeryUtils::pastryType::CAKE) {
			return *pastryMats[3][0]->getMesh().get();
		}

	}
	if (e->Has<Drink>()) {
		MaterialCreator* creator;
		bakeryUtils::drinkType type = e->Get<Drink>().getDrinkType();
		if (type == bakeryUtils::drinkType::COFFEE) {
			return *coffeeMat.getMesh().get();
		}
		if (type == bakeryUtils::drinkType::TEA) {
			return *teaMat.getMesh().get();
		}
		if (type == bakeryUtils::drinkType::MILKSHAKE) {
			return *milkshakeMat.getMesh().get();
		}


	}

}
void setTrayPastryMesh(Entity* e, bakeryUtils::pastryType type) {
	if (type == bakeryUtils::pastryType::CROISSANT) {
		e->Remove<CMeshRenderer>();
		e->Add<CMeshRenderer>(*e, *pastryMats[0][0]->getMesh().get(), *pastryMats[0][0]->getMaterial().get());
		//e->transform.m_scale = glm::vec3(0.02f, 0.02f, 0.02f); 
		//e->transform.m_scale = glm::vec3(0.02f, 0.02f, 0.02f); 
	}
	if (type == bakeryUtils::pastryType::COOKIE) {
		e->Remove<CMeshRenderer>();
		e->Add<CMeshRenderer>(*e, *pastryMats[1][0]->getMesh().get(), *pastryMats[1][0]->getMaterial().get());
		//e->transform.m_scale = glm::vec3(0.02f, 0.02f, 0.02f); 

	}
	if (type == bakeryUtils::pastryType::CUPCAKE) {
		e->Remove<CMeshRenderer>();
		e->Add<CMeshRenderer>(*e, *pastryMats[2][0]->getMesh().get(), *pastryMats[2][0]->getMaterial().get());

		//e->transform.m_scale = glm::vec3(0.015f, 0.015f, 0.015f); 
		//e->transform.m_scale = glm::vec3(0.02f, 0.02f, 0.02f); 


	}
	if (type == bakeryUtils::pastryType::BURNT) {
		e->Remove<CMeshRenderer>();
		e->Add<CMeshRenderer>(*e, *burntMat.getMesh(), *burntMat.getMaterial());
		//e->transform.m_scale = glm::vec3(0.015f, 0.015f, 0.015f); 
	}
	if (type == bakeryUtils::pastryType::CAKE) {
		e->Remove<CMeshRenderer>();
		e->Add<CMeshRenderer>(*e, *pastryMats[3][0]->getMesh().get(), *pastryMats[3][0]->getMaterial().get());



	}
	e->transform.m_scale = getTrayScale(type);
}

glm::vec3 getTrayScale(bakeryUtils::pastryType type) {
	if (type == bakeryUtils::pastryType::DOUGH) {
		return glm::vec3(0.009f, 0.009f, 0.009f);
	}
	if (type == bakeryUtils::pastryType::CUPCAKE) {
		//return glm::vec3(0.015f, 0.015f, 0.015f); 
	}
	return glm::vec3(0.02f, 0.02f, 0.02f);
}

void setDrinkMesh(Entity* e, bakeryUtils::drinkType type) {
	//std::cout << "A" << (int) type << std::endl; 
	if (type == bakeryUtils::drinkType::COFFEE) {
		if (e->Has<CMeshRenderer>()) {
			e->Remove<CMeshRenderer>();
		}

		e->Add<CMeshRenderer>(*e, *coffeeMat.getMesh(), *coffeeMat.getMaterial());
		e->transform.m_scale = glm::vec3(0.04f, 0.04f, 0.04f);
	}
	if (type == bakeryUtils::drinkType::TEA) {
		if (e->Has<CMeshRenderer>()) {
			e->Remove<CMeshRenderer>();
		}
		e->Add<CMeshRenderer>(*e, *teaMat.getMesh(), *teaMat.getMaterial());
		e->transform.m_scale = glm::vec3(0.04f, 0.04f, 0.04f);
	}
	if (type == bakeryUtils::drinkType::MILKSHAKE) {
		if (e->Has<CMeshRenderer>()) {
			e->Remove<CMeshRenderer>();
		}
		e->Add<CMeshRenderer>(*e, *milkshakeMat.getMesh(), *milkshakeMat.getMaterial());
		e->transform.m_scale = glm::vec3(0.04f, 0.04f, 0.04f);

	}
	
}

void setMachinePastryMesh(Entity* e, bakeryUtils::pastryType type) {
	e->transform.m_scale *= 10;
}
float getTrayRaise(bakeryUtils::pastryType type) {
	float raise = 0;
	if (type == bakeryUtils::pastryType::CAKE) {
		//raise = 0.013; 
	}
	return raise;
}

void setPastryFilling(Entity* e, bakeryUtils::fillType type)
{

	e->Remove<CMeshRenderer>();
	int toppingNum = (int)e->Get<Pastry>().getPastryTopping();
	int fillingNum = (int)type;
	int pastryNum = (int)e->Get<Pastry>().getPastryType() - 1;
	e->Add<CMeshRenderer>(*e, *pastryMats[pastryNum][toppingNum]->getMesh(), *pastryMats[pastryNum][fillingNum]->getMaterial());
	e->transform.m_scale = getTrayScale(e->Get<Pastry>().getPastryType());

	//e->transform.m_scale = glm::vec3(0.02f, 0.02f, 0.02f); 
}

void setPastryTopping(Entity* e, bakeryUtils::toppingType type)
{

	e->Remove<CMeshRenderer>();
	int toppingNum = (int)type;
	int fillingNum = (int)e->Get<Pastry>().getPastryFilling();
	int pastryNum = (int)e->Get<Pastry>().getPastryType() - 1;
	e->Add<CMeshRenderer>(*e, *pastryMats[pastryNum][toppingNum]->getMesh(), *pastryMats[pastryNum][fillingNum]->getMaterial());
	e->transform.m_scale = getTrayScale(e->Get<Pastry>().getPastryType());
	//e->transform.m_scale = glm::vec3(0.02f, 0.02f, 0.02f); 
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	scrollX = xoffset;
	scrollY = yoffset;
	if (yoffset < 0.f) {
		isScrollingDown = true;
	}
	else
	{
		isScrollingDown = false;
	}
	if (yoffset > 0.f) {
		isScrollingUp = true;
	}
	else
	{
		isScrollingUp = false;
	}
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

void removeFromRendering(Entity* e) {
	renderingEntities.erase(std::remove(renderingEntities.begin(), renderingEntities.end(), e), renderingEntities.end());

}

void removeFromUI(Entity* e) {
	UIEntities.erase(std::remove(UIEntities.begin(), UIEntities.end(), e), UIEntities.end());

}


bool isInUI(Entity* e){
	if (std::find(UIEntities.begin(), UIEntities.end(), e) != UIEntities.end()) {
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
	bool isSame = false;
	do {
		isSame = false;
		currentOrders[i] = Order();
		currentOrders[i].createOrder(bakeryUtils::getDifficulty());
		Pastry currentPastry = currentOrders[i].toPastry();
		for (int u = 0; u < currentOrders.size(); u++) {
			if (u != i) {
				Pastry otherPastry = currentOrders[u].toPastry();
				if (otherPastry.getPastryType() == currentPastry.getPastryType()) {
					if (otherPastry.getPastryFilling() == currentPastry.getPastryFilling()) {
						if (otherPastry.getPastryTopping() == currentPastry.getPastryTopping()) {
							isSame = true;
							continue;
						}
					}
				}
			}
		}
	} while (isSame);
	
	
	currentOrders[i].startOrder();


	if (remove) {
		orderBubblesToRemove.push_back(i);
	}


}

void resetBubble(int i, bool create) {
	//orderBubbles[i]->updateScale(UIScale); 
	orderBubbles[i]->clearRenderingEntities();
	orderBubbles[i]->setTransform(*orderBubbleTransform[i]);

	orderBubbleTimers[i]->setFill(0);
	orderBubbleTimers[i]->updateArrow();
	orderBubbles[i]->setupTimer(orderBubbleTimers[i]);
	orderBubbles[i]->setTiles(getPastryTile(currentOrders[i].type), getFillingTile(currentOrders[i].filling), getToppingTile(currentOrders[i].topping), getDrinkTile(currentOrders[i].drink));
	orderBubbles[i]->setup(&bubbleTile, &plusTile);

	if (create) {
		orderBubbles[i]->create(currentOrders[i]);
		orderBubbles[i]->updateScale(UIScale);
	}
	orderBubbles[i]->updateScale(UIScale);

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
		int subtractor = 1;
		if (max == 1) {
			subtractor = 0;
		}
		if (isClickingUp || isScrollingUp) {
			currentConfusion = 0;
			selectedOption += (max - subtractor);
			selectedOption = selectedOption % max;

			
			playSound(&tick, "Tick");
		}
		if (isClickingDown || isScrollingDown) {
			currentConfusion = 0;
			selectedOption++;
			selectedOption = selectedOption % max;
			playSound(&tick, "Tick");
		}
	}




	if (isClickingSpace || isClickingEnter) {

		
		playSound(&click, "TickTick");
		currentConfusion = 0;
		return selectedOption;


	}
	return -1;
}

void loadAnimationData(std::vector<Mesh*>& toModify, std::string prefix, int count) {
	for (int i = 1; i <= count; i++) {
		std::string filename = prefix + std::to_string(i) + ".gltf";

		Mesh* frame = new Mesh();
		GLTF::LoadMesh(filename, *frame);

		toModify.push_back(frame);
	}

}

void loadMaterialCreatorData(std::vector<MaterialCreator*>& toModify, std::string meshName, std::string prefix, int count) {
	for (int i = 1; i <= count; i++) {
		std::string filename = prefix + std::to_string(i) + ".png";

		MaterialCreator* frame = new MaterialCreator();
		frame->createMaterial(meshName, filename, *prog_texLit);

		toModify.push_back(frame);
	}

}

void loadMaterialCreatorData(std::vector<MaterialCreator*>& toModify, Mesh& meshName, std::string prefix, int count) {
	for (int i = 1; i <= count; i++) {
		std::string filename = prefix + std::to_string(i) + ".png";

		MaterialCreator* frame = new MaterialCreator();
		frame->createMaterial(meshName, filename, *prog_UI);

		toModify.push_back(frame);
	}

}

int placeInLineToIndex(int linePlace) {
	if (linePlace >= 1 && linePlace <= 3) {
		//std::cout << "AMT " << (3 + (3 - linePlace)) << std::endl; 
		return (lineStart + (3 - linePlace));
	}
	else
	{
		if (linePlace < 1) {
			return line.size();

		}
		if (linePlace > 3) {
			return 0;
		}
	}
}

int indexToPlaceInLine(int index) {
	return 3 - (index - lineStart);
}

void setScores(int totalOrders, int highscore) {
	if (totalOrders > 999) {
		totalOrders = 999;
	}
	else if (totalOrders < 0) {
		totalOrders = 0;
	}
	if (highscore > 999) {
		highscore = 999;
	}
	else if (highscore < 0) {
		highscore = 0;
	}
	std::string top = std::to_string(totalOrders);
	std::string bottom = std::to_string(highscore);
	char topArray[3];
	char bottomArray[3];
	int index = 0;

	for (int i = 0; i < 3 - top.length(); i++) {
		topArray[i] = '0';
		index++;
	}
	for each (char c in top) {
		//std::cout << c << std::endl; 
		topArray[index] = c;
		index++;
	}
	index = 0;
	for (int i = 0; i < 3 - bottom.length(); i++) {
		bottomArray[i] = '0';
		index++;
	}
	for each (char c in bottom) {
		bottomArray[index] = c;
		index++;
	}
	index = 0;
	for each (char c in topArray) {
		int numberIndex = c - '0';

		numberEntities[index]->Get<CMeshRenderer>().SetMaterial(*numberTiles[numberIndex].getMaterial());
		index++;
	}
	index = 0;
	for each (char c in bottomArray) {
		int numberIndex = c - '0';
		numberEntities[index + 3]->Get<CMeshRenderer>().SetMaterial(*numberTiles[numberIndex].getMaterial());
		index++;
	}
}

int getHighscore() {
	int lastHs = 0;

	std::fstream scoreKeeper("highscore.txt");
	std::string lastScore;
	if (scoreKeeper.is_open()) {
		if (scoreKeeper.good()) {
			try
			{
				std::getline(scoreKeeper, lastScore);
				lastHs = std::stoi(lastScore);
				scoreKeeper.close();
			}
			catch (const std::exception& e) {
				scoreKeeper << "0";
				scoreKeeper.close();
				lastHs = 0;
				std::cout << "RESET - " << e.what() << std::endl;
				system("Color 4");
			}

		}

	}
	scoreKeeper.close();
	return lastHs;
}

int saveHighscore(int hs)
{
	int lastHs = getHighscore();
	std::fstream scoreKeeper("highscore.txt");
	if (lastHs < hs) {
		scoreKeeper.close();
		scoreKeeper.open("highscore.txt", std::ofstream::out | std::ofstream::trunc);
		scoreKeeper.close();
		scoreKeeper.open("highscore.txt");
		if (scoreKeeper.is_open()) {
			if (scoreKeeper.good()) {
				scoreKeeper << std::to_string(hs);
				scoreKeeper.close();
			}

		}

		lastHs = hs;
		
	}

	scoreKeeper.close();
	return lastHs;
}

void restartGame() {
	bakeryUtils::setDifficulty(1);
	bakeryUtils::setTime(0);
	bakeryUtils::setOrdersFailed(0);
	bakeryUtils::setRoundsLasted(0);
	tablet->Remove<CMeshRenderer>();
	tablet->Add<CMeshRenderer>(*tablet, *tabletMats[0]->getMesh(), *tabletMats[0]->getMaterial());
	trayT = 0;
	trayMultiplier = 0;
	tutorialPos = 0;
	if (shouldShowTutorial) {
		tutorialMultiplier = 1;
	}
	else
	{
		tutorialMultiplier = 0;
	}
	
	//tutorialPlane->transform.m_scale = glm::vec3(0.07 * (UIScale + 0.05));
	tutorialPlane->transform.m_scale = glm::vec3((0.003 * (UIScale + 0.05))*tutorialMultiplier);

	
	for (int i = 0; i < tutorialSteps.size(); i++) {
		tutorialSteps[i].setContinueState(false);
	}
	
	UpdateTutorial();

	ent_register->Get<CMeshRenderer>().SetMaterial(*registerImages[0]->getMaterial());
	for (int i = 0; i < currentOrders.size(); i++) {
		OrderBubble* ob = orderBubbles[i];

		ob->getTimer().setFill(0);
		ob->getTimer().updateArrow();
		ob->getOrder()->setOver(false);

		ob->getOrder()->setStarted(false);

		for each (Entity * ent in ob->returnRenderingEntities()) {
			if (isInRendering(ent)) {
				removeFromRendering(ent);
			}

			//addToRendering(ent); 
		}
		resetBubble(i, false);
		ob->updateScale(UIScale);
		//ob->clearRenderingEntities(); 

	}

	for (int i = 0; i < orderBubbles.size(); i++) {
		orderBubblesToRemove.erase(std::remove(orderBubblesToRemove.begin(), orderBubblesToRemove.end(), i), orderBubblesToRemove.end());
	}

	for each (Entity * ent in orderBubbles[0]->returnRenderingEntities()) {
		//removeFromRendering(ent); 
		if (isInRendering(ent)) {
			removeFromRendering(ent);
		}
		//addToRendering(ent); 
	}

	//currentOrders.push_back(Order()); 
	currentOrders.back().createOrder(bakeryUtils::getDifficulty());

	orderBubbles[0]->setTiles(getPastryTile(currentOrders.back().type), getFillingTile(currentOrders.back().filling), getToppingTile(currentOrders.back().topping), getDrinkTile(currentOrders.back().drink));
	orderBubbles[0]->setup(&bubbleTile, &plusTile);
	orderBubbles[0]->create(currentOrders.back());
	orderBubbles[0]->updateScale(UIScale);
	//orderBubbles[1]->updateScale(UIScale); 
	//orderBubbles[2]->updateScale(UIScale); 

	for each (Entity * ent in orderBubbles[0]->returnRenderingEntities()) {
		if (isInRendering(ent)) {
			removeFromRendering(ent);
		}

		//addToRendering(ent); 
	}
	for each (Entity * ent in orderBubbles[1]->returnRenderingEntities()) {
		if (isInRendering(ent)) {
			removeFromRendering(ent);
		}

		//addToRendering(ent); 
	}
	if (currentOrders.size() > 1) {
		if (currentOrders.size() > 2) {
			//orderBubbles[2]->getOrder()->setStarted(false); 
			currentOrders.erase(currentOrders.begin() + 1);
		}
		//orderBubbles[1]->getOrder()->setStarted(false); 

		currentOrders.erase(currentOrders.begin() + 1);

	}

	for each (Entity * ent in orderBubbles[2]->returnRenderingEntities()) {
		if (isInRendering(ent)) {
			removeFromRendering(ent);
		}
		//addToRendering(ent); 
	}

	for (int i = 0; i < 6; i++)
	{
		if (isInRendering(numberEntities[i])) {
			removeFromRendering(numberEntities[i]);
		}
	}
	for (int i = 0; i < std::size(trayPastry); i++)
	{
		if (isInRendering(trayPastry[i])) {
			removeFromRendering(trayPastry[i]);
		}
		trayPastry[i] = nullptr;

	}
	
	//tutorialPlane->transform.m_pos = glm::vec3(-20); 
	lastCameraX = 0;
	lastCameraY = 0;
	//standardCameraQuat = getCameraRotation();  
	wantedCameraQuat = standardCameraQuat;
	cameraX = 90.f;
	cameraY = 0.f;
	xPos = 0;
	yPos = 0;
	//menuCameraQuat = getCameraRotation();  
	currentCameraQuat = menuCameraQuat;
	lastCameraQuat = standardCameraQuat;
}

int selectedOvenPosition(float x) {
	if (x >= ovenHeights[3]) {
		return 3;
	}
	if (x >= ovenHeights[2] && x < ovenHeights[3]) {
		return 2;
	}
	if (x >= ovenHeights[1] && x < ovenHeights[2]) {
		return 1;
	}
	if (x >= ovenHeights[0] && x < ovenHeights[1]) {
		return 0;
	}
	if (x < ovenHeights[0]) {
		return 0;
	}
}

void setTutorialChange(int i)
{
	if (i >= 0 && i < tutorialSteps.size()) {
		for (int u = 0; u <= i; u++) {
			tutorialSteps[u].setContinueState(true);
		}
		
	}
}

void UpdateTutorial()
{
	int amountSkipped = 0;
	for (int i = 0; i < tutorialSteps.size(); i++) {
		if (tutorialSteps[i].getContinueState()) {
			amountSkipped++;
		}
		else
		{
			if (tutorialSteps[i].getMaterialCreator()->getMaterial() != nothingTile.getMaterial()) {
				//tutorialPlane->transform.m_scale = glm::vec3(0.07 * (UIScale + 0.05));
				//tutorialPlane->transform.m_scale = glm::vec3(0.003 * (UIScale + 0.05));
				tutorialMultiplier = 1;
				tutorialPlane->Remove<CMeshRenderer>();
				tutorialPlane->Add<CMeshRenderer>(*tutorialPlane.get(), *tutorialSteps[i].getMaterialCreator()->getMesh(), *tutorialSteps[i].getMaterialCreator()->getMaterial());
				//std::cout << "showing: " << i << std::endl;
				break;
			}
			else
			{
				//tutorialPlane->Remove<CMeshRenderer>();
				//tutorialPlane->Add<CMeshRenderer>(*tutorialPlane.get(), *tutorialSteps[i].getMaterialCreator()->getMesh(), *tutorialSteps[i].getMaterialCreator()->getMaterial());
				//std::cout << "showing: " << i << std::endl;
				//tutorialPlane->transform.m_scale = glm::vec3(0);
				tutorialMultiplier = 0;

				break;
			}
			
			
			
			
		}
	}
	if (amountSkipped == tutorialSteps.size()) {
		//tutorialPlane->transform.m_pos.y = 5;
		//std::cout << "STOP" << std::endl;
		//removeFromRendering(tutorialPlane.get());
		tutorialPlane->transform.m_scale = glm::vec3(0);
		tutorialMultiplier = 0;
		shouldShowTutorial = false;
	}

	if (!shouldShowTutorial) {
		tutorialPlane->transform.m_scale = glm::vec3(0);
		tutorialMultiplier = 0;
	}


}

void nextStepTutorialIfNeeded() {
	for (int i = 0; i < tutorialSteps.size(); i++) {
		if (!tutorialSteps[i].getContinueState()) {
			if (tutorialSteps[i].spaceContinues()) {
				tutorialSteps[i].setContinueState(true);
				//std::cout << "stopped at: " << i << std::endl;
				
			}
			break;
		}
		
	}
	

}

void loadNumberHashMap() {
	//https://www.glfw.org/docs/3.3/group__keys.html
	for (int i = 0; i < 10; i++) {
		alphanumeric.insert(std::pair<GLuint, int>(48 + i, i));
	}
	for (int i = 10; i < 36; i++) {
		alphanumeric.insert(std::pair<GLuint, int>(65 + (i - 10), i));
	}
	

}



GLuint pictureIndexToGLuint(int j) {
	for (auto& it : alphanumeric) {
		if (it.second == j) {
			return it.first;
		}
	}
	return 0;
}

int GLuintToPictureIndex(GLuint j) {
	for (auto& it : alphanumeric) {
		if (it.first == j) {
			return it.second;
		}
	}
	return 0;
}

int getWhichKeyPressed() {
	/*for(int i = 0; i < 10; i++){
	   std::cout << "if(Input::GetKeyDown(GLFW_KEY_" << std::to_string(i) << ")){" << std::endl;
	   std::cout << "return " << std::to_string(i) << ";" << std::endl;
	   std::cout << "}" << std::endl;
   }
   int start = 10;
   std::string letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
   for (char& c : letters) {
		 std::cout << "if(Input::GetKeyDown(GLFW_KEY_" << std::string(1, c) << ")){" << std::endl;
	   std::cout << "return " << std::to_string(start) << ";" << std::endl;
	   std::cout << "}" << std::endl;
	start++;
	}
	*/
	if (isClickingSpace) {
		return -2;
	}
	if (Input::GetKeyDown(GLFW_KEY_0)) {
		return 0;
	}
	if (Input::GetKeyDown(GLFW_KEY_1)) {
		return 1;
	}
	if (Input::GetKeyDown(GLFW_KEY_2)) {
		return 2;
	}
	if (Input::GetKeyDown(GLFW_KEY_3)) {
		return 3;
	}
	if (Input::GetKeyDown(GLFW_KEY_4)) {
		return 4;
	}
	if (Input::GetKeyDown(GLFW_KEY_5)) {
		return 5;
	}
	if (Input::GetKeyDown(GLFW_KEY_6)) {
		return 6;
	}
	if (Input::GetKeyDown(GLFW_KEY_7)) {
		return 7;
	}
	if (Input::GetKeyDown(GLFW_KEY_8)) {
		return 8;
	}
	if (Input::GetKeyDown(GLFW_KEY_9)) {
		return 9;
	}
	if (Input::GetKeyDown(GLFW_KEY_A)) {
		return 10;
	}
	if (Input::GetKeyDown(GLFW_KEY_B)) {
		return 11;
	}
	if (Input::GetKeyDown(GLFW_KEY_C)) {
		return 12;
	}
	if (Input::GetKeyDown(GLFW_KEY_D)) {
		return 13;
	}
	if (Input::GetKeyDown(GLFW_KEY_E)) {
		return 14;
	}
	if (Input::GetKeyDown(GLFW_KEY_F)) {
		return 15;
	}
	if (Input::GetKeyDown(GLFW_KEY_G)) {
		return 16;
	}
	if (Input::GetKeyDown(GLFW_KEY_H)) {
		return 17;
	}
	if (Input::GetKeyDown(GLFW_KEY_I)) {
		return 18;
	}
	if (Input::GetKeyDown(GLFW_KEY_J)) {
		return 19;
	}
	if (Input::GetKeyDown(GLFW_KEY_K)) {
		return 20;
	}
	if (Input::GetKeyDown(GLFW_KEY_L)) {
		return 21;
	}
	if (Input::GetKeyDown(GLFW_KEY_M)) {
		return 22;
	}
	if (Input::GetKeyDown(GLFW_KEY_N)) {
		return 23;
	}
	if (Input::GetKeyDown(GLFW_KEY_O)) {
		return 24;
	}
	if (Input::GetKeyDown(GLFW_KEY_P)) {
		return 25;
	}
	if (Input::GetKeyDown(GLFW_KEY_Q)) {
		return 26;
	}
	if (Input::GetKeyDown(GLFW_KEY_R)) {
		return 27;
	}
	if (Input::GetKeyDown(GLFW_KEY_S)) {
		return 28;
	}
	if (Input::GetKeyDown(GLFW_KEY_T)) {
		return 29;
	}
	if (Input::GetKeyDown(GLFW_KEY_U)) {
		return 30;
	}
	if (Input::GetKeyDown(GLFW_KEY_V)) {
		return 31;
	}
	if (Input::GetKeyDown(GLFW_KEY_W)) {
		return 32;
	}
	if (Input::GetKeyDown(GLFW_KEY_X)) {
		return 33;
	}
	if (Input::GetKeyDown(GLFW_KEY_Y)) {
		return 34;
	}
	if (Input::GetKeyDown(GLFW_KEY_Z)) {
		return 35;
	}
	return -1;
}

void saveSettings() {
	std::fstream scoreKeeper("settings.txt");
	std::string lastScore;
	int index = 0;
	scoreKeeper.close();
	scoreKeeper.open("settings.txt", std::ofstream::out | std::ofstream::trunc);
	scoreKeeper.close();
	scoreKeeper.open("settings.txt");
	if (scoreKeeper.is_open()) {
		for (int i = 0; i < 9; i++) {
			scoreKeeper << std::to_string(accessSettings[index]) << std::endl;
			index++;
		}
		scoreKeeper.close();
	}
	else
	{
		for (int i = 0; i < 9; i++) {
			scoreKeeper << std::to_string(accessSettings[index]) << std::endl;
			index++;
		}
		scoreKeeper.close();
	}

}

void loadSettings() {
	std::ifstream MyReadFile("settings.txt");
	std::string readLine;
	int index = 0;

	if (MyReadFile.is_open()) {
		if (MyReadFile.good()) {
			while (std::getline(MyReadFile, readLine)) {


				accessSettings[index] = std::stoi(readLine);
				index++;
			}
		}

		MyReadFile.close();
	}

	for (int i = 4; i < 7; i++) {
		if (accessSettings[i] > 4 || accessSettings[i] < 0) {
			accessSettings[i] = 2;
		}

	}
	if (accessSettings[7] > 1 || accessSettings[7] < 0) {
		accessSettings[7] = 0;
	}
	if (accessSettings[8] > 3 || accessSettings[8] < 0) {
		accessSettings[8] = 1;
	}

}

void applySettings() {
	for (int i = 0; i < 4; i++) {
		trayKeys[i] = pictureIndexToGLuint(accessSettings[i]);
	}
	if (accessSettings[4] == 2) {
		sensitivity = -0.1;
	}
	if (accessSettings[4] == 1) {
		sensitivity = -0.075;
	}
	if (accessSettings[4] == 0) {
		sensitivity = -0.05;
	}
	if (accessSettings[4] == 3) {
		sensitivity = -0.125;
	}
	if (accessSettings[4] == 4) {
		sensitivity = -0.15;
	}
	//int soundVol = accessSettings[5];
	//int musicVol = accessSettings[6];
	musicVolume = accessSettings[5] * 0.25;
	soundVolume = accessSettings[6] * 0.25;
	bakeryUtils::setMusicVolume(musicVolume);
	bakeryUtils::setSoundVolume(soundVolume);

	if (accessSettings[7] == 1) {
		UIScale = 1.35;

	}
	else
	{
		UIScale = 0.95;
	}

	applyResolution();

}

void applyResolution() {
	if (accessSettings[8] == 3) {
		//glm::vec2 res = resoloutions[accessSettings[8]];
		glm::vec2 newSize = App::setFullscreen();
		globalCameraEntity->Get<CCamera>().Perspective(60.0f, (float)newSize.x / newSize.y, 0.001f, 100.0f);
		globalCameraEntity->transform.RecomputeGlobal();
		globalCameraEntity->Get<CCamera>().Update();
		//glDeleteTextures(1, &textureColorbuffer);
		//glGenTextures(1, &textureColorbuffer);
		if (shouldUseGraphics) {

		
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, newSize.x, newSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, res.x, res.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		// attach it to currently bound framebuffer object
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, newSize.x, newSize.y);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, newSize.x, newSize.y);
		}
		screenRes = newSize;
	}
	else
	{
		glm::vec2 res = resoloutions[accessSettings[8]];
		glm::vec2 newSize = App::setWindowed(res.x, res.y);
		globalCameraEntity->Get<CCamera>().Perspective(60.0f, (float)newSize.x / newSize.y, 0.001f, 100.0f);
		globalCameraEntity->transform.RecomputeGlobal();
		globalCameraEntity->Get<CCamera>().Update();
		//glDeleteTextures(1, &textureColorbuffer);
		//glGenTextures(1, &textureColorbuffer);
		if (shouldUseGraphics) {

		
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, newSize.x, newSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, res.x, res.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		// attach it to currently bound framebuffer object
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, newSize.x, newSize.y);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, res.x, res.y);
		}
		screenRes = res;

	}
}

void playSound(ToneFire::StudioSound* sound, std::string name) {
	
	sound->PlayEvent("event:/" + name);
	sound->SetEventParameter("event:/" + name, "parameter:/soundVolume", soundVolume);
	if (soundVolume < 0.05) {
		sound->StopEvent("event:/" + name);
	}
}

void playMusic(ToneFire::StudioSound* music, std::string name) {
	music->PlayEvent("event:/" + name);
	music->SetEventParameter("event:/" + name, "parameter:/musicVolume", musicVolume);
	if (musicVolume < 0.05) {
		music->StopEvent("event:/" + name);
	}
}

void addToRendering(Entity* e) {
	renderingEntities.push_back(e);
	
	
}

void chooseAndPlaySong() {
	int random = rand() % (songList.size()-1);
	bool isAnySongPlaying = false;
	for each (Music* m in songList) {
		if (m->isCurrentlyPlaying()) {
			isAnySongPlaying = false;
			break;
		}
	}
	if (!isAnySongPlaying) {
		songList[random]->fadeIn(0.5);
	}
}
