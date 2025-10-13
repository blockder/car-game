#include"Wall.h"
#include"Camera.h"
#include"Shader.h"
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/vector_angle.hpp>
#include<vector>
#include"Math.h"

static GLuint Indices[] =
{
	0,1,2,
	2,3,0
};

Wall::Wall(Texture texture, Vertex p1, Vertex p2, Vertex p3, Vertex p4) : Mesh(texture)
{
	this->p1 = p1;
	this->p2 = p2;
	this->p3 = p3;
	this->p4 = p4;

	this->EBOsize = sizeof(Indices) / sizeof(GLuint);
	//set EBO
	glNamedBufferData(EBO, sizeof(Indices), Indices, GL_STATIC_DRAW);

	update();
}

void Wall::update()
{
	//update vertices
	std::vector<Vertex> vertices;
	vertices.push_back(p1);
	vertices.push_back(p2);
	vertices.push_back(p3);
	vertices.push_back(p4);
	//put data in vbo 
	glNamedBufferData(VBO, sizeof(Vertex) * vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);
	//set rigid body
	std::vector<glm::vec3> points;
	points.push_back(p1.position);
	points.push_back(p2.position);
	points.push_back(p3.position);
	points.push_back(p4.position);	
}


void Wall::draw(Shader& shaderProgram, Camera& camera)
{
	shaderProgram.Active();
	//send camera matrix
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.id, "cameraMatrix"), 1, GL_FALSE, glm::value_ptr(camera.getMatrix()));
	//send position matrix
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.id, "position"), 1, GL_FALSE, glm::value_ptr(movement));
	//send rotation matrix
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.id, "rotation"), 1, GL_FALSE, glm::value_ptr(rotationMatrix));

	Mesh::draw(shaderProgram);
}
void Wall::draw(Shader& shaderProgram, Camera& camera, glm::vec3 loc, glm::vec3 direction)
{
	shaderProgram.Active();
	//send camera matrix
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.id, "cameraMatrix"), 1, GL_FALSE, glm::value_ptr(camera.getMatrix(loc, direction)));
	//send position matrix
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.id, "position"), 1, GL_FALSE, glm::value_ptr(movement));
	//send rotation matrix
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.id, "rotation"), 1, GL_FALSE, glm::value_ptr(rotationMatrix));

	Mesh::draw(shaderProgram);
}

glm::vec3 Wall::getPoint(int triangle, int point)
{
	std::vector<glm::vec3> points;
	points.push_back(p1.position);
	points.push_back(p2.position);
	points.push_back(p3.position);
	points.push_back(p4.position);
	return points[Indices[triangle * 3 + point]];
}
glm::vec3 Wall::getPoint(int i)
{
	std::vector<glm::vec3> points;
	points.push_back(p1.position);
	points.push_back(p2.position);
	points.push_back(p3.position);
	points.push_back(p4.position);
	return points[i];
}

void Wall::move(glm::vec3 location)
{
	center = location;
}
void Wall::setDirection(glm::vec3 rot, float angle)
{
	rotation = Quaternion(rot, angle);
	rotationMatrix = rotation.getMatrix();
}
