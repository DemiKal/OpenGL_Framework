#pragma once
#include "Geometry/BVH/BVH.h"
#include "Rendering/Buffer/SSBO.h"

// Top level BVH, this holds references to all BVHs it owns 
// Bvh data is put here in a contiguous array for all BVHs it owns
class BVH;
struct MeshComponent;
struct BVHComponent;

class TopLevelBVH
{
	typedef std::array<glm::vec2, 3> UV;
	typedef std::array<glm::vec4, 3> Tri;
	
	friend BVH;
	friend BVHNode;

	size_t m_Size = 0;
	size_t m_Offset = 0;
	size_t m_Reserved = 3000;

	std::vector<BVH> m_BVHs;
	//BVH m_TopLayer;
	SSBO<BVHNode> m_TopBVHBuffer;		//Top level BVH
	SSBO<glm::mat4> m_TransformBuffer;	//references to transform and node index for meshes
	SSBO<uint32_t> m_OffsetBuffer;	//references to transform and node index for meshes
	SSBO<BVHNode> m_BVHBuffer;		//complete BVH buffer of all meshes
	SSBO<Tri> m_TriangleBuffer;	//buffer of all tris of uploaded meshes
	SSBO<UV> m_TexcoordBuffer;	//texcoords

	void UpdateBuffer(size_t start, size_t end);
public:
	void UpdateTopBVH(entt::registry& registry);
	void InitBuffers();
	void Unbind();
	void Bind(uint32_t BVHIdx = 0, uint32_t indexBufferIdx = 1, uint32_t triangleBufferIndex = 2);
	TopLevelBVH();
	void DrawTopLevelBVH(Camera& camera);
	void Draw(Camera& camera, const glm::mat4& transform, BVHComponent& bvhc);
	void AddBVH(entt::registry& registry, entt::entity entity, MeshComponent& mc);
	BVH& GetBVH(int i);
	uint32_t GetBVHCount();
};