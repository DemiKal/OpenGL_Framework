#pragma once

struct PositionKey {
	float time;
	glm::vec3 position;

	PositionKey(const float t, const float px, const float py, const float pz) : time(t), position(px, py, pz) {}
	PositionKey(const float t, glm::vec3& pos) : time(t), position(pos) {} 
};

struct RotationKey {
	float time;
	glm::quat rotation;
	RotationKey(const float t, glm::quat& rot) : time(t), rotation(rot) {} 
	RotationKey(const float t, const float x, const float y, const float z, const float w) :
		time(t), rotation(x, y, z, w) {}
};
struct ScaleKey {
	float time;
	glm::vec3 scale;
	ScaleKey(float t, const glm::vec3& p_scale) : time(t), scale(p_scale) {} 
	ScaleKey(float t, const float sx, const float sy, const float sz) :
		time(t), scale(sx, sy, sz) {}
};

class AnimationChannel
{
public:
	AnimationChannel();
	AnimationChannel(
		std::string name,
		std::vector<PositionKey>& positions,
		std::vector<RotationKey>& rotations,
		std::vector<ScaleKey>& scalings);

	//copy constructor
	AnimationChannel(const AnimationChannel& a);

	const PositionKey& GetPositionByIndex( unsigned int idx);
	const RotationKey& GetRotationByIndex( unsigned int idx);
	ScaleKey GetScaleIndex(unsigned int idx);
	[[nodiscard]] std::string GetName() const;
	unsigned int FindPositionIndex(float timer);
	unsigned int FindRotationIndex(float timer);
	uint32_t GetLength() const;
private:
	std::string m_Name;
	std::vector<PositionKey> m_PositionKeys;
	std::vector<RotationKey>  m_RotationKeys;
	std::vector<ScaleKey>  m_ScaleKeys;
};

