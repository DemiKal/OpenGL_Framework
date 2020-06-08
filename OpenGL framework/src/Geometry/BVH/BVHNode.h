#pragma once
#include "Geometry/BVH/BVH.h"
#include "GameObject/Components/AABB.h"

class BVHNode
{
public:
	int m_start;		//4	+
	int m_end;			//4	+
	int m_leftFirst;	//4	+
	int m_count;		//4 = 16
	AABB m_bounds;		//6 * 4  = 24 --> 16 + 40  = 40-byte aligned
	Triangle m_triangles[2];
	
	BVHNode() : m_start(-1), m_end(-1), m_leftFirst(-1), m_count(-1)  , m_bounds(), m_triangles() {};


	void Subdivide(BVH& bvh, const std::vector<AABB>& aabbs,
		const std::vector<Triangle>& triangles, int start, int end);
	bool Traverse(BVH& bvh, const Ray& ray, std::vector<HitData>& hitdata, const unsigned int nodeIdx) const;
};

