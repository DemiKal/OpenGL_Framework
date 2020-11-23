#include "precomp.h" 
#include "Animation/Animation.h" 

Animation::Animation(const float duration, std::vector<AnimationChannel> channels):
	m_Length(duration), m_AnimationChannels(std::move(channels))
{
}

Animation::Animation(): m_Length(-1.0f), m_AnimationChannels()
{
}

Animation::Animation(const Animation& a): m_Length(a.m_Length),
                                          m_Keyframes(a.m_Keyframes),
                                          m_AnimationChannels(a.m_AnimationChannels)
{
}

void Animation::SetChannels(const std::vector<AnimationChannel>& channels)
{
	std::copy(channels.begin(), channels.end(), std::back_inserter(m_AnimationChannels));
}
