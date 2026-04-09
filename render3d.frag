#version 460 core

out vec4 FragColor;

in vec3 color;
in vec2 texCoord;
in vec3 normal;
in vec3 coordinate;

uniform sampler2D texUnit;
uniform vec3 lightPos;
uniform vec3 cameraLoc;

float calculateFogFactor()
{
	float distance = length(coordinate - cameraLoc);
	float minDis = 50.0f;
	float maxDis = 100.0f;
	float range = maxDis - minDis;
	return clamp((maxDis-distance) / range, 0.0, 1.0);
}
void main()
{
	vec3 lightRay = normalize(lightPos - coordinate);
	vec3 cameraRay = normalize(cameraLoc - coordinate);
	float light = dot(normal, lightRay) ;
	float specularLight = pow(dot(reflect(-lightRay, normal), cameraRay), 4) * 3.0f;
	float totalLight = light  + 1.0f;

	vec4 pixelColor = texture(texUnit, texCoord) * vec4(color * totalLight, 1.0f) ;

	//fog
	float fogFactor = calculateFogFactor();
	FragColor = mix(vec4(0.3f,0.5f,0.5f,1.0f), pixelColor, fogFactor);
}