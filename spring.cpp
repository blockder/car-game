#include"spring.h"
#include"RigidBody.h"
#include"Math.h"

spring::spring(RigidBody* obj1, RigidBody* obj2, float length, float stiffness)
{
	this->length = length;
	this->stiffness = stiffness;
	this->obj1 = obj1;
	this->obj2 = obj2;
}

void spring::applyForce(glm::vec3 p1, glm::vec3 p2)
{
	glm::vec3 direction = glm::normalize(p1 - p2);
	float force = (length - glm::distance(p1, p2)) * stiffness / length; 
	if (obj1 != NULL)
		obj1->addForce(p1, direction * force);
	if (obj2 != NULL)
		obj2->addForce(p2, -direction * force);
}