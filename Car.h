#pragma once
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include"Mesh.h"
#include"Vertex.h"
#include "RigidBody.h"

class Camera;
class Wall;

class Car : public Mesh, public RigidBody
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

	glm::vec3 frontLeft;
	glm::vec3 frontRight;
	glm::vec3 backLeft;
	glm::vec3 backRight;


	Car(Texture texture, Vertex p1, Vertex p2, Vertex p3, Vertex p4, Vertex p5, Vertex p6, Vertex p7, Vertex p8);
	Car() = default;

	using Mesh::draw;
	void draw(Shader& shaderProgram, Camera& camera);

	void update() override;

	glm::vec3 getPoint(int triangle, int point);
	glm::vec3 getPoint(int i);

	void movement(GLFWwindow* window);

	glm::vec3 getDirection();
	void setDirection(glm::vec3 rot, float angle);

private:
	float turnAngle = glm::radians(12.0f);
	float speed = 16.0f;
	
};