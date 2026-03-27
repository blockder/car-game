#pragma once
#include<glm/glm.hpp>
#include<vector>
#include<iostream>

class RigidBody;
class Model;


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

	static std::vector<Contact> createContacts(RigidBody* body, RigidBody* other, std::vector<glm::vec3> points);
};

class Collision
{
public:
	static std::vector<RigidBody*> objects;

	static void collisionDetector();

	static void velocityResolver(std::vector<Contact>& contacts, float c, float u);

	static void penetrationResolver(std::vector<Contact>& contacts);

	static std::vector<Contact> getContacts(RigidBody& r1, RigidBody& block);

private:
	static bool insideBody(RigidBody& body, RigidBody& other, std::vector<Contact>& contacts);
	static bool faceEdges(RigidBody& body, RigidBody& block, std::vector<Contact>& contacts);
	static float getMaxPenetration(std::vector<Contact>& contacts);
	

};
