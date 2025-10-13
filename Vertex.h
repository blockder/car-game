#pragma once
#include<glm/glm.hpp>

class Vertex
{
public:
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 texCoord;

	Vertex(glm::vec3 position, glm::vec3 color, glm::vec2 texCoord);
	Vertex();
	Vertex(const Vertex& other);
};