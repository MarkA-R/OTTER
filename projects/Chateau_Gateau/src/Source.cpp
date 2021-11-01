

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



// Keep our main cleaner
void LoadDefaultResources();

// Function to handle user inputs
void GetInput();

// Templated LERP function
template<typename T>
T Lerp(const T& p0, const T& p1, float t)
{
	return (1.0f - t) * p0 + t * p1;
}


Entity* globalCameraEntity;
GLFWwindow* gameWindow;
//Mouse State
bool firstMouse = true;
float lastX = 800 / 2;
float lastY = 600 / 2;

float yaw = -90.0f;
float pitch = 0.0f;
int main()
{
	int width = 1280;
	int height = 720;
	// Create window and set clear color
	App::Init("Chateau Gateau", 800, 600);
	App::SetClearColor(glm::vec4(0.0f, 0.27f, 0.4f, 1.0f));

	// Initialize ImGui
	App::InitImgui();

	// Load in our model/texture resources
	LoadDefaultResources();
	MaterialCreator registerMaterial = MaterialCreator();
	registerMaterial.createMaterial("bakery/models/cashregister.gltf", "bakery/textures/cashregister.png", *prog_texLit);
	

	// Create and set up camera
	Entity cameraEntity = Entity::Create();
	CCamera& cam = cameraEntity.Add<CCamera>(cameraEntity);
	cam.Perspective(60.0f, (float) width/height, 0.1f, 100.0f);
	cameraEntity.transform.m_pos = glm::vec3(0.0f, 0.0f, 0.0f);
	cameraEntity.transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f));

	// Creating duck entity
	Entity ent_ducky = Entity::Create();
	ent_ducky.Add<CMeshRenderer>(ent_ducky, *mesh_ducky, *mat_ducky);
	ent_ducky.Add<CPathAnimator>(ent_ducky);
	ent_ducky.transform.m_scale = glm::vec3(0.005f, 0.005f, 0.005f);
	ent_ducky.transform.m_pos = glm::vec3(0.0f, -1.0f, 0.0f);
	ent_ducky.transform.m_rotation = glm::angleAxis(glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	

	//Creating Cash Register Entity
	Entity ent_register = Entity::Create();
	ent_register.Add<CMeshRenderer>(ent_register, *registerMaterial.getMesh(), *registerMaterial.getMaterial());
	ent_register.transform.m_scale = glm::vec3(0.5f, 0.5f, 0.5f);
	ent_register.transform.m_rotation = glm::angleAxis(glm::radians(270.f), glm::vec3(0.0f, 1.0f, 0.0f));
	ent_register.transform.m_pos = glm::vec3(-3.f, -3.f, 0.0f);
	


	App::Tick();

	float angle = 0;
	// Main loop
	while (!App::IsClosing() && !Input::GetKeyDown(GLFW_KEY_ESCAPE))
	{
		App::FrameStart();

		// Update our LERP timers
		float deltaTime = App::GetDeltaTime();


		//cameraEntity.transform.m_rotation = glm::angleAxis(glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
		angle++;
		std::cout << angle << std::endl;
		// Update camera
		cameraEntity.Get<CCamera>().Update();

	
	
		// Update cash register transfrom
		ent_register.transform.RecomputeGlobal();

	
		// Draw register
		ent_register.Get<CMeshRenderer>().Draw();


		GetInput();
		//Get User Inputs


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

void GetInput()
{
	double xPos, yPos;
	if (Input::GetKeyDown(GLFW_KEY_SPACE))
	{
		std::cout << "SPACE\n";
	}
	if (Input::GetKey(GLFW_KEY_W))
	{
		globalCameraEntity->transform.m_rotation.x += 0.001;
	}
	if (Input::GetKeyDown(GLFW_KEY_ENTER))
	{
		glfwGetCursorPos(gameWindow, &xPos, &yPos);
		std::cout << xPos << " , " << yPos << "\n";
	}	

	if (Input::GetKey(GLFW_KEY_W))
	{
		globalCameraEntity->transform.m_rotation.x += 0.001f;
		std::cout << globalCameraEntity->transform.m_rotation.x << " , " << globalCameraEntity->transform.m_rotation.y << " , " << globalCameraEntity->transform.m_rotation.z << "\n";
	}
	else if (Input::GetKey(GLFW_KEY_S))
	{
		globalCameraEntity->transform.m_rotation.x -= 0.001f;
		std::cout << globalCameraEntity->transform.m_rotation.x << " , " << globalCameraEntity->transform.m_rotation.y << " , " << globalCameraEntity->transform.m_rotation.z << "\n";


	}
	else if (Input::GetKey(GLFW_KEY_A))
	{
		globalCameraEntity->transform.m_rotation.y+= 0.001f;

	}
	else if (Input::GetKey(GLFW_KEY_D))
	{
		globalCameraEntity->transform.m_rotation.y -= 0.001f;

	}

	glfwGetCursorPos(gameWindow, &xPos, &yPos);
	if (firstMouse == true)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}
	float xOffset = xPos - lastX;
	float yOffset = yPos - lastY;
	lastX = xPos;
	lastY = yPos;

	float sensitivity = 0.001f;
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	
		
	globalCameraEntity->transform.m_rotation.y += xOffset;
	globalCameraEntity->transform.m_rotation.x += yOffset;
	

	std::cout << globalCameraEntity->transform.m_rotation.x << " , " << globalCameraEntity->transform.m_rotation.y << " , " << globalCameraEntity->transform.m_rotation.z << "\n";

}


}

