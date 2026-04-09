#include"Camera.h"
#include"Car.h"
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>


Camera::Camera(glm::vec3 loc, glm::vec3 direction, glm::vec3 up, int screenWidth, int screenHeight)
{
	this->loc = loc;
	this->direction = glm::normalize(direction);
	this->up = up;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
}

glm::mat4 Camera::getMatrix()
{
	glm::mat4 prespective = glm::perspective(FOV, (float)screenWidth / screenHeight, nearPlane, farPlane);
	glm::mat4 lookAt = glm::lookAt(loc, loc + direction, up);

	return prespective * lookAt;
}


void Camera::updateMovement(GLFWwindow* window)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
		return;
	float speed = 0.25f;
	//update movement
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		loc += direction * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		loc -= direction * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		loc += glm::normalize(glm::cross(direction, up)) * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		loc -= glm::normalize(glm::cross(direction, up)) * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		loc += up * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		loc -= up * speed;
	}
	//update direction
	
	double mouseX, mouseY;
	float sensivity = 0.002f;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	 mouseX -= screenWidth / 2;
	 mouseY -= screenHeight / 2;
	//rotate x
	glm::vec3 newDirection = glm::normalize(direction + glm::cross(direction, up) * sensivity * (float)mouseX);
	//rotate y
	newDirection = glm::normalize(newDirection + up * sensivity * (float)-mouseY);
	//don't rotate too much
	if (glm::dot(newDirection, up) < 0.999f && glm::dot(newDirection, up) > -0.999f)
	{
		direction = newDirection;
	}

	glfwSetCursorPos(window, (screenWidth / 2), (screenHeight / 2));
	
}
void Camera::updateMovement(Car& car)
{
	glm::vec3 localForward = car.rotation * glm::vec3(0, 0, 1);
	glm::vec3 localUp = car.rotation * glm::vec3(0, 1, 0);

	
	//update location
	glm::vec3 target = car.center + car.rotation * glm::vec3(0, 2.0f, -2.0f);
	glm::vec3 distance = target - loc;
	loc += distance *  0.2f;


	//update Direction
	glm::vec3 carDirection = localForward;
	direction = carDirection -0.3f * localUp + distance * 0.1f;
	up = localUp;
}