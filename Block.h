#pragma once
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include"Vertex.h"
#include"RigidBody.h"

class Camera;

class Block : public RigidBody
{
public:
	Block(std::string texture, std::vector<Vertex> Vertices, std::vector<GLuint> Indices, glm::vec3 location, float width, float height, float depth);
	Block(Model& model ,glm::vec3 location);
	Block() = default;
	
	void setDirection(glm::vec3 rot, float angle);

};