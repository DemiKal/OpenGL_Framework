#include "precomp.h"
#include "TriangleBuffer.h"
#include "GameObject/Components/Model.h"


inline TriangleBuffer& TriangleBuffer::GetInstance()
{
	static  TriangleBuffer instance;
	return instance;
}

void TriangleBuffer::AddTriangles_I(Model& model)
{
	std::vector<glm::vec3>& posVerts = model.getMesh(0).positionVertices;
	const size_t currentIdx = m_TriangleBuffer.size();			//global StartIdx
	const size_t endIdx = currentIdx + posVerts.size() / 3;	//global EndIdx
	m_TriangleBuffer.reserve(currentIdx + posVerts.size());
	const int maxx = m_TriangleBuffer.max_size();

	m_IndexRangeBuffer.emplace_back(IndexPair(currentIdx, endIdx, model));
	//std::copy(posVerts.begin(), posVerts.end(), std::back_inserter(m_TriangleBuffer));
	for (unsigned int i = 0; i < posVerts.size() / 3; i ++)
	{
		//std::cout << "i:" << i <<  "\n";
		const glm::vec3 a = posVerts[3*i];
		const glm::vec3 b = posVerts[3*i + 1];
		const glm::vec3 c = posVerts[3*i + 2];
		m_TriangleBuffer.emplace_back(Triangle(a, b, c));
	}

	std::cout << "Added triangles of " << model.m_directory << "with " << posVerts.size()
		<< "verts at idx range [" << currentIdx << ", " << endIdx << "]\n";
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
