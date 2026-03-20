#include"Block.h"
#include"Camera.h"
#include"Shader.h"
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/vector_angle.hpp>
#include<vector>
#include"Math.h"




Block::Block(std::string texture, std::vector<Vertex> Vertices, std::vector<GLuint> Indices, glm::vec3 location, float width, float height, float depth)
	: Model(texture, Vertices,  Indices)
{
	this->center = location;

	glm::mat3 model(width, 0, 0, 0, height, 0, 0, 0, depth);
	//apply model matrix to points 
	for (int i = 0; i < points.size(); i++)
	{
		points[i] = points[i] * model;
	}
	//apply model matrix to vertices 
	for (int i = 0; i < Vertices.size(); i++)
	{
		this->Vertices[i].position = Vertices[i].position * model;
	}
	loadVBO();

	//set matrices
	movement = glm::translate(glm::mat4(1.0f), center);
	rotationMatrix = rotation.getMatrix();
}
Block::Block(std::string texture, std::vector<Vertex> Vertices, std::vector<GLuint> Indices, std::vector<glm::vec3> points, glm::vec3 location)
	: Model(texture, Vertices, Indices, points)
{
	this->center = location;
	//set matrices
	movement = glm::translate(glm::mat4(1.0f), center);
	rotationMatrix = rotation.getMatrix();
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

	Model::draw(shaderProgram);
}



void Block::setDirection(glm::vec3 rot, float angle)
{
	rotation = Quaternion(rot, angle);
	rotationMatrix = rotation.getMatrix();
}





