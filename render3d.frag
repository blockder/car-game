#version 460 core

out vec4 FragColor;

in vec3 color;
in vec2 texCoord;
in vec3 normal;

uniform sampler2D texUnit;

void main()
{
	FragColor = texture(texUnit, texCoord) * vec4(color, 1.0f);
}