#version 460 core

out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D texUnit;

void main()
{	
	FragColor = texture(texUnit, texCoord) * vec4(1.0f,1.0f, 1.0f, 1.0f);
}