#include "precomp.h"
#include "TopLevelBVH.h"
#include "GameObject/Components/Mesh.h"
#include "GameObject/Components/EntityComponents.h"
#include "Geometry/Ray.h"

void TopLevelBVH::AddBVH(entt::registry& registry, entt::entity entity, MeshComponent& mc) //TODO: meshidx or entity id?
{
	//const std::vector<glm::vec4> vertexData, uint32_t meshIdx
	BVH bvh;
	bvh.MeshIdx = mc.MeshIdx;
	Mesh& mesh = MeshManager::GetMesh(mc.MeshIdx);

	if (!m_BVHs.empty())
	{
		BVH& last = m_BVHs.back();
		uint32_t endOffset = last.EndOffset;

		uint32_t triOffset = m_TriangleBuffer.GetBuffer().size();

		bvh.BuildBVH(mesh.m_PositionVertices, endOffset, triOffset);
	}
	else
	{
		bvh.BuildBVH(mesh.m_PositionVertices, 0, 0);
	}

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
		const uint32_t idx = bvh.m_Indices[i];
		m_TriangleBuffer.m_Buffer.emplace_back(triangles[idx]);
		m_TexcoordBuffer.m_Buffer.emplace_back(mesh.m_UVs[idx]);
	}

	m_BVHs.emplace_back(bvh);

	UpdateBuffer(prevOffset, m_Offset);
}

inline BVH& TopLevelBVH::GetBVH(int i) { return m_BVHs[i]; }

uint32_t TopLevelBVH::GetBVHCount() { return m_BVHs.size(); }


//std::optional<uint32_t>  TopLevelBVH::TraverseLower(const uint32_t idx, const Ray& ray, float& nearestT)
//{
//	float t;
//	BVHNode& node = m_TopBVHBuffer[idx];
//	if (node.m_bounds.IntersectAABB(ray, t) && t < nearestT)
//	{
//		nearestT = t;
//		const uint32_t count = node.GetCount();
//		if (count > 2)
//		{
//			float t1 = nearestT;
//			float t2 = nearestT;
//			const uint32_t leftFirst = node.GetLeftFirst();
//			auto r1 = Traverse(ray, leftFirst, t1);
//			auto r2 = Traverse(ray, leftFirst + 1, t2);
//
//			if (r1 && r2) return t1 < t2 ? r1 : r2;
//			if (r1 != std::nullopt && r2 == std::nullopt) return r1;
//			if (r1 == std::nullopt && r2 != std::nullopt) return r2;
//			if (r1 == std::nullopt && r2 == std::nullopt) return std::nullopt;
//		}
//		else
//		{
//			const uint32_t childIdx = node.GetLeftFirst();
//			std::vector<std::tuple<uint32_t, float>> hits(count);
//			float nearestOfChildren = nearestT;
//			std::optional<uint32_t> nearestChild;
//
//			for (uint32_t i = 0; i < count; i++)
//			{
//				float nearest;// = nearestOfChildren;
//
//				BVH& bvh = m_BVHs[childIdx];
//				Ray transfRay = ray * m_TransformBuffer[childIdx]; //todo check order?
//
//				auto hitC = TraverseLower(childIdx, transfRay, nearest);
//				if (hitC && nearest < nearestOfChildren)
//					nearestChild = hitC;
//			}
//			return nearestChild;
//		}
//	}
//	 
//	return std::nullopt;
//}

entt::entity TopLevelBVH::PickEntity(const Ray& ray,   entt::registry& registry) const
{
	float nearestT = std::numeric_limits<float>::max();//or infinity?
	entt::entity nearestEntity = entt::null;
	auto view = registry.view<TransformComponent, MeshComponent>();
	for (entt::entity entity : view)
	{
		const auto& [tc, mc] = registry.get<TransformComponent, MeshComponent>(entity);

		const Mesh& mesh = MeshManager::GetMesh(mc.MeshIdx);
		AABB aabb = mesh.m_aabb;
		aabb.Update(tc.CalcMatrix(), mesh.m_aabb);

		float t;
		const bool hasHit = aabb.IntersectAABB(ray, t);

		if (hasHit && t < nearestT)
		{
			nearestT = t;
			nearestEntity = entity;
		}
	}
	return nearestEntity;
	//return Traverse(ray, idx, nearestT, TraversalMode::TraverseTop);
}

entt::entity TopLevelBVH::Traverse(const Ray& ray, const uint32_t idx, float& nearestT, const TraversalMode traversalMode)
{
	float t;
	BVHNode& node = traversalMode == TraversalMode::TraverseTop ? m_TopBVHBuffer[idx] : m_BVHBuffer[idx];//get idx?
	if (node.m_bounds.IntersectAABB(ray, t) && t < nearestT)
	{
		nearestT = t;
		const uint32_t count = node.GetCount();
		if (count > 2)
		{
			float t1 = nearestT;
			float t2 = nearestT;
			const uint32_t leftFirst = node.GetLeftFirst();
			auto r1 = Traverse(ray, leftFirst, t1, TraversalMode::TraverseTop);
			auto r2 = Traverse(ray, leftFirst + 1, t2, TraversalMode::TraverseTop);
			if (r1 != entt::null && r2 != entt::null) return t1 < t2 ? r1 : r2;
			if (r1 != entt::null && r2 == entt::null) return r1;
			if (r1 == entt::null && r2 != entt::null) return r2;
			if (r1 == entt::null && r2 == entt::null) return entt::null;

		}
		else
		{
			const uint32_t childIdx = node.GetLeftFirst();
			std::vector<std::tuple<uint32_t, float>> hits(count);
			float nearestDist = nearestT;
			entt::entity nearestChild = entt::null;

			for (uint32_t i = 0; i < count; i++)
			{
				float nearest;

				BVH& bvh = m_BVHs[childIdx];
				Ray transfRay = ray * m_TransformBuffer[childIdx]; //todo check order?

				const auto current = Traverse(transfRay, childIdx, nearest, TraversalMode::TraverseBottom);
				if (current != entt::null)
				{
					if (nearest < nearestDist) 
						nearestChild = current;
				}
			}
			return nearestChild;
		}
	}

	return entt::null;

}

void TopLevelBVH::UpdateBuffer(const size_t start, const size_t end)
{
	InitBuffers();
	//if (currentSize > res)
	//{
	//	m_Reserved = m_BVHBuffer.size();
	//	InitBuffers(m_BVHBuffer.data(), m_BVHIndexBuffer.data(), m_BVHTriangleBuffer.data(), m_BVHTexcoordBuffer.data());
	//	return;
	//}
	//
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVH_SSBO));
	////TODO: check if this is the right way
	//glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(BVHNode) * (start), sizeof(BVHNode) * (end), &m_BVHBuffer[start]));
	////glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	//
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVH_Index_SSBO));
	//glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(uint32_t) * 3 * (start), sizeof(uint32_t) * 3 * (end), &m_BVHIndexBuffer[start]));
	////glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	//
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVH_Triangle_SSBO));
	//glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * 3 * (start), sizeof(glm::vec4) * 3 * (end), &m_BVHTriangleBuffer[start]));
	////glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	//
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVH_Texcoord_SSBO));
	//glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec2) * 3 * (start), sizeof(glm::vec2) * 3 * (end), &m_BVHTexcoordBuffer[start]));
}

void TopLevelBVH::UpdateTopBVH(entt::registry& registry)
{
	//const uint32_t leafCount = 1;
	BVH bvh;
	//bvh.m_LeafCount = leafCount;
	auto view = registry.view<TransformComponent, MeshComponent, BVHComponent>();

	m_TransformBuffer.GetBuffer().emplace_back();
	m_OffsetBuffer.GetBuffer().emplace_back();
	std::vector<AABB> originalAABBs(view.size());
	//update all transforms
	for (auto entity : view)
	{
		const auto& [tc, mc, bvhc] = registry.get<TransformComponent, MeshComponent, BVHComponent>(entity);
		Mesh& mesh = MeshManager::GetMesh(mc.MeshIdx);
		auto idx = bvhc.BVHidx;
		m_TransformBuffer[idx] = glm::inverse(tc.CalcMatrix());
		m_OffsetBuffer[idx] = m_BVHs[idx].StartOffset;
		//m_TransformBuffer[idx].TriangleOffset = m_BVHs[idx].StartIndicesOffset;
		originalAABBs[idx] = mesh.m_aabb;
	}

	bvh.BuildTopLevelBVH(originalAABBs, m_TransformBuffer.m_Buffer);

	std::vector<BVHNode>& buffer = m_TopBVHBuffer.GetBuffer();
	buffer.clear();
	std::copy(bvh.m_Pool.begin(), bvh.m_Pool.end(), std::back_inserter(buffer));

	m_TopBVHBuffer.Init();
	m_TransformBuffer.Init();
	m_OffsetBuffer.Init();
}

void TopLevelBVH::InitBuffers()
{
	uint32_t elemCount = m_BVHBuffer.m_Buffer.size();

	m_BVHBuffer.Init(0);
	m_TriangleBuffer.Init(1);
	m_TexcoordBuffer.Init(2);
	m_TopBVHBuffer.Init(3);
	m_TransformBuffer.Init(4);
	m_OffsetBuffer.Init(5);

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

void TopLevelBVH::Unbind()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void TopLevelBVH::Bind(uint32_t BVHIdx, uint32_t indexBufferIdx, uint32_t triangleBufferIndex)
{
	m_BVHBuffer.Bind();
	m_TriangleBuffer.Bind();
	m_TexcoordBuffer.Bind();
	m_TopBVHBuffer.Bind();
	m_TransformBuffer.Bind();
}

TopLevelBVH::TopLevelBVH()
{
}

void TopLevelBVH::DrawTopLevelBVH(Camera& camera)
{
	glm::vec4 color(1, 1, 0, 1);
	BVH bvh;	//create dummy
	//bvh.m_IsBuilt = true;
	m_TopBVHBuffer.Bind(0);	//bind to 0 idx
	bvh.Draw(camera, glm::mat4(1.0f), color, 0, m_TopBVHBuffer.GetBuffer().size());
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	m_TopBVHBuffer.Bind();	//reset binding
}

void TopLevelBVH::Draw(Camera& camera, const glm::mat4& transform, BVHComponent& bvhc)
{
	BVH& bvh = GetBVH(bvhc.BVHidx);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVH_SSBO);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_BVH_SSBO);
	m_BVHBuffer.Bind();

	bvh.Draw(camera, transform, bvhc.DebugColor, bvh.StartOffset, 0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}