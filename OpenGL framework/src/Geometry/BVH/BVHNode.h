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
	AABB m_bounds;		//4 * 4  = 16 --> 16 + 16  = 32-byte aligned
	
	BVHNode()
	:
	m_start(-1),
	m_end(-1),
	m_leftFirst(-1),
	m_count(-1),
	m_bounds()
	{};


	void Subdivide(BVH& bvh, const std::vector<AABB>& boundingBoxes, const std::vector<Triangle>& triangles,   int start, int end);
	bool Traverse(BVH& bvh, const Ray& ray, std::vector<HitData>& hitData, unsigned nodeIdx) const;
	//bool BVHNode::Traverse(BVH& bvh, const Ray& ray, std::vector<HitData>& hitdata, const unsigned nodeIdx = 0) const;
};

