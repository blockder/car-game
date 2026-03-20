#pragma once
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include"Model.h"
#include"Vertex.h"
#include"RigidBody.h"

class Camera;

class Block : public Model, public RigidBody
{
public:


	Block(std::string texture, std::vector<Vertex> Vertices, std::vector<GLuint> Indices, glm::vec3 location, float width, float height, float depth);
	Block(std::string texture, std::vector<Vertex> Vertices, std::vector<GLuint> Indices, std::vector<glm::vec3> points ,glm::vec3 location);
	Block() = default;

	using Model::draw;
	void draw(Shader& shaderProgram, Camera& camera);
	
	void setDirection(glm::vec3 rot, float angle);

};