#include "precomp.h"
#include "UpperLvlBVH.h"

inline void UpperLvlBVH::AddBVH(const std::vector<glm::vec4> vertexData, uint32_t meshIdx) //TODO: meshidx or entity id?
{
	BVH bvh;
	bvh.MeshIdx = meshIdx;
	bvh.BuildBVH(vertexData);
	auto size = bvh.GetBVHSize();
	m_Offset += size;
	m_BVHBuffer.reserve(size);

	bvh.StartOffset = m_BVHBuffer.size();
	std::copy(bvh.m_Pool.begin(), bvh.m_Pool.end(), m_BVHBuffer.end());
	bvh.EndOffset = m_BVHBuffer.size();

	bvh.m_Pool.clear();
	//bvh.m_TriAABBs.clear(); CHECK THIS

	bvh.StartIndicesOffset = m_BVHIndexBuffer.size();
	std::copy(bvh.m_Indices.begin(), bvh.m_Indices.end(), m_BVHIndexBuffer.end());
	bvh.EndIndicesOffset = m_BVHIndexBuffer.size();

	m_BVHs.emplace_back(bvh);

}
