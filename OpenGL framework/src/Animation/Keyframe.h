#pragma once
class Keyframe
{
	float TimeStamp;
	std::unordered_map<std::string, glm::mat4> pose;

	Keyframe(const float _timestamp, std::unordered_map<std::string, glm::mat4> _pose)
		:
		TimeStamp(_timestamp), pose(_pose) {}
};

