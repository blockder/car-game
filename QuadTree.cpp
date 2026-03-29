#include"QuadTree.h"

HitBox::HitBox(glm::vec3 blCorner, float width, float height, float depth)
{
	this->blCorner = blCorner;
	this->width = width;
	this->height = height;
	this->depth = depth;
	pointer = -1;
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
	width = maxX - minX;
	height = maxY - minY;
	depth = maxZ - minZ;
	pointer = -1;
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
	bool touchY = otherCorner.y <= blCorner.y + height && otherCorner.y + other.height >= blCorner.y;
	bool touchZ = otherCorner.z <= blCorner.z + depth && otherCorner.z + other.depth >= blCorner.z;
	return touchX && touchY && touchZ;
}

QuadTree::QuadTree(HitBox& area)
{
	this->area = area;
}

std::array<HitBox, 8> QuadTree::getChildren()
{
	std::array<HitBox, 8> children;
	
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

void QuadTree::insert(std::vector<HitBox>& hitboxList)
{
	for (auto& hitbox : hitboxList)
	{
		insert(hitbox);
	}
}
void QuadTree::insert(HitBox& hitbox)
{
	//check if hitbox is inside node area
	if (!area.contain(hitbox))
		return;

	//check if hitbox is inside a child
	std::array<HitBox, 8> childrenHitBox = getChildren();
	for (int i = 0; i < childrenHitBox.size(); i++)
	{
		if (childrenHitBox[i].contain(hitbox))
		{
			if (children[i] == nullptr)
			{
				children[i] = std::make_unique<QuadTree>(QuadTree(childrenHitBox[i]));
			}
			children[i]->insert(hitbox);
			return;
		}
	}
	
	
	//if hitbox doesn't fit in any child put it here
	data.push_back(hitbox);
	return;
}


std::vector<HitBox*> QuadTree::getData(HitBox& hitbox)
{
	std::vector<HitBox*> collected_data;
	
	//add level data if hitbox intersects
	if (area.touch(hitbox))
	{
		for (auto& dataSample : data)
		{
			collected_data.push_back(&dataSample);
		}
	}

	//add children data
	for (int i = 0; i < 8; i++)
	{
		if (children[i])
		{
			std::vector<HitBox*> childData = children[i]->getData(hitbox);
			collected_data.insert(collected_data.begin(), childData.begin(), childData.end());
		}
	}
	return collected_data;
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
	std::vector<HitBox> result;
	for (auto& model : models)
	{
		//create a hitbox for each triangle and load into tree
		for (int i = 0; i < model.Indices.size() / 3; i++)
		{
			std::vector<glm::vec3> trianglePoints;
			trianglePoints.push_back(model.getTrianglePoint(i, 0));
			trianglePoints.push_back(model.getTrianglePoint(i, 1));
			trianglePoints.push_back(model.getTrianglePoint(i, 2));
			HitBox hitbox = HitBox(trianglePoints);
			hitbox.pointer = i;
			result.push_back(hitbox);
		}
	}
	return result;
}
