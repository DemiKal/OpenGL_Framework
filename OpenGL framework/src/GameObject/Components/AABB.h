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

class AABB
{
public:
	Min m_min; // m_min_OG;
	Max m_max; //m_max_OG;*/

	AABB() : m_min(), m_max() {}

	AABB(const glm::vec3& _min, const glm::vec3& _max) : m_min(_min), m_max(_max) {}
	AABB(const float minX, const float minY, const float minZ,
		const float maxX, const float maxY, const float maxZ)
		: m_min({ minX,minY,minZ }), m_max({ maxX,maxY,maxZ }) {};


	AABB(Min _min, Max _max) : m_min(_min), m_max(_max) {};

	//void InitOriginal() { m_min_OG = m_min; m_max_OG = m_max; } //set original
	Max GetMax() { return m_max; }
	Min GetMin() { return m_min; }

	Min minimize(const Min& minA, const Min& minB)
	{
		glm::vec3 vmin(
			std::min(minA.v.x, minB.v.x),
			std::min(minA.v.y, minB.v.y),
			std::min(minA.v.z, minB.v.z));
		return vmin;
	}

	Max maximize(const Max& maxA, const Max& maxB)
	{
		return glm::vec3(
			std::max(maxA.v.x, maxB.v.x),
			std::max(maxA.v.y, maxB.v.y),
			std::max(maxA.v.z, maxB.v.z));
	}

	AABB Union(const AABB& a)
	{
		Min minV = minimize(a.m_min, m_min);
		Max maxV = maximize(a.m_max, m_max);

		return AABB(minV, maxV);
	}

	inline std::vector<glm::vec4> GetVerticesLocal() const
	{
		return
		{   //front plane
			{m_min.v.x, m_min.v.y, m_max.v.z, 1.0f}, {m_max.v.x, m_min.v.y, m_max.v.z, 1.0f},
			{m_max.v.x, m_max.v.y, m_max.v.z, 1.0f}, {m_min.v.x, m_max.v.y, m_max.v.z, 1.0f},

			{m_min.v.x, m_min.v.y, m_min.v.z, 1.0f}, {m_max.v.x, m_min.v.y, m_min.v.z, 1.0f},
			{m_max.v.x, m_max.v.y, m_min.v.z, 1.0f}, {m_min.v.x, m_max.v.y, m_min.v.z, 1.0f},
		};
	}

	void RecalcBounds(const glm::mat4& transform, const AABB& original)
	{
		const std::vector<glm::vec4> verts_local = original.GetVerticesLocal();
		std::vector<glm::vec3> verts_world;
		std::transform(verts_local.begin(), verts_local.end(),
			std::back_inserter(verts_world),
			[transform](const glm::vec4& v) { return glm::vec3(transform * v); });

		CalcBounds(verts_world);
	}

	void CalcBounds(const std::vector<glm::vec3>& posVector)
	{
		Min vmin(glm::vec3((float)std::numeric_limits<float>::infinity()));
		Max vmax(glm::vec3((float)-std::numeric_limits<float>::infinity()));

		for (const glm::vec3& v : posVector)
		{
			vmin = minimize(vmin, v);
			vmax = maximize(vmax, v);
		}

		m_min = vmin;
		m_max = vmax;
	}

	glm::mat4 GetTransform()
	{
		glm::vec3 scale = m_max.v - m_min.v;
		glm::vec3 center = 0.5f * (m_max.v + m_min.v);

		return glm::scale(glm::translate(glm::mat4(1.0f), center), scale);

	}

	void Update(const glm::mat4& transform, const AABB& original)
	{
		RecalcBounds(transform, original);
	}

	void Draw(const ::Camera& camera, const glm::vec4& color);
	bool IntersectAABB(const Ray& ray, float& tCurrent) const;
	void UpdateArvo(const glm::mat4& m, const AABB& orig);
};

