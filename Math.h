#pragma once
#include<glm/glm.hpp>

class Math
{
public:
	static void print(glm::vec4 vector);
	static void print(glm::vec3 vector);
	static void print(glm::vec3, const char* name);
	static void print(glm::vec2 vector);
	static void print(double num);

	static glm::vec3 getNormal(glm::vec3 t1, glm::vec3 t2, glm::vec3 t3);
	static glm::vec3 planeIntersetion(glm::vec3 planeN, glm::vec3 planeP, glm::vec3 vector, glm::vec3 vecLoc);
	static float distanceTwoTriangles(glm::vec3 t1, glm::vec3 t2, glm::vec3 t3, glm::vec3 T1, glm::vec3 T2, glm::vec3 T3, glm::vec3 velocity);
	static bool insideTriangle(glm::vec3 t1, glm::vec3 t2, glm::vec3 t3, glm::vec3 point);
	static bool insidePlane(glm::vec3 t1, glm::vec3 t2, glm::vec3 t3, glm::vec3 point);
	static float distanceTrianglePoint(glm::vec3 t1, glm::vec3 t2, glm::vec3 t3, glm::vec3 point);


private:
	static double distaceLinePoint(glm::vec3 l1, glm::vec3 l2, glm::vec3 point);
	static float distanceTwoLine(glm::vec3 l1, glm::vec3 l2, glm::vec3 k1, glm::vec3 k2);
};