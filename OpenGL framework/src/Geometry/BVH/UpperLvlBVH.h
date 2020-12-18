#pragma once
#include "Geometry/BVH/BVH.h"

// Upper level BVH, this holds references to all BVHs it owns 
// Bvh data is put here in a contiguous array for all BVHs it owns
class BVH;
class MeshComponent;
class BVHComponent;

class UpperLvlBVH
{
	friend BVH;
	friend BVHNode;

	size_t m_Size = 0;	
	size_t m_Offset = 0;
	size_t m_Reserved = 3000;
	uint32_t m_BVH_SSBO = 0;
	uint32_t m_BVH_Index_SSBO = 0;
	uint32_t m_BVH_Triangle_SSBO = 0;
	std::vector<BVH> m_BVHs;
	std::vector<BVHNode> m_BVHBuffer;
	std::vector<uint32_t> m_BVHIndexBuffer;
	std::vector<glm::vec4> m_BVHTriangleBuffer;
	
	void UpdateBuffer(size_t start, size_t end);

public:
	void InitBuffers(void* dataBVH, void* dataIndex, void* dataTriangle);
	void Unbind();
	void Bind(uint32_t BVHIdx = 0, uint32_t indexBufferIdx = 1, uint32_t triangleBufferIndex = 2);
	UpperLvlBVH();
	void Draw(Camera& camera, const glm::mat4& transform, BVHComponent& bvhc);
	void AddBVH(MeshComponent& mc);
	BVH& GetBVH(int i);
	uint32_t GetBVHCount();
};