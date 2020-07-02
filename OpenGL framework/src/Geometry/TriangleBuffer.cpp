#include "precomp.h"
#include "TriangleBuffer.h"
#include "GameObject/Components/Model.h"


Triangle::Triangle(const glm::vec3& _A, const glm::vec3& _B, const glm::vec3& _C):
	A(_A), meshIdx(0), B(_B), triIdx(0), C(_C), dummy0i(0)
{
}

IndexPair::IndexPair(const size_t p_startIdx, const size_t p_endIdx, Model& model): startIdx(p_startIdx),
                                                                                    endIdx(p_endIdx), modelPtr(&model)
{
}



inline TriangleBuffer& TriangleBuffer::GetInstance()
{
	static  TriangleBuffer instance;
	return instance;
}

void TriangleBuffer::AddTriangles_I(Model& model)
{
	std::vector<glm::vec3>& posVerts = model.GetMesh(0).positionVertices;
	const size_t currentIdx = m_TriangleBuffer.size();			//global StartIdx
	const size_t endIdx = currentIdx + posVerts.size() / 3;	//global EndIdx
	m_TriangleBuffer.reserve(currentIdx + posVerts.size());

	m_IndexRangeBuffer.emplace_back(IndexPair(currentIdx, endIdx, model));

	for (uint32_t i = 0; i < posVerts.size() / 3u; i ++)
	{
		const glm::vec3 a = posVerts[3 * i];
		const glm::vec3 b = posVerts[3 * i + 1];
		const glm::vec3 c = posVerts[3 * i + 2];
		m_TriangleBuffer.emplace_back(Triangle(a, b, c));
	}
	fmt::print("size of Triangle {} bytes\n", sizeof(Triangle));
	fmt::print("Added triangles of {} with {} vertices at idx range [{},{}]\n",
		model.m_directory, posVerts.size(), currentIdx, endIdx);
}

[[nodiscard]]
std::vector<Triangle>& TriangleBuffer::GetTriangleBuffer()
{
	return GetInstance().m_TriangleBuffer;
}

std::vector<IndexPair>& TriangleBuffer::GetIndexRangeBuffer()
{
	return GetInstance().m_IndexRangeBuffer;
}

void TriangleBuffer::AddTriangles(Model& model)
{
	GetInstance().AddTriangles_I(model);

}
