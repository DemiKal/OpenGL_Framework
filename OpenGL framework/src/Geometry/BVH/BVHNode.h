#pragma once
#include "Geometry/BVH/BVH.h"
#include "GameObject/Components/AABB.h"

class BVHNode
{
public:
	//int m_leftFirst;	//4b	+
	//int m_count;		//4b  
	AABB m_bounds;		//8 * 4  = 32 --> 32 + 8 = 40-byte aligned
	
	BVHNode() {};


	void Subdivide(BVH& bvh, const std::vector<AABB>& aabbs,
		const std::vector<Triangle>& triangles, int start, int end);
	bool Traverse(BVH& bvh, const Ray& ray, std::vector<HitData>& hitdata, const unsigned int nodeIdx) const;
};

