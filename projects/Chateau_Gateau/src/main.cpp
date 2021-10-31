#include "glHeaders.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Texture.h"
#include "VBO.h"
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>


//load textures
//load uv into vbo
//send all to gpu
//set up texture parameters
//bind textures
//draw the object

#define STB_IMAGE_IMPLEMENTATION//have this line only once
#include<stb_image.h>//include this in other files if you want it there too
#include <vector>

unsigned char* image;
int width, height;
entt::registry sceneRegistry = entt::registry();
entt::entity p1 = sceneRegistry.create();
entt::entity p2 = sceneRegistry.create();
entt::entity p1b = sceneRegistry.create();
entt::entity p2b = sceneRegistry.create();
std::vector<entt::entity> renderingEntities = std::vector<entt::entity>();
GLuint textures[10];



GLFWwindow* window;




bool isOutOfBounds(float a) {
	
	
	if (a > 0.838953 || a < -0.838953) {
		return true;
	}
	
	return false;
}

bool isColliding(float a, float b) {
	glm::vec2 p1Pos = glm::vec2(sceneRegistry.get<Transform>(p1).getPosition().x, sceneRegistry.get<Transform>(p1).getPosition().y);
	glm::vec2 p2Pos = glm::vec2(sceneRegistry.get<Transform>(p2).getPosition().x, sceneRegistry.get<Transform>(p2).getPosition().y);

	return false;
}

bool initGLFW() {
	if (glfwInit() == GLFW_FALSE) {
		std::cout << "Failed to Initialize GLFW" << std::endl;
		return false;
	}

	//Create a new GLFW window
	window = glfwCreateWindow(800, 800, "Tank Game - 100782747", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	return true;
}

bool initGLAD() {
	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
		std::cout << "Failed to initialize Glad" << std::endl;
		return false;
	}
}


GLuint shader_program;

bool loadShaders() {
	// Read Shaders from file
	std::string vert_shader_str;
	std::ifstream vs_stream("vertex_shader.glsl", std::ios::in);
	if (vs_stream.is_open()) {
		std::string Line = "";
		while (getline(vs_stream, Line))
			vert_shader_str += "\n" + Line;
		vs_stream.close();
	}
	else {
		printf("Could not open vertex shader!!\n");
		return false;
	}
	const char* vs_str = vert_shader_str.c_str();

	std::string frag_shader_str;
	std::ifstream fs_stream("frag_shader.glsl", std::ios::in);
	if (fs_stream.is_open()) {
		std::string Line = "";
		while (getline(fs_stream, Line))
			frag_shader_str += "\n" + Line;
		fs_stream.close();
	}
	else {
		printf("Could not open fragment shader!!\n");
		return false;
	}
	const char* fs_str = frag_shader_str.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_str, NULL);
	glCompileShader(vs);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_str, NULL);
	glCompileShader(fs);

	shader_program = glCreateProgram();
	glAttachShader(shader_program, fs);
	glAttachShader(shader_program, vs);
	glLinkProgram(shader_program);

	return true;
}


//// Lecture 04

GLfloat P1rotZ = 0.0f;
GLfloat P2rotZ = 0.0f;
float speed = 0.006;
float rotSpeed = 1;
void keyboard() {
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		P1rotZ += rotSpeed;
		if (P1rotZ > 360) {
			P1rotZ = 0;
		}
		
		sceneRegistry.get<Transform>(p1).setRotation(sceneRegistry.get<Transform>(p1).getQuaternion(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(P1rotZ)));
		sceneRegistry.get<Transform>(p1).addRotation(glm::vec4(sceneRegistry.get<Transform>(p1).getQuaternion(glm::vec3(1, 0, 0), glm::radians(90.f))));

	}
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		if (P1rotZ < 0) {
			P1rotZ = 360;
		}
		P1rotZ -= rotSpeed;
		sceneRegistry.get<Transform>(p1).setRotation(sceneRegistry.get<Transform>(p1).getQuaternion(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(P1rotZ)));
		sceneRegistry.get<Transform>(p1).addRotation(glm::vec4(sceneRegistry.get<Transform>(p1).getQuaternion(glm::vec3(1, 0, 0), glm::radians(90.f))));
	}
	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
	glm::vec3 currentPos = sceneRegistry.get<Transform>(p1).getPosition();
		float newX = -speed * (sin(glm::radians(P1rotZ)));
		float newY = -speed * (cos(glm::radians(P1rotZ)));
		if (isOutOfBounds(currentPos.x + newX)) {
			newX = 0;
			//std::cout << "A" << std::endl;
		}
		if (isOutOfBounds(currentPos.y + newY)) {
			newY = 0;
			//std::cout << "B" << std::endl;
		}
		sceneRegistry.get<Transform>(p1).addPosition(
			glm::vec3(newX , newY, 0)
		);//speed * (1 - cos(P1rotZ))
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		sceneRegistry.get<Transform>(p1).addPosition(
			glm::vec3(speed * (sin(glm::radians(P1rotZ))), speed * (cos(glm::radians(P1rotZ))), 0)
		);//speed * (1 - cos(P1rotZ))
	}
		
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
	{
		P2rotZ += 1.f;
	}
		
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		P2rotZ -= 1.f;
	}
		
	
	//std::cout << sceneRegistry.get<Transform>(p1).getPosition().x << " " << sceneRegistry.get<Transform>(p1).getPosition().y << std::endl;
	//std::cout  <<  P1rotZ << " " << sin(glm::radians(P1rotZ)) << " " << cos(glm::radians(P1rotZ ))<< std::endl;
	//std::cout << "Y " << cos(P1rotZ) << std::endl;
}

///////////////


int main() {
	//Initialize GLFW
	if (!initGLFW())
		return 1;

	//Initialize GLAD
	if (!initGLAD())
		return 1;

	int p1ID = 0;
	sceneRegistry.emplace<Transform>(p1);
	sceneRegistry.emplace<MeshRenderer>(p1);
	sceneRegistry.emplace<Texture>(p1);
	sceneRegistry.get<Transform>(p1).setScale(glm::vec3(0.05, 0.05, 0.05));
	sceneRegistry.get<Transform>(p1).setPosition(glm::vec3(0.5,0,0));
	
	sceneRegistry.get<MeshRenderer>(p1).loadOBJ("Tank.obj", VBO(p1ID));
	sceneRegistry.get<Texture>(p1).setTexture("Tank1Tex.bmp", &textures[p1ID]);
	renderingEntities.push_back(p1);

	int p2ID = 1;
	sceneRegistry.emplace<Transform>(p2);
	sceneRegistry.emplace<MeshRenderer>(p2);
	sceneRegistry.emplace<Texture>(p2);
	sceneRegistry.get<Transform>(p2).setScale(glm::vec3(0.05, 0.05, 0.05));
	sceneRegistry.get<Transform>(p2).setPosition(glm::vec3(-0.5, 0, 0));

	sceneRegistry.get<MeshRenderer>(p2).loadOBJ("Tank.obj", VBO(p2ID));
	sceneRegistry.get<Texture>(p2).setTexture("Tank2Tex.bmp", &textures[p2ID]);
	renderingEntities.push_back(p2);

	int p2bID = 2;
	sceneRegistry.emplace<Transform>(p2b);
	sceneRegistry.emplace<MeshRenderer>(p2b);
	sceneRegistry.emplace<Texture>(p2b);
	sceneRegistry.get<Transform>(p2b).setScale(glm::vec3(0.05, 0.05, 0.05));
	sceneRegistry.get<Transform>(p2b).setPosition(glm::vec3(0, 0, 0));
	
	sceneRegistry.get<MeshRenderer>(p2b).loadOBJ("Circle.obj", VBO(p2bID));
	sceneRegistry.get<Texture>(p2b).setTexture("Bullet.bmp", &textures[p2bID]);
	renderingEntities.push_back(p2b);

	

	glm::vec3 camPos = glm::vec3(0.0f, 3.0f, 0.0f);
	/// LECTURE 05
	GLfloat cameraPos[] = { camPos.x, camPos.y, camPos.z };
	GLfloat lightPos[] = { 0.0f, 0.0f, 3.0f };
	////////////

	
	// Load our shaders

	if (!loadShaders())
		return 1;

	////////// LECTURE 04 //////////////

	// Projection - FoV, aspect ratio, near, far
	int width, height;
	//width = 1280;
	//height = 720;
	glfwGetWindowSize(window, &width, &height);

	glm::mat4 Projection = glm::perspective(glm::radians(45.0f),
		(float)width / (float)height, 0.1f, 100.0f);

	// View matrix - Camera

	glm::mat4 View = glm::lookAt(
		camPos, // camera position
		glm::vec3(0, 0, 0), //target
		glm::vec3(0, 1, 0) //up vector
	);

	// Model matrix
	glm::mat4 Model = glm::mat4(1.0f);//Identity matrix - resets your matrix

	glm::mat4 mvp;// = Projection * View * Model;

	// Handle for our mvp
	GLuint matrixMVP = glGetUniformLocation(shader_program, "MVP");

	////// LEC 05 - uniform variables
	//glGetUniformLocation(shader, l.c_str())
	GLuint matrixModel = glGetUniformLocation(shader_program, "Model");
	GLuint lightPosID = glGetUniformLocation(shader_program, "lightPos");
	GLuint cameraPosID = glGetUniformLocation(shader_program, "cameraPos");
	////////////////////////////////
	
	// GL states
	glEnable(GL_DEPTH_TEST);
	// LEC 05
	//glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK); //GL_BACK, GL_FRONT_AND_BACK

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	
	///// Game loop /////
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader_program);

		////////// Lecture 04								 X,    Y,    Z	
		//Model = glm::mat4(1.0f); // reset Model

		keyboard();
		int times = 0;
		for each (entt::entity e in renderingEntities) {
			Transform t = sceneRegistry.get<Transform>(e);
			mvp = Projection * View * t.getMatrix();

			// Send mvp to GPU
			glUniformMatrix4fv(matrixMVP, 1, GL_FALSE, &mvp[0][0]);//for camera
			//sceneRegistry.get<MeshRenderer>(e).setMatrixModel(shader_program, "Model");
			//std::cout << times << " " << sceneRegistry.get<MeshRenderer>(e).setMatrixModel(shader_program, "Model") << std::endl;
			glUniform3fv(lightPosID, 1, &lightPos[0]);//for lights
			glUniform3fv(cameraPosID, 1, &cameraPos[0]);

			glBindTexture(GL_TEXTURE_2D,*sceneRegistry.get<Texture>(e).getuint());

			sceneRegistry.get<MeshRenderer>(e).draw(&t, matrixModel);
			times++;
		}
		
		//t.addRotation(t.getQuaternion(glm::vec3(0.0f, 1.0f, 0.0f),glm::radians(P1rotZ)/2));
		//t.addRotation(t.getQuaternion(glm::vec3(1.0f, 0.0f, 0.0f),glm::radians(P1rotZ)/2));
		

		//Model = t.getMatrix();
		


		///////// LEC 05
		//glUniformMatrix4fv(matrixModel, 1, GL_FALSE, &Model[0][0]);//for mesh renderer


		glfwSwapBuffers(window);
	}
	return 0;

}