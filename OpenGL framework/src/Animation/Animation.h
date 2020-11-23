#pragma once
#include "Animation/Keyframe.h"
#include "AnimationChannel.h"

class Animation
{
public:
	float m_Length;
	std::vector<Keyframe> m_Keyframes;
	std::vector<AnimationChannel>  m_AnimationChannels;

	Animation();
	Animation(const Animation& a);
	Animation(float duration, std::vector<AnimationChannel> channels);
	void SetChannels(const std::vector<AnimationChannel>& channels);
};