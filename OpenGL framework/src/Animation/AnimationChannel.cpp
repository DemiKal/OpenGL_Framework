#include "precomp.h"

struct _CompareKeys
{
	//bool operator() (const std::pair<float, glm::vec3>& left, const std::pair<float, glm::vec3>& right)
	//{
	//	return left.first < right.first;
	//}
	//bool operator() (const std::pair<float, glm::quat>& left, const std::pair<float, glm::quat>& right)
	//{
	//	return left.first < right.first;
	//}
	bool operator() (const std::pair<float, glm::vec3>& left, const std::pair<float, glm::vec3>& right)
	{
		return left.first < right.first;
	}
	bool operator() (const std::pair<float, glm::vec3>& left, float right)
	{
		return left.first < right;
	}
	bool operator() (float left, const std::pair<float, glm::vec3>& right)
	{
		return left < right.first;
	}
	bool operator() (const std::pair<float, glm::quat>& left, const std::pair<float, glm::quat>& right)
	{
		return left.first < right.first;
	}
	bool operator() (const std::pair<float, glm::quat>& left, float right)
	{
		return left.first < right;
	}
	bool operator() (float left, const std::pair<float, glm::quat>& right)
	{
		return left < right.first;
	}



}CompareKeys;
size_t AnimationChannel::FindPositionIndex(const float timer)
{
	//for (int i = 0; i < m_positionKeys.size(); i++)
	//	if (timer < m_positionKeys[i].first)
	//		return  i - 1 ;

	auto it = std::upper_bound(m_positionKeys.begin(), m_positionKeys.end(), timer, CompareKeys);
	return it - m_positionKeys.begin() - 1;

	assert(false);//shouldn't happen
}

size_t  AnimationChannel::FindRotationIndex(const float timer)
{
	//for (unsigned int i = 0; i < m_rotationKeys.size(); i++)
	//	if (timer < m_rotationKeys[i].first)
	//		return   i - 1;

	auto it = std::upper_bound(m_rotationKeys.begin(), m_rotationKeys.end(), timer, CompareKeys);
	return it - m_rotationKeys.begin() - 1;

	assert(false); //should not happen
}
