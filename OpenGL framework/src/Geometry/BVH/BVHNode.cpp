#include "precomp.h"
#include "BVHNode.h"

 
glm::vec3 GetCenterTriangle(const Triangle& triangle);

void BVHNode::Subdivide(
	BVH& bvh,
	const std::vector<AABB>& boundingBoxes,
	const std::vector<Triangle>& triangles,
	const int start,
	const int end)
{
	std::cout << "count at: " << bvh.count++ << "\n";

	const uint32_t objcount = end - start;
	m_bounds = CalculateAABB(bvh, boundingBoxes, start, end);
	m_bounds.m_count = objcount;

	if (objcount < 3)
	{
		m_bounds.m_leftFirst = start;
		return; //TODO: SET LEAF COUNT DYNAMICALLY!
	}
	m_bounds.m_leftFirst = bvh.m_poolPtr++;


	BVHNode& l = bvh.m_pool[m_bounds.m_leftFirst];
	BVHNode& r = bvh.m_pool[bvh.m_poolPtr++];

	const uint32_t split = Partition(*this, bvh, triangles, boundingBoxes, start, end);

	l.Subdivide(bvh, boundingBoxes, triangles, start, split);
	r.Subdivide(bvh, boundingBoxes, triangles, split, end);
}

bool BVHNode::Traverse(BVH& bvh, const Ray& ray, std::vector<HitData>& hitData, const unsigned nodeIdx = 0) const
{
	float tCurrent = std::numeric_limits<float>::infinity();
	const bool i = m_bounds.IntersectAABB(ray, tCurrent);

	//return i;
	if (i)
	{
		//leaf
		if (m_bounds.m_count < 3)
		{
			hitData.emplace_back(HitData(tCurrent, nodeIdx)); //TODO COMPOSE HIT DATA
			return true;
		}

		else //traverse children
		{
			const int leftChild = m_bounds.m_leftFirst;
			const int rightChild = m_bounds.m_leftFirst + 1;
			const bool l = bvh.m_pool[leftChild].Traverse(bvh, ray, hitData, leftChild);
			const bool r = bvh.m_pool[rightChild].Traverse(bvh, ray, hitData, rightChild);
			return l | r;
		}
	}
	return false;
}


AABB BVHNode::CalculateAABB(const BVH& bvh, const std::vector<AABB>& AABBs, const unsigned int first, const unsigned int last) const
{
	float minX = INFINITY;
	float minY = INFINITY;
	float minZ = INFINITY;
	float maxX = -std::numeric_limits<float>::infinity();
	float maxY = -std::numeric_limits<float>::infinity();
	float maxZ = -std::numeric_limits<float>::infinity();

	for (unsigned int i = first; i < last; i++)
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


uint32_t BVHNode::Partition(
	const BVHNode& parent, BVH& bvh, const std::vector<Triangle>& triangles,
	const std::vector<AABB>& aabbs, const uint32_t start, const uint32_t end) const
{
	const float sahParent = CalcSAH(parent.m_bounds, parent.m_bounds.m_count);
	int longestAxis = 0;

	const float xlen = (std::abs(parent.m_bounds.m_max.v.x - parent.m_bounds.m_min.v.x));
	const float ylen = (std::abs(parent.m_bounds.m_max.v.y - parent.m_bounds.m_min.v.y));
	const float zlen = (std::abs(parent.m_bounds.m_max.v.z - parent.m_bounds.m_min.v.z));
	float axisline;
	if (xlen > ylen && xlen > zlen) longestAxis = 0, axisline = parent.m_bounds.m_min.v.x;
	else if (ylen > xlen && ylen > zlen) longestAxis = 1, axisline = parent.m_bounds.m_min.v.y;
	else if (zlen > xlen && zlen > ylen) longestAxis = 2, axisline = parent.m_bounds.m_min.v.z;

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
			const glm::vec3 c1 =  bb1.GetCenter();
			const glm::vec3 c2 =  bb2.GetCenter();
			//const glm::vec3 cp1 = GetCenterTriangle(A1);
			//const glm::vec3 cp2 = GetCenterTriangle(B1);
			//c//onst vec3 cp1 = getCenterTriangle( B1 );
			//const vec3 cp2 = getCenterTriangle( B1 );
			//return cp1[longestAxis] < cp2[longestAxis];
			const float d1 = std::abs(axisline - c1[longestAxis]);
			const float d2 = std::abs(axisline - c2[longestAxis]);
			return c1[longestAxis] < c2[longestAxis];
		});

	//get lowest SAH
	float bestSAH = sahParent;
	uint32_t splitIdx = start + 1;
	for (uint32_t i = start + 1; i < end; i++)
	{
		const float SAH_left = CombineSAH(bvh, aabbs, start, i);
		const float SAH_right = CombineSAH(bvh, aabbs, i, end);
		const float SAH = SAH_left + SAH_right;

		if (SAH < bestSAH)
		{
			bestSAH = SAH;
			splitIdx = i;
		}
	}
	if (splitIdx < 0)
	{
		ASSERT(false);
		int asdasd = 1;
	}
	return splitIdx;

}


float BVHNode::CombineSAH(BVH& bvh, const std::vector<AABB>& aabbs, const uint32_t start, const uint32_t end) const
{
	const AABB box = CalculateAABB(bvh, aabbs, start, end);
	const uint32_t count = end - start;
	const float sah = CalcSAH(box, count);
	return sah;
}
 

//calculate surface area heuristic
float BVHNode::CalcSAH(const AABB& aabb, const uint32_t objCount)
{
	const float surfaceArea = aabb.CalcSurfaceArea();
	const float SAH = surfaceArea * static_cast<float>(objCount);
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

//glm::vec3 GetCenterAABB(const AABB& aabb)
//{
//	const glm::vec3 lowerleft = aabb.m_min.v;
//	const glm::vec3 upperright = aabb.m_max.v;
//
//	const glm::vec3 dir = upperright - lowerleft;
//
//	const glm::vec3 center = lowerleft + dir * 0.5f;
//	return center; //{x, y, z};
//
//}

glm::vec3 GetCenterTriangle(const Triangle& tri)
{
	const float t = 1.0f / 3.0f;
	const glm::vec3 aa = glm::vec3(tri.A.x / 3.0f, tri.A.x / 3.0f, tri.A.z / 3.0f);
	const glm::vec3 ab = glm::vec3(tri.B.x / 3.0f, tri.B.x / 3.0f, tri.B.z / 3.0f);
	const glm::vec3 ac = glm::vec3(tri.C.x / 3.0f, tri.C.x / 3.0f, tri.C.z / 3.0f);

	//vec3 c = ( a1.A * t ) + ( a1.B * t ) + a1.C * t;
	glm::vec3 c = aa + ab + ac;
	return c;
}

