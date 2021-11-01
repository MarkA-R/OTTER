#include "MaterialCreator.h"

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

void MaterialCreator::createMaterial(std::string meshName, std::string textureName, ShaderProgram& shader) {
	mesh = std::make_unique<Mesh>();
	GLTF::LoadMesh(meshName, *mesh);
	texture = std::make_unique<Texture2D>(textureName);
	material = std::make_unique<Material>(shader);
	material->AddTexture("albedo", *texture);
}
