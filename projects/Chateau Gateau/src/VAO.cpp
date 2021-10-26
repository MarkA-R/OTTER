#include "VAO.h"
#include "Vertex.h"
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
// trim from start (in place)
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

VAO::VAO()
{
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindBuffer(GL_ARRAY_BUFFER, vao);
	trisToDraw = 0;
}

void VAO::loadData(std::string filename)
{
	// Open our file in binary mode
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
		if (command == "#")
		{
			std::getline(file, line);
		}// The v command defines a vertex's position

		else if (command == "v") {
			// Read in and store a position   
			file >> vecData.x >> vecData.y >> vecData.z;
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
				// OBJ format uses 1-based indices 
				vertexIndices -= glm::ivec3(1);
				// add the vertex indices to the list
				// NOTE: This will create duplicate vertices! 
				vertices.push_back(vertexIndices);
			}
		}
		else if (command == "vn") {//if the command is vn, read the normal data and push it back into the normals vector
			file >> normData.x >> normData.y >> normData.z;
			normals.push_back(normData);
		}
		else if (command == "vt") {//if the command is vt, read the UV data and push it back into the UV vector
			file >> uvData.x >> uvData.y;
			uvs.push_back(uvData);
		}

	}

	// We will ignore the rest of the line for comment lines

	
	std::vector<Vertex> vertexData; for (int ix = 0; ix < vertices.size(); ix++) {
		glm::ivec3 attribs = vertices[ix];
		// Extract attributes from lists (except color)
		glm::vec3 position = positions[attribs.x];
		glm::vec3 normal = normals[attribs.x];//add in normals and UVs into vertex data
		glm::vec2 uv = uvs[attribs.x];
		glm::vec4 color = glm::vec4(1.0f);// Add the vertex to the mesh 
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
		trisToDraw++;
		norm.push_back(vertexData[ix].normal.x);
		norm.push_back(vertexData[ix].normal.y);
		norm.push_back(vertexData[ix].normal.z);
		uv.push_back(vertexData[ix].uv.x);
		uv.push_back(vertexData[ix].uv.y);
		col.push_back(vertexData[ix].colour.w);
		col.push_back(vertexData[ix].colour.x);
		col.push_back(vertexData[ix].colour.y);
		col.push_back(vertexData[ix].colour.z);
	}


	
	GLuint posVBO;
	glGenBuffers(1, &posVBO);
	glBindBuffer(GL_ARRAY_BUFFER, posVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos),&pos[0] , GL_STATIC_DRAW);
	glEnableVertexArrayAttrib(posVBO, 0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	

	GLuint normVBO;
	glGenBuffers(1, &normVBO);
	glBindBuffer(GL_ARRAY_BUFFER, normVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(norm), &norm[0], GL_STATIC_DRAW);
	glEnableVertexArrayAttrib(normVBO, 1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	

	GLuint uvVBO;
	glGenBuffers(1, &uvVBO);
	glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uv), &uv[0], GL_STATIC_DRAW);
	glEnableVertexArrayAttrib(uvVBO, 2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	

	GLuint colourVBO;
	glGenBuffers(1, &colourVBO);
	glBindBuffer(GL_ARRAY_BUFFER, colourVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos), &pos[0], GL_STATIC_DRAW);
	glEnableVertexArrayAttrib(colourVBO, 3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(0);//pos, norm, uv, colour
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	
		


		
	
}

void VAO::Draw() {
	glDrawArrays(GL_TRIANGLES, 0, trisToDraw);
}

