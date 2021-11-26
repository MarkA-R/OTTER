#pragma once
#include <string>
#include "NOU/Mesh.h"
#include "NOU/CMeshRenderer.h"
#include "NOU/GLTFLoader.h"

#include "NOU/Material.h"
using namespace nou;
class MaterialCreator
{
public:
	MaterialCreator();
	void createMaterial(std::string, std::string, ShaderProgram&);
	void createMaterialOBJ(std::string, std::string, ShaderProgram&);
	const std::unique_ptr<Mesh>& getMesh();
	const std::unique_ptr<Texture2D>& getTexture();
	const std::unique_ptr<Material>& getMaterial();
	const std::unique_ptr<ShaderProgram>& getShader();
protected:
	std::unique_ptr<Mesh> mesh;
	std::unique_ptr<Texture2D> texture;
	std::unique_ptr<Material> material;
	std::unique_ptr<ShaderProgram> shade;
};

