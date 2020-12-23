#include "precomp.h"
#include "BVHNode.h"
#include "BVH.h"
//#include "Geometry/Triangle.h"

struct Triangle;

glm::vec3 GetCenterTriangle(const Triangle& triangle);

BVHNode::BVHNode()
{
}

void BVHNode::Subdivide(BVH& bvh, uint32_t start, uint32_t end, uint32_t& recursionCount)
{
	bvh.m_Count++;
	const uint32_t objCount = end - start;
	m_bounds = CalculateAABB(bvh, start, end);
	//m_bounds.m_count = objCount;
	SetCount(objCount);
	if (objCount <= bvh.m_LeafCount)
	{
		//m_bounds.m_leftFirst = start;
		SetLeftFirst(start);
		recursionCount += objCount;
		return; //TODO: SET LEAF COUNT DYNAMICALLY!
	}
	SetLeftFirst(bvh.m_PoolPtr++);

	BVHNode& l = bvh.m_Pool[GetLeftFirst()];
	BVHNode& r = bvh.m_Pool[bvh.m_PoolPtr++];

	const uint32_t split = Partition(*this, bvh, start, end);

	l.Subdivide(bvh, start, split, recursionCount);
	r.Subdivide(bvh, split, end, recursionCount);
}

bool BVHNode::Traverse(BVH& bvh, const Ray& ray, std::vector<HitData>& hitData, const unsigned nodeIdx = 0) const
{
	float tCurrent = std::numeric_limits<float>::infinity();
	const bool i = m_bounds.IntersectAABB(ray, tCurrent);

	//return i;
	if (i)
	{
		//leaf
		if (GetCount() <= bvh.m_LeafCount)
		{
			hitData.emplace_back(HitData(tCurrent, nodeIdx)); //TODO COMPOSE HIT DATA
			return true;
		}

		else //traverse children
		{
			const int leftChild = GetLeftFirst();
			const int rightChild = leftChild + 1;
			const bool l = bvh.m_Pool[leftChild].Traverse(bvh, ray, hitData, leftChild);
			const bool r = bvh.m_Pool[rightChild].Traverse(bvh, ray, hitData, rightChild);
			return l || r;
		}
	}
	return false;
}


AABB BVHNode::CalculateAABB(const BVH& bvh, const uint32_t first, const uint32_t last)
{
	AABB sAABB = bvh.m_AABBS[bvh.m_Indices[first]];
	for (unsigned int i = first + 1; i < last; i++)
	{
		const int idx = bvh.m_Indices[i];
		sAABB = sAABB.Union(bvh.m_AABBS[idx]);
	}

	return sAABB;
}


uint32_t BVHNode::Partition(const BVHNode& parent, BVH& bvh, const uint32_t start, const uint32_t end) const
{
	const float sahParent = parent.m_bounds.CalcSurfaceArea() * parent.GetCount();
	uint32_t longestAxis = 0;

	const float xlen = std::abs(parent.m_bounds.max.x - parent.m_bounds.min.x);
	const float ylen = std::abs(parent.m_bounds.max.y - parent.m_bounds.min.y);
	const float zlen = std::abs(parent.m_bounds.max.z - parent.m_bounds.min.z);

	if (xlen > ylen && xlen > zlen) longestAxis = 0;
	else if (ylen > xlen && ylen > zlen) longestAxis = 1;
	else if (zlen > xlen && zlen > ylen) longestAxis = 2;

	//auto& triangleCenters = bvh.m_TriangleCenters;
	//sort m_Indices based on longest axis;

	//tbb::parallel_sort(bvh.m_Indices.begin() + start, bvh.m_Indices.begin() + end,
	//	[&triangleCenters, longestAxis](const uint32_t a, const uint32_t b) -> bool
	//	{
	//		return triangleCenters[a][longestAxis] < triangleCenters[b][longestAxis];
	//	});


	std::sort(bvh.m_Indices.begin() + start, bvh.m_Indices.begin() + end,
		[&](const uint32_t a, const uint32_t b) -> bool
		{
			return bvh.m_TriangleCenters[a][longestAxis] < bvh.m_TriangleCenters[b][longestAxis];
		});

	//get lowest SAH
	float bestSAH = sahParent;
	uint32_t splitIdx = start + 1;	//index is first triangle of right box
	AABB leftBox = bvh.m_AABBS[bvh.m_Indices[start]];
	const uint32_t range = end - start;

	std::vector<AABB> rightBoxes;
	rightBoxes.reserve(range - 1);

	AABB rightBox = bvh.m_AABBS[bvh.m_Indices[end - 1]];
	rightBoxes.emplace_back(rightBox);

	for (uint32_t k = end - 2; k >= splitIdx; k--)
	{
		rightBox = rightBox.Union(bvh.m_AABBS[bvh.m_Indices[k]]);
		rightBoxes.push_back(rightBox);
	}


	uint32_t countLeft = 1;
	for (uint32_t rightFirstIdx = start + 1; rightFirstIdx < end; rightFirstIdx++)
	{
		const uint32_t countRight = range - countLeft;

		leftBox = leftBox.Union(bvh.m_AABBS[bvh.m_Indices[rightFirstIdx - 1]]);
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
	const AABB box = CalculateAABB(bvh, start, end);
	const uint32_t count = end - start;
	const float surfaceArea = box.CalcSurfaceArea();
	return surfaceArea * count;
}





