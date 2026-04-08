#pragma once
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include"Vertex.h"
#include "RigidBody.h"


class Camera;
class Wall;

class Car : public RigidBody
{
public:
	float speed;
	float tireFriction;
	bool onGround;
	Model* carModel;

	Car(std::string texture, std::vector<Vertex> Vertices, std::vector<GLuint> Indices);
	Car() = default;

	void movement(GLFWwindow* window, float deltaTime);

	void setDirection(glm::vec3 rot, float angle);

	void applyFriction();

	void updateCarModel();

private:
	float turnAngle = 4.0f;
	float spinAngle = 5;
	float drag;
	float angularDrag;
};