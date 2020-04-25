#include "precomp.h"
#include "Joint.h"

std::string Joint::GetName() const
{
	return m_name;
}

inline void Joint::AddChild(Joint& j)
{
	m_children.emplace_back(j);
}

inline glm::mat4 Joint::GetLocalTransform() const
{
	return m_offset;
}

inline void Joint::CalcInverseTransform(const glm::mat4& T_parent)
{
	const glm::mat4 bindT = T_parent * m_offset;

	m_inv_mat = glm::inverse(bindT);

	for (auto& j : m_children)
	{
		CalcInverseTransform(bindT);
	}
}