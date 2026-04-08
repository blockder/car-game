#pragma once
#include<glm/glm.hpp>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

class Car;
class Camera
{
public:
	glm::vec3 loc;
	glm::vec3 direction;

	Camera(glm::vec3 loc, glm::vec3 direction, glm::vec3 up, int screenWidth, int screenHeight);
	Camera() = default;

	glm::mat4 getMatrix();
	void updateMovement(GLFWwindow* window);
	void updateMovement(Car& car);

private:
	glm::vec3 up;
	int screenWidth;
	int screenHeight;
	float FOV = 90.0f;
	float nearPlane = 0.1f;
	float farPlane = 100.0f;
};