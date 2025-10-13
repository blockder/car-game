#include"Collision.h"
#include"RigidBody.h"
#include "Wall.h"
#include"Cube.h"
#include"Math.h"
#include"Car.h"
#include"Block.h"
#include<glm/gtc/type_ptr.hpp>


void Collision::collisionDetector()
{

}
void Collision::velocityResolver(std::vector<Contact>& contacts, float c, float u)
{
	for (int i = 0; i < contacts.size(); i++)
	{
		contacts[i].solveVelocity(c, u);
	}	
}
void Collision::penetrationResolver(std::vector<Contact>& contacts)
{
	for (int i = 0; i < contacts.size(); i++)
	{
		Contact& contact = contacts[i];
		glm::vec3 point = contact.point;
		glm::vec3 normal = contact.normal;
		float penetration = contact.penetration;
		if (penetration < 0)
			return;
		float adjustAmount = penetration / (contact.body->inverseMass + contact.other->inverseMass);
		contact.body->center +=  normal * adjustAmount * contact.body->inverseMass;
		contact.other->center += -normal * adjustAmount * contact.other->inverseMass;
	}
}
std::vector<Contact> Collision::getContacts(Cube& cube, Wall& wall)
{
	//check for points inside wall
	float closestContact = 999;
	std::vector<Contact> contacts;

	for (int i = 0; i < wall.EBOsize / 3; i++)
	{
		std::vector<Contact> innerContacts;
		float deepestPen = 0;
		glm::vec3 p1 = wall.worldSpace(wall.getPoint(i, 0));
		glm::vec3 p2 = wall.worldSpace(wall.getPoint(i, 1));
		glm::vec3 p3 = wall.worldSpace(wall.getPoint(i, 2));
		glm::vec3 normal = Math::getNormal(p1, p2, p3);

		for (int k = 0; k < 8; k++)
		{
			glm::vec3 point = cube.worldSpace(cube.getPoint(k));
			if (Math::insidePlane(p1, p2, p3, point))
			{
				float penetration = -Math::distanceTrianglePoint(p1, p2, p3, point);
				deepestPen = std::max(penetration, deepestPen);
				innerContacts.push_back(Contact(&cube, &wall, point, normal, penetration));
			}
		}
		//if one face is not collising then no contact
		if (innerContacts.size() == 0)
			return innerContacts;
		//update closest contacts
		if (deepestPen < closestContact)
		{
			contacts = innerContacts;
			closestContact = deepestPen;
		}
		
	} 
	//check for points inside cube

	for (int i = 0; i < cube.EBOsize / 3; i++)
	{
		std::vector<Contact> innerContacts;
		float deepestPen = 0;
		glm::vec3 p1 = cube.worldSpace(cube.getPoint(i, 0));
		glm::vec3 p2 = cube.worldSpace(cube.getPoint(i, 1));
		glm::vec3 p3 = cube.worldSpace(cube.getPoint(i, 2));
		glm::vec3 normal = Math::getNormal(p1, p2, p3);

		for (int k = 0; k < 4; k++)
		{
			glm::vec3 point = wall.worldSpace(wall.getPoint(k));
			if (Math::insidePlane(p1, p2, p3, point))
			{
				float penetration = -Math::distanceTrianglePoint(p1, p2, p3, point);
				deepestPen = std::max(penetration, deepestPen);
				innerContacts.push_back(Contact(&wall, &cube, point, normal, penetration));
			}
		}
		//if one face is not collising then no contact
		if (innerContacts.size() == 0)
			return innerContacts;
		//update closest contacts
		if (deepestPen <= closestContact)
		{
			contacts = innerContacts;
			closestContact = deepestPen;
		}
	} 
	
	//avarege contacts
	float weights = 0;
	glm::vec3 avaregePoint(0.0f);
	for (int i = 0; i < contacts.size(); i++)
	{
		Contact& contact = contacts[i];
		float distanceVenter = glm::distance(contact.point, cube.center) - glm::dot(contact.normal, cube.center - contact.point);
		float inverseDistance = 1.0f / std::pow(distanceVenter, 3);
		weights += inverseDistance;
		avaregePoint += contact.point * inverseDistance;
	}
	avaregePoint = avaregePoint / weights;

	//if averenge close round it up
	if (contacts.size() > 1)
	{
		float flatDistanceCenter = std::pow(glm::distance(avaregePoint, cube.center), 2) - std::pow((glm::dot(contacts[0].normal, cube.center - avaregePoint)), 2);
		if (flatDistanceCenter < 0.01f)
		{
			avaregePoint = cube.center - contacts[0].normal * glm::dot(contacts[0].normal, cube.center - avaregePoint);
		}
	}
	
	std::vector<Contact> newContact;
	newContact.push_back(Contact(contacts[0].body, contacts[0].other, avaregePoint, contacts[0].normal, contacts[0].penetration));
	return newContact;
}
std::vector<Contact> Collision::getContacts(Car& car, Wall& wall)
{
	//check for points inside wall
	float closestContact = 999;
	std::vector<Contact> contacts;

	for (int i = 0; i < wall.EBOsize / 3; i++)
	{
		std::vector<Contact> innerContacts;
		float deepestPen = 0;
		glm::vec3 p1 = wall.worldSpace(wall.getPoint(i, 0));
		glm::vec3 p2 = wall.worldSpace(wall.getPoint(i, 1));
		glm::vec3 p3 = wall.worldSpace(wall.getPoint(i, 2));
		glm::vec3 normal = Math::getNormal(p1, p2, p3);

		for (int k = 0; k < 8; k++)
		{
			glm::vec3 point = car.worldSpace(car.getPoint(k));
			if (Math::insidePlane(p1, p2, p3, point))
			{
				float penetration = -Math::distanceTrianglePoint(p1, p2, p3, point);
				deepestPen = std::max(penetration, deepestPen);
				innerContacts.push_back(Contact(&car, &wall, point, normal, penetration));
			}
		}
		//if one face is not collising then no contact
		if (innerContacts.size() == 0)
			return innerContacts;
		//update closest contacts
		if (deepestPen < closestContact)
		{
			contacts = innerContacts;
			closestContact = deepestPen;
		}

	}
	//check for points inside cube

	for (int i = 0; i < car.EBOsize / 3; i++)
	{
		std::vector<Contact> innerContacts;
		float deepestPen = 0;
		glm::vec3 p1 = car.worldSpace(car.getPoint(i, 0));
		glm::vec3 p2 = car.worldSpace(car.getPoint(i, 1));
		glm::vec3 p3 = car.worldSpace(car.getPoint(i, 2));
		glm::vec3 normal = Math::getNormal(p1, p2, p3);

		for (int k = 0; k < 4; k++)
		{
			glm::vec3 point = wall.worldSpace(wall.getPoint(k));
			if (Math::insidePlane(p1, p2, p3, point))
			{
				float penetration = -Math::distanceTrianglePoint(p1, p2, p3, point);
				deepestPen = std::max(penetration, deepestPen);
				innerContacts.push_back(Contact(&wall, &car, point, normal, penetration));
			}
		}
		//if one face is not collising then no contact
		if (innerContacts.size() == 0)
			return innerContacts;
		//update closest contacts
		if (deepestPen <= closestContact)
		{
			contacts = innerContacts;
			closestContact = deepestPen;
		}
	}

	//avarege contacts
	float weights = 0;
	glm::vec3 avaregePoint(0.0f);
	for (int i = 0; i < contacts.size(); i++)
	{
		Contact& contact = contacts[i];
		float distanceVenter = glm::distance(contact.point, car.center) - glm::dot(contact.normal, car.center - contact.point);
		float inverseDistance = 1.0f / std::pow(distanceVenter, 3);
		weights += inverseDistance;
		avaregePoint += contact.point * inverseDistance;
	}
	avaregePoint = avaregePoint / weights;

	//if averenge close round it up
	if (contacts.size() > 1)
	{
		float flatDistanceCenter = std::pow(glm::distance(avaregePoint, car.center), 2) - std::pow((glm::dot(contacts[0].normal, car.center - avaregePoint)), 2);
		if (flatDistanceCenter < 0.01f)
		{
			avaregePoint = car.center - contacts[0].normal * glm::dot(contacts[0].normal, car.center - avaregePoint);
		}
	}

	std::vector<Contact> newContact;
	newContact.push_back(Contact(contacts[0].body, contacts[0].other, avaregePoint, contacts[0].normal, contacts[0].penetration));
	return newContact;
}
std::vector<Contact> Collision::getContacts(Car& car, Block& block)
{
	//check for points inside wall
	float closestContact = 999;
	std::vector<Contact> contacts;

	for (int i = 0; i < block.EBOsize / 3; i++)
	{
		std::vector<Contact> innerContacts;
		float deepestPen = 0;
		glm::vec3 p1 = block.worldSpace(block.getPoint(i, 0));
		glm::vec3 p2 = block.worldSpace(block.getPoint(i, 1));
		glm::vec3 p3 = block.worldSpace(block.getPoint(i, 2));
		glm::vec3 normal = Math::getNormal(p1, p2, p3);

		for (int k = 0; k < 8; k++)
		{
			glm::vec3 point = car.worldSpace(car.getPoint(k));
			if (Math::insidePlane(p1, p2, p3, point))
			{
				float penetration = -Math::distanceTrianglePoint(p1, p2, p3, point);
				deepestPen = std::max(penetration, deepestPen);
				innerContacts.push_back(Contact(&car, &block, point, normal, penetration));
			}
		}
		//if one face is not collising then no contact
		if (innerContacts.size() == 0)
			return innerContacts;
		//update closest contacts
		if (deepestPen < closestContact)
		{
			contacts = innerContacts;
			closestContact = deepestPen;
		}

	}
	//check for points inside car
	
	for (int i = 0; i < car.EBOsize / 3; i++)
	{
		std::vector<Contact> innerContacts;
		float deepestPen = 0;
		glm::vec3 p1 = car.worldSpace(car.getPoint(i, 0));
		glm::vec3 p2 = car.worldSpace(car.getPoint(i, 1));
		glm::vec3 p3 = car.worldSpace(car.getPoint(i, 2));
		glm::vec3 normal = Math::getNormal(p1, p2, p3);

		for (int k = 0; k < 8; k++)
		{
			glm::vec3 point = block.worldSpace(block.getPoint(k));
			if (Math::insidePlane(p1, p2, p3, point))
			{
				float penetration = -Math::distanceTrianglePoint(p1, p2, p3, point);
				deepestPen = std::max(penetration, deepestPen);
				innerContacts.push_back(Contact(&block, &car, point, normal, penetration));
			}
		}
		//if one face is not collising then no contact
		if (innerContacts.size() == 0)
			return innerContacts;
		//update closest contacts
		if (deepestPen <= closestContact)
		{
			contacts = innerContacts;
			closestContact = deepestPen;
		}
	}
	
	if (contacts.size() == 1)
		return contacts;

	//smart avarege points
	glm::vec3 weight(0.0f);
	glm::vec3 avaregePoint(0.0f);
	for (int i = 0; i < contacts.size(); i++)
	{
		Contact& contact = contacts[i];
		glm::vec3 p = contact.point;
		weight.x += 1.0 / std::abs(p.x - car.center.x);
		weight.y += 1.0 / std::abs(p.y - car.center.y);
		weight.z += 1.0 / std::abs(p.z - car.center.z);
		
		avaregePoint.x += p.x/std::abs(p.x - car.center.x);
		avaregePoint.y += p.y / std::abs(p.y - car.center.y);
		avaregePoint.z += p.z / std::abs(p.z - car.center.z);
	}
	avaregePoint /= weight;
	
	
	std::vector<Contact> newContact;
	newContact.push_back(Contact(contacts[0].body, contacts[0].other, avaregePoint, contacts[0].normal, closestContact));
	return newContact;
}
glm::vec3 Collision::wheelContact(glm::vec3 wheel,glm::vec3 dir, Block& block)
{
	float closestContact = 999;
	glm::vec3 Normal;
	glm::vec3 p;
	for (int i = 0; i < block.EBOsize / 3; i++)
	{
		glm::vec3 p1 = block.worldSpace(block.getPoint(i, 0));
		glm::vec3 p2 = block.worldSpace(block.getPoint(i, 1));
		glm::vec3 p3 = block.worldSpace(block.getPoint(i, 2));
		glm::vec3 normal = Math::getNormal(p1, p2, p3);
		
		if (Math::insidePlane(p1, p2, p3, wheel))
		{
			float penetration = -Math::distanceTrianglePoint(p1, p2, p3, wheel);
			if (penetration < closestContact)
			{
				closestContact = penetration;
				Normal = normal;
				p = p1;
			}
		}
		else return wheel;
	}
	if (glm::dot(dir, Normal) < 0.01f)
		return wheel;
	return Math::planeIntersetion(Normal, p, dir, wheel);
}
Contact::Contact(RigidBody* body, RigidBody* other, glm::vec3 point, glm::vec3 normal, float penetration)
{
	this->body = body;
	this->other = other;
	this->point = point;
	this->normal = normal;
	this->penetration = penetration;
}
glm::mat3 getContactTransform(glm::vec3 normal)
{
	//calculate contact coordinates matrix
	//glm::mat4 inversTranslate = glm::translate(glm::mat4(1.0f), -point);
	glm::mat3 inverseRotation(1.0f);
	inverseRotation[1] = normal;
	inverseRotation[2] = glm::cross(normal, inverseRotation[0] + glm::vec3(0,0,1.3));
	inverseRotation[0] = glm::cross(inverseRotation[2], inverseRotation[1]);
	return glm::inverse(inverseRotation);
}
void Contact::solveVelocity(float c, float u)
{
	glm::mat3 contactCoordinate = getContactTransform(normal);
	//calculate closing velocity in contact coordinate
	glm::vec3 relVelocity = contactCoordinate * glm::vec4(body->getVelocity(point) - other->getVelocity(point), 0.0f);
	relVelocity.x *= u;relVelocity.z *= u; relVelocity.y *= (1 + c);
	//add linear drag
	glm::vec3 drag = glm::inverse(contactCoordinate) * glm::vec4(-relVelocity.x, 0, -relVelocity.z, 0);
	relVelocity.x = 0; relVelocity.z = 0;
	if (relVelocity.y >= 0)
		return;
	//return to world space
	glm::vec3 resVelocity = glm::inverse(contactCoordinate) * glm::vec4(-relVelocity, 0);
	//calculate impulse impact in a matrix
	glm::mat3 impulseToVelocity;
	impulseToVelocity[0] = body->impulseImpact(point, glm::vec3(1, 0, 0)) + other->impulseImpact(point, glm::vec3(1, 0, 0));
	impulseToVelocity[1] = body->impulseImpact(point, glm::vec3(0, 1, 0)) + other->impulseImpact(point, glm::vec3(0, 1, 0));
	impulseToVelocity[2] = body->impulseImpact(point, glm::vec3(0, 0, 1)) + other->impulseImpact(point, glm::vec3(0, 0, 1));
	glm::vec3 impulse = glm::inverse(impulseToVelocity) * resVelocity;
	body->addImpulse(point, impulse);
	other->addImpulse(point, -impulse);
	//add drag
	float dragImpact = 1.0f / (body->inverseMass + other->inverseMass);
	body->addImpulse(body->center, drag * dragImpact);
	other->addImpulse(other->center, -drag * dragImpact);
	
}