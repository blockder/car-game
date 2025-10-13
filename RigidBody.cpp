#pragma once
#include"RigidBody.h"
#include<glm/gtc/type_ptr.hpp>
#include"Math.h"
#include<iostream>

RigidBody::RigidBody()
{
	//default settings
	this->inverseMass = 0;
	this->inverseInersia = glm::mat3(0.0f);
	this->center = glm::vec3(0.0f);
	this->rotation = Quaternion(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));

	this->velocity = glm::vec3(0.0f);
	this->angularVelocity = glm::vec3(0.0f);

	clearForces();
}
void RigidBody::setPhysics(float mass, std::vector<glm::vec3> points)
{
	if (mass != 0)
	{
	this->inverseMass = 1 / mass;
	this->inverseInersia = setInersia(points);
	}
}
void RigidBody::clearForces()
{
	this->netForce = glm::vec3(0.0f);
	this->torque = glm::vec3(0.0f);
	this->netImpulse = glm::vec3(0.0f);
	this->angularImpulse = glm::vec3(0.0f);
}
void RigidBody::addForce(glm::vec3 force)
{
	this->netForce += force;
}
void RigidBody::addForce(glm::vec3 point, glm::vec3 force)
{
	addForce(force);
	this->torque += glm::cross(point - center, force);
}
void RigidBody::addImpulse(glm::vec3 point, glm::vec3 impulse)
{
	this->netImpulse += impulse ;
	this->angularImpulse += cross(point - center, impulse);
	//this->velocity += inverseMass * impulse;
	//this->angularImpulse += getInverseInersia() * cross(point - center, impulse);
}
void RigidBody::push(glm::vec3 point, glm::vec3 vec)
{
	this->center += inverseMass * vec;
	Quaternion nextRot((rotation * inverseInersia) * glm::cross(point - center, vec));
	this->rotation = rotation * nextRot;
	
}
glm::vec3 RigidBody::worldSpace(glm::vec3 point)
{
	//return movement * glm::vec4(rotation * point, 1.0f);
	return (rotation * point) + center;
}
glm::vec3 RigidBody::getVelocity(glm::vec3 point)
{
	//in world-sapce
	return velocity + glm::cross(angularVelocity, point-center);
}
glm::vec3 RigidBody::impulseImpact(glm::vec3 point, glm::vec3 impulse)
{
	//glm::vec3 unitImpulse = glm::normalize(impulse);
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

void RigidBody::Integrate(float t)
{
	//Integrate velocities
	this->velocity = velocity * drag + inverseMass * netForce * t;
	this->angularVelocity = angularVelocity * angularDrag + getInverseInersia() * torque * t;
	//apply impulses
	this->velocity += inverseMass * netImpulse;
	this->angularVelocity += getInverseInersia() * angularImpulse;
	
	//Intergrate positions
	Quaternion nextRot(angularVelocity * 0.5f);
	this->center += velocity * t;
	this->rotation = nextRot * rotation;

	//update matrix
	movement = glm::translate(glm::mat4(1.0f), center);
	rotationMatrix = rotation.getMatrix();
	
	//reset forces for next update
	clearForces();
}