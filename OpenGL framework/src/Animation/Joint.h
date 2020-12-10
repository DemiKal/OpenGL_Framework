#pragma once
class Joint
{
public:
	int  m_Index;
	std::string m_Name;
	std::vector<Joint> m_Children;
	glm::mat4 m_Offset; //=offset!
	glm::mat4 m_InvMat;
	glm::mat4 m_PoseTransform{ glm::mat4(1.0f) };	//transform by interpolation of position, rotation, and scale, and combining into matrix
	glm::mat4 m_NodeTransform;	//transform of the node by assimp
	std::vector<std::pair<std::string, unsigned int>> m_ChildrenPair;
	std::vector<uint32_t> m_ChildrenIndices;

	Joint(int idx, const std::string& name, const glm::mat4& localMat);
	Joint(const Joint& j)
		:
		m_Index(j.m_Index), m_Name(j.m_Name), m_Offset(j.m_Offset),
		m_PoseTransform(j.m_PoseTransform), m_ChildrenIndices(j.m_ChildrenIndices) {}

	//Joint& operator = (const Joint& j)
	//{
	//	std::swap(*this, j);
	//	return *this;
	//}
	Joint();

	[[nodiscard]] std::string GetName() const;
	[[nodiscard]] glm::mat4 GetLocalTransform() const;

	void AddChild(Joint& j);
	void CalcInverseTransform(const glm::mat4& tParent);
};





