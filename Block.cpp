#include"Block.h"
#include"Camera.h"
#include"Shader.h"
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/vector_angle.hpp>
#include<vector>
#include"Math.h"




Block::Block(std::string texture, std::vector<Vertex> Vertices, std::vector<GLuint> Indices, glm::vec3 location, float width, float height, float depth)
	: RigidBody(Model(texture, Vertices, Indices))
{

	glm::mat3 modelMatrix(width, 0, 0, 0, height, 0, 0, 0, depth);
	//apply worldSpace to points
	for (int i = 0; i < model.points.size(); i++)
	{
		model.points[i] = model.points[i] * modelMatrix;
		model.points[i] = model.points[i] + location;
	}
	//apply worldSpace to vertices 
	for (int i = 0; i < model.Vertices.size(); i++)
	{
		model.Vertices[i].position = model.Vertices[i].position * modelMatrix;
		model.Vertices[i].position = model.Vertices[i].position + location;
	}
	model.loadVBO();

}
Block::Block(Model& model, glm::vec3 location) : RigidBody(model)
{
	//apply worldSpace to points 
	for (int i = 0; i < model.points.size(); i++)
	{
		this->model.points[i] =  model.points[i] + location ;
	}
	//apply worldSpace to vertices 
	for (int i = 0; i < model.Vertices.size(); i++)
	{
		this->model.Vertices[i].position = model.Vertices[i].position + location;
	}
	this->model.loadVBO();

}






void Block::setDirection(glm::vec3 rot, float angle)
{
	rotation = Quaternion(rot, angle);
	model.rotationMatrix = rotation.getMatrix();
}





