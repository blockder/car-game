#version 460 core

out vec4 FragColor;

in vec3 color;
in vec2 texCoord;
in vec3 normal;
in vec3 coordinate;

uniform sampler2D texUnit;
uniform vec3 lightPos;

void main()
{
	vec3 lightRay = normalize(lightPos - coordinate);
	float light = dot(normal, lightRay) ;
	FragColor = texture(texUnit, texCoord) * vec4(color * light, 1.0f);
}