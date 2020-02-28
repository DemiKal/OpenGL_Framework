#pragma once
//#include "Geometry/BVHNode.h"
#include "Geometry/TriangleBuffer.h"
#include "GameObject/Components/AABB.h"

class BVHNode;
struct Triangle;

//struct BVHNode
//{
//	int start = -1;
//	int end = -1;
//	int leftFirst;
//	int count = -1;
//	bool init = false;
//	AABB bounds;
//};
class BVH
{
public:
	std::vector<unsigned int> m_indices;
	BVHNode* m_pool{};
	BVHNode* m_root{};
	int m_poolPtr{};
	std::vector<AABB> m_localBounds;

	BVH() = default;
	BVH(std::vector<unsigned int>& indices, BVHNode* pool, BVHNode* root, int poolPtr)
		: m_indices(indices), m_pool(pool), m_root(root), m_poolPtr(poolPtr) {};

	void BuildBVH(const std::vector<Triangle>& triangles, const std::vector<AABB>& aabbs);
	
};

