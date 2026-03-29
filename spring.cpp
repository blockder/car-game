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
	map = NULL;
}
void Spring::setP1(int i1)
{
	this->i1 = i1;
}
void Spring::setP2(int i2)
{
	this->i2 = i2;
}
void Spring::setMap(QuadTree* map, Model* mapModel)
{
	this->map = map;
	this->mapModel = mapModel;
}
float Spring::getLength()
{
	return glm::distance(p1, p2);
}
glm::vec3 Spring::projectSpring( glm::vec3 p)
{
	glm::vec3 projectedPoint = p + glm::vec3(0,-1,0) * (restingLength + 0.1f);
	float minDistance = restingLength;

	//make hitbox of r1
	std::vector<glm::vec3> wsPoints;
	for (auto& point : r1->model.points)
	{
		wsPoints.push_back(r1->worldSpace(point));
	}
	HitBox r1Hitbox = HitBox(wsPoints);

	HitBox zone = HitBox(glm::vec3(-50), 100, 100, 100);
	
	std::vector<HitBox*> triangles = map->getData(r1Hitbox);
	//std::cout << "triangles count " << triangles.size() << std::endl;
	for (auto hitbox : triangles)
	{
		
		int i = hitbox->pointer;
		
		glm::vec3 t1 = mapModel->getTrianglePoint(i, 0);
		glm::vec3 t2 = mapModel->getTrianglePoint(i, 1);
		glm::vec3 t3 = mapModel->getTrianglePoint(i, 2);
		glm::vec3 normal = Math::getNormal(t1, t2, t3);

		//skip if no apt intersection
		float distanceTriangle = Math::distanceTrianglePoint(t1, t2, t3, p);
		if (distanceTriangle <= 0.01f && distanceTriangle > -3.0f)
			continue;

		glm::vec3 susP = Math::triangleIntersection(t1, t2, t3, normal, p);

		if (glm::length(susP) > 0 && glm::distance(susP, p) < minDistance)
		{
			minDistance = glm::distance(susP, p);
			projectedPoint = susP;
		}
		
	}

	return projectedPoint;
}
void Spring::calculatePoints()
{
	//project p2
	if (i1 > -1 && i2 < 0)
	{
		p1 = r1->worldSpace(r1->model.points[i1]);
		p2 = projectSpring(p1);
		return;
	}

	//project p1
	if (i2 > -1 && i1 < 0)
	{
		p2 = r2->worldSpace(r1->model.points[i2]);
		p1 = projectSpring(p2);
		return;
	}

	//no projection calculate both points
	if (i1 > -1 && i2 > -1)
	{
		p1 = r1->worldSpace(r1->model.points[i1]);
		p2 = r2->worldSpace(r2->model.points[i2]);
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
