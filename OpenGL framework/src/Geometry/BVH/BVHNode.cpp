#include "precomp.h"
#include "BVHNode.h"
#include "Geometry/TriangleBuffer.h"

glm::vec3 GetCenterTriangle(const Triangle& triangle);

BVHNode::BVHNode()
{
}

void BVHNode::Subdivide(
	BVH& bvh,
	const std::vector<AABB>& boundingBoxes,
	const std::vector<Triangle>& triangles,
	const uint32_t start,
	const uint32_t end)
{
	bvh.count++;
	const uint32_t objCount = end - start;
	m_bounds = CalculateAABB(bvh, boundingBoxes, start, end);
	//m_bounds.m_count = objCount;
	m_bounds.SetCount(objCount);
	if (objCount <= 2)
	{
		//m_bounds.m_leftFirst = start;
		m_bounds.SetLeftFirst(start);
		return; //TODO: SET LEAF COUNT DYNAMICALLY!
	}
	
	m_bounds.SetLeftFirst(bvh.m_poolPtr++);


	BVHNode& l = bvh.m_pool[m_bounds.GetLeftFirst()];
	BVHNode& r = bvh.m_pool[bvh.m_poolPtr++];

	const uint32_t split = Partition(*this, bvh, boundingBoxes, start, end);

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
		if (m_bounds.GetCount() <= 2)
		{
			hitData.emplace_back(HitData(tCurrent, nodeIdx)); //TODO COMPOSE HIT DATA
			return true;
		}

		else //traverse children
		{
			const int leftChild = m_bounds.GetLeftFirst();
			const int rightChild = leftChild + 1;
			const bool l = bvh.m_pool[leftChild].Traverse(bvh, ray, hitData, leftChild);
			const bool r = bvh.m_pool[rightChild].Traverse(bvh, ray, hitData, rightChild);
			return l | r;
		}
	}
	return false;
}


AABB BVHNode::CalculateAABB(const BVH& bvh, const std::vector<AABB>& AABBs, const unsigned int first, const unsigned int last)
{
	AABB sAABB = AABBs[bvh.m_indices[first]];
	for (unsigned int i = first + 1; i < last; i++)
	{
		const int idx = bvh.m_indices[i];
		sAABB = sAABB.Union(AABBs[idx]);
	}

	return sAABB;
}


uint32_t BVHNode::Partition(
	const BVHNode& parent, BVH& bvh,
	const std::vector<AABB>& boundingBoxes,
	const uint32_t start,
	const uint32_t end) const
{
	const float sahParent = parent.m_bounds.CalcSurfaceArea() * parent.m_bounds.GetCount();
	uint32_t longestAxis = 0;

	const float xlen = std::abs(parent.m_bounds.max.x - parent.m_bounds.min.x);
	const float ylen = std::abs(parent.m_bounds.max.y - parent.m_bounds.min.y);
	const float zlen = std::abs(parent.m_bounds.max.z - parent.m_bounds.min.z);

	if (xlen > ylen && xlen > zlen) longestAxis = 0;
	else if (ylen > xlen && ylen > zlen) longestAxis = 1;
	else if (zlen > xlen && zlen > ylen) longestAxis = 2;

	auto& triangleCenters = bvh.m_triangleCenters;
	//sort indices based on longest axis;
	std::sort(bvh.m_indices.begin() + start, bvh.m_indices.begin() + end,
		[&triangleCenters, longestAxis](const uint32_t a, const uint32_t b) -> bool 
		{
			return triangleCenters[a][longestAxis] < triangleCenters[b][longestAxis];
		});

	//get lowest SAH
	float bestSAH = sahParent;
	uint32_t splitIdx = start + 1;	//index is first triangle of right box
	AABB leftBox = boundingBoxes[bvh.m_indices[start]];
	const uint32_t range = end - start;

	std::vector<AABB> rightBoxes;
	rightBoxes.reserve(range - 1);

	AABB rightBox = boundingBoxes[bvh.m_indices[end - 1]];
	rightBoxes.emplace_back(rightBox);

	for (uint32_t k = end - 2; k >= splitIdx; k--)
	{
		rightBox = rightBox.Union(boundingBoxes[bvh.m_indices[k]]);
		rightBoxes.push_back(rightBox);
	}


	uint32_t countLeft = 1;
	for (uint32_t rightFirstIdx = start + 1; rightFirstIdx < end; rightFirstIdx++)
	{
		const uint32_t countRight = range - countLeft;

		leftBox = leftBox.Union(boundingBoxes[bvh.m_indices[rightFirstIdx - 1]]);
		const float SAH_left = leftBox.CalcSurfaceArea() * countLeft;

		const AABB rBox = rightBoxes[countRight - 1];
		const float SAH_right = rBox.CalcSurfaceArea() * countRight;

		const float SAH = SAH_left + SAH_right;

		countLeft++;
		if (SAH < bestSAH)
		{
			bestSAH = SAH;
			splitIdx = rightFirstIdx;
		}
	}

	return splitIdx;
}


inline float BVHNode::CombineSAH(BVH& bvh, const std::vector<AABB>& boundingBoxes, const uint32_t start, const uint32_t end)
{
	const AABB box = CalculateAABB(bvh, boundingBoxes, start, end);
	const uint32_t count = end - start;
	const float surfaceArea = box.CalcSurfaceArea();
	return surfaceArea * count;
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

