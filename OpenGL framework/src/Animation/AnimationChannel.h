#pragma once
class AnimationChannel
{
public:
	AnimationChannel(
		const std::string& name,
		std::vector<std::pair<float, glm::vec3>>  positions,
		std::vector<std::pair<float, glm::quat>>  rotations,
		std::vector<std::pair<float, glm::vec3>>  scalings)
		:
		m_name(name), m_positionKeys(positions), m_rotationKeys(rotations), m_scaleKeys(m_scaleKeys) {};

	AnimationChannel() : m_name(""), m_positionKeys(), m_rotationKeys(), m_scaleKeys() {};
	AnimationChannel(const AnimationChannel& a)
		: m_name(a.m_name), m_positionKeys(a.m_positionKeys),
		m_rotationKeys(a.m_rotationKeys), m_scaleKeys(a.m_scaleKeys) {}

	size_t FindPositionIndex(float timer);
	size_t FindRotationIndex(float timer);

//private:
	std::string m_name;
	std::vector<std::pair<float, glm::vec3>> m_positionKeys;
	std::vector<std::pair<float, glm::quat>> m_rotationKeys;
	std::vector<std::pair<float, glm::vec3>> m_scaleKeys;
};

