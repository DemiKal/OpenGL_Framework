#pragma once
class Model;

struct Triangle;

struct IndexPair
{
	size_t startIdx, endIdx;
	Model* modelPtr;
	IndexPair(const size_t p_startIdx, const size_t p_endIdx, Model& model);
};

class TriangleBuffer
{
	//buffer of sequential m_Vertices, every three verts = 1 Triangle
	std::vector<Triangle> m_TriangleBuffer;
	//every mesh has a start and end idx.
	std::vector<IndexPair> m_IndexRangeBuffer;

	TriangleBuffer() = default;


	static TriangleBuffer& GetInstance();

	void AddTriangles_I(Model& model);

public:
	TriangleBuffer operator=(TriangleBuffer& other) = delete;
	static std::vector<Triangle>& GetTriangleBuffer();
	static std::vector<IndexPair>& GetIndexRangeBuffer();
	static void AddTriangles(Model& model);

};

