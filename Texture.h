#pragma once
#include<glad/glad.h>

class Texture
{
public:
	GLuint id;
	unsigned char* bytes;
	const char* source; 

	Texture(const char* source);
	Texture() = default;

	void bind(GLuint unit);

	void unbind(GLuint unit);

private:
	int width;
	int height;
	int numColch;
};