#pragma once
#include<glm/glm.hpp>
#include<vector>
#include<iostream>

class RigidBody;
class Model;
class Car;
class Block;

struct Contact
{
	RigidBody* body;
	RigidBody* other;
	glm::vec3 point;
	glm::vec3 normal;
	float penetration; 
	float accumulatedNormal;
	glm::vec3 accumulatedSide;

	Contact(RigidBody* body, RigidBody* other, glm::vec3 point, glm::vec3 normal, float penetration);

	float getClosingVelocity();

	void solveVelocity(float c, float u);

	static std::vector<Contact> createContacts(Model* m, RigidBody* r, RigidBody* other, std::vector<glm::vec3> points);
};

class Collision
{
public:
	static std::vector<RigidBody*> objects;

	static void collisionDetector();

	static void velocityResolver(std::vector<Contact>& contacts, float c, float u);

	static void penetrationResolver(std::vector<Contact>& contacts);

	static std::vector<Contact> getContacts(Car& car, Block& block);

private:
	static bool insideBlock(Car& car, Block& block, std::vector<Contact>& contacts);
	static bool insideCar(Car& car, Block& block, std::vector<Contact>& contacts);
	static bool Edges(Car& car, Block& block, std::vector<Contact>& contacts);
	static bool faceEdges(Car& car, Block& blockm, std::vector<Contact>& contacts);
	static glm::vec3 avaregeContact(std::vector<Contact>& contacts, glm::vec3 center);
	static bool insideCar(Car& car, glm::vec3 point);
	static bool insideBlock(Block& block, glm::vec3 point);
	static float getMaxPenetration(std::vector<Contact>& contacts);
	

};
