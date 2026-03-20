#pragma once
#include<glm/glm.hpp>
#include<vector>

class Block;
class RigidBody;
class Model;
struct Contact;

class Spring
{
public:
	glm::vec3 p1;
	glm::vec3 p2;
	float k;
	float restingLength;
	float dampingC;

	RigidBody* r1;
	RigidBody* r2;

	Spring() = default;
	Spring(float restingLength, float k, float dampingC, RigidBody* r1, RigidBody* r2);

	void setP1(Model* m1, int i1);
	void setP2(Model* m2, int i2);
	void setMap(std::vector<Block>* map);
	float getLength();

	void applyForce();
	bool makeContact(std::vector<Contact>& contacts);
	static bool onGround(Spring& f1, Spring& f2, Spring& b1, Spring& b2);
private:
	float maxSP;
	int i1;
	int i2;
	Model* m1;
	Model* m2;
	std::vector<Block>* map;
	glm::vec3 projectSpring( glm::vec3 p);
	void calculatePoints();
};
