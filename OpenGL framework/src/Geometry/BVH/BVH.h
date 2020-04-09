#pragma once
//#include "Geometry/BVHNode.h"
#include "Geometry/TriangleBuffer.h"
#include "GameObject/Components/AABB.h"
#include "Geometry/Ray.h"
#include "GameObject/Components/Texture1D.h"

class BVHNode;
struct Triangle;

struct HitData
{
	HitData(float _distance, unsigned int _nodeIdx)
		:
		distance(_distance),
		nodeIdx(_nodeIdx) 
	{}
	
	unsigned int triangleIdx;
	float distance;
	unsigned int nodeIdx;
};

class BVH
{
private: //todo:: check if unsigned
	//unsigned int m_triangleIndexTexture;
	//unsigned int m_triangleTexture;
	//unsigned int m_indexTexture;
	//unsigned int m_minTexture;
	//unsigned int m_maxTexture;
public:
	std::vector<unsigned int> m_indices;
	BVHNode* m_pool{};
	BVHNode* m_root{};
	int m_poolPtr{};
	std::vector<AABB> m_localBounds;
	std::vector<glm::mat4> m_aabbMatrices;
	Model* m_wireCube;

	Texture1D  m_aabbNodesTexture;
	Texture1D  m_minTexture;
	Texture1D  m_maxTexture;
	Texture1D  m_triangleTexture;
	Texture1D  m_triangleIdxTexture;

	BVH() = default;
	BVH(std::vector<unsigned int>& indices, BVHNode* pool, BVHNode* root, int poolPtr)
		: m_indices(indices),
		m_pool(pool),
		m_root(root),
		m_poolPtr(poolPtr)
	{};

	Texture1D& GetAABBNodesTexture() { return  m_aabbNodesTexture; }
	Texture1D& GetMinTexture() { return  m_minTexture; }
	Texture1D& GetMaxTexture() { return  m_maxTexture; }
	Texture1D& GetTriangleTexture() { return  m_triangleTexture; }
	Texture1D& GetTriangleIndexTexture() { return  m_triangleIdxTexture; }

	void BuildBVH();
	void InitBVHRenderer();
	void Draw(const Camera& camera);
	void TraceRay(const Ray& ray);
	void DrawTriangle(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C) const;
	void InitTriangleRenderer();
	void CreateBVHTextures();
	void DrawSingleAABB(Camera& cam, int index);
private:
	unsigned int m_triangleVAO;
	unsigned int m_triangleVBO;
};

