#pragma once
#include "Geometry/BVH/BVH.h"
#include "GameObject/Components/AABB.h"

class BVHNode
{
public:
	AABB m_bounds;

	BVHNode() {}

	void Subdivide(BVH& bvh, const std::vector<AABB>& boundingBoxes, const std::vector<Triangle>& triangles, const std::vector<uint32_t>&
	               indX, const std::vector<uint32_t>& indY, const std::vector<uint32_t>& indZ, const uint32_t startRecursion);
	bool Traverse(BVH& bvh, const Ray& ray, std::vector<HitData>& hitData, unsigned nodeIdx) const;
	static AABB CalculateAABB(const BVH& bvh, const std::vector<AABB>& AABBs, const unsigned first, const unsigned last, const std::vector<uint32_t>&
	                          indX, const std::vector<uint32_t>& indY, const std::vector<uint32_t>& indZ);
	uint32_t CalcLongestAxis();
	uint32_t Partition(const BVHNode& parent, BVH& bvh, const std::vector<AABB>& boundingBoxes, const std::vector<uint32_t>& indicesX, const
	                   std::vector<uint32_t>& indicesY, const std::vector<uint32_t>& indicesZ) const;
	//static float CombineSAH(BVH& bvh, const std::vector<AABB>& boundingBoxes, uint32_t start, uint32_t end);
};

