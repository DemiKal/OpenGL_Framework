#pragma once
#include "Geometry/BVH/BVH.h"
#include "GameObject/Components/AABB.h"

class BVHNode
{

	//friend class BVH;
public:
	AABB m_bounds;
	
	BVHNode() {};
	 

	static float CalcSAH(const AABB& aabb, uint32_t objCount);
	void Subdivide(BVH& bvh, const std::vector<AABB>& boundingBoxes, const std::vector<Triangle>& triangles, const uint32_t start, const
	               uint32_t end);
	bool Traverse(BVH& bvh, const Ray& ray, std::vector<HitData>& hitData, unsigned nodeIdx) const;
	static AABB CalculateAABB(const BVH& bvh, const std::vector<AABB>& AABBs, unsigned first, unsigned last);
	uint32_t Partition(const BVHNode& parent, BVH& bvh, const std::vector<AABB>& boundingBoxes, 
						uint32_t start, uint32_t end) const;
	static float CombineSAH(BVH& bvh, const std::vector<AABB>& boundingBoxes, uint32_t start, uint32_t end);
};

