#pragma once
#include "Geometry/BVH/BVH.h"
#include "GameObject/Components/AABB.h"

class BVHNode
{
public:
	AABB m_bounds;
	
	BVHNode()
	:
	m_bounds()
	{};


	void Subdivide(BVH& bvh, const std::vector<AABB>& boundingBoxes, const std::vector<Triangle>& triangles,   int start, int end);
	bool Traverse(BVH& bvh, const Ray& ray, std::vector<HitData>& hitData, unsigned nodeIdx) const;
	//bool BVHNode::Traverse(BVH& bvh, const Ray& ray, std::vector<HitData>& hitdata, const unsigned nodeIdx = 0) const;
};

