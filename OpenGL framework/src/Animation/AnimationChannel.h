#pragma once

struct PositionKey {
	float time;
	glm::vec3 position;
	PositionKey(const std::pair<float, glm::vec3>& timeAndPos) : time(timeAndPos.first), position(timeAndPos.second) {};
};

struct RotationKey {
	float time;
	glm::quat rotation;
	RotationKey(const std::pair<float, glm::quat>& timeAndRot) : time(timeAndRot.first), rotation(timeAndRot.second) {};
};
struct ScaleKey {
	float time;
	glm::vec3 scale;
	ScaleKey(const std::pair<float, glm::vec3>& timeAndScale) : time(timeAndScale.first), scale(timeAndScale.second) {};
};

class AnimationChannel
{
public:
	AnimationChannel() : m_name(""), m_positionKeys(), m_rotationKeys(), m_scaleKeys() {};
	AnimationChannel(
		const std::string& name,
		std::vector<PositionKey>& positions,
		std::vector<RotationKey>& rotations,
		std::vector<ScaleKey>& scalings)
		:
		m_name(name), m_positionKeys(positions), m_rotationKeys(rotations), m_scaleKeys(m_scaleKeys) {};

	//copy constructor
	AnimationChannel(const AnimationChannel& a)
		: m_name(a.m_name), m_positionKeys(a.m_positionKeys),
		m_rotationKeys(a.m_rotationKeys), m_scaleKeys(a.m_scaleKeys) {}

	PositionKey  GetPositionByIndex(const unsigned int idx) { return m_positionKeys[idx]; }
	RotationKey GetRotationByIndex(const unsigned int idx) { return m_rotationKeys[idx]; }
	ScaleKey GetScaleIndex(unsigned int idx) { return m_scaleKeys[idx]; }
	std::string GetName() { return m_name; }
	size_t FindPositionIndex(float timer);
	size_t FindRotationIndex(float timer);

private:
	std::string m_name;
	std::vector<PositionKey  > m_positionKeys;
	std::vector<RotationKey >  m_rotationKeys;
	std::vector<ScaleKey >  m_scaleKeys;
};

