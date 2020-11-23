#include "precomp.h"
#include "AnimationChannel.h"

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
	bool operator() (const PositionKey& left, const PositionKey& right) const
	{
		return left.time < right.time;
	}
	bool operator() (const PositionKey& left, float right) const
	{
		return left.time < right;
	}
	bool operator() (float left, PositionKey& right) const
	{
		return left < right.time;
	}
	bool operator() (const RotationKey& left, const RotationKey& right) const
	{
		return left.time < right.time;
	}
	bool operator() (const RotationKey& left, float right) const
	{
		return left.time < right;
	}
	bool operator() (float left, const RotationKey& right) const
	{
		return left < right.time;
	}

	//TODO: add scalekeys?

}CompareKeys;

unsigned int AnimationChannel::FindPositionIndex(const float timer)
{
	const auto it = std::upper_bound(m_PositionKeys.begin(), m_PositionKeys.end(), timer, CompareKeys);
	return static_cast<unsigned int>(it - m_PositionKeys.begin() - 1);

	assert(false);//shouldn't happen
}

unsigned int AnimationChannel::FindRotationIndex(const float timer)
{
	auto it = std::upper_bound(m_RotationKeys.begin(), m_RotationKeys.end(), timer, CompareKeys);
	return static_cast<unsigned int>(it - m_RotationKeys.begin() - 1);

	assert(false); //should not happen
}
