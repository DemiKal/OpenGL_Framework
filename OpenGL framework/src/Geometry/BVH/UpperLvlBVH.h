#pragma once
#include "Geometry/BVH/BVH.h"
#include "Rendering/Buffer/SSBO.h"

// Upper level BVH, this holds references to all BVHs it owns 
// Bvh data is put here in a contiguous array for all BVHs it owns
class BVH;
class MeshComponent;
class BVHComponent;

//this goes to GPU



class UpperLvlBVH
{
	friend BVH;
	friend BVHNode;

	size_t m_Size = 0;
	size_t m_Offset = 0;
	size_t m_Reserved = 3000;

	std::vector<BVH> m_BVHs;
	std::vector<> m_BVHBuffer;
	std::vector< > m_TopBVHBuffer;
	std::vector<TopNodeRef> m_TransformBuffer;
	//std::vector<uint32_t> m_BVHIndexBuffer;
	std::vector<std::array<glm::vec4, 3>> m_BVHTriangleBuffer;
	std::vector<std::array<glm::vec2, 3>> m_BVHTexcoordBuffer;

	SSBO<BVHNode> m_BVHBufferSSBO;
	//SSBO m_IndexBuffer;
	SSBO<BVHNode> m_TriangleBuffer;
	SSBO m_TexcoordBuffer;

	void UpdateBuffer(size_t start, size_t end);
	void UpdateTopBVH(entt::registry& registry);
public:
	void InitBuffers();
	void Unbind();
	void Bind(uint32_t BVHIdx = 0, uint32_t indexBufferIdx = 1, uint32_t triangleBufferIndex = 2);
	UpperLvlBVH();
	void DrawTopLevelBVH(Camera& camera);
	void Draw(Camera& camera, const glm::mat4& transform, BVHComponent& bvhc);
	void AddBVH(entt::registry& registry, entt::entity entity, MeshComponent& mc);
	BVH& GetBVH(int i);
	uint32_t GetBVHCount();
};