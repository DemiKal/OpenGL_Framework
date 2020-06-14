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
		triangleIdx(2<<32),
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
	Texture1D	m_aabbNodesTexture;
	Texture1D	m_minTexture;
	Texture1D	m_maxTexture;
	Texture1D	m_triangleTexture;
	Texture1D	m_triangleIdxTexture;
	uint32_t	m_bvh_ssbo;
	bool		m_isBuilt = false;
	BVHNode*	m_root;
	uint32_t	m_poolPtr; //points to current idx while building, becomes size pointer at end
	std::vector<unsigned int> m_indices;
	std::vector<BVHNode> m_pool;
	unsigned int m_triangleVAO = 0;
	unsigned int m_triangleVBO = 0;
public:
	friend class BVHNode;

	uint32_t count = 0;

	BVH() = default;
	BVH(std::vector<unsigned> indices, std::vector<BVHNode> pool, BVHNode* root, int poolPtr);

	Texture1D& GetAABBNodesTexture() { return  m_aabbNodesTexture; }
	Texture1D& GetMinTexture() { return  m_minTexture; }
	Texture1D& GetMaxTexture() { return  m_maxTexture; }
	Texture1D& GetTriangleTexture() { return  m_triangleTexture; }
	Texture1D& GetTriangleIndexTexture() { return  m_triangleIdxTexture; }

 	void BuildBVH(const  Renderer& renderer);
	void Draw(const Camera& camera, Renderer& renderer) const;
	void CastRay(const Ray& ray);
	void DrawTriangle(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C) const;
	void InitTriangleRenderer();
	void CreateBVHTextures();
	void DrawSingleAABB(Camera& cam, uint32_t index);
	void CreateBuffers();
	bool IsBuilt() const { return m_isBuilt; }
	uint32_t GetBVHSize() const { return m_poolPtr; }
	float CalcSah(const AABB& aabb, unsigned objCount);
	float CalcSAH(const AABB& aabb, uint32_t objCount);;

 

	float CalcSAH(const AABB& aabb, const uint32_t objCount) const;

};

