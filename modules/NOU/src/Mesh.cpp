/*
NOU Framework - Created for INFR 2310 at Ontario Tech.
(c) Samantha Stahlke 2020

Mesh.cpp
Very basic class for managing the data associated with a 3D model.
*/

#include "NOU/Mesh.h"

namespace nou
{
	void Mesh::SetVerts(const std::vector<glm::vec3>& verts)
	{
		m_verts = verts;
		SetVBO(Attrib::POSITION, 3, m_verts);
	}

	void Mesh::SetNormals(const std::vector<glm::vec3>& normals)
	{
		m_normals = normals;
		SetVBO(Attrib::NORMAL, 3, m_normals);			
	}

	void Mesh::SetUVs(const std::vector<glm::vec2>& uvs)
	{
		m_uvs = uvs;
		SetVBO(Attrib::UV, 2, m_uvs);
	}

	void Mesh::setTangentAndBitangents()
	{
		//m_uvs = uvs;
		//SetVBO(Attrib::UV, 2, m_uvs);
		
		for (int i = 0; i < m_verts.size() - 3; i++) {
			glm::vec3 Edge1 = m_verts[i+1] - m_verts[i];
			glm::vec3 Edge2 = m_verts[i + 2] - m_verts[i];
			glm::vec2 Edge1uv = m_uvs[i + 1] - m_uvs[i];
			glm::vec2 Edge2uv = m_uvs[i + 2] - m_uvs[i];

			float cp = Edge1uv.y * Edge2uv.x - Edge1uv.x * Edge2uv.y;

			if (cp != 0.0f) {
				float mul = 1.0f / cp;
				glm::vec3 tangent = (Edge1 * -Edge2uv.y + Edge2 * Edge1uv.y) * mul;
				glm::vec3 bitangent = (Edge1 * -Edge2uv.x + Edge2 * Edge1uv.x) * mul;

				glm::normalize(tangent);
				glm::normalize(bitangent);
				m_tangents.push_back(tangent);
				m_bitangents.push_back(bitangent);
			}
			m_tangents.push_back(glm::vec3(0));
			m_bitangents.push_back(glm::vec3(0));
		}

		SetVBO(Attrib::TANGENT, 3, m_tangents);
		SetVBO(Attrib::BITANGENT, 3, m_bitangents);
	}

	std::vector<glm::vec3> Mesh::getVerts() {
		return m_verts;
	}
	std::vector<glm::vec3> Mesh::getNormals() {
		return m_normals;
	}
	std::vector<glm::vec2> Mesh::getUVS() {
		return m_uvs;
	}
	const VertexBuffer* Mesh::GetVBO(Mesh::Attrib attrib) const
	{
		auto it = m_vbo.find(attrib);

		if (it == m_vbo.end())
			return nullptr;

		return it->second.get();
	}
}