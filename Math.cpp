#include"Math.h"
#include<glm/gtx/vector_angle.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include<iostream>
#include<vector>

void Math::print(glm::vec4 vector)
{
	std::cout << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w << std::endl;
}
void Math::print(glm::vec3 vector)
{
	std::cout << "[" << vector.x << ", " << vector.y << ", " << vector.z << "]" << std::endl;
}
void Math::print(glm::vec3 vector, const char* name)
{
	std::cout << name << " [" << vector.x << ", " << vector.y << ", " << vector.z << "]" << std::endl;
}
void Math::print(glm::vec2 vector)
{
	std::cout << vector.x << ", " << vector.y << std::endl;
}
void Math::print(double num)
{
	std::cout << num << std::endl;
}
glm::vec3 Math::getNormal(glm::vec3 t1, glm::vec3 t2, glm::vec3 t3)
{
	return glm::normalize(glm::cross(t2 - t1, t3 - t1));
}
glm::vec3 Math::planeIntersetion(glm::vec3 planeN, glm::vec3 planeP, glm::vec3 vector, glm::vec3 vecLoc)
{
	glm::vec3 intersectionPoint;
	double t;
	double topEquation = planeN.x * (planeP.x - vecLoc.x) + planeN.y * (planeP.y - vecLoc.y) + planeN.z * (planeP.z - vecLoc.z);
	double buttomEquation = planeN.x * vector.x + planeN.y * vector.y + planeN.z * vector.z;
	t = topEquation / buttomEquation;

	intersectionPoint = vecLoc + vector * (float)t;
	return intersectionPoint;
}
double Math::distaceLinePoint(glm::vec3 l1, glm::vec3 l2, glm::vec3 point)
{
	glm::vec3 v = l2 - l1;
	return std::abs(glm::length(glm::cross(v, point - l1)) / glm::length(v));
}

float Math::distanceTrianglePoint(glm::vec3 t1, glm::vec3 t2, glm::vec3 t3, glm::vec3 point)
{
	glm::vec3 normal = getNormal(t1, t2, t3);
	return glm::dot(normal, point - t1);
}
float Math::distanceTwoLine(glm::vec3 l1, glm::vec3 l2, glm::vec3 k1, glm::vec3 k2)
{
	glm::vec3 vk = glm::normalize(k2 - k1);
	glm::vec3 vl = glm::normalize(l2 - l1);
	//not continue of parallel
	if (vk.y / vk.x == vl.y / vl.x) return 999;
	float x, y, tk, tl;
	if (vl.x == 0)
	{
		x = l1.x;
		tk = (x - k1.x) / vk.x;
		y = k1.y + vk.y * tk;
		tl = (y - l1.y) / vl.y;
	}
	else
	{
		if (vk.x == 0)
		{
			x = k1.x;
			tl = (x - l1.x) / vl.x;
			y = l1.y + vl.y * tl;
			tk = (y - k1.y) / vl.y;
		}
		else
		{
			float m1 = vl.y / vl.x;
			float m2 = vk.y / vk.x;
			float b1 = -m1 * l1.x + l1.y;
			float b2 = -m2 * k1.x + k1.y;
			x = (b2 - b1) / (m1 - m2);
			tk = (x - k1.x) / vk.x;
			tl = (x - l1.x) / vl.x;
			y = l1.y + vl.y * tl;
		}
	}
	glm::vec2 intersection(x, y);
	//check if inside line
	if (glm::distance(glm::vec2(l1), intersection) + glm::distance(glm::vec2(l2), intersection) < glm::distance(glm::vec2(l1), glm::vec2(l2)) + 0.001f
		&& glm::distance(glm::vec2(k1), intersection) + glm::distance(glm::vec2(k2), intersection) < glm::distance(glm::vec2(k1), glm::vec2(k2)) + 0.001f)
	{
		float zk = k1.z + vk.z * tk;
		float zl = l1.z + vl.z * tl;
		return std::abs(zk - zl);
	}
	return 999;
}

bool Math::insideTriangle(glm::vec3 t1, glm::vec3 t2, glm::vec3 t3, glm::vec3 point)
{
	glm::vec3 normal = getNormal(t1, t2, t3);
	if (glm::dot(glm::cross(t3 - t1, point - t1), normal) < 0)
		return false;
	if (glm::dot(glm::cross(t1 - t2, point - t2), normal) < 0)
		return false;
	return glm::dot(glm::cross(t2 - t3, point - t3), normal) > 0;
}
float Math::distanceTwoTriangles(glm::vec3 t1, glm::vec3 t2, glm::vec3 t3, glm::vec3 T1, glm::vec3 T2, glm::vec3 T3, glm::vec3 velocity)
{
	glm::mat4 triangleEye = glm::lookAt(t1, t1 + glm::normalize(velocity), glm::vec3(0.0f, 0.01, 0.0f));
	t1 = triangleEye * glm::vec4(t1, 1.0f);
	t2 = triangleEye * glm::vec4(t2, 1.0f);
	t3 = triangleEye * glm::vec4(t3, 1.0f);
	T1 = triangleEye * glm::vec4(T1, 1.0f);
	T2 = triangleEye * glm::vec4(T2, 1.0f);
	T3 = triangleEye * glm::vec4(T3, 1.0f);
	
	std::vector<float> distances;
	//looking for intersection of edges
	distances.push_back(distanceTwoLine(t1, t2, T1, T2));
	distances.push_back(distanceTwoLine(t1, t2, T2, T3));
	distances.push_back(distanceTwoLine(t1, t2, T3, T1));

	distances.push_back(distanceTwoLine(t2, t3, T1, T2));
	distances.push_back(distanceTwoLine(t2, t3, T2, T3));
	distances.push_back(distanceTwoLine(t2, t3, T3, T1));

	distances.push_back(distanceTwoLine(t3, t1, T1, T2));
	distances.push_back(distanceTwoLine(t3, t1, T2, T3));
	distances.push_back(distanceTwoLine(t3, t1, T3, T1));

	//return smallest distance found
	float result = 999.0f;
	for (int i = 0; i < distances.size(); i++)
	{
		result = std::min(result, distances[i]);
	}
	return result;

}
bool Math::insidePlane(glm::vec3 t1, glm::vec3 t2, glm::vec3 t3, glm::vec3 point)
{
	//anti clockwise
	glm::vec3 normal = getNormal(t1, t2, t3);
	return glm::dot(point - t1, normal) < 0;
}