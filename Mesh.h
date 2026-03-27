#pragma once
#include<glad/glad.h>
#include"Texture.h"
#include<glm/glm.hpp>
#include"Shader.h"

class Mesh
{
public:
	Mesh();
	
	virtual void draw(Shader& shaderProgram) = 0;

	virtual void loadVBO() = 0;

protected:
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
};