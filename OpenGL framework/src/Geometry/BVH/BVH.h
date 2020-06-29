#pragma once
//#include "Geometry/BVHNode.h"
#include "Geometry/TriangleBuffer.h"
#include "GameObject/Components/AABB.h"
#include "Geometry/Ray.h"
#include "GameObject/Components/Texture1D.h"

class Renderer;
class BVHNode;
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
private:
	uint32_t m_bvh_ssbo;
	BVHNode* m_root;
	uint32_t	m_poolPtr; //points to current idx while building, becomes size pointer at end
	std::vector<uint32_t> m_indices;
	std::vector<BVHNode> m_pool;
	std::vector<AABB> m_triAABBs;
	bool m_isBuilt = false;
	unsigned int m_triangleVAO = 0;
	unsigned int m_triangleVBO = 0;
public:
	friend class BVHNode;

	uint32_t count = 0;

	BVH() = default;
	std::vector<unsigned>& GetAxis(unsigned axis);
	BVH(std::vector<uint32_t> indices, std::vector<BVHNode> pool, uint32_t poolPtr);

	BVH(std::vector<unsigned> indices, std::vector<BVHNode> pool, BVHNode* root, int poolPtr);
	void BuildBVH(const  Renderer& renderer);
	void Draw(const Camera& camera, Renderer& renderer) const;
	void CastRay(const Ray& ray);
	void DrawTriangle(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C) const;
	void InitTriangleRenderer();
	void CreateBVHTextures();
	void DrawSingleAABB(Camera& cam, uint32_t index);
	void CreateBuffers();
	bool IsBuilt() const { return m_isBuilt; }
	[[nodiscard]] uint32_t GetBVHSize() const { return m_poolPtr; }
};

