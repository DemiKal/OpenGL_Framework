#pragma once
#include "Geometry/Ray.h"


class Camera;
class Model;


// ReSharper disable once CppInconsistentNaming
class AABB
{
public:
	glm::vec3 min;
	union
	{
		uint32_t m_leftFirst;
		float dummy1;
	};

	glm::vec3 max;
	union
	{
		uint32_t m_count;
		float dummy2;
	};


	AABB() : m_leftFirst(0),  m_count(0)
	{
	}

	AABB(const glm::vec3& _min, const glm::vec3& _max) : min(_min), m_leftFirst(0), max(_max), m_count(0)
	{
	}

	AABB(const float minX, const float minY, const float minZ,
	     const float maxX, const float maxY, const float maxZ)
		:
		min({minX, minY, minZ}), m_leftFirst(0), max({maxX, maxY, maxZ}), m_count(0) 
	{
	};


	//AABB(const glm::vec3&_min, const glm::vec3&  _max) : min(_min), m_leftFirst(0),  max(_max), m_count(0)
	//{
	//};

	[[nodiscard]] glm::vec3 GetMin() const { return min; }
	[[nodiscard]] glm::vec3	GetMax() const { return max; }
	[[nodiscard]] glm::vec3 Minimize(const glm::vec3& minA, const glm::vec3& minB) const;
	[[nodiscard]] glm::vec3	Maximize(const glm::vec3 & maxA, const glm::vec3 & maxB) const;
	[[nodiscard]] AABB		Union(const AABB& a) const;
	[[nodiscard]] float		CalcSurfaceArea() const;
	[[nodiscard]] glm::vec3 GetCenter() const;
	[[nodiscard]] glm::mat4 GetTransform() const;
	[[nodiscard]] std::vector<glm::vec4> GetVerticesLocal() const;

	void RecalcBounds(const glm::mat4& transform, const AABB& original);
	void CalcBounds(const std::vector<glm::vec3>& posVector);
	void Update(const glm::mat4& transform, const AABB& original);
	void Draw(const ::Camera& camera, const glm::vec4& color) const;
	bool IntersectAABB(const Ray& ray, float& tCurrent) const;
	void UpdateArvo(const glm::mat4& m, const AABB& orig);
	
};

