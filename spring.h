#pragma once
#include<glm/glm.hpp>
#include<vector>
#include<iostream>
class RigidBody;

class spring
{
public:
	float length;
	float stiffness;

	spring(RigidBody* obj1, RigidBody* obj2, float length, float stiffness);
	spring() = default;

	void applyForce(glm::vec3 p1, glm::vec3 p2);

private:
	RigidBody* obj1;
	RigidBody* obj2;
};