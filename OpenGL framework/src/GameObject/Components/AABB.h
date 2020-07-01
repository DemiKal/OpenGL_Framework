#pragma once
#include "Geometry/Ray.h"


class Camera;
class Model;
#define SIMD

class AABB
{
	friend class BVHNode;
private:
#ifdef SIMD
	union
	{
		__m128 vmin;
		struct
		{
			glm::vec3 min;
			uint32_t m_leftFirst;
		};
	};
	union
	{
		__m128 vmax;
		struct
		{
			glm::vec3 max;
			uint32_t m_count;
		};
	};

#else	
	glm::vec3 min;
	union
	{
		uint32_t m_leftFirst = 0;
		float dummy1;
	};

	glm::vec3 max;
	union
	{
		uint32_t m_count = 0;
		float dummy2;
	};
#endif

public:
	AABB() : m_leftFirst(0), m_count(0), min(), max()
	{
	}

	AABB(const glm::vec3& _min, const glm::vec3& _max) : min(_min), max(_max)
	{
	}

	AABB(const float minX, const float minY, const float minZ,
		const float maxX, const float maxY, const float maxZ)
		:
		min(minX, minY, minZ), max(maxX, maxY, maxZ)
	{
	};


	void RecalcBounds(const glm::mat4& transform, const AABB& original);
	void CalcBounds(const std::vector<glm::vec3>& posVector);
	void Update(const glm::mat4& transform, const AABB& original);
	void Draw(const ::Camera& camera, const glm::vec4& color) const;
	bool IntersectAABB(const Ray& ray, float& tCurrent) const;
	void UpdateArvo(const glm::mat4& m, const AABB& orig);

	[[nodiscard]] uint32_t GetLongestAxis() const;
	[[nodiscard]] glm::mat4 GetTransform() const;
	[[nodiscard]] glm::vec3 GetMin() const { return min; }
	[[nodiscard]] std::vector<glm::vec4> GetVerticesLocal() const;
	[[nodiscard]] glm::vec3	GetMax() const { return max; }
	[[nodiscard]] glm::vec3 Minimize(const glm::vec3& minA, const glm::vec3& minB) const;
	[[nodiscard]] glm::vec3	Maximize(const glm::vec3& maxA, const glm::vec3& maxB) const;

	[[nodiscard]]
	AABB Union(const AABB& a) const
	{
		//const glm::vec3 minV = Minimize(a.min, min);
		//const glm::vec3 maxV = Maximize(a.max, max);

		AABB newB;
		newB.vmin = _mm_min_ps(a.vmin, vmin);
		newB.vmax = _mm_max_ps(a.vmax, vmax);
		return newB;//WARNING DOESN'T PRESERVE COUNT
	}
	[[nodiscard]]
	float  CalcSurfaceArea() const
	{
		//const float xlen = max.x - min.x;
		//const float zlen = max.y - min.y;
		//const float ylen = max.z - min.z;
		//const float ground = 2 * xlen * zlen;
		//const float side1 = 2 * ylen * xlen;
		//const float side2 = 2 * ylen * zlen;
		//return ground + side1 + side2;
		union
		{
			__m128 diff; float d[4];
		};

		diff = _mm_sub_ps(vmax, vmin);

		return std::max(0.0f, d[0] * d[1] + d[0] * d[2] + d[1] * d[2]);


	}

	[[nodiscard]] glm::vec3  GetCenter() const
	{
		return 0.5f * (max + min);
	}

	[[nodiscard]] glm::vec3 Min() const { return min; };
	[[nodiscard]] glm::vec3 Max() const { return max; };
	[[nodiscard]] uint32_t GetCount() const { return m_count; };
	[[nodiscard]] uint32_t GetLeftFirst() const { return m_leftFirst; };

	void SetCount(const uint32_t cnt) { m_count = cnt; };
	void SetLeftFirst(const uint32_t lf) { m_leftFirst = lf; };


};

