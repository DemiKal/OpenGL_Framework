#include "precomp.h"

inline TriangleBuffer& TriangleBuffer::GetInstance()
{
	static  TriangleBuffer instance;
	return instance;
}

 void TriangleBuffer::AddTriangles_I(Model& model)
{
	std::vector<glm::vec3>& posVerts = model.getMesh(0).positionVertices;
	const size_t currentIdx = m_PositionBuffer.size();			//global StartIdx
	const size_t endIdx = currentIdx + posVerts.size();	//global EndIdx
	m_IndexRangeBuffer.emplace_back(IndexPair(currentIdx, endIdx, model));
	std::copy(posVerts.begin(), posVerts.end(), std::back_inserter(m_PositionBuffer));
	std::cout << "Added triangles of " << model.directory << "with " << posVerts.size()
		<< "verts at idx range [" << currentIdx << "," << endIdx << "]\n";

}

inline std::vector<glm::vec3>& TriangleBuffer::GetTriangleBuffer()
{
	return GetInstance().m_PositionBuffer;
}

void TriangleBuffer::AddTriangles(Model& model)
{
	GetInstance().AddTriangles_I(model);

}
