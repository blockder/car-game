#include"Block.h"
#include"Camera.h"
#include"Shader.h"
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/vector_angle.hpp>
#include<vector>
#include"Math.h"

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

 static Vertex w1(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1.0f), glm::vec2(1.0f, 0.0f));
 static Vertex w2(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(1.0f), glm::vec2(0.0f, 0.0f));
 static Vertex w3(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f), glm::vec2(0.0f, 1.0f));
 static Vertex w4(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f), glm::vec2(1.0f, 1.0f));
 static Vertex w5(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1.0f), glm::vec2(1.0f, 1.0f));
 static Vertex w6(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(1.0f), glm::vec2(0.0f, 1.0f));
 static Vertex w7(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(1.0f), glm::vec2(0.0f, 0.0f));
 static Vertex w8(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f), glm::vec2(1.0f, 0.0f));

Block::Block(Texture texture, glm::vec3 location, float width, float height, float depth) : Mesh(texture)
{
	this->center = location;
	this->p1 = w1;
	this->p2 = w2;
	this->p3 = w3;
	this->p4 = w4;
	this->p5 = w5;
	this->p6 = w6;
	this->p7 = w7;
	this->p8 = w8;

	glm::mat3 model(width, 0, 0, 0, height, 0, 0, 0, depth);
	p1.position = p1.position * model;
	p2.position = p2.position * model;
	p3.position = p3.position * model;
	p4.position = p4.position * model;
	p5.position = p5.position * model;
	p6.position = p6.position * model;
	p7.position = p7.position * model;
	p8.position = p8.position * model;


	this->EBOsize = sizeof(Indices) / sizeof(GLuint);
	//set EBO
	glNamedBufferData(EBO, sizeof(Indices), Indices, GL_STATIC_DRAW);

	update();
	//set matrices
	movement = glm::translate(glm::mat4(1.0f), center);
	rotationMatrix = rotation.getMatrix();
}

void Block::update()
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

}

void Block::draw(Shader& shaderProgram, Camera& camera)
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
glm::vec3 Block::getPoint(int triangle, int point)
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
glm::vec3 Block::getPoint(int i)
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

void Block::setDirection(glm::vec3 rot, float angle)
{
	rotation = Quaternion(rot, angle);
	rotationMatrix = rotation.getMatrix();
}





