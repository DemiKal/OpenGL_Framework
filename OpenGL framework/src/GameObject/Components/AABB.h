#pragma once
#include "Geometry/Ray.h"


class Camera;
class Model;

struct Max {
	glm::vec3 v;
	Max() : v(0) {};
	Max(const  Max& m) : v(m.v) {};
	Max(const glm::vec3& _max) : v(_max) {}
};

struct Min {
	glm::vec3 v;
	Min() : v(0) {};
	Min(const Min& m) : v(m.v) {};
	Min(const glm::vec3& _min) : v(_min) {}
};

// ReSharper disable once CppInconsistentNaming
class AABB
{
public:
	Min m_min;
	union
	{
		uint32_t m_leftFirst;
		float dummy1;
	};

	Max m_max; 
	union
	{
		uint32_t m_count;
		float dummy2;
	};


	AABB() : m_min(), m_max() {}

	AABB(const glm::vec3& _min, const glm::vec3& _max) : m_min(_min), m_max(_max) {}
	AABB(const float minX, const float minY, const float minZ,
		const float maxX, const float maxY, const float maxZ)
		:
	m_min({ minX,minY,minZ }),
	m_max({ maxX,maxY,maxZ }) {};


	AABB(const Min _min, const Max _max) : m_min(_min), m_max(_max) {};

	//void InitOriginal() { m_min_OG = m_min; m_max_OG = m_max; } //set original
	[[nodiscard]] Max GetMax() const { return m_max; }
	[[nodiscard]] Min GetMin() const { return m_min; }

	[[nodiscard]] Min Minimize(const Min& minA, const Min& minB) const;

	[[nodiscard]] Max Maximize(const Max& maxA, const Max& maxB) const;

	[[nodiscard]] AABB Union(const AABB& a) const;

	[[nodiscard]] std::vector<glm::vec4> GetVerticesLocal() const;

	void RecalcBounds(const glm::mat4& transform, const AABB& original);

	void CalcBounds(const std::vector<glm::vec3>& posVector);

	[[nodiscard]] glm::mat4 GetTransform() const;

	void Update(const glm::mat4& transform, const AABB& original);

	void Draw(const ::Camera& camera, const glm::vec4& color);
	bool IntersectAABB(const Ray& ray, float& tCurrent) const;

	// ReSharper disable once IdentifierTypo
	void UpdateArvo(const glm::mat4& m, const AABB& orig);
};

