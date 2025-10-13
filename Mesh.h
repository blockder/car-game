#pragma once
#include<glad/glad.h>
#include"Texture.h"
#include<glm/glm.hpp>

class Shader;

class Mesh
{
public:
	Texture texture;
	int EBOsize;

	Mesh(Texture texture);
	Mesh() = default;

	virtual void draw(Shader& shaderProgram);

	virtual void update() = 0;



protected:
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
};