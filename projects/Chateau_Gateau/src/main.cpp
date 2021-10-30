#include "glHeaders.h"
#include "Transform.h"
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

static inline void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch);
		}));
}

// trim from end (in place)
static inline void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string& s) {
	ltrim(s);
	rtrim(s);
}

struct Vertex {
	Vertex(glm::vec3 p, glm::vec3 n, glm::vec2 u, glm::vec4 c);
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec4 colour;
};

Vertex::Vertex(glm::vec3 p, glm::vec3 n, glm::vec2 u, glm::vec4 c)
{
	pos = p;
	normal = n;
	uv = u;
	colour = c;
}
void loadImage(const std::string& fileName) {
	int channels;
	//open gl loads image flipped
	stbi_set_flip_vertically_on_load(true);//cause opengl loads it in flipped
	
	//load image
	image = stbi_load(fileName.c_str(), &width, &height, &channels, 0);
	if (image) {
		std::cout << "image loaded: " << width << " " << height << std::endl;
	}
	else
	{
		std::cout << "image failed to load - " << fileName << std::endl;
	}
}

GLFWwindow* window;

bool initGLFW() {
	if (glfwInit() == GLFW_FALSE) {
		std::cout << "Failed to Initialize GLFW" << std::endl;
		return false;
	}

	//Create a new GLFW window
	window = glfwCreateWindow(800, 800, "Week 7 - 100782747", nullptr, nullptr);
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

GLfloat rotY = 0.0f;
GLfloat rotX = 0.0f;

void keyboard() {
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		rotY += 1.f;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		rotY -= 1.f;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		rotX += 1.f;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		rotX -= 1.f;

}

///////////////


int main() {
	//Initialize GLFW
	if (!initGLFW())
		return 1;

	//Initialize GLAD
	if (!initGLAD())
		return 1;

	//// Lecture 3 starts here
	entt::entity duck = sceneRegistry.create();
	sceneRegistry.emplace<Transform>(duck);


	std::string filename = "ChickenFull.obj";
	int trisToDraw = 0;
	std::ifstream file;
	file.open(filename, std::ios::binary);

	// If our file fails to open, we will throw an error
	if (!file) {
		throw std::runtime_error("Failed to open file");
	}



	std::string line;


	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<glm::ivec3> vertices;

	glm::vec3 vecData;
	glm::ivec3 vertexIndices;
	glm::vec3 normData;
	glm::vec2 uvData;


	// Read and process the entire file
	while (file.peek() != EOF) {

		// Read in the first part of the line (ex: f, v, vn, etc...)
		std::string command;
		file >> command;// Rest of part 1 here
		//std::cout << command << std::endl;
		if (command == "#")
		{
			std::getline(file, line);
		}// The v command defines a vertex's position

		else if (command == "v") {
			// Read in and store a position   
			file >> vecData.x >> vecData.y >> vecData.z;
			//std::cout << command << ": " << vecData.x << " " << vecData.y << " " << vecData.z << std::endl;
			positions.push_back(vecData);
		}
		else if (command == "f") {
			// Read the rest of the line from the file
			std::getline(file, line);
			// Trim whitespace from either end of the line
			trim(line);// Create a string stream so we can use streaming operators on it
			std::stringstream stream = std::stringstream(line);
			// Next slide goes here
			for (int ix = 0; ix < 3; ix++) {
				
				// Read in the 3 attributes (position, UV, normal)
				char separator;
				stream >> vertexIndices.x >> separator >> vertexIndices.y >> separator >> vertexIndices.z;
				//std::cout << command << ": " << vertexIndices.x << " " << vertexIndices.y << " " << vertexIndices.z << std::endl;
				if (vertexIndices.x < 0) {
					vertexIndices.x = positions.size() + 1 + vertexIndices.x;
				}
				if (vertexIndices.y < 0) {
					vertexIndices.y = uvs.size() + 1 + vertexIndices.y;
				}
				if (vertexIndices.z < 0) {
					vertexIndices.z = normals.size() + 1 + vertexIndices.z;
				}
				// OBJ format uses 1-based indices 
				vertexIndices -= glm::ivec3(1);
				// add the vertex indices to the list
				// NOTE: This will create duplicate vertices! 
				vertices.push_back(vertexIndices);
			}
		}
		else if (command == "vn") {//if the command is vn, read the normal data and push it back into the normals vector
			file >> normData.x >> normData.y >> normData.z;
			//std::cout << command << ": " << normData.x << " " << normData.y << " " << normData.z << std::endl;

			normals.push_back(normData);
		}
		else if (command == "vt") {//if the command is vt, read the UV data and push it back into the UV vector
			file >> uvData.x >> uvData.y;
			//std::cout << command << ": " << uvData.x << " " << uvData.y << std::endl;

			uvs.push_back(uvData);
		}

	}

	// We will ignore the rest of the line for comment lines


	std::vector<Vertex> vertexData;
	for (int ix = 0; ix < vertices.size(); ix++) {
		glm::ivec3 attribs = vertices[ix];
		// Extract attributes from lists (except color)
		glm::vec3 position = positions[attribs.x];
		glm::vec3 normal = normals[attribs.z];//add in normals and UVs into vertex data
		glm::vec2 uv = uvs[attribs.y];
		glm::vec4 color = glm::vec4(0.5f, 0.5f, 0.5f, 1.f);// Add the vertex to the mesh 
		vertexData.push_back(Vertex(position, normal, uv, color));
	}

	// Create a vertex buffer and load all our vertex data
	//VertexBuffer::Sptr vertexBuffer = VertexBuffer::Create();

	std::vector<float> pos = std::vector<float>();
	std::vector<float> norm = std::vector<float>();
	std::vector<float> uv = std::vector<float>();
	std::vector<float> col = std::vector<float>();

	for (int ix = 0; ix < vertexData.size(); ix++) {
		pos.push_back(vertexData[ix].pos.x);
		pos.push_back(vertexData[ix].pos.y);
		pos.push_back(vertexData[ix].pos.z);
		//std::cout << "P: " << vertexData[ix].pos.x << " " << vertexData[ix].pos.y << " " << vertexData[ix].pos.z << std::endl;
		trisToDraw++;
		norm.push_back(vertexData[ix].normal.x);
		norm.push_back(vertexData[ix].normal.y);
		norm.push_back(vertexData[ix].normal.z);
		//std::cout << "N: " << vertexData[ix].normal.x << " " << vertexData[ix].normal.y << " " << vertexData[ix].normal.z << std::endl;

		uv.push_back(vertexData[ix].uv.x);
		uv.push_back(vertexData[ix].uv.y);
		//std::cout << "UV: " << vertexData[ix].uv.x << " " << vertexData[ix].uv.y << " " << std::endl;

		col.push_back(vertexData[ix].colour.w);
		col.push_back(vertexData[ix].colour.x);
		col.push_back(vertexData[ix].colour.y);
		col.push_back(vertexData[ix].colour.z);
		//std::cout << "C: " << vertexData[ix].colour.w << " " << vertexData[ix].colour.x << " " << vertexData[ix].colour.y << " " << vertexData[ix].colour.z << std::endl;

	}

	/// LECTURE 05
	GLfloat cameraPos[] = { 0.0f, 0.0f, 3.0f };
	GLfloat lightPos[] = { 0.0f, 0.0f, 3.0f };
	////////////


	//VBO - Vertex buffer object
	GLuint pos_vbo = 0;
	glGenBuffers(1, &pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(float), &pos[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glBindBuffer(GL_ARRAY_BUFFER, pos_vbo);

	GLuint color_vbo = 1;
	glGenBuffers(1, &color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, color_vbo);
	glBufferData(GL_ARRAY_BUFFER, col.size() * sizeof(float), &col[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	//glBindBuffer(GL_ARRAY_BUFFER, color_vbo);
	
	

	//						index, size, type, normalize?, stride, pointer
	

	

	/////////// LECTURE 05
	GLuint normal_vbo = 2;
	glGenBuffers(1, &normal_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, normal_vbo);
	glBufferData(GL_ARRAY_BUFFER, norm.size() * sizeof(float), &norm[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	//////////////


	glEnableVertexAttribArray(0);//pos
	glEnableVertexAttribArray(1);//colors
	/// LEC 05
	glEnableVertexAttribArray(2);//normals
	////////////
	

	GLuint uv_vbo = 3;
	glGenBuffers(1, &uv_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, uv_vbo);
	glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(float), &uv[0], GL_STATIC_DRAW);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(3);//uv
	
	loadImage("chick.bmp");
	
	GLuint textureHandle;
	glGenTextures(1, &textureHandle);
	glBindTexture(GL_TEXTURE_2D, textureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//free image from RAM since its in VRAM
	stbi_image_free(image);

	// Load our shaders

	if (!loadShaders())
		return 1;

	////////// LECTURE 04 //////////////

	// Projection - FoV, aspect ratio, near, far
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	glm::mat4 Projection = glm::perspective(glm::radians(45.0f),
		(float)width / (float)height, 0.1f, 100.0f);

	// View matrix - Camera

	glm::mat4 View = glm::lookAt(
		glm::vec3(0, -1, 3), // camera position
		glm::vec3(0, 0, 0), //target
		glm::vec3(0, 1, 0) //up vector
	);

	// Model matrix
	glm::mat4 Model = glm::mat4(1.0f);//Identity matrix - resets your matrix

	glm::mat4 mvp;// = Projection * View * Model;

	// Handle for our mvp
	GLuint matrixMVP = glGetUniformLocation(shader_program, "MVP");

	////// LEC 05 - uniform variables
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
		Model = glm::mat4(1.0f); // reset Model

		keyboard();
		Transform t = sceneRegistry.get<Transform>(duck);
		t.addRotation(t.getQuaternion(glm::vec3(0.0f, 1.0f, 0.0f),glm::radians(rotY)/2));
		t.addRotation(t.getQuaternion(glm::vec3(1.0f, 0.0f, 0.0f),glm::radians(rotX)/2));
		

		Model = glm::rotate(Model, t.getAxisAngle(), t.getRotationAxis()) *
			glm::scale(Model, glm::vec3(0.4,0.4,0.4));
		mvp = Projection * View * Model;

		// Send mvp to GPU
		glUniformMatrix4fv(matrixMVP, 1, GL_FALSE, &mvp[0][0]);

		///////// LEC 05
		glUniformMatrix4fv(matrixModel, 1, GL_FALSE, &Model[0][0]);

		glUniform3fv(lightPosID, 1, &lightPos[0]);
		glUniform3fv(cameraPosID, 1, &cameraPos[0]);

		/////////////////

		glDrawArrays(GL_TRIANGLES, 0, trisToDraw*3); //36


		glfwSwapBuffers(window);
	}
	return 0;

}