#include"Car.h"
#include"Camera.h"
#include"Shader.h"
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/vector_angle.hpp>
#include<vector>
#include"Math.h"
#include"Quaternion.h"



Car::Car(std::string texture, std::vector<Vertex> Vertices, std::vector<GLuint> Indices) 
	: RigidBody(Model(texture, Vertices, Indices))
{
	speed = 10;
	tireFriction = 10;
	drag = 0.99;
	angularDrag = 20;
	onGround = false;
	//set physics
	RigidBody::setMass(2);
}



void Car::movement(GLFWwindow* window, float deltaTime)
{
	
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		return;
	
	//air mode
	if (!onGround)
	{
		angularVelocity *= 0.95;
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			glm::vec3 spin = rotation * glm::vec3(1, 0, 0) * -spinAngle;
			Quaternion nextRot(spin * deltaTime);
			rotation = nextRot * rotation;
		}
		return;
	}
	//ground mode
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		addForce(rotation * glm::vec3(0,0,1) * speed);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		addForce(rotation * glm::vec3(0, 0, 1) * -speed);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		angularVelocity *= 0;
		glm::vec3 right = rotation * glm::vec3(0, 1, 0) * turnAngle;
		Quaternion nextRot(right * deltaTime);
		rotation = nextRot * rotation;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		angularVelocity *= 0;
		glm::vec3 left = rotation * glm::vec3(0, 1, 0) * -turnAngle;
		Quaternion nextRot(left * deltaTime);
		rotation = nextRot * rotation;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		speed = 50;
		tireFriction = 5;
	}
	else
	{
		speed = 25;
		tireFriction = 15;
	}
	
	
}

void Car::applyFriction()
{
	if (!onGround)
		return;

	//drag
	velocity *= drag;

	//angular drag
	glm::vec3 up = rotation * glm::vec3(0, 1, 0);
	glm::vec3 upRot = up * glm::dot(up, angularVelocity);
	if (glm::length(upRot) > 0)
		addTorque(angularDrag * glm::normalize(-upRot) * inverseInersia);
	

	//tire friction
	glm::vec3 sideDirection = rotation * glm::vec3(1, 0, 0);
	glm::vec3 sideVelocity = sideDirection * glm::dot(sideDirection, velocity);

	if (glm::length(sideVelocity) > 0.01f)
		addForce(glm::normalize(-sideVelocity) / inverseMass * tireFriction);
}


void Car::setDirection(glm::vec3 rot, float angle)
{
	rotation = Quaternion(rot, angle);
	model.rotationMatrix = rotation.getMatrix();
}
