#pragma once
class Keyframe
{
	float m_TimeStamp;
	std::unordered_map<std::string, glm::mat4> m_Pose;

	Keyframe(float timestamp, std::unordered_map<std::string, glm::mat4> pose);
};



