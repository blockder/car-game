#include"Car.h"
#include"Camera.h"
#include"Shader.h"
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/vector_angle.hpp>
#include<vector>
#include"Math.h"
#include"Quaternion.h"



Car::Car(std::string texture, std::vector<Vertex> Vertices, std::vector<GLuint> Indices) 
	: RigidBody(Model(texture, Vertices, Indices))
{
	speed = 10;
	tireFriction = 10;
	drag = 0.99;
	angularDrag = 1200;
	onGround = false;
	atStartLine = true;
	carModel = NULL;
	//set physics
	RigidBody::setMass(2);
}



void Car::movement(GLFWwindow* window, float deltaTime)
{
	
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		return;
	
	
	//ground mode
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		addForce(rotation * glm::vec3(0,0,1) * speed);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		addForce(rotation * glm::vec3(0, 0, 1) * -speed);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		angularVelocity *= 0;
		glm::vec3 right = rotation * glm::vec3(0, 1, 0) * turnAngle;
		Quaternion nextRot(right * deltaTime);
		rotation = nextRot * rotation;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		angularVelocity *= 0;
		glm::vec3 left = rotation * glm::vec3(0, 1, 0) * -turnAngle;
		Quaternion nextRot(left * deltaTime);
		rotation = nextRot * rotation;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		speed = 30;
		tireFriction = 1;
	}
	else
	{
		speed = 20;
		tireFriction = 10;
	}
	
	
}

void Car::applyFriction()
{

	//drag
	velocity *= drag;

	//angular drag
	glm::vec3 up = rotation * glm::vec3(0, 1, 0);
	float upRot = glm::dot(up, angularVelocity);
	addTorque(angularDrag * upRot * -up * inverseInersia);

	//tire friction
	glm::vec3 sideDirection = rotation * glm::vec3(1, 0, 0);
	float sideVelocity = glm::dot(sideDirection, velocity);
	addForce(-sideDirection / inverseMass * tireFriction * sideVelocity);
}

void Car::updateCarModel()
{
	//update matrix
	carModel->movement = glm::translate(glm::mat4(1.0f), center);
	carModel->rotationMatrix = rotation.getMatrix();

	model.movement = glm::translate(glm::mat4(1.0f), center);
	model.rotationMatrix = rotation.getMatrix();

	timer();
}

std::vector<Contact> Car::generateWallContacts(float restDistance, std::vector<int>& triangles, std::vector<Model>& collisionMap)
{
	std::vector<Contact> wallContacts;
	wallContacts.push_back(generateContact(worldSpace(model.points[4]), restDistance, triangles, collisionMap));
	wallContacts.push_back(generateContact(worldSpace(model.points[5]), restDistance, triangles, collisionMap));
	wallContacts.push_back(generateContact(worldSpace(model.points[6]), restDistance, triangles, collisionMap));
	wallContacts.push_back(generateContact(worldSpace(model.points[7]), restDistance, triangles, collisionMap));

	//delete empty contacts
	for (int i = 0; i < wallContacts.size(); i++)
	{
		if (wallContacts[i].normal == glm::vec3(0))
		{
			wallContacts.erase(wallContacts.begin() + i);
			i--;
		}
		else
			wallContacts[i].uStatic = 100.0f;
	}
	return wallContacts;
}
std::vector<Contact> Car::generateGroundContacts(float restDistance, std::vector<int>& triangles, std::vector<Model>& collisionMap)
{
	std::vector<Contact> groundContacts;
	groundContacts.push_back(generateContact(worldSpace(model.points[0]), restDistance, triangles, collisionMap));
	groundContacts.push_back(generateContact(worldSpace(model.points[1]), restDistance, triangles, collisionMap));
	groundContacts.push_back(generateContact(worldSpace(model.points[2]), restDistance, triangles, collisionMap));
	groundContacts.push_back(generateContact(worldSpace(model.points[3]), restDistance, triangles, collisionMap));

	//delete empty contacts
	for (int i = 0; i < groundContacts.size(); i++)
	{
		if (groundContacts[i].normal == glm::vec3(0))
		{
			groundContacts.erase(groundContacts.begin() + i);
			i--;
		}
	}

	if (groundContacts.size() == 0)
		this->onGround = false;
	else this->onGround = true;
	return groundContacts;
}

Contact Car::generateContact(glm::vec3 point, float restDistance, std::vector<int>& triangles, std::vector<Model>& collisionMap)
{
	float minDistance = restDistance;
	glm::vec3 contactNormal = glm::vec3(0);

	//project ray for each triangle and find shortest distance found
	for (int k = 0; k < triangles.size(); k += 2)
	{
		int i = triangles[k];
		int n = triangles[k + 1];
		glm::vec3 t1 = collisionMap[n].getTrianglePoint(i, 0);
		glm::vec3 t2 = collisionMap[n].getTrianglePoint(i, 1);
		glm::vec3 t3 = collisionMap[n].getTrianglePoint(i, 2);
		glm::vec3 normal = Math::getNormal(t1, t2, t3);

		//skip if no apt intersection
		float distanceTriangle = Math::distanceTrianglePoint(t1, t2, t3, point);
		if (distanceTriangle <= 0.01f || distanceTriangle > minDistance)
			continue;

		glm::vec3 susP = Math::triangleIntersection(t1, t2, t3, normal, point);

		if (glm::length(susP) > 0)
		{
			minDistance = distanceTriangle;
			contactNormal = normal;
		}
	}
	return Contact(this, NULL, point, -contactNormal, restDistance - minDistance);
}



void Car::setDirection(glm::vec3 rot, float angle)
{
	rotation = Quaternion(rot, angle);
	model.rotationMatrix = rotation.getMatrix();
}

int lap = 1;
bool atStartline = true;
double start =0, end;
void Car::timer()
{
	//check where car
	bool startUpdate = center.x > -6.0f && center.x < 6.0f && center.z > -20.0f && center.z < 5.0f;
	
	//start lap
	if (!startUpdate && atStartLine)
	{
		start = glfwGetTime();
	}

	//finish lap
	if (startUpdate && !atStartLine)
	{
		end = glfwGetTime();
		double time = (end - start);
		std::cout << "lap number " << lap << " time: " << time <<" second" << std::endl;
		lap++;
	}
	atStartLine = startUpdate;
}