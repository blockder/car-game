#pragma once
#include"RigidBody.h"
#include<glm/gtc/type_ptr.hpp>
#include"Math.h"
#include<iostream>
#include <iomanip>

RigidBody::RigidBody(Model model)
{
	this->model = model;

	//default settings
	inverseMass = 0;
	inverseInersia = glm::mat3(0.0f);

	center = glm::vec3(0.0f);
	rotation = Quaternion(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));

	velocity = glm::vec3(0.0f);
	angularVelocity = glm::vec3(0.0f);

	acceleration = glm::vec3(0);
	angularAcceleration = glm::vec3(0);

	clearForces();
}
void RigidBody::setMass(float mass)
{
	if (mass != 0)
	{
		this->inverseMass = 1 / mass;
		this->inverseInersia = setInersia(model.points);
	}
}
void RigidBody::clearForces()
{
	this->netForce = glm::vec3(0.0f);
	this->netTorque = glm::vec3(0.0f);
}
void RigidBody::addForce(glm::vec3 force)
{
	this->netForce += force;
}
void RigidBody::addForce(glm::vec3 point, glm::vec3 force)
{
	addForce(force);
	this->netTorque += glm::cross(point - center, force);
}
void RigidBody::applyImpulse(glm::vec3 point, glm::vec3 impulse)
{
	this->velocity += inverseMass * impulse;
	this->angularVelocity += getInverseInersia() * cross(point - center, impulse);
}
void RigidBody::addTorque(glm::vec3 torque)
{
	this->netTorque += torque;
}

glm::vec3 RigidBody::worldSpace(glm::vec3 point)
{
	return (rotation * point) + center ;
}
glm::vec3 RigidBody::getVelocity(glm::vec3 point)
{
	//in world-sapce
	return velocity  + glm::cross(angularVelocity , point-center);
}

glm::vec3 RigidBody::impulseImpact(glm::vec3 point, glm::vec3 impulse)
{
	glm::vec3 resAngular = glm::cross(getInverseInersia() * glm::cross(point - center, impulse), point - center);
	return  (impulse * inverseMass) + resAngular;
}
glm::mat3 RigidBody::setInersia(std::vector<glm::vec3> points)
{
	glm::mat3 inInersia;
	//in local space
	glm::vec3 X(1,0,0);
	glm::vec3 Y(0, 1, 0);
	glm::vec3 Z(0, 0, 1);
	//calculate inersia in all 3 axis
	float Ix = 0, Iy = 0, Iz = 0, Ixy = 0, Ixz = 0, Iyz = 0;
	for (int i = 0;i < points.size(); i++)
	{
		Ix += points[i].y * points[i].y + points[i].z * points[i].z;
		Iy += points[i].x * points[i].x + points[i].z * points[i].z;
		Iz += points[i].y * points[i].y + points[i].x * points[i].x;

		Ixy += points[i].x * points[i].y;
		Ixz += points[i].x * points[i].z;
		Iyz += points[i].y * points[i].z;
	}
	inInersia[0] = glm::vec3(Ix, -Ixy, -Ixz);
	inInersia[1] = glm::vec3(-Ixy, Iy, -Iyz);
	inInersia[2] = glm::vec3(-Ixz, -Iyz, Iz);
	return  glm::inverse(inInersia);
}
glm::mat3 RigidBody::getInverseInersia()
{
	return rotation * (rotation.inverse() * inverseInersia);
}

void RigidBody::Integrate(float deltaTime)
{
	//update inersia according to rotation
	inverseInersia = getInverseInersia();

	//Integrate acceleration
	this->acceleration = netForce * inverseMass;
	this->angularAcceleration = inverseInersia * netTorque ;
	
	//Integrate velocities
	this->velocity = velocity + acceleration * deltaTime;
	this->angularVelocity = angularVelocity + angularAcceleration * deltaTime;

	//Intergrate positions
	Quaternion nextRot(angularVelocity * deltaTime);
	this->center += velocity * deltaTime;
	this->rotation = nextRot * rotation;
	
	
	//update matrix
	model.movement = glm::translate(glm::mat4(1.0f), center );
	model.rotationMatrix = rotation.getMatrix();
	
	//reset forces for next update
	clearForces();
}