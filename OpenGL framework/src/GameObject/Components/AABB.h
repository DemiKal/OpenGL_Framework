#pragma once

class Ray;
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
	AABB();

	AABB(const glm::vec3& _min, const glm::vec3& _max);

	AABB(const float minX, const float minY, const float minZ,
	     const float maxX, const float maxY, const float maxZ);


	void RecalcBounds(const glm::mat4& transform, const AABB& original);
	void CalcBounds(const std::vector<glm::vec3>& posVector);
	void Update(const glm::mat4& transform, const AABB& original);
	void Draw(const ::Camera& camera, const glm::vec4& color) const;
	bool IntersectAABB(const Ray& ray, float& tCurrent) const;
	void UpdateArvo(const glm::mat4& m, const AABB& orig);

	[[nodiscard]] uint32_t GetLongestAxis() const;
	[[nodiscard]] glm::mat4 GetTransform() const;
	[[nodiscard]] glm::vec3 GetMin() const;
	[[nodiscard]] std::vector<glm::vec4> GetVerticesLocal() const;
	[[nodiscard]] glm::vec3 GetMax() const;
	[[nodiscard]] glm::vec3 Minimize(const glm::vec3& minA, const glm::vec3& minB) const;
	[[nodiscard]] glm::vec3	Maximize(const glm::vec3& maxA, const glm::vec3& maxB) const;

	[[nodiscard]]
	AABB Union(const AABB& a) const;

	[[nodiscard]]
	float CalcSurfaceArea() const;

	[[nodiscard]] glm::vec3 GetCenter() const;

	[[nodiscard]] glm::vec3 Min() const;
	[[nodiscard]] glm::vec3 Max() const;
};

