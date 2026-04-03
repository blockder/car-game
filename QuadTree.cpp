#include"QuadTree.h"
#include <algorithm>

HitBox::HitBox(glm::vec3 blCorner, float width, float height, float depth)
{
	this->blCorner = blCorner;
	this->width = width;
	this->height = height;
	this->depth = depth;
	triNum = -1;
	modelNum = -1;
}

HitBox::HitBox(std::vector<glm::vec3>& points)
{
	float minX , maxX, minY, maxY, minZ, maxZ;
	minX = maxX = points[0].x;
	minY = maxY = points[0].y;
	minZ = maxZ = points[0].z;

	for (auto& point : points)
	{
		minX = std::min(minX, point.x);
		minY = std::min(minY, point.y);
		minZ = std::min(minZ, point.z);
		maxX = std::max(maxX, point.x);
		maxY = std::max(maxY, point.y);
		maxZ = std::max(maxZ, point.z);
	}

	blCorner = glm::vec3(minX, minY, minZ);
	width = std::abs(maxX - minX);
	height = std::abs(maxY - minY);
	depth = std::abs(maxZ - minZ);
	triNum = -1;
	modelNum = -1;
}
void HitBox::enLarge(float size)
{
	blCorner.x -= (size -1) * 0.5f * width;
	blCorner.y -= (size - 1) * 0.5f * height;
	blCorner.z -= (size - 1) * 0.5f * depth;
	width *= size;
	height *= size;
	depth *= size;
	return;
}
float HitBox::getVolume()
{
	return width * height * depth;
}
bool HitBox::contain(HitBox& other)
{
	glm::vec3 otherCorner = other.blCorner;
	bool insideX = otherCorner.x >= blCorner.x && otherCorner.x + other.width <= blCorner.x + width;
	bool insideY = otherCorner.y >= blCorner.y && otherCorner.y + other.height <= blCorner.y + height;
	bool insideZ = otherCorner.z >= blCorner.z && otherCorner.z + other.depth <= blCorner.z + depth;
	return insideX && insideY && insideZ;
}

bool HitBox::touch(HitBox& other)
{
	glm::vec3 otherCorner = other.blCorner;
	bool touchX = otherCorner.x <= blCorner.x + width && otherCorner.x + other.width >= blCorner.x;
	if (!touchX) return false;
	bool touchY = otherCorner.y <= blCorner.y + height && otherCorner.y + other.height >= blCorner.y;
	if (!touchY) return false;
	bool touchZ = otherCorner.z <= blCorner.z + depth && otherCorner.z + other.depth >= blCorner.z;
	return touchZ;
}

QuadTree::QuadTree(HitBox& area, int level, std::vector<HitBox>* hitboxArray)
{
	this->area = area;
	this->level = level;
	this->hitboxArray = hitboxArray;
	this->leaf = true;
	children.resize(8);
}

std::vector<HitBox> QuadTree::getChildren()
{
	std::vector<HitBox> children(8);
	
	//buttom
	glm::vec3 dbr_corner = area.blCorner; dbr_corner.x += area.width / 2;
	glm::vec3 dtl_corner = area.blCorner; dtl_corner.z += area.depth / 2;
	glm::vec3 dtr_corner = dtl_corner; dtr_corner.x += area.width / 2;

	//up
	glm::vec3 ubl_corner = area.blCorner; ubl_corner.y += area.height / 2;
	glm::vec3 ubr_corner = ubl_corner; ubr_corner.x += area.width / 2;
	glm::vec3 utl_corner = ubl_corner; utl_corner.z += area.depth / 2;
	glm::vec3 utr_corner = utl_corner; utr_corner.x += area.width / 2;

	//create hitboxes for children
	children[0] = HitBox(area.blCorner, area.width/2, area.height/2, area.depth/2);
	children[1] = HitBox(dbr_corner, area.width/2, area.height/2, area.depth/2);
	children[2] = HitBox(dtl_corner, area.width/2, area.height/2, area.depth/2);
	children[3] = HitBox(dtr_corner, area.width/2, area.height/2, area.depth/2);
	children[4] = HitBox(ubl_corner, area.width/2, area.height/2, area.depth/2);
	children[5] = HitBox(ubr_corner, area.width/2, area.height/2, area.depth/2);
	children[6] = HitBox(utl_corner, area.width/2, area.height/2, area.depth/2);
	children[7] = HitBox(utr_corner, area.width/2, area.height/2, area.depth/2);

	return children;
}
std::vector<int> QuadTree::getChildren(HitBox& hitbox)
{
	//will only work in hitbox touches area
	std::vector<int> approvedChildren;
	std::vector<bool> chunks(8, true);

	//try to remove in x
	if (hitbox.blCorner.x + hitbox.width <= area.blCorner.x + area.width/2)
	{
		//delete right
		chunks[1] = chunks[3] = chunks[5] = chunks[7] = false;
	}
	else
	{
		if (hitbox.blCorner.x >= area.blCorner.x + area.width / 2)
		{
			//delete left
			chunks[0] = chunks[2] = chunks[4] = chunks[6] = false;
		}
	}

	//try to remove in y
	if (hitbox.blCorner.y + hitbox.height <= area.blCorner.y + area.height / 2)
	{
		//remove up
		chunks[4] = chunks[5] = chunks[6] = chunks[7] = false;
	}
	else
	{
		if (hitbox.blCorner.y >= area.blCorner.y + area.height / 2)
		{
			//remove down
			chunks[0] = chunks[1] = chunks[2] = chunks[3] = false;
		}
	}

	//try  to remove in z
	if (hitbox.blCorner.z + hitbox.depth <= area.blCorner.z + area.depth / 2)
	{
		//remove far
		chunks[2] = chunks[3] = chunks[6] = chunks[7] = false;
	}
	else
	{
		if (hitbox.blCorner.z >= area.blCorner.z + area.depth / 2)
		{
			//remove close
			chunks[0] = chunks[1] = chunks[4] = chunks[5] = false;
		}
	}

	//return only chunks with true value
	for (int i = 0; i < chunks.size(); i++)
	{
		if (chunks[i])
			approvedChildren.push_back(i);
	}
	return approvedChildren;
}

void QuadTree::insert(std::vector<HitBox>& hitboxList)
{
	for (int i = 0; i < hitboxList.size(); i++)
	{
		if (area.touch(hitboxList[i])) 
		{
			insert(i);
		}
	}
}
void QuadTree::insert(int hitboxNum)
{
	//assume was checked before
	data.push_back(hitboxNum);

	//return if not need to put into children
	if ((leaf && data.size() < 16) || level > 8 )
	{
		return;
	}

	//insert data to children
	std::vector<int> bigObjects;
	std::vector<HitBox> childrenHitBox = getChildren();
	for (int hNum : data)
	{
		HitBox& dataHitbox = (*hitboxArray)[hNum];

		//if size bigger than children size keep here
		if (dataHitbox.getVolume() > area.getVolume() / 8)
		{
			bigObjects.push_back(hNum);
			continue;
		}

		std::vector<int> nextChildren = getChildren(dataHitbox);
		for (int& i : nextChildren)
		{
			if (children[i] == nullptr)
			{
				this->leaf = false;
				children[i] = std::make_unique<QuadTree>(QuadTree(childrenHitBox[i], level +1, hitboxArray));
			}
			children[i]->insert(hNum);
		}
	}
	data = bigObjects;
	return;
}


std::vector<int> QuadTree::getData(HitBox& hitbox)
{
	std::vector<int> collected_data;
	
	//add level data 
	collected_data.insert(collected_data.end(), data.begin(), data.end());
	
	//add children data
	for (int& i : getChildren(hitbox))
	{
		if (children[i])
		{
			//add child data
			std::vector<int> childData = children[i]->getData(hitbox);
			collected_data.insert(collected_data.end(), childData.begin(), childData.end());	
		}
	}
	return collected_data;
}


std::vector<int> QuadTree::unDupe(std::vector<HitBox>* hitboxArray, std::vector<int> dupe)
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

void QuadTree::print(int level)
{
	
	std::cout << "level: " << level << " ,count: " << data.size() << std::endl;

	for (int i = 0; i < children.size(); i++)
	{
		if (children[i])
		{
			children[i]->print(level + 1);
		}
	}
	return;
}

std::vector<HitBox> HitBox::convertToHitbox(std::vector<Model>& models)
{
	std::vector<HitBox> hitboxList;
	for (int k =0; k< models.size(); k++)
	{
		//create a hitbox for each triangle and load into tree
		for (int i = 0; i < models[k].Indices.size() / 3; i++)
		{
			std::vector<glm::vec3> trianglePoints;
			trianglePoints.push_back(models[k].getTrianglePoint(i, 0));
			trianglePoints.push_back(models[k].getTrianglePoint(i, 1));
			trianglePoints.push_back(models[k].getTrianglePoint(i, 2));
			HitBox hitbox = HitBox(trianglePoints);
			hitbox.triNum = i;
			hitbox.modelNum = k;
			hitboxList.push_back(hitbox);
		}
	}
	return hitboxList;
}
