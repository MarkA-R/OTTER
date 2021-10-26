#include "Vertex.h"

Vertex::Vertex(glm::vec3 p, glm::vec3 n, glm::vec2 u, glm::vec4 c)
{
	 pos = p;
	 normal = n;
	uv = u;
	colour = c;
}
