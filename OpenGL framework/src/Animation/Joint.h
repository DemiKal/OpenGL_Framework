#pragma once
class Joint
{
public:
	int  m_Index;
	std::string m_Name;
	std::vector<Joint> m_Children;
	glm::mat4 m_Offset; //=offset!
	glm::mat4 m_InvMat;
	glm::mat4 m_PoseTransform;	//transform by interpolation of position, rotation, and scale, and combining into matrix
	glm::mat4 m_NodeTransform;	//transform of the node by assimp
	std::vector<std::pair<std::string, unsigned int>> m_ChildrenPair;

	Joint(int idx, const std::string& name, const glm::mat4& localMat);

	Joint();

	[[nodiscard]] std::string GetName() const;
	[[nodiscard]] glm::mat4 GetLocalTransform() const;

	void AddChild(Joint& j);
	void CalcInverseTransform(const glm::mat4& tParent);
};





