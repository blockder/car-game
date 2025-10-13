#include"Mesh.h"
#include"Vertex.h"
#include"Shader.h"




Mesh::Mesh(Texture texture)
{
	//create VAO, EBO and VBO
	glCreateVertexArrays(1, &VAO);
	glCreateBuffers(1, &VBO);
	glCreateBuffers(1, &EBO);

	//connect VBO and EBO to VAO
	glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(Vertex));
	glVertexArrayElementBuffer(VAO, EBO);

	//set layouts
	glEnableVertexArrayAttrib(VAO, 0);													//positions
	glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, (GLuint)0);

	glEnableVertexArrayAttrib(VAO, 1);													//colors
	glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE, (GLuint)3 * sizeof(float));

	glEnableVertexArrayAttrib(VAO, 2);													//texCoord
	glVertexArrayAttribFormat(VAO, 2, 2, GL_FLOAT, GL_FALSE, (GLuint)6 * sizeof(float));
	
	//assign layouts to buffer 0 
	glVertexArrayAttribBinding(VAO, 0, 0);
	glVertexArrayAttribBinding(VAO, 1, 0);
	glVertexArrayAttribBinding(VAO, 2, 0);

	this->texture = texture;

}

void Mesh::draw(Shader& shaderProgram)
{
	shaderProgram.Active();
	int textureUnit = 0;
	//bind VAO and texture
	glBindVertexArray(VAO);
	glBindTextureUnit(textureUnit, texture.id);

	//send texture to shader
	glUniform1i(glGetUniformLocation(shaderProgram.id, "texUnit"), textureUnit);

	//draw call
	glDrawElements(GL_TRIANGLES, EBOsize, GL_UNSIGNED_INT, 0);

	//unbind VAO and texture
	glBindVertexArray(0);
	glBindTextureUnit(textureUnit, 0);

}

