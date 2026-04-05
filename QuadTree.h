#pragma once
#include<glm/glm.hpp>
#include"Math.h"
#include<vector>
#include <iostream>
#include"Model.h"
#include<thread>
#include<future>
#include<mutex>

struct HitBox
{
	glm::vec3 blCorner;
	float width;
	float height;
	float depth;
	int triNum;
	int modelNum;

	HitBox() = default;
	HitBox(glm::vec3 blCorner, float width, float height, float depth);
	HitBox(std::vector<glm::vec3>& points);

	bool contain(HitBox& other);
	bool touch(HitBox& other);

	void enLarge(float size);
	float getVolume();

	static std::vector<HitBox> convertToHitbox(std::vector<Model>& models);

};
class QuadTree
{
public:
	//children
	std::vector<std::unique_ptr<QuadTree>> children;
	
	//data
	std::vector<HitBox>* hitboxArray;
	std::vector<int> data;
	HitBox area;
	int level;
	bool leaf;

	QuadTree(HitBox& area, int level, std::vector<HitBox>* hitboxArray);

	std::vector<HitBox> getChildren();
	std::vector<int> getChildren(HitBox& hitbox);

	void insert(int hitboxNum);
	void insert(std::vector<HitBox>& hitboxList);

	std::vector<int> getData(HitBox& area);


	void print(int level);

	static std::vector<int> unDupe(std::vector<HitBox>* hitboxArray, std::vector<int> dupe);
private:
	

};