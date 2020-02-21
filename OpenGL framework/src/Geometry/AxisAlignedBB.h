#pragma once

struct AABB
{
	union {
		glm::vec3 minVec;
		float minX, minY, minZ;
	};
	union {
		glm::vec3 maxVec;
		float maxX, maxY, maxZ;
	};


	AABB() : minVec(glm::vec3(0)), maxVec(glm::vec3(0)) {};

	AABB(const AABB& other) : minVec(other.minVec), maxVec(other.maxVec) {};
	AABB operator= (const AABB& other)
	{
		this->maxVec = other.maxVec;
		this->minVec = other.minVec;
		return *this;
	}

	AABB(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
		: minVec(minX, minY, minZ), maxVec(maxX, maxY, maxZ)
	{
	};

	AABB(const glm::vec3& min, const glm::vec3& max)
		: minVec(min), maxVec(max)
	{
	};

	void ExpandMin(const glm::vec3& p_minVec)
	{
		minX = std::max(minX, p_minVec.z);
		minY = std::max(minX, p_minVec.y);
		minZ = std::max(minZ, p_minVec.z);
	}

	void ExpandMax(const glm::vec3& p_maxVec)
	{
		maxX = std::max(maxX, p_maxVec.z);
		maxY = std::max(maxX, p_maxVec.y);
		maxZ = std::max(maxZ, p_maxVec.z);
	}

	static glm::vec3 MinimizeVec(const glm::vec3& A, const glm::vec3& B)
	{
		return glm::vec3(std::min(A.x, B.x), std::min(A.y, B.y), std::min(A.z, B.z));
	}
	static glm::vec3 MaximizeVec(const glm::vec3& A, const glm::vec3& B)
	{
		return glm::vec3(std::max(A.x, B.x), std::max(A.y, B.y), std::max(A.z, B.z));
	}

	AABB Combine(const AABB& other)
	{
		glm::vec3 minV = MinimizeVec(minVec, other.minVec);
		glm::vec3 maxV = MaximizeVec(maxVec, other.maxVec);

		return AABB(minV, maxV);
	}

	glm::vec3 GetMin() { return minVec; }
	glm::vec3 GetMax() { return maxVec; }

	glm::vec3 GetMin(const  glm::mat4& Transform)
	{
		glm::vec4 newV = Transform * glm::vec4(minVec, 1.0f);
		return glm::vec3(newV.x, newV.y, newV.z);
	}

	glm::vec3 GetMax(const  glm::mat4& Transform)
	{
		glm::vec4 newV = Transform * glm::vec4(maxVec, 1.0f);
		return glm::vec3(newV.x, newV.y, newV.z);
	}

	void RecalcAABB(std::vector<float>& vBuffer,
		VertexBufferLayout& vbl,
		const glm::mat4& transform)
	{
		const unsigned int stride = vbl.GetStride() / sizeof(float);
		const size_t size = vBuffer.size();

		float minX = std::numeric_limits<float>::max(); //vBuffer[0];
		float minY = std::numeric_limits<float>::max();
		float minZ = std::numeric_limits<float>::max();
		float maxX = std::numeric_limits<float>::min();
		float maxY = std::numeric_limits<float>::min();
		float maxZ = std::numeric_limits<float>::min();


		//loop over world vertices
		for (size_t i = 0; i < size; i += stride)
		{
			glm::vec4 local_pos(vBuffer[i + 0], vBuffer[i + 1], vBuffer[i + 2], 1.0f);
			glm::vec3 world_pos = glm::vec3(transform * local_pos);


			minX = std::min(minX, world_pos.x);
			minY = std::min(minY, world_pos.y);
			minZ = std::min(minZ, world_pos.z);

			maxX = std::max(maxX, world_pos.x);
			maxY = std::max(maxY, world_pos.y);
			maxZ = std::max(maxZ, world_pos.z);
		}

		minVec = glm::vec3(minX, minY, minZ);
		maxVec = glm::vec3(maxX, maxY, maxZ);
	}

	//to transform 1x1x1 unit cube (extends with -0.5 -- 0.5
	glm::mat4 GetTransform()
	{
		float lengthX = maxX - minX;
		float lengthY = maxY - minY;
		float lengthZ = maxZ - minZ;

		glm::vec3  scale(lengthX, lengthY, lengthZ);
		glm::vec3 center(
			(maxX + minX) * 0.5f,
			(maxY + minY) * 0.5f,
			(maxZ + minZ) * 0.5f
		);

		return glm::scale(glm::translate(glm::mat4(1.0f), center), scale);
	}
};
