#pragma once
#include<glm/glm.hpp>
#include<glad/glad.h>
#include"Quaternion.h"
#include<vector>
class RigidBody 
{
public:
	//body properties
	float inverseMass;
	glm::mat3 inverseInersia;
	//position
	glm::vec3 center;
	Quaternion rotation;
	//speed
	glm::vec3 velocity;
	glm::vec3 angularVelocity;
	//acceleration
	glm::vec3 acceleration;
	glm::vec3 angularAcceleration;
	//force
	glm::vec3 netForce;
	glm::vec3 netTorque;

	RigidBody();

	void Integrate(float deltaTime);

	void addForce(glm::vec3 force);
	void addForce(glm::vec3 point, glm::vec3 force);
	void applyImpulse(glm::vec3 point, glm::vec3 impulse);
	void addTorque(glm::vec3 torque);
	glm::vec3 worldSpace(glm::vec3 point);
	glm::vec3 getVelocity(glm::vec3 point);
	glm::vec3 impulseImpact(glm::vec3 point, glm::vec3 impulse);
protected:
	glm::mat4 movement;
	glm::mat4 rotationMatrix;
	void clearForces();
	void setPhysics(float mass, std::vector<glm::vec3> points);

private:
	glm::mat3 setInersia(std::vector<glm::vec3> points);
	glm::mat3 getInverseInersia();
};