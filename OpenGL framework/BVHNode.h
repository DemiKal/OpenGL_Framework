#pragma once
#include "Geometry/BVH/BVH.h"
#include "GameObject/Components/AABB.h"

class BVHNode
{
public:
	int m_start;
	int m_end;
	int m_leftFirst;
	int m_count;
	bool m_init;
	AABB m_bounds;

	BVHNode() : m_start(-1), m_end(-1), m_leftFirst(-1), m_count(-1), m_init(false), m_bounds() {};


	void Subdivide(BVH& bvh, const std::vector<AABB>& aabbs,
		const std::vector<glm::vec3>& triangles, const int start, const int end);
};

