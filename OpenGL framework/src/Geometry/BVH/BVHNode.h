#pragma once
#include "Geometry/BVH/BVH.h"
#include "GameObject/Components/AABB.h"

class BVHNode
{
public:
	AABB m_bounds;

	BVHNode();

	void Subdivide(BVH& bvh, const std::vector<AABB>& boundingBoxes, const std::vector<Triangle>& triangles, uint32_t start, uint32_t end);
	bool Traverse(BVH& bvh, const Ray& ray, std::vector<HitData>& hitData, unsigned nodeIdx) const;
	static AABB CalculateAABB(const BVH& bvh, const std::vector<AABB>& AABBs, unsigned first, unsigned last);
	uint32_t Partition(const BVHNode& parent, BVH& bvh, const std::vector<AABB>& boundingBoxes, uint32_t start, uint32_t end) const;
	float CombineSAH(BVH& bvh, const std::vector<AABB>& boundingBoxes, uint32_t start, uint32_t end);
};

