#pragma once

struct PositionKey {
	float time;
	glm::vec3 position;

	PositionKey(const float t, const float px, const float py, const float pz) : time(t), position(px, py, pz) {};
	PositionKey(const float t, glm::vec3& pos) : time(t), position(pos) {};
};

struct RotationKey {
	float time;
	glm::quat rotation;
	RotationKey(const float t, glm::quat& rot) : time(t), rotation(rot) {};
	RotationKey(const float t, const float x, const float y, const float z, const float w) :
		time(t), rotation(x, y, z, w) {};
};
struct ScaleKey {
	float time;
	glm::vec3 scale;
	ScaleKey(float t, const glm::vec3& p_scale) : time(t), scale(p_scale) {};
	ScaleKey(float t, const float sx, const float sy, const float sz) :
		time(t), scale(sx, sy, sz) {};
};

class AnimationChannel
{
public:
	AnimationChannel() : m_name(""), m_PositionKeys(), m_RotationKeys(), m_scaleKeys() {};
	AnimationChannel(
		const std::string& name,
		std::vector<PositionKey>& positions,
		std::vector<RotationKey>& rotations,
		std::vector<ScaleKey>& scalings)
		:
		m_name(name),
		m_PositionKeys(positions),
		m_RotationKeys(rotations),
		m_scaleKeys(m_scaleKeys) {};

	//copy constructor
	AnimationChannel(const AnimationChannel& a)
		: m_name(a.m_name),
		m_PositionKeys(a.m_PositionKeys),
		m_RotationKeys(a.m_RotationKeys),
		m_scaleKeys(a.m_scaleKeys)
	{}

	const PositionKey& GetPositionByIndex(const unsigned int idx) { return m_PositionKeys[idx]; }
	const RotationKey& GetRotationByIndex(const unsigned int idx) { return  m_RotationKeys[idx]; }
	ScaleKey GetScaleIndex(unsigned int idx) { return m_scaleKeys[idx]; }
	std::string GetName() { return m_name; }
	unsigned int FindPositionIndex(float timer);
	unsigned int FindRotationIndex(float timer);

private:
	std::string m_name;
	std::vector<PositionKey> m_PositionKeys;
	std::vector<RotationKey>  m_RotationKeys;
	std::vector<ScaleKey>  m_scaleKeys;
};

