#include "precomp.h"
#include "UpperLvlBVH.h"
#include "GameObject/Components/Mesh.h"
#include "GameObject/Components/EntityComponents.h"

void UpperLvlBVH::AddBVH(entt::registry& registry, entt::entity entity, MeshComponent& mc) //TODO: meshidx or entity id?
{
	//const std::vector<glm::vec4> vertexData, uint32_t meshIdx
	BVH bvh;
	bvh.MeshIdx = mc.MeshIdx;
	Mesh& mesh = MeshManager::GetMesh(mc.MeshIdx);
	bvh.BuildBVH(mesh.m_PositionVertices);

	auto size = bvh.GetBVHSize();
	size_t prevOffset = m_Offset;
	m_Offset += size;
	m_BVHBuffer.reserve(size);

	bvh.StartOffset = m_BVHBuffer.size();
	std::copy(bvh.m_Pool.begin(), bvh.m_Pool.end(), std::back_inserter(m_BVHBuffer));
	bvh.EndOffset = m_BVHBuffer.size();

	bvh.m_Pool.clear();
	//bvh.m_TriAABBs.clear(); CHECK THIS



	//bvh.StartIndicesOffset = m_BVHIndexBuffer.size();
	//std::copy(bvh.m_Indices.begin(), bvh.m_Indices.end(), std::back_inserter(m_BVHIndexBuffer));
	//bvh.EndIndicesOffset = m_BVHIndexBuffer.size();
	//bvh.m_Indices.clear();

	const auto& triangles = *(reinterpret_cast<const std::vector<std::array<glm::vec4, 3>>*>(&mesh.m_PositionVertices));

	//std::copy(triangles.begin(), triangles.end(), std::back_inserter(m_BVHTriangleBuffer));
	//std::copy(mesh.m_UVs.begin(), mesh.m_UVs.end(), std::back_inserter(m_BVHTexcoordBuffer));

	for (uint32_t i = 0; i < bvh.m_Indices.size(); i++)
	{
		//std::swap(m_BVHTexcoordBuffer[i], m_BVHTexcoordBuffer[bvh.m_Indices[i]]);
		//std::swap(m_BVHTriangleBuffer[i], m_BVHTriangleBuffer[bvh.m_Indices[i]]);
		const uint32_t idx = bvh.m_Indices[i];
		m_BVHTriangleBuffer.emplace_back(triangles[idx]);
		m_BVHTexcoordBuffer.emplace_back(mesh.m_UVs[idx]);
	}

	m_BVHs.emplace_back(bvh);
	UpdateBuffer(prevOffset, m_Offset);
	auto& transf = registry.get<TransformComponent>(entity);

	//m_TopBVHBuffer.push_back({ mc.BoundingBox });
	UpdateTopBVH(registry);
}

inline BVH& UpperLvlBVH::GetBVH(int i) { return m_BVHs[i]; }

uint32_t UpperLvlBVH::GetBVHCount() { return m_BVHs.size(); }

void UpperLvlBVH::UpdateBuffer(const size_t start, const size_t end)
{
	//if (m_BVH_SSBO) glDeleteBuffers(1, &m_BVH_SSBO);
	size_t currentSize = sizeof(BVHNode) * m_BVHBuffer.size();
	size_t res = sizeof(BVHNode) * m_Reserved;
	InitBuffers();
	//if (currentSize > res)
	//{
	//	m_Reserved = m_BVHBuffer.size();
	//	InitBuffers(m_BVHBuffer.data(), m_BVHIndexBuffer.data(), m_BVHTriangleBuffer.data(), m_BVHTexcoordBuffer.data());
	//	return;
	//}
	//
	//GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVH_SSBO));
	////TODO: check if this is the right way
	//GLCall(glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(BVHNode) * (start), sizeof(BVHNode) * (end), &m_BVHBuffer[start]));
	////glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	//
	//GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVH_Index_SSBO));
	//GLCall(glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(uint32_t) * 3 * (start), sizeof(uint32_t) * 3 * (end), &m_BVHIndexBuffer[start]));
	////glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	//
	//GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVH_Triangle_SSBO));
	//GLCall(glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * 3 * (start), sizeof(glm::vec4) * 3 * (end), &m_BVHTriangleBuffer[start]));
	////glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	//
	//GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVH_Texcoord_SSBO));
	//GLCall(glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec2) * 3 * (start), sizeof(glm::vec2) * 3 * (end), &m_BVHTexcoordBuffer[start]));
}

inline void UpperLvlBVH::UpdateTopBVH(entt::registry& registry)
{
	//for (MeshComponent& mc : m_Meshrefs)
	//{
	//	Mesh& mesh = MeshManager::GetMesh(mc.MeshIdx);
	//
	//}
	const uint32_t leafCount = 1;
	BVH bvh;
	bvh.m_LeafCount = leafCount;
	auto view = registry.view<TransformComponent, MeshComponent, BVHComponent>();
	std::vector<BVHNode> nodes;

	nodes.reserve(view.size());

	for (auto entity : view)
	{
		const auto& [tc, mc, bvhc] = registry.get<TransformComponent, MeshComponent, BVHComponent>(entity);
		Mesh& mesh = MeshManager::GetMesh(mc.MeshIdx);
		auto idx = bvhc.BVHidx;
		//m_TopBVHBuffer[idx].InverseMat = glm::inverse(tc.CalcMatrix());
		nodes.emplace_back();
	}

	bvh.BuildTopLevelBVH(m_TopBVHBuffer, m_TransformBuffer);

	uint32_t i = 0;
	for (auto& node : bvh.m_Pool)
	{
		if (node.GetCount() <= leafCount)
		{
			auto offset = m_BVHs[i].StartOffset;
			node.SetLeftFirst(offset);	//
		}

		i++;
	}

	//for (int i = 0; i < bvh.m_Indices.size(); i++)
	//{
	//	auto ind = bvh.m_Indices[i];
	//	std::swap(m_TopBVHBuffer[i], m_TopBVHBuffer[ind]);	//sort according to indices
	//}


}

void UpperLvlBVH::InitBuffers()
{
	uint32_t elemCount = m_BVHBuffer.size();

	m_BVHBufferSSBO.Init(m_BVHBuffer.data(), sizeof(BVHNode), elemCount, 0);
	//m_IndexBuffer.Init(m_BVHIndexBuffer.data(), sizeof(uint32_t) * 3, elemCount, 1);
	m_TriangleBuffer.Init(m_BVHTriangleBuffer.data(), sizeof(glm::vec4) * 3, elemCount, 1);
	m_TexcoordBuffer.Init(m_BVHTexcoordBuffer.data(), sizeof(glm::vec2) * 3, elemCount, 2);


	//if (m_BVH_SSBO)
	//{
	//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	//	glDeleteBuffers(1, &m_BVH_SSBO);
	//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	//	glDeleteBuffers(1, &m_BVH_Index_SSBO);
	//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	//	glDeleteBuffers(1, &m_BVH_Triangle_SSBO);
	//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	//
	//	glDeleteBuffers(1, &m_BVH_Texcoord_SSBO);
	//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	//}
	//
	//const unsigned int poolSizeBVH = sizeof(BVHNode) * m_Reserved;
	//glGenBuffers(1, &m_BVH_SSBO);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVH_SSBO);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, poolSizeBVH, dataBVH, GL_DYNAMIC_COPY);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_BVH_SSBO);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	//
	//
	//const unsigned int poolSizeBVHIndexBuffer = sizeof(uint32_t) * 3  * m_Reserved;
	//glGenBuffers(1, &m_BVH_Index_SSBO);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVH_Index_SSBO);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, poolSizeBVHIndexBuffer, dataIndex, GL_DYNAMIC_COPY);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_BVH_Index_SSBO);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	//
	//
	//const unsigned int poolSizeBVHTriangleBuffer = sizeof(glm::vec4) * 3 * m_Reserved;
	//glGenBuffers(1, &m_BVH_Triangle_SSBO);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVH_Triangle_SSBO);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, poolSizeBVHTriangleBuffer, dataTriangle, GL_DYNAMIC_COPY);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_BVH_Triangle_SSBO);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	//
	//const unsigned int poolSizeBVHTexcoordBuffer = sizeof(glm::vec2) * 3 * m_Reserved;
	//glGenBuffers(1, &m_BVH_Texcoord_SSBO);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVH_Texcoord_SSBO);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, poolSizeBVHTexcoordBuffer,  nullptr, GL_DYNAMIC_COPY);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_BVH_Texcoord_SSBO);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


}

void UpperLvlBVH::Unbind()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void UpperLvlBVH::Bind(uint32_t BVHIdx, uint32_t indexBufferIdx, uint32_t triangleBufferIndex)
{
	m_BVHBufferSSBO.Bind();
	m_TriangleBuffer.Bind();
	m_TexcoordBuffer.Bind();
}


UpperLvlBVH::UpperLvlBVH()
{
	//InitBuffers();	//nullptr for init
}

void UpperLvlBVH::DrawTopLevelBVH(Camera& camera)
{

	glm::vec4 color(1, 1, 0, 1);



}

void UpperLvlBVH::Draw(Camera& camera, const glm::mat4& transform, BVHComponent& bvhc)
{
	BVH& bvh = GetBVH(bvhc.BVHidx);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVH_SSBO);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_BVH_SSBO);
	m_BVHBufferSSBO.Bind();

	bvh.Draw(camera, transform, bvhc.DebugColor, bvh.StartOffset);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}