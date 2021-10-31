#include "glHeaders.h"
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include "MeshRenderer.h"
#include<stb_image.h>

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

void MeshRenderer::loadOBJ(std::string filename, VBO& v)
{
	trisToDraw = 0;
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
	//std::cout << v.pos << std::endl;
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &v.pos);
	glBindBuffer(GL_ARRAY_BUFFER, v.pos);
	glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(float), &pos[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glBindBuffer(GL_ARRAY_BUFFER, pos_vbo);

	
	glGenBuffers(1, &v.colour);
	glBindBuffer(GL_ARRAY_BUFFER, v.colour);
	glBufferData(GL_ARRAY_BUFFER, col.size() * sizeof(float), &col[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	//glBindBuffer(GL_ARRAY_BUFFER, color_vbo);



	//						index, size, type, normalize?, stride, pointer




	/////////// LECTURE 05
	
	glGenBuffers(1, &v.norm);
	glBindBuffer(GL_ARRAY_BUFFER, v.norm);
	glBufferData(GL_ARRAY_BUFFER, norm.size() * sizeof(float), &norm[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	//////////////


	glEnableVertexAttribArray(0);//pos
	glEnableVertexAttribArray(1);//colors
	/// LEC 05
	glEnableVertexAttribArray(2);//normals
	////////////


	
	glGenBuffers(1, &v.uv);
	glBindBuffer(GL_ARRAY_BUFFER, v.uv);
	glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(float), &uv[0], GL_STATIC_DRAW);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(3);//uv

	//glBindVertexArray(vao);

	//glBindVertexArray(_handle);
	trisToDraw *= 3;
	//matrixModel = &v.model;
}

void MeshRenderer::loadTexture(Texture t)
{
	

	//GLuint textureHandle;
	
	glGenTextures(1, t.getuint());
	glBindTexture(GL_TEXTURE_2D, *t.getuint());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, t.getTexture());


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//free image from RAM since its in VRAM
	stbi_image_free(t.getTexture());
}

void MeshRenderer::draw(Transform* owner, GLuint mm)
{
	//std::cout << matrixModel << std::endl;
	glBindVertexArray(vao);
	glUniformMatrix4fv(matrixModel, 1, GL_FALSE, &owner->getMatrix()[0][0]);//for mesh renderer
	glDrawArrays(GL_TRIANGLES, 0, trisToDraw);
}

int MeshRenderer::getTrisToDraw()
{
	return trisToDraw;
}

GLuint MeshRenderer::setMatrixModel(GLuint shader, std::string l)
{
	
	matrixModel = glGetUniformLocation(shader, l.c_str());
	return matrixModel;
}
