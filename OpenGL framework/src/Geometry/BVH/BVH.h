#pragma once
#include "Geometry/BVH/BVHNode.h"
#include "GameObject/Components/AABB.h"

class Renderer;
class BVHNode;
class Ray;
struct Triangle;



class BVH
{
	friend class BVHNode;
private:
	bool m_IsBuilt = false;
	BVHNode* m_Root = nullptr;
	uint32_t m_BVH_SSBO = 0;
	uint32_t m_PoolPtr = 0; //points to current idx while building, becomes size pointer at end
	unsigned int m_TriangleVAO = 0;
	unsigned int m_TriangleVBO = 0;
	std::vector<BVHNode> m_Pool;
	std::vector<AABB> m_TriAABBs;
	std::vector<uint32_t> m_Indices;
	std::vector<glm::vec3> m_TriangleCenters;
public:
	uint32_t m_Count = 0;


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

