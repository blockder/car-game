#include"Collision.h"
#include"RigidBody.h"
#include"Model.h"
#include"Math.h"
#include"Car.h"
#include"Block.h"
#include<glm/gtc/type_ptr.hpp>
#include<cmath>

struct Edge
{
	glm::vec3 p1;
	glm::vec3 p2;
	Edge(glm::vec3 p1, glm::vec3 p2)
		:p1(p1), p2(p2){}
};
glm::mat3 getContactTransform(glm::vec3 normal)
{
	//calculate contact coordinates matrix
	glm::mat3 inverseRotation(1.0f);
	inverseRotation[1] = normal;
	inverseRotation[2] = glm::cross(normal, inverseRotation[0] + glm::vec3(0,0,1.3));
	inverseRotation[0] = glm::cross(inverseRotation[2], inverseRotation[1]);
	return glm::inverse(inverseRotation);
}
void Collision::collisionDetector()
{

}
void Collision::velocityResolver(std::vector<Contact>& contacts, float c, float u)
{
	//calculate initial closing velocity 
	//and solves the biggest one
	if (contacts.size() == 0)
		return;

	float maxClosingVelocity = 99;
	int k = 0;
	while (maxClosingVelocity > 0.1f && k<15)
	{
		maxClosingVelocity = 0;
		int contactNum = 0;
		for (int i = 0; i < contacts.size(); i++)
		{
			float closingVelocity = contacts[i].getClosingVelocity();
			if (closingVelocity >= maxClosingVelocity)
			{
				maxClosingVelocity = closingVelocity;
				contactNum = i;
			}
		}
		contacts[contactNum].solveVelocity(c, u);
		k++;
	}
}
void Collision::penetrationResolver(std::vector<Contact>& contacts)
{
	//solves the biggest penetration
	if (contacts.size() == 0)
		return;
	float maxPen = 0;
	int contactNum = 0;
	for (int i = 0; i < contacts.size(); i++)
	{
		if (contacts[i].penetration > maxPen)
		{
			maxPen = contacts[i].penetration;
			contactNum = i;
		}
	}
	Contact& contact = contacts[contactNum];
	glm::vec3 point = contact.point;
	glm::vec3 normal = contact.normal;
	
	float adjustAmount = maxPen / (contact.body->inverseMass + contact.other->inverseMass);
	contact.other->center +=  normal * adjustAmount * contact.other->inverseMass;
	contact.body->center += -normal * adjustAmount * contact.body->inverseMass;
}
bool Collision::insideBlock(Car& car, Block& block, std::vector<Contact>& contacts)
{
	std::vector<glm::vec3> points;

	//get all points to check
	for (int i = 0; i < 8; i++)
		points.push_back(car.worldSpace(car.points[i]));

	//return points inside cube
	for (int i = 0; i < block.Indices.size() / 3; i++)
	{
		glm::vec3 p1 = block.worldSpace(block.getPoint(i, 0));
		glm::vec3 p2 = block.worldSpace(block.getPoint(i, 1));
		glm::vec3 p3 = block.worldSpace(block.getPoint(i, 2));

		for (int k = 0; k < points.size(); k++)
		{
			//if points outside car erase it
			if (!Math::insidePlane(p1, p2, p3, points[k]))
			{
				points.erase(points.begin() + k);
				k--;
			}
		}

		//terminate early
		if (points.size() == 0)
			return false;
	}

	contacts = Contact::createContacts(&block, &block, &car, points);
	return true;
}
bool Collision::insideCar(Car& car, Block& block, std::vector<Contact>& contacts)
{
	std::vector<glm::vec3> points;

	//get all points to check
	for (int i = 0; i < 8; i++)
		points.push_back(block.worldSpace(block.points[i]));

	//return points inside cube
	for (int i = 0; i < car.Indices.size() / 3; i++)
	{
		glm::vec3 p1 = car.worldSpace(car.getPoint(i, 0));
		glm::vec3 p2 = car.worldSpace(car.getPoint(i, 1));
		glm::vec3 p3 = car.worldSpace(car.getPoint(i, 2));
		
		for (int k = 0; k < points.size(); k++)
		{
			//if points outside car erase it
			if (!Math::insidePlane(p1, p2, p3, points[k]))
			{
				points.erase(points.begin() + k);
				k--;
			}
		}

		//terminate early
		if (points.size() == 0)
			return false;
	}

	contacts = Contact::createContacts(&car, &car, &block, points);
	return true;
}
bool Collision::Edges(Car& car, Block& block, std::vector<Contact>& contacts)
{
	bool result = false;
	glm::vec3 cLDB = car.worldSpace(car.points[0]); glm::vec3 bLDB = block.worldSpace(block.points[0]);
	glm::vec3 cRDB = car.worldSpace(car.points[1]); glm::vec3 bRDB = block.worldSpace(block.points[1]);
	glm::vec3 cRDF = car.worldSpace(car.points[2]); glm::vec3 bRDF = block.worldSpace(block.points[2]);
	glm::vec3 cLDF = car.worldSpace(car.points[3]); glm::vec3 bLDF = block.worldSpace(block.points[3]);
	glm::vec3 cLUF = car.worldSpace(car.points[4]); glm::vec3 bLUF = block.worldSpace(block.points[4]);
	glm::vec3 cRUF = car.worldSpace(car.points[5]); glm::vec3 bRUF = block.worldSpace(block.points[5]);
	glm::vec3 cRUB = car.worldSpace(car.points[6]); glm::vec3 bRUB = block.worldSpace(block.points[6]);
	glm::vec3 cLUB = car.worldSpace(car.points[7]); glm::vec3 bLUB = block.worldSpace(block.points[7]);

	std::vector<Edge> blockEdges;
	//buttom 
	blockEdges.push_back(Edge(bLDB, bLDF));
	blockEdges.push_back(Edge(bRDB, bRDF));
	blockEdges.push_back(Edge(bRDB, bLDB));
	blockEdges.push_back(Edge(bRDF, bLDF));
	//sides
	blockEdges.push_back(Edge(bLDB, bLUB));
	blockEdges.push_back(Edge(bRDB, bRUB));
	blockEdges.push_back(Edge(bLDF, bLUF));
	blockEdges.push_back(Edge(bRDF, bRUF));
	//up
	blockEdges.push_back(Edge(bLUB, bLUF));
	blockEdges.push_back(Edge(bRUF, bRUB));
	blockEdges.push_back(Edge(bRUB, bLUB));
	blockEdges.push_back(Edge(bRUF, bLUF));

	std::vector<Edge> carEdges;
	//buttom 
	carEdges.push_back(Edge(cLDB ,cLDF));
	carEdges.push_back(Edge(cRDB ,cRDF));
	carEdges.push_back(Edge(cRDB, cLDB));
	carEdges.push_back(Edge(cRDF, cLDF));
	//sides
	carEdges.push_back(Edge(cLUB, cLDB));
	carEdges.push_back(Edge(cRUB, cRDB));
	carEdges.push_back(Edge(cLUF, cLDF));
	carEdges.push_back(Edge(cRUF, cRDF));
	//up
	carEdges.push_back(Edge(cLUB, cLUF));
	carEdges.push_back(Edge(cRUB, cRUF));
	carEdges.push_back(Edge(cRUB, cLUB));
	carEdges.push_back(Edge(cRUF, cLUF));
	
	
	for (int i = 0; i < blockEdges.size(); i++)
	{
		Edge& blockEdge = blockEdges[i];
		float minPenetration = 999;
		std::vector<Contact> innerContacts;
		for (int k = 0; k < carEdges.size(); k++)
		{
			Edge& carEdge = carEdges[k];
			glm::vec3 interC, interB; 
			//check if there are intersection points
			if (Math::LinesIntersection(blockEdge.p1, blockEdge.p2, carEdge.p1, carEdge.p2, interC) && Math::LinesIntersection(carEdge.p1, carEdge.p2, blockEdge.p1, blockEdge.p2, interB))
			{
				if (insideCar(car, interB) && insideBlock(block, interC))
				{
					glm::vec3 edgeNormal = glm::normalize(glm::cross(carEdge.p2 - carEdge.p1,blockEdge.p1 - blockEdge.p2));
					if (glm::dot(glm::normalize(interB - interC), edgeNormal) < 0) edgeNormal *= -1;
					float penetration = glm::distance(interB, interC);
					if (std::abs(penetration - minPenetration) < 0.001f)
						innerContacts.push_back(Contact(&block, &car, interC, edgeNormal, penetration));
					else if (penetration < minPenetration && penetration < 0.1f )
					{
						minPenetration = penetration;
						innerContacts.erase(innerContacts.begin(), innerContacts.end());
						innerContacts.push_back(Contact(&block, &car, interC, edgeNormal, penetration));
						result = true;
					}
				}
			}
		}
		contacts.insert(contacts.end(), innerContacts.begin(), innerContacts.end());

	}
	return result;
}
glm::vec3 edgeFaceIntersection(Edge& edge, glm::vec3 t1, glm::vec3 t2, glm::vec3 t3)
{
	glm::vec3 SUSinter = Math::triangleIntersection(t1, t2, t3, edge.p2 - edge.p1, edge.p1);
	if (glm::dot(edge.p2 - SUSinter, edge.p1 - SUSinter) > 0)
		return glm::vec3(0);
	return SUSinter;
}
bool Collision::faceEdges(Car& car, Block& block, std::vector<Contact>& contacts)
{
	glm::vec3 bLDB = block.worldSpace(block.points[0]);
	glm::vec3 bRDB = block.worldSpace(block.points[1]);
	glm::vec3 bRDF = block.worldSpace(block.points[2]);
	glm::vec3 bLDF = block.worldSpace(block.points[3]);
	glm::vec3 bLUF = block.worldSpace(block.points[4]);
	glm::vec3 bRUF = block.worldSpace(block.points[5]);
	glm::vec3 bRUB = block.worldSpace(block.points[6]);
	glm::vec3 bLUB = block.worldSpace(block.points[7]);

	std::vector<Edge> blockEdges;
	//buttom 
	blockEdges.push_back(Edge(bLDB, bLDF));
	blockEdges.push_back(Edge(bRDB, bRDF));
	blockEdges.push_back(Edge(bRDB, bLDB));
	blockEdges.push_back(Edge(bRDF, bLDF));
	//sides
	blockEdges.push_back(Edge(bLDB, bLUB));
	blockEdges.push_back(Edge(bRDB, bRUB));
	blockEdges.push_back(Edge(bLDF, bLUF));
	blockEdges.push_back(Edge(bRDF, bRUF));
	//up
	blockEdges.push_back(Edge(bLUB, bLUF));
	blockEdges.push_back(Edge(bRUF, bRUB));
	blockEdges.push_back(Edge(bRUB, bLUB));
	blockEdges.push_back(Edge(bRUF, bLUF));

	//create an array of all edge intersections inside car
	std::vector<glm::vec3> points;

	//find points for each edge
	for (auto blockEdge : blockEdges)
	{
		for (int i = 0; i < car.Indices.size() / 3; i++)
		{
			//get points of current face
			glm::vec3 p1 = car.worldSpace(car.getPoint(i, 0));
			glm::vec3 p2 = car.worldSpace(car.getPoint(i, 1));
			glm::vec3 p3 = car.worldSpace(car.getPoint(i, 2));

			glm::vec3 inter = edgeFaceIntersection(blockEdge, p1, p2, p3);
			//check if point is inside car
			if (glm::length(inter) > 0)
			{
				points.push_back(inter);
			}
		}

	}

	//terminate early
	if (points.size() == 0)
		return false;

	//create contacts of points
	contacts = Contact::createContacts(&car, &car, &block, points);
	return true;
}
glm::vec3 Collision::avaregeContact(std::vector<Contact>& contacts, glm::vec3 center)
{
	//smart avarege points
	glm::vec3 weight(0.0f);
	glm::vec3 avaregePoint(0.0f);
	for (int i = 0; i < contacts.size(); i++)
	{
		glm::vec3 p =  contacts[i].point;
		weight.x += 1.0 / std::abs(p.x - center.x);
		weight.y += 1.0 / std::abs(p.y - center.y);
		weight.z += 1.0 / std::abs(p.z - center.z);

		avaregePoint.x += p.x / std::abs(p.x - center.x);
		avaregePoint.y += p.y / std::abs(p.y - center.y);
		avaregePoint.z += p.z / std::abs(p.z - center.z);
	}
	avaregePoint /= weight;
	if (!std::isnormal(avaregePoint.x)) avaregePoint.x = center.x;
	if (!std::isnormal(avaregePoint.y)) avaregePoint.y = center.y;
	if (!std::isnormal(avaregePoint.z)) avaregePoint.z = center.z;
	return avaregePoint;
}
std::vector<Contact> Collision::getContacts(Car& car, Block& block)
{
	
	std::vector<Contact> carContacts, blockContacts,edgeContacts, contacts;
	bool insideCarI = !insideCar(car, block, carContacts);
	bool insideBlockI = !insideBlock(car, block, blockContacts);
	bool edges = !faceEdges(car, block, edgeContacts);
	if (insideBlockI && insideCarI && edges )
		return contacts;
	
	contacts.insert(contacts.end(), carContacts.begin(), carContacts.end());
	
	contacts.insert(contacts.end(), blockContacts.begin(), blockContacts.end());

	contacts.insert(contacts.end(), edgeContacts.begin(), edgeContacts.end());

	//std::cout << "block contacts:" << blockContacts.size()  << std::endl;
	//std::cout << "car contacts:" << carContacts.size() << std::endl;
	//std::cout << "edge contacts:" << edgeContacts.size()  << std::endl;

	return contacts;
}
bool Collision::insideCar(Car& car, glm::vec3 point)
{
	for (int i = 0; i < car.Indices.size() / 3; i++)
	{
		std::vector<Contact> innerContacts;
		float deepestPen = 0;
		glm::vec3 p1 = car.worldSpace(car.getPoint(i, 0));
		glm::vec3 p2 = car.worldSpace(car.getPoint(i, 1));
		glm::vec3 p3 = car.worldSpace(car.getPoint(i, 2));
		glm::vec3 normal = Math::getNormal(p1, p2, p3);

		if(Math::distanceTrianglePoint(p1 ,p2, p3, point) > 0.0)
		//if (glm::dot(point - p1, Math::getNormal(p1,p2,p3)) > 0.0001)
			return false;
	}
	return true;
}
bool Collision::insideBlock(Block& block, glm::vec3 point)
{
	for (int i = 0; i < block.Indices.size() / 3; i++)
	{
		std::vector<Contact> innerContacts;
		float deepestPen = 0;
		glm::vec3 p1 = block.worldSpace(block.getPoint(i, 0));
		glm::vec3 p2 = block.worldSpace(block.getPoint(i, 1));
		glm::vec3 p3 = block.worldSpace(block.getPoint(i, 2));
		glm::vec3 normal = Math::getNormal(p1, p2, p3);

		if (Math::distanceTrianglePoint(p1, p2, p3, point) > 0.00001)
		//if (glm::dot(point - p1, Math::getNormal(p1, p2, p3)) > 0.0001)
			return false;
	}
	return true;
}
float Collision::getMaxPenetration(std::vector<Contact>& contacts)
{
	float maxPen = 0;
	for (int i = 0; i < contacts.size(); i++)
	{
		maxPen = std::max(maxPen, contacts[i].penetration);
	}
	return maxPen;
}
Contact::Contact(RigidBody* body, RigidBody* other, glm::vec3 point, glm::vec3 normal, float penetration)
{
	this->body = body;
	this->other = other;
	this->point = point;
	this->normal = normal;
	this->penetration = penetration;
	this->accumulatedNormal = 0;
	this->accumulatedSide = glm::vec3(0);
}
float Contact::getClosingVelocity()
{
	glm::vec3 relVelocity = other->getVelocity(point) - body->getVelocity(point);
	return std::abs(glm::dot(relVelocity, normal));
}
void Contact::solveVelocity(float c, float u)
{
	glm::mat3 contactCoordinate = getContactTransform(normal);
	glm::mat3 contactToWorldSpace = glm::inverse(contactCoordinate);

	//calculate closing velocity in contact coordinate
	glm::vec3 relVelocity = contactCoordinate * (other->getVelocity(point) - body->getVelocity(point));

	//update accumulated normal
	float oldAccumulatedNormal = accumulatedNormal;
	//accumulatedNormal = std::max(accumulatedNormal - relVelocity.y , 0.0f);
	accumulatedNormal = accumulatedNormal - relVelocity.y;
	

	//update accumulated side(friction)
	glm::vec3 oldSide = accumulatedSide;
	accumulatedSide = accumulatedSide + glm::vec3(-relVelocity.x, 0 , -relVelocity.z);
	float maxFriction = u * std::abs(accumulatedNormal);
	if (glm::length(accumulatedSide) > maxFriction)
	{
		accumulatedSide = glm::normalize(accumulatedSide) * maxFriction;
	}
	
	//return to world space
	glm::vec3 resVelocityUp = contactToWorldSpace * glm::vec3(0, accumulatedNormal - oldAccumulatedNormal, 0);
	glm::vec3 resVelocitySide = contactToWorldSpace * (accumulatedSide - oldSide);

	//calculate impulse impact in a matrix
	glm::mat3 impulseToVelocity;
	impulseToVelocity[0] = body->impulseImpact(point, glm::vec3(1, 0, 0)) + other->impulseImpact(point, glm::vec3(1, 0, 0));
	impulseToVelocity[1] = body->impulseImpact(point, glm::vec3(0, 1, 0)) + other->impulseImpact(point, glm::vec3(0, 1, 0));
	impulseToVelocity[2] = body->impulseImpact(point, glm::vec3(0, 0, 1)) + other->impulseImpact(point, glm::vec3(0, 0, 1));
	glm::mat3 velocityToImpulse = glm::inverse(impulseToVelocity);
	glm::vec3 normalImpulse = velocityToImpulse * resVelocityUp;
	glm::vec3 sideImpulse = velocityToImpulse * resVelocitySide;

	//apply normal impulse
	other->applyImpulse(point, normalImpulse);
	body->applyImpulse(point, -normalImpulse);
	
	//apply friction impulse
	other->applyImpulse(point, sideImpulse);
	body->applyImpulse(point, -sideImpulse);
	return;
	
}

std::vector<Contact> Contact::createContacts(Model* m, RigidBody* r,RigidBody* other, std::vector<glm::vec3> points)
{
	std::vector<Contact> contacts;

	//stop early
	if (points.size() == 0)
		return contacts;

	//create contacts of the points
	float minPenetration = 999;
	for (int i = 0; i < m->Indices.size() / 3; i++)
	{
		std::vector<Contact> innerContacts;
		float deepestPen = 0;
		//get points of current face
		glm::vec3 p1 = r->worldSpace(m->getPoint(i, 0));
		glm::vec3 p2 = r->worldSpace(m->getPoint(i, 1));
		glm::vec3 p3 = r->worldSpace(m->getPoint(i, 2));
		glm::vec3 normal = Math::getNormal(p1, p2, p3);

		//create contact from each point to current face
		for (auto point : points)
		{
			float penetration = -Math::distanceTrianglePoint(p1, p2, p3, point);
			deepestPen = std::max(deepestPen, penetration);
			innerContacts.push_back(Contact(r, other, point, normal, penetration));
		}

		//set most suitble contacts
		if (deepestPen < minPenetration)
		{
			contacts = innerContacts;
			minPenetration = deepestPen;
		}
	}
	return contacts;
}
