#include"Vertex.h"

Vertex::Vertex(glm::vec3 position, glm::vec3 color, glm::vec2 texCoord)
{
	this->position = position;
	this->color = color;
	this->texCoord = texCoord;
}
Vertex::Vertex()
{
	this->position = glm::vec3(0.0f);
	this->color = glm::vec3(1.0f);
	this->texCoord = glm::vec2(0.0f);
}

Vertex::Vertex(const Vertex& other)
{
	this->position = other.position;
	this->color = other.color;
	this->texCoord = other.texCoord;
}