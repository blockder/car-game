#version 460 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 normal;
out vec3 color;
out vec2 texCoord;

uniform mat4 cameraMatrix;

in DATA
{	
	vec3 color;
	vec2 texCoord;
} data_in[];

void main()
{
	//calculate normal
	vec3 normal = vec3(0,0,0);
	vec3 vector0 = vec3(gl_in[2].gl_Position - gl_in[1].gl_Position);
	vec3 vector1 = vec3(gl_in[0].gl_Position - gl_in[1].gl_Position);
	normal = normalize(cross(vector0, vector1));
	
	color = data_in[0].color;
	texCoord = data_in[0].texCoord;
	gl_Position = cameraMatrix * gl_in[0].gl_Position;
	EmitVertex();

	color = data_in[1].color;
	texCoord = data_in[1].texCoord;
	gl_Position = cameraMatrix * gl_in[1].gl_Position;
	EmitVertex();

	color = data_in[2].color;
	texCoord = data_in[2].texCoord;
	gl_Position = cameraMatrix * gl_in[2].gl_Position;
	EmitVertex();

	EndPrimitive();
}