

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


template<typename T>
T Lerp(const T& p0, const T& p1, float t)
{
	return (1.0f - t) * p0 + t * p1;
}


int main()
{
	int width = 1280;
	int height = 720;
	// Create window and set clear color
	App::Init("Chateau Gateau", width, height);
	App::SetClearColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
	glfwGetCurrentContext();
	
	
	// Load in our model/texture resources
	LoadDefaultResources();
	MaterialCreator registerMaterial = MaterialCreator();
	registerMaterial.createMaterial("bakery/models/chicken.gltf", "bakery/textures/chick.png", *prog_texLit);
	

	// Create and set up camera
	Entity cameraEntity = Entity::Create();
	CCamera& cam = cameraEntity.Add<CCamera>(cameraEntity);
	cam.Perspective(60.0f, (float) width/height, 0.1f, 100.0f);
	cameraEntity.transform.m_pos = glm::vec3(4.0f, 0.0f, 0.0f);
	cameraEntity.transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f));


	

	//Creating Cash Register Entity
	Entity ent_register = Entity::Create();
	ent_register.Add<CMeshRenderer>(ent_register, *registerMaterial.getMesh(), *registerMaterial.getMaterial());
	ent_register.transform.m_scale = glm::vec3(0.5f, 0.5f, 0.5f);
	ent_register.transform.m_pos = glm::vec3(0.f, 0.f, 0.0f);
	ent_register.transform.m_rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f));


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

