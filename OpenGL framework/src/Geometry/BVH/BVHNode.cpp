#include "precomp.h"
#include "BVHNode.h"
int partition(const BVHNode& parent, BVH& bvh, const std::vector<Triangle>& triangles,
	const std::vector<AABB>& aabbs, int start, int end);

AABB calculate_bb(const BVH& bvh, const std::vector<AABB>& aabbs, int first, int last);
float combine_sah(BVH& bvh, const std::vector<AABB>& aabbs, int start, int end);
float calc_sah(const AABB& aabb, int objCount);

glm::vec3 GetCenterAABB(const AABB& aabb);
glm::vec3 GetCenterTriangle(const Triangle& triangle);


void BVHNode::Subdivide(BVH& bvh, const std::vector<AABB>& aabbs,
	const std::vector<Triangle>& triangles, const int start, const int end)
{
	const int objcount = end - start;
	m_bounds = calculate_bb(bvh, aabbs, start, end);
	m_count = objcount;

	if (m_count < 3) return; //TODO: SET LEAF COUNT DYNAMICALLY!

	m_leftFirst = bvh.m_poolPtr++;
	BVHNode& l = bvh.m_pool[m_leftFirst];
	BVHNode& r = bvh.m_pool[bvh.m_poolPtr++];
	l.m_init = true;
	r.m_init = true;

	const int split = partition(*this, bvh, triangles, aabbs, start, end);
	l.m_start = start;
	l.m_end = split;
	r.m_start = split;
	r.m_end = end;

	l.Subdivide(bvh, aabbs, triangles, start, split);
	r.Subdivide(bvh, aabbs, triangles, split, end);
}

bool BVHNode::Traverse(BVH& bvh, const Ray& ray, std::vector<HitData>& hitdata, const unsigned nodeIdx = 0) const
{
	float tCurrent = std::numeric_limits<float>::infinity();
	const bool i = m_bounds.IntersectAABB(ray, tCurrent);

	//return i;
	if (i)
	{
		//leaf
		if (m_count < 3)
		{
			hitdata.emplace_back(HitData(tCurrent, nodeIdx)); //TODO COMPOSE HIT DATA
			return true;
		}

		else //traverse children
		{
			const int leftChild = m_leftFirst;
			const int rightChild = m_leftFirst + 1;
			const bool l = bvh.m_pool[leftChild].Traverse(bvh, ray, hitdata, leftChild);
			const bool r = bvh.m_pool[rightChild].Traverse(bvh, ray, hitdata, rightChild);
			return l | r;
		}
	}
	return false;
}


AABB calculate_bb(const BVH& bvh, const std::vector<AABB>& AABBs, const int first, const int last)
{
	float minX = INFINITY;
	float minY = INFINITY;
	float minZ = INFINITY;
	float maxX = -std::numeric_limits<float>::infinity();
	float maxY = -std::numeric_limits<float>::infinity();
	float maxZ = -std::numeric_limits<float>::infinity();

	for (int i = first; i < last; i++)
	{
		const int idx = bvh.m_indices[i];
		minX = std::min(minX, AABBs[idx].m_min.v.x);
		minY = std::min(minY, AABBs[idx].m_min.v.y);
		minZ = std::min(minZ, AABBs[idx].m_min.v.z);
		maxX = std::max(maxX, AABBs[idx].m_max.v.x);
		maxY = std::max(maxY, AABBs[idx].m_max.v.y);
		maxZ = std::max(maxZ, AABBs[idx].m_max.v.z);
	}

	return { minX, minY, minZ, maxX, maxY, maxZ };
}






int partition(const BVHNode& parent, BVH& bvh, const std::vector<Triangle>& triangles,
	const std::vector<AABB>& aabbs, int start, int end)
{
	const float sahParent = calc_sah(parent.m_bounds, parent.m_count);
	int longestAxis = 0;

	const float xlen = (std::abs(parent.m_bounds.m_max.v.x - parent.m_bounds.m_min.v.x));
	const float ylen = (std::abs(parent.m_bounds.m_max.v.y - parent.m_bounds.m_min.v.y));
	const float zlen = (std::abs(parent.m_bounds.m_max.v.z - parent.m_bounds.m_min.v.z));
	float axisline;
	if (xlen > ylen&& xlen > zlen) longestAxis = 0, axisline = parent.m_bounds.m_min.v.x;
	if (ylen > xlen&& ylen > zlen) longestAxis = 1, axisline = parent.m_bounds.m_min.v.y;
	if (zlen > xlen&& zlen > ylen) longestAxis = 2, axisline = parent.m_bounds.m_min.v.z;

	//sort indices based on longest axis
	std::sort(bvh.m_indices.begin() + start, bvh.m_indices.begin() + end,
		[aabbs, triangles, longestAxis, axisline](const int& a, const int& b) -> bool {
			const Triangle& A1 = triangles[a];
			const Triangle& B1 = triangles[b];
			const AABB bb1 = aabbs[a];
			const AABB bb2 = aabbs[b];

			const float f = 1.0f / 3.0f;
			//  const vec3 c1 = A1.A * f + A1.B * f + A1.C * f;
			//  const vec3 c2 = B1.A * f + B1.B * f + B1.C * f;
			const glm::vec3 c1 = GetCenterAABB(bb1);
			const glm::vec3 c2 = GetCenterAABB(bb2);
			const glm::vec3 cp1 = GetCenterTriangle(A1);
			const glm::vec3 cp2 = GetCenterTriangle(B1);
			//c//onst vec3 cp1 = getCenterTriangle( B1 );
			//const vec3 cp2 = getCenterTriangle( B1 );
			//return cp1[longestAxis] < cp2[longestAxis];
			const float d1 = std::abs(axisline - c1[longestAxis]);
			const float d2 = std::abs(axisline - c2[longestAxis]);
			return c1[longestAxis] < c2[longestAxis];
		});

	//get lowest SAH
	float bestSAH = sahParent;
	int splitIdx = -1;
	for (int i = start + 1; i < end; i++)
	{
		const float SAH_left = combine_sah(bvh, aabbs, start, i);
		const float SAH_right = combine_sah(bvh, aabbs, i, end);
		const float SAH = SAH_left + SAH_right;

		if (SAH < bestSAH)
		{
			bestSAH = SAH;
			splitIdx = i;
		}
	}
	if (splitIdx < 0)
	{
		int asdasd = 1;
	}
	return splitIdx;

}


float combine_sah(BVH& bvh, const std::vector<AABB>& aabbs, const  int start, const int end)
{
	const AABB box = calculate_bb(bvh, aabbs, start, end);
	const int count = end - start;
	const float sah = calc_sah(box, count);
	return sah;


}

//calculate surface area heuristic
float calc_sah(const AABB& aabb, const int objCount)
{
	const float xlen = (aabb.m_max.v.x - aabb.m_min.v.x);
	const float zlen = (aabb.m_max.v.y - aabb.m_min.v.y);
	const float ylen = (aabb.m_max.v.z - aabb.m_min.v.z);
	const float ground = 2 * xlen * zlen;
	const float side1 = 2 * ylen * xlen;
	const float side2 = 2 * ylen * zlen;
	const float SAH = (ground + side1 + side2) * static_cast<float>(objCount);
	return SAH;
}

float getSmallestVertex(const int axis, const Triangle& tri)
{
	float min = INFINITY;
	int idx = -1;
	const float f = 1.0f / 3.0f;
	glm::vec3 center = tri.A * f + tri.B * f + tri.C * f;

	if (tri.A[axis] < min) min = tri.A[axis], idx = 0;
	if (tri.B[axis] < min) min = tri.B[axis], idx = 1;
	if (tri.C[axis] < min) min = tri.C[axis], idx = 2;

	return center[axis];

	//if ( idx == 0 )
	//	return tri.A[axis];
	//else if ( idx == 1 )
	//	return tri.B[axis];
	//else if ( idx == 2 )
	//	return tri.C[axis];
}

glm::vec3 GetCenterAABB(const AABB& aabb)
{
	const glm::vec3 lowerleft = aabb.m_min.v;
	const glm::vec3 upperright = aabb.m_max.v;

	const glm::vec3 dir = upperright - lowerleft;

	const glm::vec3 center = lowerleft + dir * 0.5f;
	return center; //{x, y, z};

}

glm::vec3 GetCenterTriangle(const Triangle& a1)
{
	const float t = 1.0f / 3.0f;
	const glm::vec3 aa = glm::vec3(a1.A.x / 3.0f, a1.A.x / 3, a1.A.z / 3.0f);
	const glm::vec3 ab = glm::vec3(a1.B.x / 3.0f, a1.B.x / 3, a1.B.z / 3.0f);
	const glm::vec3 ac = glm::vec3(a1.C.x / 3.0f, a1.C.x / 3, a1.C.z / 3.0f);

	//vec3 c = ( a1.A * t ) + ( a1.B * t ) + a1.C * t;
	glm::vec3 c = aa + ab + ac;
	return c;
}

