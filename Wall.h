#pragma once
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include"Mesh.h"
#include"Vertex.h"
#include"RigidBody.h"
#include"Car.h"

class Camera;
class Car;

class Wall : public Mesh, public RigidBody
{
public:
	Vertex p1;
	Vertex p2;
	Vertex p3;
	Vertex p4;


	Wall(Texture texture, Vertex p1, Vertex p2, Vertex p3, Vertex p4);
	Wall() = default;

	using Mesh::draw;
	void draw(Shader& shaderProgram, Camera& camera);
	void draw(Shader& shaderProgram, Camera& camera, glm::vec3 loc, glm::vec3 direction);

	void update() override;
	
	glm::vec3 getPoint(int triangle, int point);
	glm::vec3 getPoint(int i);
	void move(glm::vec3 location);
	
	void setDirection(glm::vec3 rot, float angle);
	
};