#pragma once
#include "GameObject/Components/AABB.h"
#include "Geometry/TriangleBuffer.h"

class BVH;

struct HitData
{
	HitData(float _distance, unsigned int _nodeIdx)
		:
		triangleIdx(0),
		distance(_distance),
		nodeIdx(_nodeIdx)
	{}

	unsigned int triangleIdx;
	float distance;
	unsigned int nodeIdx;
};



class BVHNode
{
public:
	AABB m_bounds;

	BVHNode();

	void Subdivide(BVH& bvh, const std::vector<AABB>& boundingBoxes, const std::vector<Triangle>& triangles, const std::vector<glm::vec3>& triangleCenters, uint32_t start, uint32_t end, uint32_t& recursionCount);
	bool Traverse(BVH& bvh, const Ray& ray, std::vector<HitData>& hitData, unsigned nodeIdx) const;
	static AABB CalculateAABB(const BVH& bvh, const std::vector<AABB>& AABBs, unsigned first, unsigned last);
	//uint32_t Partition(const BVHNode& parent, BVH& bvh, const std::vector<AABB>& boundingBoxes, const std::vector<glm::vec3>& triangleCenters, uint32_t start, uint32_t end) const;
	uint32_t Partition(const BVHNode& parent, BVH& bvh, const std::vector<AABB>& boundingBoxes, const std::vector<glm::vec3>& triangleCenters, const uint32_t start, const uint32_t end) const;
	//uint32_t Partition(const BVHNode& parent, BVH& bvh, const std::vector<AABB>& boundingBoxes, const std::vector<glm::vec3>& triangleCenters, const uint32_t start, const uint32_t end, ) const;
	float CombineSAH(BVH& bvh, const std::vector<AABB>& boundingBoxes, uint32_t start, uint32_t end);
};

