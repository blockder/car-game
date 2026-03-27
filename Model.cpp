#include"Model.h"
#include<iostream>
#include<fstream>
#include<glm/gtc/type_ptr.hpp>




Model::Model(std::string texture, std::vector<Vertex> Vertices, std::vector<GLuint> Indices) : Mesh()
{
	this->Vertices = Vertices;
	this->Indices = Indices;
	this->texture = Texture(texture.c_str());

	//insert points
	for (int i = 0; i < Vertices.size(); i++)
	{
		this->points.push_back(Vertices[i].position);
	}

	loadVBO();

}

Model::Model(std::string ModelFile, std::string texture) : Mesh()
{
	this->texture = Texture(texture.c_str());

	loadModel(ModelFile);

	loadVBO();
}
std::vector<Model> Model::loadModels(std::string filePath, std::string texture)
{
	std::vector<Model> models;
	//check if file is valid
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		std::cerr << "ERROR: Could not open " << filePath << std::endl;
		return models;
	}

	//go through the file 
	std::vector<glm::vec2> texCoord;
	std::vector<glm::vec3> positions;
	std::vector<GLuint> indices;
	std::string line;
	Model model = Model();
	int pointCount = 0;
	while (std::getline(file, line))
	{
		//split the line
		std::vector<std::string> splitLine = split(line, " ");

		//add position vector
		if (splitLine[0] == "v")
		{
			positions.push_back(glm::vec3(std::stof(splitLine[1]), std::stof(splitLine[2]), std::stof(splitLine[3])));
			model.points.push_back(glm::vec3(std::stof(splitLine[1]), std::stof(splitLine[2]), std::stof(splitLine[3])));
		}

		//add texture coordinates
		if (splitLine[0] == "vt")
			texCoord.push_back(glm::vec2(std::stof(splitLine[1]), std::stof(splitLine[2])));

		//add Vertex
		if (splitLine[0] == "f")
		{
			//triangle 1
			model.Vertices.push_back(readFace(splitLine[1], positions, texCoord));
			model.Vertices.push_back(readFace(splitLine[2], positions, texCoord));
			model.Vertices.push_back(readFace(splitLine[3], positions, texCoord));
			//triangle 2
			model.Vertices.push_back(readFace(splitLine[1], positions, texCoord));
			model.Vertices.push_back(readFace(splitLine[3], positions, texCoord));
			model.Vertices.push_back(readFace(splitLine[4], positions, texCoord));
			
		}

		//insert model
		if (splitLine[0] == "o")
		{
			model.texture = Texture(texture.c_str());
			model.setDefaultIndices();
			model.loadVBO();
			models.push_back(model);
			model = Model();
		}
	}
	//add last model
	model.texture = Texture(texture.c_str());
	model.setDefaultIndices();
	model.loadVBO();
	models.push_back(model);

	return models;
}
void Model::loadVBO()
{
	//put data in VBO 
	glNamedBufferData(VBO, sizeof(Vertex) * Vertices.size(), Vertices.data(), GL_DYNAMIC_DRAW);

	//set EBO
	glNamedBufferData(EBO, Indices.size() * sizeof(GLuint), Indices.data(), GL_STATIC_DRAW);
}
void Model::loadModel(std::string ModelFile)
{
	//check if file is valid
	std::ifstream file(ModelFile);
	if (!file.is_open())
	{
		std::cerr << "ERROR: Could not open " << ModelFile << std::endl;
		return;
	}

	//go through line by line and generate  vertices 
	std::vector<glm::vec2> texCoord;
	std::string line;
	while(std::getline(file, line))
	{
		//split the line
		std::vector<std::string> splitLine = split(line, " ");
		//add position vector
		if (splitLine[0] == "v")
			points.push_back(glm::vec3(std::stof(splitLine[1]), std::stof(splitLine[2]), std::stof(splitLine[3])));

		//add texture coordinates
		if (splitLine[0] == "vt")
			texCoord.push_back(glm::vec2(std::stof(splitLine[1]), std::stof(splitLine[2])));

		//add Vertex
		if (splitLine[0] == "f")
		{
			//triangle 1
			this->Vertices.push_back(readFace(splitLine[1], points, texCoord));
			this->Vertices.push_back(readFace(splitLine[2], points, texCoord));
			this->Vertices.push_back(readFace(splitLine[3], points, texCoord));
			//triangle 2
			this->Vertices.push_back(readFace(splitLine[1], points, texCoord));
			this->Vertices.push_back(readFace(splitLine[3], points, texCoord));
			this->Vertices.push_back(readFace(splitLine[4], points, texCoord));
		}
	}
	//generating EBO
	setDefaultIndices();
}

void Model::setDefaultIndices()
{
	std::vector<GLuint> indices;
	for (int i = 0; i < Vertices.size(); i++)
	{
		indices.push_back(i);
	}
	this->Indices = indices;
}
void Model::draw(Shader& shaderProgram)
{
	shaderProgram.Active();
	int textureUnit = 0;
	//bind VAO and texture
	glBindVertexArray(VAO);
	glBindTextureUnit(textureUnit, texture.id);

	//send texture to shader
	glUniform1i(glGetUniformLocation(shaderProgram.id, "texUnit"), textureUnit);

	//send lightPos to shader
	glm::vec3 lightPos(0,100,0);
	glUniform3fv(glGetUniformLocation(shaderProgram.id, "lightPos"), 1,glm::value_ptr(lightPos));

	//draw call
	glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);
	

	//unbind VAO and texture
	glBindVertexArray(0);
	glBindTextureUnit(textureUnit, 0);
}
void Model::draw(Shader& shaderProgram, Camera& camera)
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
std::vector<std::string> Model::split(std::string string, std::string delimiter)
{
	std::vector<std::string> split;
	while (string.find(delimiter) != std::string::npos)
	{
		split.push_back(string.substr(0, string.find(delimiter)));
		string.erase(0, string.find(delimiter) + delimiter.size());
	}
	split.push_back(string);
	return split;
}
Vertex Model::readFace(std::string string, std::vector<glm::vec3>& points, std::vector<glm::vec2>& texCoord)
{
	std::vector<std::string> v_vt = split(string, "/");
	return Vertex(points[std::stoi(v_vt[0])-1], glm::vec3(1.0f), texCoord[std::stoi(v_vt[1])-1]);
}
glm::vec3 Model::getTrianglePoint(int triangle, int point)
{
	return Vertices[Indices[triangle * 3 + point]].position;
}