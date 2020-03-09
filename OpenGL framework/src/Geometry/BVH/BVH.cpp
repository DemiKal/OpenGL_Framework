#include "precomp.h"
#include "BVH.h"

#include "GameObject/Camera.h"
#include "GameObject/EntityManager.h"
#include "Geometry/BVH/BVHNode.h"
#include "misc/InputManager.h"
#include "Rendering/ShaderManager.h" 

void BVH::BuildBVH()
{
	InitTriangleRenderer();

	std::vector<AABB> triAABBs;
	const std::vector<Triangle>& triangles = TriangleBuffer::GetTriangleBuffer();

	triAABBs.reserve(triangles.size());
	for (auto& tri : triangles)
		triAABBs.emplace_back(
			AABB(
				std::min(std::min(tri.A.x, tri.B.x), tri.C.x),
				std::min(std::min(tri.A.y, tri.B.y), tri.C.y),
				std::min(std::min(tri.A.z, tri.B.z), tri.C.z),
				std::max(std::max(tri.A.x, tri.B.x), tri.C.x),
				std::max(std::max(tri.A.y, tri.B.y), tri.C.y),
				std::max(std::max(tri.A.z, tri.B.z), tri.C.z)
			));

	const int N = triangles.size();
	m_indices.resize(N);
	std::iota(m_indices.begin(), m_indices.end(), 0);

	m_pool = new BVHNode[N * 2 - 1];
	m_root = &m_pool[0];
	m_poolPtr = 1;

	m_root->m_leftFirst = 1;
	m_root->m_start = 0;
	m_root->m_end = N;
	m_root->m_count = N;

	//actually  build now
	m_root->Subdivide(*this, triAABBs, triangles, 0, N);

	for (int i = 0; i < m_poolPtr; i++)
	{
		m_localBounds.emplace_back(m_pool[i].m_bounds);
	}

	InitBVHRenderer();
}


void BVH::InitBVHRenderer()
{
	m_aabbMatrices.reserve(m_localBounds.size());
	for (auto& b : m_localBounds)
		m_aabbMatrices.emplace_back(b.GetTransform());

	unsigned int AABB_matBuffer;
	glGenBuffers(1, &AABB_matBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, AABB_matBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_aabbMatrices.size() * sizeof(glm::mat4), &m_aabbMatrices[0], GL_STATIC_DRAW);
	m_wireCube = &EntityManager::GetEntity("WireCube");

	const unsigned int cubeVAO = m_wireCube->getMesh(0).GetVAO();
	glBindVertexArray(cubeVAO);
	// set attribute pointers for matrix (4 times vec4)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);

	glBindVertexArray(0);
}

void BVH::Draw(const Camera& camera)
{
	auto& aabbshader = m_wireCube->GetShader();
	aabbshader.Bind();
	aabbshader.SetUniformMat4f("view", camera.GetViewMatrix());
	aabbshader.SetUniformMat4f("projection", camera.GetProjectionMatrix());
	const unsigned int vcount = m_wireCube->getMesh(0).GetVertexCount();
	glBindVertexArray(m_wireCube->getMesh(0).GetVAO());
	glDrawArraysInstanced(GL_LINES, 0, vcount, m_localBounds.size());
	glBindVertexArray(0);

}
void BVH::TraceRay(const Ray& ray)
{
	if (!InputManager::m_isClicked) return;

	std::vector<HitData> hitData;
	m_root->Traverse(*this, ray, hitData, 0);
	//const glm::vec3 norm = glm::normalize(ray.Direction());

	float minDistance = std::numeric_limits<float>::infinity();
	int nearestTriIdx = -1;
	int triangleAABBidx = -1;
	if (!hitData.empty())
	{
		//std::cout << "Hit leaf of BVH!\n";
		auto& triangles = TriangleBuffer::GetTriangleBuffer();
		for (HitData& hd : hitData)
		{
			BVHNode& node = m_pool[hd.nodeIdx];
			const int nrTris = node.m_count;
			//node.m_bounds.Draw(*Camera::GetMain(), { 1.0f, 1.0f, 1.0f, 1.0f });

			for (int i = 0; i < nrTris; i++)
			{
				const int j = node.m_start + i;
				const int triIdx = m_indices[j];
				const Triangle& triangle = triangles[triIdx];
				glm::vec2 baryCentric;
				float distance;;
				//DrawTriangle(triangle.A, triangle.B, triangle.C);
				//bool i = intersectRayTriangle(ray.Origin(), ray.Direction(), triangle.A, triangle.B, triangle.C);

				const bool doesIntrsct = glm::intersectRayTriangle(ray.Origin(), ray.Direction(),
					triangle.A, triangle.B, triangle.C, baryCentric, distance);

				if (doesIntrsct && distance < minDistance && distance > 0)
				{
					minDistance = distance;
					nearestTriIdx = triIdx;
					triangleAABBidx = hd.nodeIdx;
				}
			}
		}

		if (nearestTriIdx > -1)
		{
			Triangle& tri = triangles[nearestTriIdx];
			DrawTriangle(tri.A, tri.B, tri.C);
			m_pool[triangleAABBidx].m_bounds.Draw(*Camera::GetMain(), { 1.0f,0.2f,0.8f, 1.0f });
			auto& triangleMap = TriangleBuffer::GetIndexRangeBuffer();
			for (auto& rangeIdx : triangleMap)
			{
				if (nearestTriIdx >= rangeIdx.startIdx && nearestTriIdx <  rangeIdx.endIdx)
				{
					ImGui::LabelText("selected object is: ", rangeIdx.modelPtr->name.c_str());
					ImGui::LabelText("Tri Idx: ", std::to_string(nearestTriIdx).c_str());
				}
			}

		}
	}
}

//draw triangles in world space by updating buffer data;
//TODO: try transforming with a  matrix?
void BVH::DrawTriangle(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C) const
{
	Camera* cam = Camera::GetMain();
	glm::vec3 normal = glm::normalize(glm::cross(A - C, B - C));
	glm::vec3 toScreen = glm::normalize(cam->GetPosition() - (A + B + C) / 3.0f);

	const  float dot = glm::dot(normal, toScreen);
	ImGui::Text("Dot: %f", dot);
	ImGui::Text("Normal: {%f.3, %f.3, %f.3)", normal.x, normal.y, normal.z);
	if (dot < 0)
		normal = -normal;
	normal *= 0.01f;
	auto& shader = ShaderManager::GetShader("singleTriangle");
	shader.Bind();
	//shader.SetUniformMat4f("model", model);
	shader.SetUniformMat4f("view", cam->GetViewMatrix());
	shader.SetUniformMat4f("projection", cam->GetProjectionMatrix());
	shader.SetUniform4f("u_color", 1, 1, 0.0f, 1.0f);
	glBindBuffer(GL_ARRAY_BUFFER, m_triangleVBO);

	glBindVertexArray(m_triangleVAO);
	std::vector<glm::vec3 > newBuffer = { A + normal, B + normal,C + normal };
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(glm::vec3), &newBuffer[0]);
	glDrawArrays(GL_TRIANGLES, 0, 3); //plane!

	//newBuffer = { A - normal,B - normal,C - normal };
	//glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(glm::vec3), &newBuffer[0]);
	//glDrawArrays(GL_TRIANGLES, 0, 3); //plane!

	glBindVertexArray(0);
}
void BVH::InitTriangleRenderer()
{
	std::vector<glm::vec3> triangleBuffer = { {-0.5,-1.5, 0.5},{1, -1.5f,	0.5},{1, -2, -1} }; //world pos

	unsigned int  triangleVAO, triangleVBO;
	glGenVertexArrays(1, &triangleVAO);
	glGenBuffers(1, &triangleVBO);
	glBindVertexArray(triangleVAO);
	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 3, &triangleBuffer[0], GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	m_triangleVAO = triangleVAO;
	m_triangleVBO = triangleVBO;


	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}