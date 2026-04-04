#include"Collision.h"
#include"RigidBody.h"
#include"Math.h"
#include"Car.h"
#include"Block.h"
#include<glm/gtc/type_ptr.hpp>
#include<cmath>

RigidBody* Collision::empty = nullptr;
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
std::vector<int> Collision::collisionDetector(QuadTree& tree, RigidBody& body)
{
	std::vector<int> triangles;

	//create hitbox for body
	std::vector<glm::vec3> wsPoints;
	for (auto& point : body.model.points)
	{
		wsPoints.push_back(body.worldSpace(point));
	}
	HitBox hitbox = HitBox(wsPoints);
	hitbox.enLarge(1.5);
	hitbox.blCorner.y -= 0.5f;
	hitbox.height += 0.5f;

	triangles = QuadTree::unDupe(tree.hitboxArray, tree.getData(hitbox));
	//std::cout << "triangles count " << triangles.size()/2 << std::endl;
	return triangles;

}
std::vector<int> Collision::collisionDetector(Grid& grid, RigidBody& body)
{
	std::vector<int> triangles;

	//create hitbox for body
	std::vector<glm::vec3> wsPoints;
	for (auto& point : body.model.points)
	{
		wsPoints.push_back(body.worldSpace(point));
	}
	HitBox hitbox = HitBox(wsPoints);
	hitbox.enLarge(1.5);
	hitbox.blCorner.y -= 0.5f;
	hitbox.height += 0.5f;

	triangles = QuadTree::unDupe(grid.hitboxList, grid.getData(hitbox));
	//std::cout << "triangles count " << triangles.size()/2 << std::endl;
	return triangles;
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
		//handle one object cases
		if (contacts[i].body == NULL)
			contacts[i].body = Collision::empty;
		if (contacts[i].other == NULL)
			contacts[i].other = Collision::empty;

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

bool Collision::insideBody(RigidBody& body, RigidBody& other, std::vector<Contact>& contacts)
{
	std::vector<glm::vec3> points;

	//get all points to check
	for (int i = 0; i < 8; i++)
		points.push_back(other.worldSpace(other.model.points[i]));

	//return points inside cube
	for (int i = 0; i < body.model.Indices.size() / 3; i++)
	{
		glm::vec3 p1 = body.worldSpace(body.model.getTrianglePoint(i, 0));
		glm::vec3 p2 = body.worldSpace(body.model.getTrianglePoint(i, 1));
		glm::vec3 p3 = body.worldSpace(body.model.getTrianglePoint(i, 2));
		
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

	contacts = Contact::createContacts(&body, &other, points);
	return true;
}

glm::vec3 edgeFaceIntersection(Edge& edge, glm::vec3 t1, glm::vec3 t2, glm::vec3 t3)
{
	glm::vec3 SUSinter = Math::triangleIntersection(t1, t2, t3, edge.p2 - edge.p1, edge.p1);
	if (glm::dot(edge.p2 - SUSinter, edge.p1 - SUSinter) > 0)
		return glm::vec3(0);
	return SUSinter;
}
bool Collision::faceEdges(RigidBody& body, RigidBody& block, std::vector<Contact>& contacts)
{
	glm::vec3 bLDB = block.worldSpace(block.model.points[0]);
	glm::vec3 bRDB = block.worldSpace(block.model.points[1]);
	glm::vec3 bRDF = block.worldSpace(block.model.points[2]);
	glm::vec3 bLDF = block.worldSpace(block.model.points[3]);
	glm::vec3 bLUF = block.worldSpace(block.model.points[4]);
	glm::vec3 bRUF = block.worldSpace(block.model.points[5]);
	glm::vec3 bRUB = block.worldSpace(block.model.points[6]);
	glm::vec3 bLUB = block.worldSpace(block.model.points[7]);

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
	for (auto &blockEdge : blockEdges)
	{
		for (int i = 0; i < body.model.Indices.size() / 3; i++)
		{
			//get points of current face
			glm::vec3 p1 = body.worldSpace(body.model.getTrianglePoint(i, 0));
			glm::vec3 p2 = body.worldSpace(body.model.getTrianglePoint(i, 1));
			glm::vec3 p3 = body.worldSpace(body.model.getTrianglePoint(i, 2));

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
	contacts = Contact::createContacts(&body, &block, points);
	return true;
}

std::vector<Contact> Collision::getContacts(RigidBody& r1, RigidBody& block)
{
	
	std::vector<Contact> carContacts, blockContacts,edgeContacts, contacts;
	bool notInsideR1 = !insideBody(r1, block, carContacts);
	bool notInsideR2 = !insideBody(block, r1, blockContacts);
	bool edges = !faceEdges(r1, block, edgeContacts);
	if (notInsideR1 && notInsideR2 && edges )
		return contacts;
	
	contacts.insert(contacts.end(), carContacts.begin(), carContacts.end());
	
	contacts.insert(contacts.end(), blockContacts.begin(), blockContacts.end());

	contacts.insert(contacts.end(), edgeContacts.begin(), edgeContacts.end());

	//std::cout << "block contacts:" << blockContacts.size()  << std::endl;
	//std::cout << "car contacts:" << carContacts.size() << std::endl;
	//std::cout << "edge contacts:" << edgeContacts.size()  << std::endl;

	return contacts;
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
	//handle one object cases
	if (body == NULL)
		body = Collision::empty;
	if (other == NULL)
		other = Collision::empty;

	glm::vec3 relVelocity = other->getVelocity(point) - body->getVelocity(point);
	return std::abs(glm::dot(relVelocity, normal));
}
void Contact::solveVelocity(float c, float u)
{
	//handle one object cases
	if (body == NULL)
		body = Collision::empty;
	if (other == NULL)
		other = Collision::empty;

	//create contact coordinate matrix
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

std::vector<Contact> Contact::createContacts(RigidBody* body, RigidBody* other, std::vector<glm::vec3> points)
{
	std::vector<Contact> contacts;

	//stop early
	if (points.size() == 0)
		return contacts;

	//create contacts of the points
	float minPenetration = 999;
	for (int i = 0; i < body->model.Indices.size() / 3; i++)
	{
		std::vector<Contact> innerContacts;
		float deepestPen = 0;
		//get points of current face
		glm::vec3 p1 = body->worldSpace(body->model.getTrianglePoint(i, 0));
		glm::vec3 p2 = body->worldSpace(body->model.getTrianglePoint(i, 1));
		glm::vec3 p3 = body->worldSpace(body->model.getTrianglePoint(i, 2));
		glm::vec3 normal = Math::getNormal(p1, p2, p3);

		//create contact from each point to current face
		for (auto& point : points)
		{
			float penetration = -Math::distanceTrianglePoint(p1, p2, p3, point);
			deepestPen = std::max(deepestPen, penetration);
			innerContacts.push_back(Contact(body, other, point, normal, penetration));
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
