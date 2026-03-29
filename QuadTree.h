#pragma once
#include<glm/glm.hpp>
#include"Math.h"
#include<vector>
#include<array>
#include <iostream>
#include"Model.h"

struct HitBox
{
	glm::vec3 blCorner;
	float width;
	float height;
	float depth;
	int pointer;

	HitBox() = default;
	HitBox(glm::vec3 blCorner, float width, float height, float depth);
	HitBox(std::vector<glm::vec3>& points);

	bool contain(HitBox& other);
	bool touch(HitBox& other);

	static std::vector<HitBox> convertToHitbox(std::vector<Model>& models);

};
class QuadTree
{
public:
	//children
	std::array< std::unique_ptr<QuadTree>, 8> children;
	
	//data
	std::vector<HitBox> data;
	HitBox area;

	QuadTree(HitBox& area);

	std::array<HitBox, 8> getChildren();

	void insert(HitBox& hitbox);
	void insert(std::vector<HitBox>& hitboxList);

	std::vector<HitBox*> getData(HitBox& area);

	void print(int level);

};