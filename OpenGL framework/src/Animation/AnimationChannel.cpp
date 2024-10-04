#include "precomp.h"
#include "AnimationChannel.h"

#include <utility>

struct CompareKeys
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

};

AnimationChannel::AnimationChannel(): m_Name(""), m_PositionKeys(), m_RotationKeys(), m_ScaleKeys()
{
}

AnimationChannel::AnimationChannel(std::string name, std::vector<PositionKey>& positions,
                                   std::vector<RotationKey>& rotations, std::vector<ScaleKey>& scalings):
	m_Name(std::move(name)),
	m_PositionKeys(positions),
	m_RotationKeys(rotations),
	m_ScaleKeys(scalings)
{
}

AnimationChannel::AnimationChannel(const AnimationChannel& a): m_Name(a.m_Name),
                                                               m_PositionKeys(a.m_PositionKeys),
                                                               m_RotationKeys(a.m_RotationKeys),
                                                               m_ScaleKeys(a.m_ScaleKeys)
{
}

const PositionKey& AnimationChannel::GetPositionByIndex(const unsigned idx)
{
	return m_PositionKeys[idx];
}

const RotationKey& AnimationChannel::GetRotationByIndex(const unsigned idx)
{
	return m_RotationKeys[idx];
}

ScaleKey AnimationChannel::GetScaleIndex(const unsigned idx)
{
	return m_ScaleKeys[idx];
}

std::string AnimationChannel::GetName() const
{
	return m_Name;
}

unsigned int AnimationChannel::FindPositionIndex(const float timer)
{
	const auto it = std::upper_bound(m_PositionKeys.begin(), m_PositionKeys.end(), timer, CompareKeys());
	return static_cast<unsigned int>(it - m_PositionKeys.begin() - 1);
}

unsigned int AnimationChannel::FindRotationIndex(const float timer)
{
	const auto it = std::upper_bound(m_RotationKeys.begin(), m_RotationKeys.end(), timer, CompareKeys());
	return static_cast<unsigned int>(it - m_RotationKeys.begin() - 1);
}

uint32_t AnimationChannel::GetLength() const
{
	return std::max(m_PositionKeys.size(), m_RotationKeys.size());
}
