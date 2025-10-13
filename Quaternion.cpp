#pragma once
#include"Quaternion.h"
#include<glm/gtx/vector_angle.hpp>
#include<iostream>

Quaternion::Quaternion(glm::vec4 quaternion)
{
	this->quaternion = quaternion;
}
Quaternion::Quaternion(glm::vec3 axis, float angle)
{
	angle = glm::radians(angle);
	this->quaternion.x = glm::cos(angle / 2);
	this->quaternion.y = glm::sin(angle / 2) * axis.x;
	this->quaternion.z = glm::sin(angle / 2) * axis.y;
	this->quaternion.w = glm::sin(angle / 2) * axis.z;

	this->quaternion = glm::normalize(quaternion);
}
Quaternion::Quaternion(glm::vec3 angularVelocity)
{
	glm::vec3 axis = angularVelocity;
	float angle = glm::atan(glm::length(angularVelocity));

	this->quaternion.x = glm::cos(angle / 2);
	this->quaternion.y = glm::sin(angle / 2) * axis.x;
	this->quaternion.z = glm::sin(angle / 2) * axis.y;
	this->quaternion.w = glm::sin(angle / 2) * axis.z;

	this->quaternion = glm::normalize(quaternion);
}
Quaternion Quaternion::operator*(Quaternion& other)
{
	glm::vec4 q1 = this->quaternion;
	glm::vec4 q2 = other.quaternion;
	glm::vec3 v1 = glm::vec3(q1.y, q1.z, q1.w);
	glm::vec3 v2 = glm::vec3(q2.y, q2.z, q2.w);
	glm::vec4 result(q1.x*q2.x-glm::dot(v1,v2), q1.x*v2 + q2.x*v1 + glm::cross(v1, v2));

	//result.x = q1.x * q2.x - q1.y * q2.y - q1.z * q2.z - q1.w * q2.w;
	//result.y = q1.x * q2.y + q1.y * q2.x + q1.z * q2.w - q1.w * q2.z;
	//result.z = q1.x * q2.z - q1.y * q2.w + q1.z * q2.x + q1.w * q2.y;
	//result.w = q1.x * q2.w + q1.y * q2.z - q1.z * q2.y + q1.w * q2.x;
	
	return Quaternion(result);
}
glm::vec3 Quaternion::operator*(glm::vec3 point)
{
	Quaternion pos(glm::vec4(0, point.x, point.y, point.z));
	Quaternion opp = inverse();
	Quaternion res =  *this * pos * opp;
	return glm::vec3(res.quaternion.y, res.quaternion.z, res.quaternion.w);
}
glm::mat3 Quaternion::operator*(glm::mat3 matrix)
{
	glm::mat3 res;
	res[0] = *this * matrix[0];
	res[1] = *this * matrix[1];
	res[2] = *this * matrix[2];
	return res;
}
Quaternion Quaternion::operator+ (Quaternion& other)
{
	return Quaternion(this->quaternion + other.quaternion);
}
Quaternion Quaternion::inverse()
{
	glm::vec4 inverse(quaternion.x, -quaternion.y, -quaternion.z, -quaternion.w);
	return Quaternion(inverse);
}
glm::mat4 Quaternion::getMatrix()
{
	glm::mat4 matrix(1.0f);
	matrix[0] = glm::vec4(*this * glm::vec3(1, 0, 0), 0.0f);
	matrix[1] = glm::vec4(*this * glm::vec3(0, 1, 0), 0.0f);
	matrix[2] = glm::vec4(*this * glm::vec3(0, 0, 1), 0.0f);
	return matrix;
}
void Quaternion::print()
{
	std::cout << quaternion.x << ", " << quaternion.y << ", " << quaternion.z << ", " << quaternion.w << std::endl;
}