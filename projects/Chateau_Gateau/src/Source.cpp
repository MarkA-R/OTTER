

#include "NOU/App.h"
#include "NOU/Input.h"
#include "NOU/Entity.h"
#include "NOU/CCamera.h"
#include "NOU/CMeshRenderer.h"
#include "NOU/Shader.h"
#include "NOU/GLTFLoader.h"
#include "MaterialCreator.h"
#include "Raycast.h"
#include "BoundingBox.h"
#include "Machine.h"
#include "Fridge.h"
#include "Oven.h"
#include "bakeryUtils.h"

#include <iostream>

#include "imgui.h"

#include <memory>

using namespace nou;

std::unique_ptr<ShaderProgram> prog_texLit, prog_lit, prog_unlit;
std::unique_ptr<Material>  mat_unselected, mat_selected, mat_line;
glm::vec3 cameraPos = glm::vec3(-1.f, -0.5f, -0.7f);
glm::quat cameraQuat = glm::quat();

double xPos, yPos;
double cameraX = 0, cameraY = 0;
double sensitivity = -0.1;

Entity* globalCameraEntity;
GLFWwindow* gameWindow;
bool isClicking = false;
//Mouse State
bool firstMouse = true;
int width = 1280;
int height = 720;
float lastX = width / 2;
float lastY = height / 2;
std::vector<Entity*> renderingEntities = std::vector<Entity*>();
Transform traySlot[4] = {};
std::unique_ptr<Entity> trayPastry[4] = {nullptr, nullptr, nullptr, nullptr};



// Keep our main cleaner
void LoadDefaultResources();
void getCursorData(GLFWwindow* window, double x, double y);
void outputCameraFacing(glm::vec3 cf);
void putDough(Entity& e, int slot);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
int getFirstTraySlot();
bool isSlotEmpty(int i);

// Function to handle user inputs
void GetInput();

// Templated LERP function
template<typename T>
T Lerp(const T& p0, const T& p1, float t)
{
	return (1.0f - t) * p0 + t * p1;
}

int main()
{
	
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

	MaterialCreator registerMaterial = MaterialCreator();
	registerMaterial.createMaterial("bakery/models/cashregister.gltf", "bakery/textures/cashregister.png", *prog_texLit);
	
	MaterialCreator counterMat = MaterialCreator();
	counterMat.createMaterial("bakery/models/counter.gltf", "bakery/textures/counter.png", *prog_texLit);

	MaterialCreator trayMat = MaterialCreator();
	trayMat.createMaterial("bakery/models/tray.gltf", "bakery/textures/wood.png", *prog_texLit);

	MaterialCreator fridgeMat = MaterialCreator();
	fridgeMat.createMaterial("bakery/models/fridge.gltf", "bakery/textures/fridge.png", *prog_texLit);

	MaterialCreator crossantMat = MaterialCreator();
	crossantMat.createMaterial("bakery/models/croissant.gltf", "bakery/textures/croissant.png", *prog_texLit);

	MaterialCreator doughMat = MaterialCreator();
	doughMat.createMaterial("bakery/models/dough.gltf", "bakery/textures/dough.png", *prog_texLit);

	MaterialCreator ovenMat = MaterialCreator();
	ovenMat.createMaterial("bakery/models/oven.gltf", "bakery/textures/oven.png", *prog_texLit);

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
	ent_register.Add<BoundingBox>(glm::vec3(0.5,2.3,0.06) , ent_register);

	
	ent_register.transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
	ent_register.transform.m_pos = glm::vec3(-1.f, -3.f, -3.0f);
	renderingEntities.push_back(&ent_register);

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

	Entity fridge = Entity::Create();
	fridge.Add<CMeshRenderer>(fridge, *fridgeMat.getMesh(), *fridgeMat.getMaterial());
	fridge.Add<BoundingBox>(glm::vec3(0.67, 4, 0.5), fridge);
	fridge.Add<Machine>();
	fridge.transform.m_scale = glm::vec3(0.5f, 1.f, 0.5f);
	fridge.transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f));
	fridge.transform.m_pos = glm::vec3(-3.f, -1.f, 2.0f);
	renderingEntities.push_back(&fridge);

	Entity oven = Entity::Create();
	oven.Add<CMeshRenderer>(oven, *ovenMat.getMesh(), *ovenMat.getMaterial());
	oven.Add<BoundingBox>(glm::vec3(0.67, 2, 0.5), oven);
	oven.Add<Machine>();
	oven.transform.m_scale = glm::vec3(0.4f, 0.4f, 0.4f);
	oven.transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
	oven.transform.m_pos = glm::vec3(1.f, -1.5f, 2.0f);
	renderingEntities.push_back(&oven);


	App::Tick();

	float angle = 0;
	// Main loop
	while (!App::IsClosing() && !Input::GetKeyDown(GLFW_KEY_ESCAPE))
	{
		bool keepCheckingRaycast = true;
		 isClicking = false;
		 int addedSlot = -1;
		 glfwSetCursorPosCallback(gameWindow, getCursorData);
		App::FrameStart();
		
		
		GetInput();

		// Update our LERP timers
		float deltaTime = App::GetDeltaTime();


		//cameraEntity.transform.m_rotation = glm::angleAxis(glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
		//angle++;
		//std::cout << angle << std::endl;
		// Update camera
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
		
		//outputCameraFacing(cameraFacingVector);
		Raycast r = Raycast(cameraEntity.transform.m_pos, cameraFacingVector,10.f);//needs to be after GetInput so cameraQuat is initialized;
		std::vector<glm::vec3> raycastPoints = r.crossedPoints();
		//std::cout << cameraRotEuler.x << " " << cameraRotEuler.y << " " << cameraRotEuler.z << std::endl;
		for each (Entity* e in renderingEntities) {
			
			e->transform.RecomputeGlobal();


			
			e->Get<CMeshRenderer>().Draw();
			if (keepCheckingRaycast) {
				if (e->Has<BoundingBox>()) {

					for each (glm::vec3	pos in raycastPoints) {

						if (e->Get<BoundingBox>().isColliding(pos)) {
							if (e->Has<Machine>()) {//check for fridge tomorrow
								if (e->Has<Fridge>()) {
									//std::cout << "A" << std::endl;
									if (isClicking) {
										int slot = getFirstTraySlot();
										if (slot >= 0) {
											trayPastry[slot] = Entity::Allocate();



											trayPastry[slot]->Add<CMeshRenderer>(*trayPastry[slot], *doughMat.getMesh(), *doughMat.getMaterial());
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
								
								
									
								
								
							}
							keepCheckingRaycast = false;
							break;
						}

					}
				}
			}
			
			
		}
		

	
	
		if (addedSlot >= 0) {
			renderingEntities.push_back(trayPastry[addedSlot].get());
		}


		
	

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



	// Set up point and line materials
	mat_unselected = std::make_unique<Material>(*prog_lit);
	mat_unselected->m_color = glm::vec3(0.5f, 0.5f, 0.5f);

	mat_selected = std::make_unique<Material>(*prog_lit);
	mat_selected->m_color = glm::vec3(1.0f, 0.0f, 0.0f);

	mat_line = std::make_unique<Material>(*prog_unlit);
	mat_line->m_color = glm::vec3(1.0f, 1.0f, 1.0f);




}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		isClicking = true;
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

void putDough(Entity& croissant, int slot) {
	
	
		
		
		
	
}





