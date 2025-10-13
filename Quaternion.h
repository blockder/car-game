#pragma once
#include<glm/glm.hpp>

class Quaternion
{
public:
	glm::vec4 quaternion;

	Quaternion() = default;
	Quaternion(glm::vec4 quaternion);
	Quaternion(glm::vec3 axis, float angle);
	Quaternion(glm::vec3 angularVelocity);

	Quaternion operator* (Quaternion& other);
	glm::vec3 operator*(glm::vec3 point);
	glm::mat3 operator*(glm::mat3 matrix);
	Quaternion operator+(Quaternion& other);

	glm::mat4 getMatrix();
	void print();
	Quaternion inverse();
private:
};