#pragma once
#include<glm/glm.hpp>
#include<vector>
#include<iostream>

class RigidBody;
class Cube;
class Wall;
class Car;
class Block;

struct Contact
{
	RigidBody* body;
	RigidBody* other;
	glm::vec3 point;
	glm::vec3 normal;
	float penetration;

	Contact(RigidBody* body, RigidBody* other, glm::vec3 point, glm::vec3 normal, float penetration);

	void solveVelocity(float c, float u);
};

class Collision
{
public:
	static std::vector<RigidBody*> objects;

	static void collisionDetector();

	static void velocityResolver(std::vector<Contact>& contacts, float c, float u);

	static void penetrationResolver(std::vector<Contact>& contacts);

	static std::vector<Contact> getContacts(Cube& cube, Wall& wall);

	static std::vector<Contact> getContacts(Car& car, Wall& wall);

	static std::vector<Contact> getContacts(Car& car, Block& block);

	static glm::vec3 wheelContact(glm::vec3 wheel,glm::vec3 dir, Block& block);
	
};
