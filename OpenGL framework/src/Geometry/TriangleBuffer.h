#pragma once
class Model;

struct Triangle
{
	glm::vec3 A;
	float dummy1 = 0;
	glm::vec3 B;
	float dummy2 = 0;
	glm::vec3 C;
	float dummy3 = 0;
	Triangle(const glm::vec3& _A, const glm::vec3& _B, const glm::vec3& _C) : A(_A), B(_B), C(_C) {}

	bool intersect(const glm::vec3& orign, glm::vec3& direction, glm::vec2& bary_coords)
	{

	}
};

struct IndexPair
{
	size_t startIdx, endIdx;
	Model* modelPtr;
	IndexPair(const size_t p_startIdx, const size_t p_endIdx, Model& model)
		: startIdx(p_startIdx), endIdx(p_endIdx), modelPtr(&model) {}
};

class TriangleBuffer
{
	//buffer of sequential vertices, every three verts = 1 Triangle
	std::vector<Triangle> m_TriangleBuffer;
	//every mesh has a start and end idx.
	std::vector<IndexPair> m_IndexRangeBuffer;

	TriangleBuffer() {}
	

	static TriangleBuffer& GetInstance();

	void AddTriangles_I(Model& model);;

public:
	TriangleBuffer operator=(TriangleBuffer& other) = delete;
	static std::vector<Triangle>& GetTriangleBuffer();
	static std::vector<IndexPair>& GetIndexRangeBuffer();
	static void AddTriangles(Model& model);

};

