#pragma once
class Joint
{
public:
	uint32_t m_Index;
	std::string m_Name;
	glm::mat4 m_Offset; //=offset!
	glm::mat4 m_PoseTransform = glm::mat4(1.0f);	//transform by interpolation of position, rotation, and scale, and combining into matrix
	std::vector<uint32_t> m_ChildrenIndices;

	Joint(uint32_t idx, const std::string& name, const glm::mat4& localMat);
	Joint(const Joint& j);
	Joint();

	[[nodiscard]] std::string GetName() const;
	[[nodiscard]] glm::mat4 GetLocalTransform() const;

	//void AddChild(Joint& j);
	//void CalcInverseTransform(const glm::mat4& tParent);
};





