#pragma once
#include "Animation/Keyframe.h"
#include "AnimationChannel.h"

class Animation
{
public:
	float m_length;
	std::vector<Keyframe> m_keyframes;
	std::vector<AnimationChannel>  m_animationChannels;

	Animation(float duration, const std::vector<AnimationChannel>& channels)
		:
		m_length(duration), m_animationChannels(channels) {};

	Animation() : m_length(-1.0f), m_animationChannels() {};
	Animation(const Animation& a)
		: m_length(a.m_length),
		m_keyframes(a.m_keyframes),
		m_animationChannels(a.m_animationChannels)
	{}


	void SetChannels(const std::vector<AnimationChannel>& channels) {
		std::copy(channels.begin(), channels.end(), std::back_inserter(m_animationChannels));
	}
};

