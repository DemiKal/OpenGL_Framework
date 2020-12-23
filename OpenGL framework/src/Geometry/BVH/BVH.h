#pragma once
#include "Geometry/BVH/BVHNode.h"
#include "GameObject/Components/AABB.h"

class Renderer;
class BVHNode;
class Ray;

//Bounding Volume Hierarchy
class BVH
{
	friend class BVHNode;
protected:

private:
	bool m_IsBuilt = false;
	uint32_t m_BVH_SSBO = 0;
	uint32_t m_Tri_SSBO = 0;
	uint32_t m_TriIdx_SSBO = 0;
	uint32_t m_PoolPtr = 0; //points to current idx while building, becomes size pointer at end
	unsigned int m_TriangleVAO = 0;
	unsigned int m_TriangleVBO = 0;
	uint32_t m_GlobalOffset = 0;

//	void* dataptr;
public:
	uint32_t m_LeafCount = 2;
	uint32_t m_Count = 0;
	std::vector<BVHNode> m_Pool;
	uint32_t StartOffset = 0;
	uint32_t EndOffset = 0;
	uint32_t m_TriangleOffset = 0;
	std::vector<uint32_t> m_Indices;
	std::vector<AABB> m_AABBS;
	std::vector<glm::vec3> m_TriangleCenters;
	uint32_t MeshIdx;

	BVH() = default;
	BVH(std::vector<uint32_t> indices, std::vector<BVHNode> pool, uint32_t poolPtr);
	BVH(std::vector<unsigned> indices, std::vector<BVHNode> pool, BVHNode* root, int poolPtr);
	
	
	void BuildTopLevelBVH(const std::vector<AABB>& originalAABBs, const std::vector<glm::mat4>& transforms);

	void BuildBVH(const std::vector<glm::vec4>& tris, uint32_t offset, uint32_t triangleOffset);
	void Draw(const Camera& camera, const glm::mat4& transform) const;

	void Draw(const Camera& camera, const glm::mat4& transform, const glm::vec4& color, int offset, uint32_t size) const;

	void CastRay(const Ray& ray);
	void DrawTriangle(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C) const;
	void InitTriangleRenderer();
	void CreateBVHTextures();
	void DrawSingleAABB(Camera& cam, uint32_t index);
	void CreateBuffers(const std::vector<Triangle>& tris);
	[[nodiscard]] bool IsBuilt() const;
	[[nodiscard]] uint32_t GetBVHSize() const;
};

