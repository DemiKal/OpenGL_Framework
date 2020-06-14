#pragma once
#include "Geometry/BVH/BVH.h"
#include "GameObject/Components/AABB.h"

class BVHNode
{

	//friend class BVH;
public:
	AABB m_bounds;
	
	BVHNode() : m_bounds()	{};
	 

	void Subdivide(BVH& bvh, const std::vector<AABB>& boundingBoxes, const std::vector<Triangle>& triangles,   int start, int end);
	bool Traverse(BVH& bvh, const Ray& ray, std::vector<HitData>& hitData, unsigned nodeIdx) const;
	AABB CalculateAABB(const BVH& bvh, const std::vector<AABB>& AABBs, unsigned first, unsigned last) const;
	uint32_t Partition(const BVHNode& parent, BVH& bvh, const std::vector<Triangle>& triangles,
	                   const std::vector<AABB>& aabbs, uint32_t start, uint32_t end) const;
	float CombineSAH(BVH& bvh, const std::vector<AABB>& aabbs, uint32_t start, uint32_t end) const;
	static float CalcSAH(const AABB& aabb, uint32_t objCount);
};

