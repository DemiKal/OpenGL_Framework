#pragma once
class Model;

struct Triangle
{
	glm::vec3 A, B, C;
	Triangle(const glm::vec3& _A, const glm::vec3& _B, const glm::vec3& _C) : A(_A), B(_B), C(_C) {}

	bool intersect(const glm::vec3& orign, glm::vec3& direction, glm::vec2 bary_coords)
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
	std::vector<glm::vec3> m_PositionBuffer;
	//every mesh has a start and end idx.
	std::vector<IndexPair> m_IndexRangeBuffer;

	TriangleBuffer() {}
	TriangleBuffer operator=(TriangleBuffer& other) = delete;

	static TriangleBuffer& GetInstance();

	void AddTriangles_I(Model& model);;

public:
	static std::vector<glm::vec3>& GetTriangleBuffer();

	static void AddTriangles(Model& model);

};

