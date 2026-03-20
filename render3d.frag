#version 460 core

out vec4 FragColor;

in vec3 color;
in vec2 texCoord;
in vec3 normal;

uniform sampler2D texUnit;

void main()
{
	vec3 lightPos = vec3(10,20,0);
	float light = dot(normal, normalize(lightPos)) + 0.2f;
	FragColor = texture(texUnit, texCoord) * vec4(color * light, 1.0f);
}