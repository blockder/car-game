#include"Spring.h"
#include"Math.h"
#include"Block.h"
#include"Model.h"
#include "RigidBody.h"
#include"Collision.h"


Spring::Spring(float restingLength, float k, float dampingC, RigidBody* r1, RigidBody* r2)
{
	this->restingLength = restingLength;
	this->k = k;
	this->dampingC = dampingC;
	this->r1 = r1;
	this->r2 = r2;
	p1 = glm::vec3(0);
	p2 = glm::vec3(0);
	i1 = -1;
	i2 = -1;
	m1 = NULL;
	m2 = NULL;
	map = NULL;
}
void Spring::setP1(Model* m1, int i1)
{
	this->m1 = m1;
	this->i1 = i1;
}
void Spring::setP2(Model* m2, int i2)
{
	this->m2 = m2;
	this->i2 = i2;
}
void Spring::setMap(std::vector<Block>* map)
{
	this->map = map;
}
float Spring::getLength()
{
	return glm::distance(p1, p2);
}
glm::vec3 Spring::projectSpring( glm::vec3 p)
{
	glm::vec3 projectedPoint = p + glm::vec3(0,-1,0) * (restingLength + 0.1f);
	float minDistance = restingLength;
	for (auto block : *map)
	{
		for (int i = 0; i < block.Indices.size() / 3; i++)
		{
			glm::vec3 t1 = block.worldSpace(block.getPoint(i, 0));
			glm::vec3 t2 = block.worldSpace(block.getPoint(i, 1));
			glm::vec3 t3 = block.worldSpace(block.getPoint(i, 2));
			glm::vec3 normal = Math::getNormal(t1, t2, t3);

			glm::vec3 susP = Math::triangleIntersection(t1, t2, t3, normal, p);

			//skip if no apt intersection
			if (glm::length(susP) == 0 || Math::distanceTrianglePoint(t1, t2, t3, p) <= 0.01f)
				continue;

			if (glm::distance(susP, p) < minDistance)
			{
				minDistance = glm::distance(susP, p);
				projectedPoint = susP;
			}
		}
		
	}
	return projectedPoint;
}
void Spring::calculatePoints()
{
	//project p2
	if (i1 > -1 && i2 < 0)
	{
		p1 = r1->worldSpace(m1->points[i1]);
		p2 = projectSpring( p1);
		return;
	}

	//project p1
	if (i2 > -1 && i1 < 0)
	{
		p2 = r2->worldSpace(m2->points[i2]);
		p1 = projectSpring( p2);
		return;
	}

	//no projection calculate both points
	if (i1 > -1 && i2 > -1)
	{
		p1 = r1->worldSpace(m1->points[i1]);
		p2 = r2->worldSpace(m2->points[i2]);
		return;
	}
	return;
}

void Spring::applyForce()
{
	//calculate points first
	calculatePoints();

	glm::vec3 direction = glm::normalize(p1 - p2);
	
	//claculate force
	float magnitude = glm::max( - k * (getLength() - restingLength), 0.0f);

	//return if there is no force
	if (magnitude == 0)
		return;
	

	//apply to each object
	if (r1 != NULL)
	{
		float closingVelocity = glm::dot(r1->getVelocity(p1), direction);
		
		//add dampning
		float damping =  dampingC * closingVelocity / r1->inverseMass;
		 r1->addForce(p1, direction * (magnitude - damping));

	}
	if (r2 != NULL)
	{
		//add dampning
		float damping = dampingC * glm::dot(r2->getVelocity(p2), -direction) / r2->inverseMass;

		r2->addForce(p2, -direction * (magnitude - damping));
	}
}

bool Spring::makeContact(std::vector<Contact>& contacts)
{	
	//return if spring not compressed
	if (getLength() > restingLength)
		return false;

	if (r1 != NULL) 
	{
		contacts.push_back(Contact(r1, new RigidBody(), p1, glm::normalize(p2 - p1), 0));
		return true;
	}

	if (r2 != NULL)
	{
		contacts.push_back(Contact(r2, new RigidBody(), p2, glm::normalize(p1 - p2), 0));
		return true;
	}
}


bool Spring::onGround(Spring& f1, Spring& f2, Spring& b1, Spring& b2)
{
	return f1.getLength() < f1.restingLength || f2.getLength() < f2.restingLength
		|| b1.getLength() < b1.restingLength || b2.getLength() < b2.restingLength;
}
