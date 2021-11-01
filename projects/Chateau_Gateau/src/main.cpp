#include "glHeaders.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Texture.h"
#include "Bullet.h"
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
entt::entity ob1 = sceneRegistry.create();
entt::entity ob2 = sceneRegistry.create();
entt::entity wup = sceneRegistry.create();
entt::entity wdown = sceneRegistry.create();
entt::entity wleft = sceneRegistry.create();
entt::entity wright = sceneRegistry.create();
entt::entity ex1 = sceneRegistry.create();
entt::entity ex2 = sceneRegistry.create();
entt::entity name1 = sceneRegistry.create();
entt::entity name2 = sceneRegistry.create();
entt::entity name1badge = sceneRegistry.create();
entt::entity name2badge = sceneRegistry.create();
entt::entity score1 = sceneRegistry.create();
entt::entity score2 = sceneRegistry.create();
glm::vec3 o1pos = glm::vec3(0, 0.45, -0.1);
glm::vec3 o2pos = glm::vec3(0, -0.45, -0.1);
int p1ExplosionFrames = -1;
int p2ExplosionFrames = -1;
int maxExplosionFrames = 30;
int p1Score = 0;
int p2Score = 0;
int maxScore = 5;
std::vector<entt::entity> renderingEntities = std::vector<entt::entity>();
std::vector<entt::entity> UIentities = std::vector<entt::entity>();

GLuint textures[18];



GLFWwindow* window;


void setScore(Transform* t, int score) {
	if (score == 0) {
		t->setScale(glm::vec3(0, 0, 0));
		t->setRotation(t->getQuaternion(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(180.f)));
		t->addRotation(glm::vec4(t->getQuaternion(glm::vec3(0, 1, 0), glm::radians(0.f))));
	}
	if (score == 1) {
		t->setScale(glm::vec3(0.07, 0.07, 0.07));
		t->setRotation(t->getQuaternion(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(180.f)));
		t->addRotation(glm::vec4(t->getQuaternion(glm::vec3(0, 1, 0), glm::radians(0.f))));

	}
	if (score == 2) {
		t->setScale(glm::vec3(0.07, 0.07, 0.07));
		t->setRotation(t->getQuaternion(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(180.f)));
		t->addRotation(glm::vec4(t->getQuaternion(glm::vec3(0, 1, 0), glm::radians(90.f))));
	}
	if (score == 3) {
		t->setScale(glm::vec3(0.07, 0.07, 0.07));
		t->setRotation(t->getQuaternion(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(180.f)));
		t->addRotation(glm::vec4(t->getQuaternion(glm::vec3(0, 1, 0), glm::radians(181.f))));
	}
	if (score == 4) {
		t->setScale(glm::vec3(0.07, 0.07, 0.07));
		t->setRotation(t->getQuaternion(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(180.f)));
		t->addRotation(glm::vec4(t->getQuaternion(glm::vec3(0, 1, 0), glm::radians(270.f))));
	}
	if (score == 5) {
		t->setScale(glm::vec3(0.07, 0.07, 0.07));
		t->setRotation(t->getQuaternion(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(180.f)));
		t->addRotation(glm::vec4(t->getQuaternion(glm::vec3(1, 0, 0), glm::radians(270.f))));
	}
}

float distance(glm::vec2 a, glm::vec2 b) {
	return sqrt(abs(pow(a.x - b.x, 2) + pow(a.y - b.y, 2)));
}

bool isOutOfBounds(float a) {
	
	
	if (a > 0.838953 || a < -0.838953) {
		return true;
	}
	
	return false;
}

bool bulletIsOutOfBounds(float a) {


	if (a > 0.938953 || a < -0.938953) {
		return true;
	}

	return false;
}

bool bulletIsHittingSquareX(float a) {
	if (a <= (o1pos.x + 0.219991) && a >= (o1pos.x - 0.219991)) {
		//std::cout << "X" << std::endl;
		return true;
	}
	if (a <= (o2pos.x + 0.219991) && a >= (o2pos.x - 0.219991)) {
		//std::cout << "X" << std::endl;
		return true;
	}
	
	return false;
}
bool bulletIsHittingSquareY(float a) {
	if (a <= (o2pos.y + 0.219991) && a >= (o2pos.y - 0.219991)) {
		//std::cout << "Y" << std::endl;
		return true;
	}
	if (a <= (o1pos.y + 0.219991) && a >= (o1pos.y - 0.219991)) {
		//std::cout << "Y" << std::endl;
		return true;
	}

	return false;
}

bool isHittingSquareX(float a) {
	if (a <= (o1pos.x + 0.299991) && a >= (o1pos.x - 0.299991)) {
		//std::cout << "X" << std::endl;
		return true;
	}
	if (a <= (o2pos.x + 0.299991) && a >= (o2pos.x - 0.299991)) {
		//std::cout << "X" << std::endl;
		return true;
	}

	return false;
}
bool isHittingSquareY(float a) {
	if (a <= (o2pos.y + 0.299991) && a >= (o2pos.y - 0.299991)) {
		//std::cout << "Y" << std::endl;
		return true;
	}
	if (a <= (o1pos.y + 0.299991) && a >= (o1pos.y - 0.299991)) {
		//std::cout << "Y" << std::endl;
		return true;
	}

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


///////////////

void player1Action() {
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		P1rotZ += rotSpeed;
		if (P1rotZ > 360) {
			P1rotZ = 0;
		}

		sceneRegistry.get<Transform>(p1).setRotation(sceneRegistry.get<Transform>(p1).getQuaternion(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(P1rotZ)));
		sceneRegistry.get<Transform>(p1).addRotation(glm::vec4(sceneRegistry.get<Transform>(p1).getQuaternion(glm::vec3(1, 0, 0), glm::radians(90.f))));

	}
	else if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
	{
		if (P1rotZ < 0) {
			P1rotZ = 360;
		}
		P1rotZ -= rotSpeed;
		sceneRegistry.get<Transform>(p1).setRotation(sceneRegistry.get<Transform>(p1).getQuaternion(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(P1rotZ)));
		sceneRegistry.get<Transform>(p1).addRotation(glm::vec4(sceneRegistry.get<Transform>(p1).getQuaternion(glm::vec3(1, 0, 0), glm::radians(90.f))));
	}
	else if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
	{
		glm::vec3 currentPos = sceneRegistry.get<Transform>(p1).getPosition();
		float newX = -speed * (sin(glm::radians(P1rotZ)));
		float newY = -speed * (cos(glm::radians(P1rotZ)));

		if (isOutOfBounds(currentPos.x + newX)) {//isHittingObsticleX(sceneRegistry.get<Transform>(p1).getPosition().x))
			newX = 0;

		}
		if (isOutOfBounds(currentPos.y + newY)) {
			newY = 0;

		}
		if (isHittingSquareX(currentPos.x + newX) && isHittingSquareY(currentPos.y + newY)) {
			if (isHittingSquareX(currentPos.x + newX)) {
				newX = 0;
			}
			if (isHittingSquareY(currentPos.y + newY)) {
				newY = 0;
			}

		}

		Transform* ot = &sceneRegistry.get<Transform>(p2);


		if (distance(glm::vec2(currentPos.x + newX, currentPos.y), glm::vec2(ot->getPosition().x, ot->getPosition().y)) < 0.15) {
			//std::cout << distance(glm::vec2(currentPos.x + newX, currentPos.y), glm::vec2(ot->getPosition().x, ot->getPosition().y)) << std::endl;
			newX = 0;
		}

		if (distance(glm::vec2(currentPos.x, currentPos.y + newY), glm::vec2(ot->getPosition().x, ot->getPosition().y)) < 0.15) {
			newY = 0;
		}
		sceneRegistry.get<Transform>(p1).addPosition(
			glm::vec3(newX, newY, 0)
		);//speed * (1 - cos(P1rotZ))

	}
	else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
	{
		glm::vec3 currentPos = sceneRegistry.get<Transform>(p1).getPosition();
		float newX = speed * (sin(glm::radians(P1rotZ)));
		float newY = speed * (cos(glm::radians(P1rotZ)));

		if (isOutOfBounds(currentPos.x + newX)) {//isHittingObsticleX(sceneRegistry.get<Transform>(p1).getPosition().x))
			newX = 0;
			//std::cout << "A" << std::endl;
		}
		if (isOutOfBounds(currentPos.y + newY)) {
			newY = 0;
			//std::cout << "B" << std::endl;
		}
		if (isHittingSquareX(currentPos.x + newX) && isHittingSquareY(currentPos.y + newY)) {
			if (isHittingSquareX(currentPos.x + newX)) {
				newX = 0;
			}
			if (isHittingSquareY(currentPos.y + newY)) {
				newY = 0;
			}

		}

		Transform* ot = &sceneRegistry.get<Transform>(p2);


		if (distance(glm::vec2(currentPos.x + newX, currentPos.y), glm::vec2(ot->getPosition().x, ot->getPosition().y)) < 0.15) {
			//std::cout << distance(glm::vec2(currentPos.x + newX, currentPos.y), glm::vec2(ot->getPosition().x, ot->getPosition().y)) << std::endl;
			newX = 0;
		}

		if (distance(glm::vec2(currentPos.x, currentPos.y + newY), glm::vec2(ot->getPosition().x, ot->getPosition().y)) < 0.15) {
			newY = 0;
		}
		sceneRegistry.get<Transform>(p1).addPosition(
			glm::vec3(newX, newY, 0)
		);//speed * (1 - cos(P1rotZ))
	}
	else if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
		bool isActive = sceneRegistry.get<Bullet>(p1b).isActive;
		if (!isActive) {
			glm::vec3 tank1Pos = sceneRegistry.get<Transform>(p1).getPosition();
			float bulletSpeed = sceneRegistry.get<Bullet>(p1b).bulletSpeed;
			sceneRegistry.get<Bullet>(p1b).isActive = true;
			float newX = (sin(glm::radians(P1rotZ)));
			float newY = (cos(glm::radians(P1rotZ)));
			sceneRegistry.get<Bullet>(p1b).setVelocity(glm::vec3(newX * -bulletSpeed, newY * -bulletSpeed, 0));
			sceneRegistry.get<Transform>(p1b).setPosition(glm::vec3(tank1Pos.x + (newX * -0.15), tank1Pos.y + (newY * -0.15), 0));
		}
	}
}



void player2Action() {
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		P2rotZ += rotSpeed;
		if (P2rotZ > 360) {
			P2rotZ = 0;
		}

		sceneRegistry.get<Transform>(p2).setRotation(sceneRegistry.get<Transform>(p2).getQuaternion(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(P2rotZ)));
		sceneRegistry.get<Transform>(p2).addRotation(glm::vec4(sceneRegistry.get<Transform>(p2).getQuaternion(glm::vec3(1, 0, 0), glm::radians(90.f))));

	}
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		P2rotZ -= rotSpeed;
		if (P2rotZ < 0) {
			P2rotZ = 360;
		}
		
		sceneRegistry.get<Transform>(p2).setRotation(sceneRegistry.get<Transform>(p2).getQuaternion(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(P2rotZ)));
		sceneRegistry.get<Transform>(p2).addRotation(glm::vec4(sceneRegistry.get<Transform>(p2).getQuaternion(glm::vec3(1, 0, 0), glm::radians(90.f))));
	}
	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		glm::vec3 currentPos = sceneRegistry.get<Transform>(p2).getPosition();
		float newX = -speed * (sin(glm::radians(P2rotZ)));
		float newY = -speed * (cos(glm::radians(P2rotZ)));

		if (isOutOfBounds(currentPos.x + newX)) {//isHittingObsticleX(sceneRegistry.get<Transform>(p1).getPosition().x))
			newX = 0;

		}
		if (isOutOfBounds(currentPos.y + newY)) {
			newY = 0;

		}
		if (isHittingSquareX(currentPos.x + newX) && isHittingSquareY(currentPos.y + newY)) {
			if (isHittingSquareX(currentPos.x + newX)) {
				newX = 0;
			}
			if (isHittingSquareY(currentPos.y + newY)) {
				newY = 0;
			}

		}
		Transform* ot = &sceneRegistry.get<Transform>(p1);


		if (distance(glm::vec2(currentPos.x + newX, currentPos.y), glm::vec2(ot->getPosition().x, ot->getPosition().y)) < 0.15) {
			//std::cout << distance(glm::vec2(currentPos.x + newX, currentPos.y), glm::vec2(ot->getPosition().x, ot->getPosition().y)) << std::endl;
			newX = 0;
		}

		if (distance(glm::vec2(currentPos.x, currentPos.y + newY), glm::vec2(ot->getPosition().x, ot->getPosition().y)) < 0.15) {
			newY = 0;
		}

		sceneRegistry.get<Transform>(p2).addPosition(
			glm::vec3(newX, newY, 0)
		);//speed * (1 - cos(P1rotZ))

	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		glm::vec3 currentPos = sceneRegistry.get<Transform>(p2).getPosition();
		float newX = speed * (sin(glm::radians(P2rotZ)));
		float newY = speed * (cos(glm::radians(P2rotZ)));

		if (isOutOfBounds(currentPos.x + newX)) {//isHittingObsticleX(sceneRegistry.get<Transform>(p1).getPosition().x))
			newX = 0;
			//std::cout << "A" << std::endl;
		}
		if (isOutOfBounds(currentPos.y + newY)) {
			newY = 0;
			//std::cout << "B" << std::endl;
		}
		if (isHittingSquareX(currentPos.x + newX) && isHittingSquareY(currentPos.y + newY)) {
			if (isHittingSquareX(currentPos.x + newX)) {
				newX = 0;
			}
			if (isHittingSquareY(currentPos.y + newY)) {
				newY = 0;
			}

		}
		
		Transform* ot = &sceneRegistry.get<Transform>(p1);


		if (distance(glm::vec2(currentPos.x + newX, currentPos.y), glm::vec2(ot->getPosition().x, ot->getPosition().y)) < 0.15) {
			//std::cout << distance(glm::vec2(currentPos.x + newX, currentPos.y), glm::vec2(ot->getPosition().x, ot->getPosition().y)) << std::endl;
			newX = 0;
		}

		if (distance(glm::vec2(currentPos.x , currentPos.y + newY), glm::vec2(ot->getPosition().x, ot->getPosition().y)) < 0.15) {
			newY = 0;
		}
		
		sceneRegistry.get<Transform>(p2).addPosition(
			glm::vec3(newX, newY, 0)
		);//speed * (1 - cos(P1rotZ))
	}
	else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		bool isActive = sceneRegistry.get<Bullet>(p2b).isActive;
		if (!isActive) {
			glm::vec3 tank2Pos = sceneRegistry.get<Transform>(p2).getPosition();
			float bulletSpeed = sceneRegistry.get<Bullet>(p2b).bulletSpeed;
			sceneRegistry.get<Bullet>(p2b).isActive = true;
			float newX = (sin(glm::radians(P2rotZ)));
			float newY = (cos(glm::radians(P2rotZ)));
			sceneRegistry.get<Bullet>(p2b).setVelocity(glm::vec3(newX * -bulletSpeed, newY * -bulletSpeed, 0));
			sceneRegistry.get<Transform>(p2b).setPosition(glm::vec3(tank2Pos.x + (newX * -0.15), tank2Pos.y + (newY * -0.15), 0));
		}
	}
}
void keyboard() {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		//reset the game
		p1Score = 0;
		p2Score = 0;
		setScore(&sceneRegistry.get<Transform>(score1), p1Score);
		setScore(&sceneRegistry.get<Transform>(score2), p2Score);
	}
	if (p1Score < maxScore && p2Score < maxScore) {
		if (p1ExplosionFrames <= -1) {
			player1Action();
		}
		if (p2ExplosionFrames <= -1) {
			player2Action();
		}
	}
	
	
	

}

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
	sceneRegistry.emplace<Bullet>(p2b);
	sceneRegistry.emplace<Texture>(p2b);
	sceneRegistry.get<Transform>(p2b).setScale(glm::vec3(0.01, 0.01, 0.01));
	sceneRegistry.get<Transform>(p2b).setPosition(glm::vec3(1.2, 0, 0));
	
	sceneRegistry.get<MeshRenderer>(p2b).loadOBJ("Circle.obj", VBO(p2bID));
	sceneRegistry.get<Texture>(p2b).setTexture("Bullet.bmp", &textures[p2bID]);
	renderingEntities.push_back(p2b);

	int p1bID = 3;
	sceneRegistry.emplace<Transform>(p1b);
	sceneRegistry.emplace<MeshRenderer>(p1b);
	sceneRegistry.emplace<Texture>(p1b);
	sceneRegistry.emplace<Bullet>(p1b);
	sceneRegistry.get<Transform>(p1b).setScale(glm::vec3(0.01, 0.01, 0.01));
	sceneRegistry.get<Transform>(p1b).setPosition(glm::vec3(1.1, 0, 0));

	sceneRegistry.get<MeshRenderer>(p1b).loadOBJ("Circle.obj", VBO(p1bID));
	sceneRegistry.get<Texture>(p1b).setTexture("Bullet.bmp", &textures[p1bID]);
	renderingEntities.push_back(p1b);

	int o1 = 4;
	sceneRegistry.emplace<Transform>(ob1);
	sceneRegistry.emplace<MeshRenderer>(ob1);
	sceneRegistry.emplace<Texture>(ob1);
	sceneRegistry.get<Transform>(ob1).setScale(glm::vec3(0.19, 0, 0.19));
	sceneRegistry.get<Transform>(ob1).setPosition(o1pos);

	sceneRegistry.get<MeshRenderer>(ob1).loadOBJ("Obsticle.obj", VBO(o1));
	sceneRegistry.get<Texture>(ob1).setTexture("Obsticle.bmp", &textures[o1]);
	renderingEntities.push_back(ob1);

	int o2 = 5;
	sceneRegistry.emplace<Transform>(ob2);
	sceneRegistry.emplace<MeshRenderer>(ob2);
	sceneRegistry.emplace<Texture>(ob2);
	sceneRegistry.get<Transform>(ob2).setScale(glm::vec3(0.19, 0, 0.19));
	sceneRegistry.get<Transform>(ob2).setPosition(o2pos);

	sceneRegistry.get<MeshRenderer>(ob2).loadOBJ("Obsticle.obj", VBO(o2));
	sceneRegistry.get<Texture>(ob2).setTexture("Obsticle.bmp", &textures[o2]);
	renderingEntities.push_back(ob2);

	int e1 = 6;
	sceneRegistry.emplace<Transform>(ex1);
	sceneRegistry.emplace<MeshRenderer>(ex1);
	sceneRegistry.emplace<Texture>(ex1);
	sceneRegistry.get<Transform>(ex1).setScale(glm::vec3(0.10, 0.10, 0.10));
	sceneRegistry.get<Transform>(ex1).setPosition(glm::vec3(0,0,0));

	sceneRegistry.get<MeshRenderer>(ex1).loadOBJ("Obsticle.obj", VBO(e1));
	sceneRegistry.get<Texture>(ex1).setTexture("Explosion.bmp", &textures[e1]);

	int e2 = 7;
	sceneRegistry.emplace<Transform>(ex2);
	sceneRegistry.emplace<MeshRenderer>(ex2);
	sceneRegistry.emplace<Texture>(ex2);
	sceneRegistry.get<Transform>(ex2).setScale(glm::vec3(0.10, 0.10, 0.10));
	sceneRegistry.get<Transform>(ex2).setPosition(glm::vec3(0, 0, 0));

	sceneRegistry.get<MeshRenderer>(ex2).loadOBJ("Obsticle.obj", VBO(e2));
	sceneRegistry.get<Texture>(ex2).setTexture("Explosion.bmp", &textures[e2]);

	int n1 = 8;
	sceneRegistry.emplace<Transform>(name1);
	sceneRegistry.emplace<MeshRenderer>(name1);
	sceneRegistry.emplace<Texture>(name1);
	sceneRegistry.get<Transform>(name1).setScale(glm::vec3(0.07, 0.07, 0.07));
	sceneRegistry.get<Transform>(name1).setPosition(glm::vec3(o1pos.x - 0.07, o1pos.y + 0.07, o1pos.z - 0.1));
	sceneRegistry.get<Transform>(name1).setRotation(sceneRegistry.get<Transform>(name1).getQuaternion(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(180.f)));
	sceneRegistry.get<Transform>(name1).addRotation(glm::vec4(sceneRegistry.get<Transform>(name1).getQuaternion(glm::vec3(1, 0, 0), glm::radians(90.f))));


	sceneRegistry.get<MeshRenderer>(name1).loadOBJ("Obsticle.obj", VBO(n1));
	sceneRegistry.get<Texture>(name1).setTexture("numbers.bmp", &textures[n1]);
	UIentities.push_back(name1);

	int n2 = 8;
	sceneRegistry.emplace<Transform>(name2);
	sceneRegistry.emplace<MeshRenderer>(name2);
	sceneRegistry.emplace<Texture>(name2);
	sceneRegistry.get<Transform>(name2).setScale(glm::vec3(0.07, 0.07, 0.07));
	sceneRegistry.get<Transform>(name2).setPosition(glm::vec3(o2pos.x - 0.07, o2pos.y + 0.07, o2pos.z - 0.1));
	sceneRegistry.get<Transform>(name2).setRotation(sceneRegistry.get<Transform>(name2).getQuaternion(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(180.f)));
	sceneRegistry.get<Transform>(name2).addRotation(glm::vec4(sceneRegistry.get<Transform>(name2).getQuaternion(glm::vec3(1, 0, 0), glm::radians(90.f))));


	sceneRegistry.get<MeshRenderer>(name2).loadOBJ("Obsticle.obj", VBO(n2));
	sceneRegistry.get<Texture>(name2).setTexture("numbers.bmp", &textures[n2]);
	UIentities.push_back(name2);

	int n1b = 9;
	sceneRegistry.emplace<Transform>(name1badge);
	sceneRegistry.emplace<MeshRenderer>(name1badge);
	sceneRegistry.emplace<Texture>(name1badge);
	sceneRegistry.get<Transform>(name1badge).setScale(glm::vec3(0.07, 0.07, 0.07));
	sceneRegistry.get<Transform>(name1badge).setPosition(glm::vec3(o1pos.x + 0.07, o1pos.y + 0.07, o1pos.z - 0.1));
	sceneRegistry.get<Transform>(name1badge).setRotation(sceneRegistry.get<Transform>(name1badge).getQuaternion(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(180.f)));
	sceneRegistry.get<Transform>(name1badge).addRotation(glm::vec4(sceneRegistry.get<Transform>(name1badge).getQuaternion(glm::vec3(0, 1, 0), glm::radians(0.f))));

	
	sceneRegistry.get<MeshRenderer>(name1badge).loadOBJ("Obsticle.obj", VBO(n1b));
	sceneRegistry.get<Texture>(name1badge).setTexture("numbers.bmp", &textures[n1b]);
	UIentities.push_back(name1badge);
	
	int n2b = 9;
	sceneRegistry.emplace<Transform>(name2badge);
	sceneRegistry.emplace<MeshRenderer>(name2badge);
	sceneRegistry.emplace<Texture>(name2badge);
	sceneRegistry.get<Transform>(name2badge).setScale(glm::vec3(0.07, 0.07, 0.07));
	sceneRegistry.get<Transform>(name2badge).setPosition(glm::vec3(o2pos.x + 0.07, o2pos.y + 0.07, o2pos.z - 0.1));
	sceneRegistry.get<Transform>(name2badge).setRotation(sceneRegistry.get<Transform>(name2badge).getQuaternion(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(180.f)));
	sceneRegistry.get<Transform>(name2badge).addRotation(glm::vec4(sceneRegistry.get<Transform>(name2badge).getQuaternion(glm::vec3(0, 1, 0), glm::radians(90.f))));


	sceneRegistry.get<MeshRenderer>(name2badge).loadOBJ("Obsticle.obj", VBO(n2b));
	sceneRegistry.get<Texture>(name2badge).setTexture("numbers.bmp", &textures[n2b]);
	UIentities.push_back(name2badge);

	int s1 = 10;
	sceneRegistry.emplace<Transform>(score1);
	sceneRegistry.emplace<MeshRenderer>(score1);
	sceneRegistry.emplace<Texture>(score1);
	sceneRegistry.get<Transform>(score1).setScale(glm::vec3(0.0, 0.0, 0.0));
	sceneRegistry.get<Transform>(score1).setPosition(glm::vec3(o1pos.x, o1pos.y - 0.07, o1pos.z - 0.1));
	sceneRegistry.get<Transform>(score1).setRotation(sceneRegistry.get<Transform>(score1).getQuaternion(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(180.f)));
	sceneRegistry.get<Transform>(score1).addRotation(glm::vec4(sceneRegistry.get<Transform>(score1).getQuaternion(glm::vec3(0, 1, 0), glm::radians(90.f))));


	sceneRegistry.get<MeshRenderer>(score1).loadOBJ("Obsticle.obj", VBO(s1));
	sceneRegistry.get<Texture>(score1).setTexture("numbers.bmp", &textures[s1]);
	UIentities.push_back(score1);

	int s2 = 11;
	sceneRegistry.emplace<Transform>(score2);
	sceneRegistry.emplace<MeshRenderer>(score2);
	sceneRegistry.emplace<Texture>(score2);
	sceneRegistry.get<Transform>(score2).setScale(glm::vec3(0.0, 0.0, 0.0));
	sceneRegistry.get<Transform>(score2).setPosition(glm::vec3(o2pos.x, o2pos.y - 0.07, o2pos.z - 0.1));
	sceneRegistry.get<Transform>(score2).setRotation(sceneRegistry.get<Transform>(score2).getQuaternion(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(180.f)));
	sceneRegistry.get<Transform>(score2).addRotation(glm::vec4(sceneRegistry.get<Transform>(score2).getQuaternion(glm::vec3(0, 1, 0), glm::radians(90.f))));


	sceneRegistry.get<MeshRenderer>(score2).loadOBJ("Obsticle.obj", VBO(s2));
	sceneRegistry.get<Texture>(score2).setTexture("numbers.bmp", &textures[s2]);
	UIentities.push_back(score2);


	int wallleft = 12;
	sceneRegistry.emplace<Transform>(wleft);
	sceneRegistry.emplace<MeshRenderer>(wleft);
	sceneRegistry.emplace<Texture>(wleft);
	sceneRegistry.get<Transform>(wleft).setScale(glm::vec3(1, 1, 1));
	sceneRegistry.get<Transform>(wleft).setPosition(glm::vec3(-1.937953, 0, 0));

	sceneRegistry.get<MeshRenderer>(wleft).loadOBJ("Obsticle.obj", VBO(wallleft));
	sceneRegistry.get<Texture>(wleft).setTexture("Obsticle.bmp", &textures[wallleft]);
	renderingEntities.push_back(wleft);

	int wallright = 13;
	sceneRegistry.emplace<Transform>(wright);
	sceneRegistry.emplace<MeshRenderer>(wright);
	sceneRegistry.emplace<Texture>(wright);
	sceneRegistry.get<Transform>(wright).setScale(glm::vec3(1, 1, 1));
	sceneRegistry.get<Transform>(wright).setPosition(glm::vec3(1.937953, 0, 0));

	sceneRegistry.get<MeshRenderer>(wright).loadOBJ("Obsticle.obj", VBO(wallright));
	sceneRegistry.get<Texture>(wright).setTexture("Obsticle.bmp", &textures[wallright]);
	renderingEntities.push_back(wright);

	int wallup = 14;
	sceneRegistry.emplace<Transform>(wup);
	sceneRegistry.emplace<MeshRenderer>(wup);
	sceneRegistry.emplace<Texture>(wup);
	sceneRegistry.get<Transform>(wup).setScale(glm::vec3(1, 1, 1));
	sceneRegistry.get<Transform>(wup).setPosition(glm::vec3(0, 1.937953, 0));

	sceneRegistry.get<MeshRenderer>(wup).loadOBJ("Obsticle.obj", VBO(wallup));
	sceneRegistry.get<Texture>(wup).setTexture("Obsticle.bmp", &textures[wallup]);
	renderingEntities.push_back(wup);

	int walldown = 15;
	sceneRegistry.emplace<Transform>(wdown);
	sceneRegistry.emplace<MeshRenderer>(wdown);
	sceneRegistry.emplace<Texture>(wdown);
	sceneRegistry.get<Transform>(wdown).setScale(glm::vec3(1, 1, 1));
	sceneRegistry.get<Transform>(wdown).setPosition(glm::vec3(0, -1.937953, 0));

	sceneRegistry.get<MeshRenderer>(wdown).loadOBJ("Obsticle.obj", VBO(walldown));
	sceneRegistry.get<Texture>(wdown).setTexture("Obsticle.bmp", &textures[walldown]);
	renderingEntities.push_back(wdown);

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

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader_program);

		////////// Lecture 04								 X,    Y,    Z	
		//Model = glm::mat4(1.0f); // reset Model

		keyboard();
		int times = 0;
		for (int i = 0; i < 2; i++) {
			entt::entity activeBullet = p1b;
			entt::entity otherBullet = p2b;
			if (i == 1) {
				activeBullet = p2b;
				otherBullet = p1b;
			}
			Bullet* bb = &sceneRegistry.get<Bullet>(activeBullet);
			Transform* bt = &sceneRegistry.get<Transform>(activeBullet);
			if (bb->isActive) {

				if (bulletIsOutOfBounds(bt->getPosition().x)) {
					glm::vec3 newV = bb->getVelocity();
					newV.x *= -1;
					bb->setVelocity(newV);
					bb->addHit();
				}
				if (bulletIsOutOfBounds(bt->getPosition().y)) {
					glm::vec3 newV = bb->getVelocity();
					newV.y *= -1;
					bb->setVelocity(newV);
					bb->addHit();

				}

				if (bulletIsHittingSquareX(bt->getPosition().x) && bulletIsHittingSquareY(bt->getPosition().y)) {
					glm::vec3 newV = bb->getVelocity();
					glm::vec3 nextPos = bt->getPosition();
					if (!bulletIsHittingSquareX(bt->getPosition().x - bb->getVelocity().x)) {
						newV.x *= -1;
					}
					if (!bulletIsHittingSquareY(bt->getPosition().y - bb->getVelocity().y)) {
						newV.y *= -1;
					}
					bb->setVelocity(newV);
					bb->addHit();
					//sceneRegistry.get<Transform>(p1b).setPosition(nextPos);
				}

				Transform* t1t = &sceneRegistry.get<Transform>(p1);
				Transform* t2t = &sceneRegistry.get<Transform>(p2);
				float t1d = distance(glm::vec2(bt->getPosition().x, bt->getPosition().y), glm::vec2(t1t->getPosition().x, t1t->getPosition().y));
				float t2d = distance(glm::vec2(bt->getPosition().x, bt->getPosition().y), glm::vec2(t2t->getPosition().x, t2t->getPosition().y));
				//std::cout << "P1: " <<t1d << std::endl;
				//std::cout << "P2: " << t2d << std::endl;

				if (t1d < 0.12) {
					bb->isActive = false;
					renderingEntities.erase(std::remove(renderingEntities.begin(), renderingEntities.end(), p1), renderingEntities.end());
					p1ExplosionFrames = 0;
					p2Score++;
					setScore(&sceneRegistry.get<Transform>(score2), p2Score);

				}
				if (t2d < 0.12) {
					bb->isActive = false;
					renderingEntities.erase(std::remove(renderingEntities.begin(), renderingEntities.end(), p2), renderingEntities.end());
					p2ExplosionFrames = 0;
					p1Score++;
					setScore(&sceneRegistry.get<Transform>(score1), p1Score);
				}

					bt->addPosition(bb->getVelocity());

					Transform* abp = &sceneRegistry.get<Transform>(activeBullet);
					Transform* obp = &sceneRegistry.get<Transform>(otherBullet);

					if (distance(glm::vec2(abp->getPosition().x, abp->getPosition().y),
						glm::vec2(obp->getPosition().x, obp->getPosition().y)) < 0.05) {
						sceneRegistry.get<Bullet>(activeBullet).isActive = false;
						sceneRegistry.get<Bullet>(otherBullet).isActive = false;
					}
				if (bb->getHits() >= bb->getMaxHits()) {
					bb->isActive = false;
				}
			}
			else
			{
				sceneRegistry.get<Transform>(activeBullet).setPosition(glm::vec3(1.1, 0, 0));
				bb->setHits(0);
				//p1bb->isActive = false;
			}
		}
		
		
		
		if (p1ExplosionFrames > -1) {
			sceneRegistry.get<Transform>(ex1).setPosition(sceneRegistry.get<Transform>(p1).getPosition());
			sceneRegistry.get<Transform>(ex1).addRotation(sceneRegistry.get<Transform>(ex1).getQuaternion(glm::vec3(0, 1, 0), 90));
			if (p1ExplosionFrames == 0) {
				renderingEntities.push_back(ex1);
			}
			
			p1ExplosionFrames++;
			if (p1ExplosionFrames >= maxExplosionFrames) {
				sceneRegistry.get<Transform>(ex1).setRotation(sceneRegistry.get<Transform>(ex1).getQuaternion(glm::vec3(1, 0, 0), 90));
				p1ExplosionFrames = -1;
				renderingEntities.erase(std::remove(renderingEntities.begin(), renderingEntities.end(), ex1), renderingEntities.end());
				renderingEntities.push_back(p1);
			}
		}

		if (p2ExplosionFrames > -1) {
			sceneRegistry.get<Transform>(ex2).setPosition(sceneRegistry.get<Transform>(p2).getPosition());
			sceneRegistry.get<Transform>(ex2).addRotation(sceneRegistry.get<Transform>(ex2).getQuaternion(glm::vec3(0, 1, 0), 90));
			if (p2ExplosionFrames == 0) {
				renderingEntities.push_back(ex2);
				
			}

			p2ExplosionFrames++;
			if (p2ExplosionFrames >= maxExplosionFrames) {
				sceneRegistry.get<Transform>(ex2).setRotation(sceneRegistry.get<Transform>(ex2).getQuaternion(glm::vec3(1, 0, 0), 90));
				p2ExplosionFrames = -1;
				renderingEntities.erase(std::remove(renderingEntities.begin(), renderingEntities.end(), ex2), renderingEntities.end());
				renderingEntities.push_back(p2);

			}
		}




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

		for each (entt::entity e in UIentities) {
			Transform t = sceneRegistry.get<Transform>(e);
			mvp = Projection * View * t.getMatrix();

			// Send mvp to GPU
			glUniformMatrix4fv(matrixMVP, 1, GL_FALSE, &mvp[0][0]);//for camera
			//sceneRegistry.get<MeshRenderer>(e).setMatrixModel(shader_program, "Model");
			//std::cout << times << " " << sceneRegistry.get<MeshRenderer>(e).setMatrixModel(shader_program, "Model") << std::endl;
			glUniform3fv(lightPosID, 1, &lightPos[0]);//for lights
			glUniform3fv(cameraPosID, 1, &cameraPos[0]);

			glBindTexture(GL_TEXTURE_2D, *sceneRegistry.get<Texture>(e).getuint());

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