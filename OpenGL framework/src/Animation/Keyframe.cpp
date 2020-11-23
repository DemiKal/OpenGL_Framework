#include "precomp.h"
#include "Keyframe.h"

#include <utility>

Keyframe::Keyframe(const float timestamp, std::unordered_map<std::string, glm::mat4> pose)
	:
	m_TimeStamp(timestamp),
	m_Pose(std::move(pose))
{
}