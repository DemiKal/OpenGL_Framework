#pragma once
//#include "Geometry/BVHNode.h"

#include "GameObject/Components/AABB.h"

class Renderer;
class BVHNode;
class Ray;
struct Triangle;

struct HitData
{
	HitData(float _distance, unsigned int _nodeIdx)
		:
		triangleIdx(0),
		distance(_distance),
		nodeIdx(_nodeIdx)
	{}

	unsigned int triangleIdx;
	float distance;
	unsigned int nodeIdx;
};

class BVH
{
	friend class BVHNode;
private:
	bool m_isBuilt = false;
	BVHNode* m_root = nullptr;
	uint32_t m_bvh_ssbo = 0;
	uint32_t m_poolPtr = 0; //points to current idx while building, becomes size pointer at end
	unsigned int m_triangleVAO = 0;
	unsigned int m_triangleVBO = 0;
	std::vector<BVHNode> m_pool;
	std::vector<AABB> m_triAABBs;
	std::vector<uint32_t> m_indices;
public:
	uint32_t count = 0;


	BVH() = default;
	BVH(std::vector<uint32_t> indices, std::vector<BVHNode> pool, uint32_t poolPtr);
	BVH(std::vector<unsigned> indices, std::vector<BVHNode> pool, BVHNode* root, int poolPtr);
	
	
	void BuildBVH(const Renderer& renderer);
	void Draw(const Camera& camera, Renderer& renderer) const;
	void CastRay(const Ray& ray);
	void DrawTriangle(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C) const;
	void InitTriangleRenderer();
	void CreateBVHTextures();
	void DrawSingleAABB(Camera& cam, uint32_t index);
	void CreateBuffers();
	[[nodiscard]] bool IsBuilt() const;
	[[nodiscard]] uint32_t GetBVHSize() const;
};

