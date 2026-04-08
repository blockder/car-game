#version 460 core

out vec4 FragColor;

in vec3 color;
in vec2 texCoord;
in vec3 normal;
in vec3 coordinate;

uniform sampler2D texUnit;
uniform vec3 lightPos;
uniform vec3 cameraLoc;

void main()
{
	vec3 lightRay = normalize(lightPos - coordinate);
	vec3 cameraRay = normalize(cameraLoc - coordinate);
	float light = dot(normal, lightRay) ;
	float specularLight = pow(dot(reflect(-lightRay, normal), cameraRay), 4) * 3.0f;
	float totalLight = light  + 1.0f;

	FragColor = texture(texUnit, texCoord) * vec4(color * totalLight, 1.0f) ;
}