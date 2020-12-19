#include "precomp.h"
#include "UpperLvlBVH.h"
#include "GameObject/Components/Mesh.h"
#include "GameObject/Components/EntityComponents.h"

void UpperLvlBVH::AddBVH(MeshComponent& mc) //TODO: meshidx or entity id?
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

	bvh.StartIndicesOffset = m_BVHIndexBuffer.size();
	std::copy(bvh.m_Indices.begin(), bvh.m_Indices.end(), std::back_inserter(m_BVHIndexBuffer));
	bvh.EndIndicesOffset = m_BVHIndexBuffer.size();
	bvh.m_Indices.clear();

	//bvh.StartIndicesOffset = m_BVHIndexBuffer.size();
	std::copy(mesh.m_PositionVertices.begin(), mesh.m_PositionVertices.end(), std::back_inserter(m_BVHTriangleBuffer));
	//bvh.EndIndicesOffset = m_BVHIndexBuffer.size();


	std::copy(mesh.m_UVs.begin(), mesh.m_UVs.end(), std::back_inserter(m_BVHTexcoordBuffer));
	
	m_BVHs.emplace_back(bvh);
	UpdateBuffer(prevOffset, m_Offset);
}

inline BVH& UpperLvlBVH::GetBVH(int i) { return m_BVHs[i]; }

uint32_t UpperLvlBVH::GetBVHCount() { return m_BVHs.size(); }

void UpperLvlBVH::UpdateBuffer(const size_t start, const size_t end)
{
	//if (m_BVH_SSBO) glDeleteBuffers(1, &m_BVH_SSBO);
	size_t currentSize = sizeof(BVHNode) * m_BVHBuffer.size();
	size_t res = sizeof(BVHNode) * m_Reserved;

	if (currentSize > res)
	{
		m_Reserved = m_BVHBuffer.size();
		InitBuffers(m_BVHBuffer.data(), m_BVHIndexBuffer.data(), m_BVHTriangleBuffer.data());
		return;
	}

	GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVH_SSBO));
	//TODO: check if this is the right way
	GLCall(glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(BVHNode) * (start), sizeof(BVHNode) * (end), &m_BVHBuffer[start]));
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVH_Index_SSBO));
	GLCall(glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(uint32_t) * 3 * (start), sizeof(uint32_t) * 3 * (end), &m_BVHIndexBuffer[start]));
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVH_Triangle_SSBO));
	GLCall(glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * 3 * (start), sizeof(glm::vec4) * 3 * (end), &m_BVHTriangleBuffer[start]));
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVH_Texcoord_SSBO));
	GLCall(glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec2) * 3 * (start), sizeof(glm::vec2) * 3 * (end), &m_BVHTexcoordBuffer[start]));


}


void UpperLvlBVH::InitBuffers(void* dataBVH, void* dataIndex, void* dataTriangle)
{
	if (m_BVH_SSBO)
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		glDeleteBuffers(1, &m_BVH_SSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		glDeleteBuffers(1, &m_BVH_Index_SSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		glDeleteBuffers(1, &m_BVH_Triangle_SSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		glDeleteBuffers(1, &m_BVH_Texcoord_SSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	const unsigned int poolSizeBVH = sizeof(BVHNode) * m_Reserved;
	glGenBuffers(1, &m_BVH_SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVH_SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, poolSizeBVH, dataBVH, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_BVH_SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


	const unsigned int poolSizeBVHIndexBuffer = sizeof(uint32_t) * 3  * m_Reserved;
	glGenBuffers(1, &m_BVH_Index_SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVH_Index_SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, poolSizeBVHIndexBuffer, dataIndex, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_BVH_Index_SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


	const unsigned int poolSizeBVHTriangleBuffer = sizeof(glm::vec4) * 3 * m_Reserved;
	glGenBuffers(1, &m_BVH_Triangle_SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVH_Triangle_SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, poolSizeBVHTriangleBuffer, dataTriangle, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_BVH_Triangle_SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	const unsigned int poolSizeBVHTexcoordBuffer = sizeof(glm::vec2) * 3 * m_Reserved;
	glGenBuffers(1, &m_BVH_Texcoord_SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVH_Texcoord_SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, poolSizeBVHTexcoordBuffer, dataTriangle, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_BVH_Texcoord_SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


}

void UpperLvlBVH::Unbind()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
void UpperLvlBVH::Bind(uint32_t BVHIdx, uint32_t indexBufferIdx, uint32_t triangleBufferIndex)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVH_SSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BVHIdx, m_BVH_SSBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVH_Index_SSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, indexBufferIdx, m_BVH_Index_SSBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVH_Triangle_SSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, triangleBufferIndex, m_BVH_Triangle_SSBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVH_Texcoord_SSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_BVH_Texcoord_SSBO);

}


UpperLvlBVH::UpperLvlBVH()
{
	InitBuffers(nullptr, nullptr, nullptr);	//nullptr for init
}

void UpperLvlBVH::Draw(Camera& camera, const glm::mat4& transform, BVHComponent& bvhc)
{
	BVH& bvh = GetBVH(bvhc.BVHidx);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVH_SSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_BVH_SSBO);

	bvh.Draw(camera, transform, bvhc.DebugColor, bvh.StartOffset);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}