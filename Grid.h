#pragma once
#include<glm/glm.hpp>
#include<vector>
#include"Model.h"
#include"QuadTree.h"


template<typename T>
struct doubleArray
{
	std::vector<T> positive;
	std::vector<T> negative;

	T& operator[] (int i);
	void resize(int size);
};

class Grid
{
public:
	doubleArray<doubleArray<doubleArray<std::vector<int>>>> chunkGrid;
	std::vector<HitBox>* hitboxList;
	int chunkSize;

	Grid(int chunkSize, std::vector<HitBox>* hitboxList);

	void insert(int hitboxNum);
	std::vector<int> getData(HitBox& hitbox);

	static std::vector<int> unDupe(std::vector<HitBox>* hitboxArray, std::vector<int> dupe);

private:
	 glm::vec3 getStart(HitBox& hitbox);
	 glm::vec3 getEnd(HitBox& hitbox);
	int round(float num);

};

