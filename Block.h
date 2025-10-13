#pragma once
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include"Mesh.h"
#include"Vertex.h"
#include"RigidBody.h"

class Camera;

class Block : public Mesh, public RigidBody
{
public:
	Vertex p1;
	Vertex p2;
	Vertex p3;
	Vertex p4;
	Vertex p5;
	Vertex p6;
	Vertex p7;
	Vertex p8;

	Block(Texture texture, glm::vec3 location, float width, float height, float depth);
	Block() = default;

	using Mesh::draw;
	void draw(Shader& shaderProgram, Camera& camera);

	void update() override;

	glm::vec3 getPoint(int triangle, int point);
	glm::vec3 getPoint(int i);
	
	void setDirection(glm::vec3 rot, float angle);

};