#include "precomp.h"
#include "BVH.h"
#include "Geometry/BVH/BVHNode.h"

void BVH::BuildBVH(const std::vector<Triangle>& triangles, const std::vector<AABB>& aabbs)
{
	const int N = triangles.size();
	//create index array
	for (int i = 0; i < N; i++)
		m_indices.push_back(i);

	m_pool = new BVHNode[N * 2 - 1];
	m_root = &m_pool[0];
	m_poolPtr = 1;

	m_root->m_leftFirst = 1;
	m_root->m_start = 0;
	m_root->m_end = N;
	m_root->m_count = N;
	//root->bounds = CalculateBB( AABBs, 0, N );
	m_root->m_init = true;
	m_root->Subdivide(*this, aabbs, triangles, 0, N);

	for (int i = 0; i < m_poolPtr; i++)
	{
		m_localBounds.emplace_back(m_pool[i].m_bounds);
	}


}
