#include "precomp.h"
#include "BVHNode.h"


glm::vec3 GetCenterTriangle(const Triangle& triangle);

std::vector<uint32_t> filterIdx(const std::vector<uint32>& OGidx, const std::unordered_set<uint32_t> &filter )
{
	std::vector<uint32_t> copy;
	copy.reserve(OGidx.size());
	std::copy_if(OGidx.begin(), OGidx.end(), std::back_inserter(copy),
		[&filter](const uint32_t i)
		{
			return filter.find(i) != filter.end();
		});
	
	return copy;
}

void BVHNode::Subdivide(
	BVH& bvh,
	const std::vector<AABB>& boundingBoxes,
	const std::vector<Triangle>& triangles,
	const std::vector<uint32_t>& indX,
	const std::vector<uint32_t>& indY,
	const std::vector<uint32_t>& indZ,
	const uint32_t startRecursion)
{
	fmt::print("Count at: {}.\n", bvh.count++);
	const uint32_t start = 0;
	const uint32_t end = indX.size();
	
	const uint32_t objCount = end - start;
	m_bounds = CalculateAABB(bvh, boundingBoxes, start, end, indX, indY, indZ);

	
	m_bounds.SetCount(objCount);

	if (objCount <= 2)
	{
		for (uint32_t i = 0; i < objCount; i++)
			bvh.m_indices.push_back(indX[i]);

		//m_bounds.m_leftFirst = start;
		m_bounds.SetLeftFirst(startRecursion);
		return; //TODO: SET LEAF COUNT DYNAMICALLY!
	}
	//m_bounds.m_leftFirst = bvh.m_poolPtr++;
	m_bounds.SetLeftFirst(bvh.m_poolPtr++);

	BVHNode& l = bvh.m_pool[m_bounds.GetLeftFirst()];
	BVHNode& r = bvh.m_pool[bvh.m_poolPtr++];

	//add bbox return!
	const uint32_t split = Partition(*this, bvh, boundingBoxes, indX, indY, indZ);

	std::unordered_set<uint32_t> left;
	std::unordered_set<uint32_t> right;

	auto longestAxis = m_bounds.GetLongestAxis();
	auto& idxSet = longestAxis == 0 ? indX : longestAxis == 1 ? indY : indZ;
	for (uint32_t i = 0; i < split; i++) left.insert(idxSet[i]);
	for (uint32_t i = split; i < end; i++)	right.insert(idxSet[i]);

	const std::vector<uint32_t> indXCopyLeft = filterIdx(indX, left);
	const std::vector<uint32_t> indYCopyLeft = filterIdx(indY, left);
	const std::vector<uint32_t> indZCopyLeft = filterIdx(indZ, left);

	/*indXCopyLeft.reserve(end);
	std::vector<uint32_t> indXCopyLeft; indXCopyLeft.reserve(end);
	std::copy_if(indX.begin(), indX.end(), indXCopyLeft.begin(),
		[&left](const uint32_t i)
		{
			return left.find(i) != left.end();
		});;
			std::vector<uint32_t> indYCopyLeft(indY.size());
	std::copy_if(indYCopyLeft.begin(), indYCopyLeft.end(), indYCopyLeft.begin(),
		[&left](const uint32_t i)
		{
			return left.find(i) != left.end();
		});;
	std::vector<uint32_t> indZCopyLeft(indZ.size());
	std::copy_if(indZCopyLeft.begin(), indYCopyLeft.end(), indZCopyLeft.begin(),
		[&left](const uint32_t i)
		{
			return left.find(i) != left.end();
		});;*/

	uint32_t endFirst = startRecursion + split;
	l.Subdivide(bvh, boundingBoxes, triangles, indXCopyLeft, indYCopyLeft, indZCopyLeft, startRecursion);

	const std::vector<uint32_t> indXCopyRight = filterIdx(indX, right);
	const std::vector<uint32_t> indYCopyRight = filterIdx(indY, right);
	const std::vector<uint32_t> indZCopyRight = filterIdx(indZ, right);

	r.Subdivide(bvh, boundingBoxes, triangles, indXCopyRight, indYCopyRight, indZCopyRight, endFirst);
}

bool BVHNode::Traverse(BVH& bvh, const Ray& ray, std::vector<HitData>& hitData, const unsigned nodeIdx = 0) const
{
	float tCurrent = std::numeric_limits<float>::infinity();
	const bool i = m_bounds.IntersectAABB(ray, tCurrent);

	//return i;
	if (i)
	{
		//leaf
		if (m_bounds.GetCount() < 3)
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


AABB BVHNode::CalculateAABB(
	const BVH& bvh, 
	const std::vector<AABB>& AABBs,
	const unsigned int first,
	const unsigned int last,
	const std::vector<uint32_t>& indX,
	const std::vector<uint32_t>& indY, 
	const std::vector<uint32_t>& indZ)
{
	//auto& indices = axis == 0 ? indX : axis == 1 ? indY : indZ;
	AABB sAABB = AABBs[indX[0]];
	//for (unsigned int i = first + 1; i < last; i++)
	//{
	//	const int idx = indices[i];
	//	sAABB = sAABB.Union(AABBs[idx]);
	//}

	for (unsigned int i : indX)
	{
		//const int idx = indices[i];
		sAABB = sAABB.Union(AABBs[i]);
	}

	return sAABB;
}

uint32_t BVHNode::Partition(
	const BVHNode& parent, BVH& bvh,
	const std::vector<AABB>& boundingBoxes,
	const std::vector<uint32_t>& indicesX,
	const std::vector<uint32_t>& indicesY,
	const std::vector<uint32_t>& indicesZ) const
{
	const float sahParent = parent.m_bounds.CalcSurfaceArea() * parent.m_bounds.GetCount();
	const uint32_t longestAxis = parent.m_bounds.GetLongestAxis();

	const auto& indices = longestAxis == 0 ? indicesX : longestAxis == 1 ? indicesY : indicesZ;
	const uint32_t start = 0;
	const uint32_t end = indices.size();

	//get lowest SAH
	float bestSAH = sahParent;
	uint32_t splitIdx = start + 1;	//index is first triangle of right box
	AABB leftBox = boundingBoxes[indices[start]];
	const uint32_t range = end - start;

	std::vector<AABB> rightBoxes;
	rightBoxes.reserve(range - 1);

	AABB rightBox = boundingBoxes[indices[end - 1]];
	rightBoxes.emplace_back(rightBox);

	for (uint32_t k = end - 2; k >= splitIdx; k--)
	{
		rightBox = rightBox.Union(boundingBoxes[indices[k]]);
		rightBoxes.push_back(rightBox);
	}

	uint32_t countLeft = 1;
	for (uint32_t rightFirstIdx = start + 1; rightFirstIdx < end; rightFirstIdx++)
	{
		const uint32_t countRight = range - countLeft;

		leftBox = leftBox.Union(boundingBoxes[indices[rightFirstIdx - 1]]);
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

