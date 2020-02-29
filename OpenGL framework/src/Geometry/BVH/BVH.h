#pragma once
//#include "Geometry/BVHNode.h"
#include "Geometry/TriangleBuffer.h"
#include "GameObject/Components/AABB.h"
#include "Geometry/Ray.h"

class BVHNode;
struct Triangle;

struct HitData
{
	HitData(/*unsigned int _triangleIdx,*/ float _distance, unsigned int _nodeIdx) :
	/*	triangleIdx(_triangleIdx),*/ distance(_distance), nodeIdx(_nodeIdx) {}
	unsigned int triangleIdx;
	float distance;
	unsigned int nodeIdx;
};

class BVH
{
public:
	std::vector<unsigned int> m_indices;
	BVHNode* m_pool{};
	BVHNode* m_root{};
	int m_poolPtr{};
	std::vector<AABB> m_localBounds;

	BVH() = default;
	BVH(std::vector<unsigned int>& indices, BVHNode* pool, BVHNode* root, int poolPtr)
		: m_indices(indices), m_pool(pool), m_root(root), m_poolPtr(poolPtr) {};

	void BuildBVH(const std::vector<Triangle>& triangles, const std::vector<AABB>& aabbs);
	void TraceRay(const Ray& ray);
	void DrawTriangle(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C) const;
	void InitTriangleRenderer();
private:
	unsigned int m_triangleVAO;
	unsigned int m_triangleVBO;
};
