#include"Grid.h"
#include"Math.h"


template<typename T>
T& doubleArray<T>::operator[](int i)
{

	if (i > 0)
	{
		return positive[i - 1];
	}
	if (i == 0)
		std::cout << "i == 0 in doubleArray";
	return negative[-i-1];
	
}

template<typename T>
void doubleArray<T>::resize(int size)
{
	if (size > 0)
		positive.resize(size);
	if (size < 0)
		negative.resize(-size);
}



Grid::Grid(int chunkSize, std::vector<HitBox>* hitboxList)
{
	this->chunkSize = chunkSize;
	this->hitboxList = hitboxList;
	
	//insert hotboxList
	for (int i = 0; i < hitboxList->size(); i++)
	{
		insert(i);
	}
	
}
void Grid::insert(int hitboxNum)
{
	HitBox& hitbox = (*hitboxList)[hitboxNum];
	glm::vec3 start = getStart(hitbox);
	glm::vec3 end = getEnd(hitbox);
	
	//resize positive to end or negative to start
	if (end.x > (int)chunkGrid.positive.size())  chunkGrid.resize(end.x); 
	if(start.x < -(int)chunkGrid.negative.size()) chunkGrid.resize(start.x); 

	for (int x = start.x; x <= end.x; x++)
	{
		if (x == 0) continue;
		if (end.y > (int)chunkGrid[x].positive.size())  chunkGrid[x].resize(end.y); 
		if (start.y < -(int)chunkGrid[x].negative.size()) chunkGrid[x].resize(start.y); 

		for (int y = start.y; y <= end.y; y++)
		{
			if (y == 0) continue;
			if (end.z > (int)chunkGrid[x][y].positive.size())	chunkGrid[x][y].resize(end.z);
			if (start.z < -(int)chunkGrid[x][y].negative.size()) chunkGrid[x][y].resize(start.z);

			for (int z = start.z; z <= end.z; z++)
			{
				if (z == 0) continue;
				chunkGrid[x][y][z].push_back(hitboxNum);
			}
		}
	}	
}

std::vector<int> Grid::getData(HitBox& hitbox)
{
	std::vector<int> collected_data;
	glm::vec3 start = getStart(hitbox);
	glm::vec3 end = getEnd(hitbox);
	
	for (int x = std::max((int)start.x, -(int)chunkGrid.negative.size()); x <= std::min((int)end.x, (int)chunkGrid.positive.size()); x++)
	{
		if (x == 0) continue;
		for (int y = std::max((int)start.y, -(int)chunkGrid[x].negative.size()); y <= std::min((int)end.y, (int)chunkGrid[x].positive.size()); y++)
		{
			if (y == 0) continue;
			for (int z = std::max((int)start.z, -(int)chunkGrid[x][y].negative.size()); z <= std::min((int)end.z, (int)chunkGrid[x][y].positive.size()); z++)
			{
				if (z == 0) continue;
				std::vector<int> chunkData = chunkGrid[x][y][z];
				collected_data.insert(collected_data.end(), chunkData.begin(), chunkData.end());
			}

		}
	}
	return collected_data;
}
std::vector<int> Grid::unDupe(std::vector<HitBox>* hitboxArray, std::vector<int> dupe)
{
	std::vector<int> unDupe;
	std::vector<bool> indication(0, false);

	int maxValue = -1;
	for (int& hitboxNum : dupe)
	{
		HitBox& hitbox = (*hitboxArray)[hitboxNum];
		if (hitboxNum > maxValue)
		{
			maxValue = hitboxNum;
			indication.resize(maxValue + 1, false);
		}
		if (indication[hitboxNum] == false)
		{
			indication[hitboxNum] = true;
			unDupe.push_back(hitbox.triNum);
			unDupe.push_back(hitbox.modelNum);
		}
	}
	return unDupe;
}
int Grid::round(float num)
{
	int res = num / chunkSize;
	if (num > 0)
	{
		return res +1;
	}
	if (num < 0)
	{
		return res - 1;
	}
	return 1;
}

glm::vec3 Grid::getStart(HitBox& hitbox)
{
	glm::vec3 start;
	start.x = round(hitbox.blCorner.x);
	start.y = round(hitbox.blCorner.y);
	start.z = round(hitbox.blCorner.z);
	return start;
}
glm::vec3 Grid::getEnd(HitBox& hitbox)
{
	glm::vec3 end;
	end.x = round(hitbox.blCorner.x + hitbox.width);
	end.y = round(hitbox.blCorner.y + hitbox.height);
	end.z = round(hitbox.blCorner.z + hitbox.depth);
	return end;
}
