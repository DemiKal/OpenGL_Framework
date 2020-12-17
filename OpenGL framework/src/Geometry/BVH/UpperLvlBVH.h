#pragma once
#include "Geometry/BVH/BVH.h"

// Upper level BVH, this holds references to all BVHs it owns 
// Bvh data is put here in a contiguous array for all BVHs it owns
class BVH;

class UpperLvlBVH
{
	friend BVH;
	friend BVHNode;
	size_t m_Size = 0;	
	size_t m_Offset = 0;

	std::vector<BVH> m_BVHs;
	std::vector<BVHNode> m_BVHBuffer;
	std::vector<uint32_t> m_BVHIndexBuffer;

	void AddBVH(const std::vector<glm::vec4> vertexData, uint32_t meshIdx);

};