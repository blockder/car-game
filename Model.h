#pragma once
#include<glad/glad.h>
#include"Mesh.h"
#include"Vertex.h"
#include<glm/glm.hpp>
#include"Texture.h"
#include<vector>
#include<string>
#include"Camera.h"

class Model : public Mesh
{
public:
	Texture texture;
	std::vector<Vertex> Vertices;
	std::vector<glm::vec3> points;
	std::vector<GLuint> Indices;

	Model() = default;
	Model(std::string ModelFile, std::string texture);
	Model(std::string texture, std::vector<Vertex> Vertices, std::vector<GLuint> Indices);
	Model(std::string texture, std::vector<Vertex> Vertices, std::vector<GLuint> Indices, std::vector<glm::vec3> points);

	void draw(Shader& shaderProgram) override;
	void draw(Shader& shaderProgram, Camera& camera);

	glm::vec3 getPoint(int triangle, int point);

	static std::vector<Model> loadModels(std::string filePath, std::string texture);
	void loadVBO();
protected:
private:
	void loadModel(std::string ModelFile);
	void setDefaultIndices();
	static std::vector<std::string> split(std::string string, std::string delimiter);
	static Vertex readFace(std::string string, std::vector<glm::vec3>& positions, std::vector<glm::vec2>& UVcoordinates);
};