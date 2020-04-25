#pragma once
class Keyframe
{
	float m_timeStamp;
	std::unordered_map<std::string, glm::mat4> m_pose;

	Keyframe(const float _timestamp, const std::unordered_map<std::string, glm::mat4> _pose)
		:
		m_timeStamp(_timestamp),
		m_pose(_pose)
	{}
};

