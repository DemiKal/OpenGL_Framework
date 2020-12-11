#include "precomp.h"
#include "Joint.h"

Joint::Joint(const uint32_t idx, const std::string& name, const glm::mat4& localMat) :
	m_Index(idx),
	m_Name(name),
	m_Offset(localMat),
	m_PoseTransform()
{
}

Joint::Joint(const Joint& j) :
	m_Index(j.m_Index),
	m_Name(j.m_Name),
	m_Offset(j.m_Offset),
	m_PoseTransform(j.m_PoseTransform),
	m_ChildrenIndices(j.m_ChildrenIndices)
{
}

Joint::Joint() :
	m_Index(-1),
	m_Name(""),
	m_Offset(glm::mat4(1.0f)),
	//m_InvMat(glm::mat4(1.0f)),
	m_PoseTransform()
	//m_NodeTransform()
{
}

std::string Joint::GetName() const
{
	return m_Name;
}

//inline void Joint::AddChild(Joint& j)
//{
//	m_Children.emplace_back(j);
//}

inline glm::mat4 Joint::GetLocalTransform() const
{
	return m_Offset;
}

//inline void Joint::CalcInverseTransform(const glm::mat4& tParent)
//{
//	const glm::mat4 bindT = tParent * m_Offset;
//
//	m_InvMat = glm::inverse(bindT);
//
//	for (auto& j : m_Children)
//	{
//		CalcInverseTransform(bindT);
//	}
//}