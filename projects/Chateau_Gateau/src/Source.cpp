

#include "NOU/App.h"
#include "NOU/Input.h"
#include "NOU/Entity.h"
#include "NOU/CCamera.h"
#include "NOU/CMeshRenderer.h"
#include "NOU/Shader.h"
#include "NOU/GLTFLoader.h"
#include "MaterialCreator.h"

#include <iostream>

#include "imgui.h"

#include <memory>

using namespace nou;

std::unique_ptr<ShaderProgram> prog_texLit, prog_lit, prog_unlit;
std::unique_ptr<Material>  mat_unselected, mat_selected, mat_line;
glm::vec3 cameraPos = glm::vec3(-1.f, -0.5f, -0.7f);


double xPos, yPos;
double cameraX = 0, cameraY = 0;
double sensitivity = -0.1;

Entity* globalCameraEntity;
GLFWwindow* gameWindow;
//Mouse State
bool firstMouse = true;
int width = 1280;
int height = 720;
float lastX = width / 2;
float lastY = height / 2;
std::vector<Entity*> renderingEntities = std::vector<Entity*>();



// Keep our main cleaner
void LoadDefaultResources();
void getCursorData(GLFWwindow* window, double x, double y);
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
	gameWindow = glfwGetCurrentContext();
	// Initialize ImGui
	App::InitImgui();

	// Load in our model/texture resources
	LoadDefaultResources();
	MaterialCreator registerMaterial = MaterialCreator();
	registerMaterial.createMaterial("bakery/models/cashregister.gltf", "bakery/textures/cashregister.png", *prog_texLit);
	
	MaterialCreator counterMat = MaterialCreator();
	counterMat.createMaterial("bakery/models/counter.gltf", "bakery/textures/counter.png", *prog_texLit);


	// Create and set up camera
	Entity cameraEntity = Entity::Create();
	CCamera& cam = cameraEntity.Add<CCamera>(cameraEntity);
	cam.Perspective(60.0f, (float) width/height, 0.1f, 100.0f);
	cameraEntity.transform.m_pos = cameraPos;
	cameraEntity.transform.m_rotation = glm::angleAxis(glm::radians(0.f), glm::vec3(0.0f, 1.0f, 0.0f));
	globalCameraEntity = &cameraEntity;
	

	

	//Creating Cash Register Entity
	Entity ent_register = Entity::Create();
	ent_register.Add<CMeshRenderer>(ent_register, *registerMaterial.getMesh(), *registerMaterial.getMaterial());
	ent_register.transform.m_scale = glm::vec3(0.5f, 0.5f, 0.5f);
	ent_register.transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
	ent_register.transform.m_pos = glm::vec3(-1.f, -3.f, -3.0f);
	renderingEntities.push_back(&ent_register);

	Entity counter = Entity::Create();
	counter.Add<CMeshRenderer>(counter, *counterMat.getMesh(), *counterMat.getMaterial());
	counter.transform.m_scale = glm::vec3(1.f, 0.5f, 0.5f);
	counter.transform.m_rotation = glm::angleAxis(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
	counter.transform.m_pos = glm::vec3(-1.f, -3.f, -3.0f);
	renderingEntities.push_back(&counter);



	App::Tick();

	float angle = 0;
	// Main loop
	while (!App::IsClosing() && !Input::GetKeyDown(GLFW_KEY_ESCAPE))
	{

		App::FrameStart();
		App::StartImgui();
		static bool go = true;
		Transform* t = &counter.transform;
		/*
		float rx, ry, rz;
		glm::vec3 rotEuler = glm::eulerAngles(t->m_rotation);
		rx = glm::degrees(rotEuler.x);
		ry = glm::degrees(rotEuler.y);
		rz = glm::degrees(rotEuler.z);
		*/
		ImGui::Begin("Point Coordinates", &go, ImVec2(300, 300));

		//This will tie the position of the selected 
		//waypoint to input fields rendered with Imgui. 
		ImGui::DragFloat("X", &(t->m_pos.x), 0.1f);
		ImGui::DragFloat("Y", &(t->m_pos.y), 0.1f);
		ImGui::DragFloat("Z", &(t->m_pos.z), 0.1f);

		//ImGui::DragFloat("RX", &(rx), 0.1f);
		//ImGui::DragFloat("RY", &(ry), 0.1f);
		//ImGui::DragFloat("RZ", &(rz), 0.1f);

		

		ImGui::End();
		
		//Get User Inputs
		GetInput();

		// Update our LERP timers
		float deltaTime = App::GetDeltaTime();


		//cameraEntity.transform.m_rotation = glm::angleAxis(glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
		//angle++;
		//std::cout << angle << std::endl;
		// Update camera
		cameraEntity.Get<CCamera>().Update();



		for each (Entity* e in renderingEntities) {
			
			e->transform.RecomputeGlobal();


			// Draw register
			e->Get<CMeshRenderer>().Draw();
		}
		

	
	
		


		
		App::EndImgui();

		// Draw everything we queued up to the screen
		App::SwapBuffers();
	}

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

void getCursorData(GLFWwindow* window, double x, double y) {

	//globalCameraEntity->transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f));;
	
	double deltaX = (x - xPos) * sensitivity;
	double deltaY = (y - yPos) * sensitivity;
	deltaX += cameraX;
	deltaY += cameraY;
	//std::cout << deltaX << " " << deltaY << std::endl;
	//glm::quat yRot = glm::angleAxis(glm::radians((float)deltaX), glm::vec3(0.0f, 1.0f, 0.0f));
	//glm::quat xRot = glm::angleAxis(glm::radians((float)deltaY), glm::vec3(1.0f, 0.0f, 0.0f));

	glm::quat QuatAroundX = glm::angleAxis(glm::radians((float)deltaY), glm::vec3(1.0, 0.0, 0.0));
	glm::quat QuatAroundY = glm::angleAxis(glm::radians((float)deltaX), glm::vec3(0.0, 1.0, 0.0));
	glm::quat QuatAroundZ = glm::angleAxis(glm::radians(0.f), glm::vec3(0.0, 0.0, 1.0));
	glm::quat finalOrientation = QuatAroundX * QuatAroundY * QuatAroundZ;
	//t->m_rotation = finalOrientation;
	globalCameraEntity->transform.m_rotation = finalOrientation;
	//globalCameraEntity->transform.m_rotation = glm::angleAxis(glm::radians((float)deltaY), glm::vec3(0.0f, 1.0f, 0.0f));

	globalCameraEntity->transform.m_pos = cameraPos;


	glfwSetCursorPos(gameWindow, width/2, height/2);
	cameraX = deltaX;
	cameraY = deltaY;

}

void GetInput()
{
	
	double xRot, yRot;

	
	glfwGetCursorPos(gameWindow, &xPos, &yPos);
	glfwSetCursorPosCallback(gameWindow, getCursorData);
	//glfwGetCursorPos(gameWindow, &xPos, &yPos);
	
	//std::cout << xPos << " " << yPos << std::endl;

	

	//std::cout << globalCameraEntity->transform.m_rotation.x << " , " << globalCameraEntity->transform.m_rotation.y << " , " << globalCameraEntity->transform.m_rotation.z << "\n";

}



