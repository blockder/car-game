#include"Car.h"
#include"Camera.h"
#include"Shader.h"
#include"Wall.h"
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/vector_angle.hpp>
#include<vector>
#include"Math.h"
#include"Quaternion.h"


static GLuint Indices[] =
{
	4,5,6,
	6,7,4,
	0,1,2,
	2,3,0,
	2,1,6,
	6,5,2,
	0,3,4,
	4,7,0,
	3,2,5,
	5,4,3,
	1,0,7,
	7,6,1
};

Car::Car(Texture texture, Vertex p1, Vertex p2, Vertex p3, Vertex p4, Vertex p5, Vertex p6, Vertex p7, Vertex p8) : Mesh(texture)
{
	this->p1 = p1;
	this->p2 = p2;
	this->p3 = p3;
	this->p4 = p4;
	this->p5 = p5;
	this->p6 = p6;
	this->p7 = p7;
	this->p8 = p8;

	this->frontLeft = p1.position + glm::vec3(0, -0.2f, 0);
	this->frontRight = p2.position + glm::vec3(0, -0.2f, 0);
	this->backLeft = p4.position + glm::vec3(0, -0.2f, 0);
	this->backRight = p3.position + glm::vec3(0, -0.2f, 0);

	this->EBOsize = sizeof(Indices)/sizeof(GLuint);
	//set EBO
	glNamedBufferData(EBO, sizeof(Indices), Indices, GL_STATIC_DRAW);

	update();
}

void Car::update()
{
	//update vertices
	std::vector<Vertex> vertices;
	vertices.push_back(p1);
	vertices.push_back(p2);
	vertices.push_back(p3);
	vertices.push_back(p4);
	vertices.push_back(p5);
	vertices.push_back(p6);
	vertices.push_back(p7);
	vertices.push_back(p8);
	//put data in vbo 
	glNamedBufferData(VBO, sizeof(Vertex) * vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);
	//set rigid body
	std::vector<glm::vec3> points;
	points.push_back(p1.position);
	points.push_back(p2.position);
	points.push_back(p3.position);
	points.push_back(p4.position);
	points.push_back(p5.position);
	points.push_back(p6.position);
	points.push_back(p7.position);
	points.push_back(p8.position);

	RigidBody::setPhysics(1, points);

}

void Car::draw(Shader& shaderProgram, Camera& camera)
{
	shaderProgram.Active();
	//send camera matrix
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.id, "cameraMatrix"), 1, GL_FALSE, glm::value_ptr(camera.getMatrix()));
	//send position matrix
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.id, "position"), 1, GL_FALSE, glm::value_ptr(RigidBody::movement));
	//send rotation matrix
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.id, "rotation"), 1, GL_FALSE, glm::value_ptr(rotationMatrix));

	Mesh::draw(shaderProgram);
}
glm::vec3 Car::getPoint(int triangle, int point)
{
	std::vector<glm::vec3> points;
	points.push_back(p1.position);
	points.push_back(p2.position);
	points.push_back(p3.position);
	points.push_back(p4.position);
	points.push_back(p5.position);
	points.push_back(p6.position);
	points.push_back(p7.position);
	points.push_back(p8.position);
	return points[Indices[triangle * 3 + point]];
}
glm::vec3 Car::getPoint(int i)
{
	std::vector<glm::vec3> points;
	points.push_back(p1.position);
	points.push_back(p2.position);
	points.push_back(p3.position);
	points.push_back(p4.position);
	points.push_back(p5.position);
	points.push_back(p6.position);
	points.push_back(p7.position);
	points.push_back(p8.position);
	return points[i];
}
void Car::movement(GLFWwindow* window)
{
	angularDrag = 0.98f;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		return;
	if (glm::length(netForce+ netImpulse) < 0.01f )
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE)
			angularDrag = 0.5f;
		else
			RigidBody::torque +=   rotation * glm::vec3(-1,0,0) ;
		return;
	}
	glm::vec3 right(0.0f);
	glm::vec3 left(0.0f);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		RigidBody::addForce(- getDirection() * speed);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		left = glm::vec3(rotationMatrix[1]) * glm::sin(turnAngle);
		Quaternion nextRot(left);
		rotation = nextRot * rotation;
		angularVelocity = glm::vec3(0.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		right = glm::vec3(rotationMatrix[1]) * glm::sin(-turnAngle);
		Quaternion nextRot(right);
		rotation = nextRot * rotation;
		angularVelocity = glm::vec3(0.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		RigidBody::addForce(getDirection() * speed);
	}
	
}



glm::vec3 Car::getDirection()
{
	return glm::vec3(rotationMatrix[2]);
}
void Car::setDirection(glm::vec3 rot, float angle)
{
	rotation = Quaternion(rot, angle);
	rotationMatrix = rotation.getMatrix();
}
