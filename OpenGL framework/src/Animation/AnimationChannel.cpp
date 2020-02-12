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
	bool operator() (const PositionKey& left, const PositionKey& right)
	{
		return left.time < right.time;
	}
	bool operator() (const PositionKey& left, float right)
	{
		return left.time < right;
	}
	bool operator() (float left, PositionKey& right)
	{
		return left < right.time;
	}
	bool operator() (const RotationKey& left, const RotationKey& right)
	{
		return left.time < right.time;
	}
	bool operator() (const RotationKey& left, float right)
	{
		return left.time < right;
	}
	bool operator() (float left, const RotationKey& right)
	{
		return left < right.time;
	}

	//TODO: add scalekeys?

}CompareKeys;

size_t AnimationChannel::FindPositionIndex(const float timer)
{
	auto it = std::upper_bound(m_positionKeys.begin(), m_positionKeys.end(), timer, CompareKeys);
	return it - m_positionKeys.begin() - 1;

	assert(false);//shouldn't happen
}

size_t  AnimationChannel::FindRotationIndex(const float timer)
{
	auto it = std::upper_bound(m_rotationKeys.begin(), m_rotationKeys.end(), timer, CompareKeys);
	return it - m_rotationKeys.begin() - 1;

	assert(false); //should not happen
}
