#pragma once
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include"Vertex.h"
#include "RigidBody.h"
#include"Collision.h"



class Camera;
class Wall;

class Car : public RigidBody
{
public:
	float speed;
	float tireFriction;
	bool onGround;
	bool atStartLine;
	Model* carModel;

	Car(std::string texture, std::vector<Vertex> Vertices, std::vector<GLuint> Indices);
	Car() = default;

	void movement(GLFWwindow* window, float deltaTime);
	void setDirection(glm::vec3 rot, float angle);
	void applyFriction();
	void updateCarModel();
	std::vector<Contact> generateWallContacts(float restDistance, std::vector<int>& triangles, std::vector<Model>& collisionMap);
	std::vector<Contact> generateGroundContacts(float restDistance, std::vector<int>& triangles, std::vector<Model>& collisionMap);

private:
	float turnAngle = 4.0f;
	float spinAngle = 5;
	float drag;
	float angularDrag;
	Contact generateContact(glm::vec3 point, float restDistance, std::vector<int>& triangles, std::vector<Model>& collisionMap);
	void timer();
	
};