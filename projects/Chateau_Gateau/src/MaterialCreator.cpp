#include "MaterialCreator.h"
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

struct Vertex {
	Vertex(glm::vec3, glm::vec3, glm::vec2, glm::vec4);
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec4 colour;
};

Vertex::Vertex(glm::vec3 p, glm::vec3 n, glm::vec2 u, glm::vec4 c) {
	pos = p;
	normal = n;
	uv = u;
	colour = c;
}
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
MaterialCreator::MaterialCreator()
{
	
}

const std::unique_ptr<Mesh>& MaterialCreator::getMesh()
{
	return mesh;
}

const std::unique_ptr<Texture2D>& MaterialCreator::getTexture()
{
	return texture;
}

const std::unique_ptr<Material>& MaterialCreator::getMaterial()
{
	return material;
}
 
const std::unique_ptr<ShaderProgram>& MaterialCreator::getShader() {
	return shade;
}

void MaterialCreator::createMaterial(std::string meshName, std::string textureName, ShaderProgram& shader) {
	mesh = std::make_unique<Mesh>();
	GLTF::LoadMesh(meshName, *mesh);
	texture = std::make_unique<Texture2D>(textureName);
	material = std::make_unique<Material>(shader);
	shade = std::make_unique<ShaderProgram>(shader);
	material->AddTexture("albedo", *texture);
	
}

void MaterialCreator::createMaterial(Mesh& m, Texture2D& t, ShaderProgram& s)
{
	mesh = std::make_unique<Mesh>();
	mesh.get()->SetVerts(m.getVerts());
	mesh.get()->SetNormals(m.getNormals());
	mesh.get()->SetUVs(m.getUVS());
	//mesh = m;
	texture = std::make_unique<Texture2D>(t);
	material = std::make_unique<Material>(s);
	shade = std::make_unique<ShaderProgram>(s);
	material->AddTexture("albedo", *texture);
}



void MaterialCreator::createMaterialOBJ(std::string filename, std::string textureName, ShaderProgram& shader)
{
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

	std::vector<glm::vec3> pos = std::vector<glm::vec3>();
	std::vector<glm::vec3> norm = std::vector<glm::vec3>();
	std::vector<glm::vec2> uv = std::vector<glm::vec2>();
	std::vector<glm::vec4> col = std::vector<glm::vec4>();

	for (int ix = 0; ix < vertexData.size(); ix++) {
		pos.push_back(vertexData[ix].pos);
		
		//std::cout << "P: " << vertexData[ix].pos.x << " " << vertexData[ix].pos.y << " " << vertexData[ix].pos.z << std::endl;
		
		norm.push_back(vertexData[ix].normal);
		
		//std::cout << "N: " << vertexData[ix].normal.x << " " << vertexData[ix].normal.y << " " << vertexData[ix].normal.z << std::endl;

		uv.push_back(vertexData[ix].uv);
	
		//std::cout << "UV: " << vertexData[ix].uv.x << " " << vertexData[ix].uv.y << " " << std::endl;

		col.push_back(vertexData[ix].colour);
	
		//std::cout << "C: " << vertexData[ix].colour.w << " " << vertexData[ix].colour.x << " " << vertexData[ix].colour.y << " " << vertexData[ix].colour.z << std::endl;

	}

	mesh = std::make_unique<Mesh>();
	//GLTF::LoadMesh(meshName, *mesh);
	mesh.get()->SetVerts(pos);
	mesh.get()->SetNormals(norm);
	mesh.get()->SetUVs(uv);
	texture = std::make_unique<Texture2D>(textureName);
	material = std::make_unique<Material>(shader);
	shade = std::make_unique<ShaderProgram>(shader);
	material->AddTexture("albedo", *texture);
}
