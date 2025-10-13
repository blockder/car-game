#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTex;

out DATA
{	
	vec3 color;
	vec2 texCoord;
} data_out;


uniform mat4 rotation;
uniform mat4 position;

void main()
{
	data_out.color = aColor;
	data_out.texCoord = aTex;
	gl_Position = position * rotation * vec4(aPos, 1.0f);
}