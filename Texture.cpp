#include"Texture.h"
#include"stb/stb_image.h"
#include<iostream>

Texture::Texture(const char* source)
{
	stbi_set_flip_vertically_on_load(true);
	this->bytes = stbi_load(source, &width, &height, &numColch, 0); 
	this->source = source;
	if (!bytes)
		throw std::runtime_error("Failed to load texture");

	glCreateTextures(GL_TEXTURE_2D, 1, &id);
	//set parameters
	glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);

	int mipmapLevel = 1;
	//put image into texture object
	if (numColch == 4)
	{
		glTextureStorage2D(id, mipmapLevel, GL_RGBA8, width, height);
		glTextureSubImage2D(id, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	}
	else if (numColch == 3)
	{
		glTextureStorage2D(id, mipmapLevel, GL_RGB8, width, height);
		glTextureSubImage2D(id, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, bytes);
	}
	else if (numColch == 1)
	{
		glTextureStorage2D(id, mipmapLevel, GL_R8, width, height);
		glTextureSubImage2D(id, 0, 0, 0, width, height, GL_RED, GL_UNSIGNED_BYTE, bytes);
	}
	else throw std::invalid_argument("automatic texture type recognition falied");

	glGenerateTextureMipmap(id);

	stbi_image_free(bytes);
}

void Texture::bind(GLuint unit)
{
	glBindTextureUnit(unit, id);
}
void Texture::unbind(GLuint unit)
{
	glBindTextureUnit(unit, 0);
}