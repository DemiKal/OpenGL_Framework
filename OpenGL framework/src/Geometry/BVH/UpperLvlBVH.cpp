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
	
	auto& bvhBuffer = m_BVHBuffer.GetBuffer();
	bvhBuffer.reserve(size);

	bvh.StartOffset = bvhBuffer.size();
	std::copy(bvh.m_Pool.begin(), bvh.m_Pool.end(), std::back_inserter(bvhBuffer));
	bvh.EndOffset = bvhBuffer.size();

	bvh.m_Pool.clear();

	const auto& triangles = *(reinterpret_cast<const std::vector<std::array<glm::vec4, 3>>*>(&mesh.m_PositionVertices));

	for (uint32_t i = 0; i < bvh.m_Indices.size(); i++)
	{
		//std::swap(m_BVHTexcoordBuffer[i], m_BVHTexcoordBuffer[bvh.m_Indices[i]]);
		//std::swap(m_BVHTriangleBuffer[i], m_BVHTriangleBuffer[bvh.m_Indices[i]]);
		const uint32_t idx = bvh.m_Indices[i];
		m_TriangleBuffer.m_Buffer.emplace_back(triangles[idx]);
		m_TexcoordBuffer.m_Buffer.emplace_back(mesh.m_UVs[idx]);
	}

	m_BVHs.emplace_back(bvh);

	UpdateBuffer(prevOffset, m_Offset);
	//auto& transf = registry.get<TransformComponent>(entity);

	//m_TopBVHBuffer.push_back({ mc.BoundingBox });
	
}

inline BVH& UpperLvlBVH::GetBVH(int i) { return m_BVHs[i]; }

uint32_t UpperLvlBVH::GetBVHCount() { return m_BVHs.size(); }

void UpperLvlBVH::UpdateBuffer(const size_t start, const size_t end)
{
	//if (m_BVH_SSBO) glDeleteBuffers(1, &m_BVH_SSBO);
	size_t currentSize = sizeof(BVHNode) * m_BVHBuffer.m_Buffer.size();
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

void UpperLvlBVH::UpdateTopBVH(entt::registry& registry)
{

	const uint32_t leafCount = 1;
	BVH bvh;
	bvh.m_LeafCount = leafCount;
	auto view = registry.view<TransformComponent, MeshComponent, BVHComponent>();
	
	m_TransformBuffer.GetBuffer().emplace_back();
	std::vector<AABB> originalAABBs(view.size());
	//update all transforms
	for (auto entity : view)
	{
		const auto& [tc, mc, bvhc] = registry.get<TransformComponent, MeshComponent, BVHComponent>(entity);
		Mesh& mesh = MeshManager::GetMesh(mc.MeshIdx);
		auto idx = bvhc.BVHidx;
		m_TransformBuffer[idx].InverseMat = glm::inverse(tc.CalcMatrix());
		m_TransformBuffer[idx].Offset = m_BVHs[idx].StartOffset;
		originalAABBs[idx] = mesh.m_aabb;
	}

	bvh.BuildTopLevelBVH(originalAABBs, m_TransformBuffer.m_Buffer);

	uint32_t i = 0;
	for (auto& node : bvh.m_Pool)
	{
		if (node.GetCount() <= leafCount)
		{
			auto offset = m_BVHs[i].StartOffset;
			node.SetLeftFirst(offset);	//
			i++;
		}		
	}

	std::vector<BVHNode>& buffer = m_TopBVHBuffer.GetBuffer();
	buffer.clear();
	std::copy(bvh.m_Pool.begin(), bvh.m_Pool.end(), std::back_inserter(buffer));

	m_TopBVHBuffer.Init();
	//for (int i = 0; i < bvh.m_Indices.size(); i++)
	//{
	//	auto ind = bvh.m_Indices[i];
	//	std::swap(m_TopBVHBuffer[i], m_TopBVHBuffer[ind]);	//sort according to indices
	//}


}

void UpperLvlBVH::InitBuffers()
{
	uint32_t elemCount = m_BVHBuffer.m_Buffer.size();

	m_BVHBuffer.Init(0);
	m_TriangleBuffer.Init(1);
	m_TexcoordBuffer.Init(2);
	m_TopBVHBuffer.Init(3);
	m_TransformBuffer.Init(4);

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
	m_BVHBuffer.Bind();
	m_TriangleBuffer.Bind();
	m_TexcoordBuffer.Bind();
	m_TopBVHBuffer.Bind();
	m_TransformBuffer.Bind();
}

UpperLvlBVH::UpperLvlBVH()
{
	//InitBuffers();	//nullptr for init
}

void UpperLvlBVH::DrawTopLevelBVH(Camera& camera)
{
	glm::vec4 color(1, 1, 0, 1);
	BVH bvh;	//create dummy
	//bvh.m_IsBuilt = true;
	m_TopBVHBuffer.Bind(0);	//bind to 0 idx
	bvh.Draw(camera, glm::mat4(1.0f), color, 0, m_TopBVHBuffer.GetBuffer().size());
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	m_TopBVHBuffer.Bind();	//reset binding
}

void UpperLvlBVH::Draw(Camera& camera, const glm::mat4& transform, BVHComponent& bvhc)
{
	BVH& bvh = GetBVH(bvhc.BVHidx);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVH_SSBO);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_BVH_SSBO);
	m_BVHBuffer.Bind();

	bvh.Draw(camera, transform, bvhc.DebugColor, bvh.StartOffset, 0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}