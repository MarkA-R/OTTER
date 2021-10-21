#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <filesystem>
#include <json.hpp>
#include <fstream>
#include <entt.hpp>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <Camera.h>

/*
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexArrayObject.h"
#include "Shader.h"
#include "Camera.h"

#include "Utils/ObjLoader.h"
#include "VertexTypes.h"
*/

int main() { 
	entt::registry registry;
	entt::entity entity = registry.create();
	registry.emplace<Camera>(entity);
	
	return 0;
	
} 
