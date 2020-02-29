#include "precomp.h"
#include "BVH.h"

#include "GameObject/Camera.h"
#include "GameObject/EntityManager.h"
#include "Geometry/BVH/BVHNode.h"
#include "misc/InputManager.h"
#include "Rendering/ShaderManager.h" 
//#include "Camera"
//class Camera;

void BVH::BuildBVH(const std::vector<Triangle>& triangles, const std::vector<AABB>& aabbs)
{
	InitTriangleRenderer();

	const int N = triangles.size();
	//create index array
	for (int i = 0; i < N; i++)
		m_indices.push_back(i);

	m_pool = new BVHNode[N * 2 - 1];
	m_root = &m_pool[0];
	m_poolPtr = 1;

	m_root->m_leftFirst = 1;
	m_root->m_start = 0;
	m_root->m_end = N;
	m_root->m_count = N;
	//root->bounds = CalculateBB( AABBs, 0, N );
	m_root->m_init = true;
	m_root->Subdivide(*this, aabbs, triangles, 0, N);

	for (int i = 0; i < m_poolPtr; i++)
	{
		m_localBounds.emplace_back(m_pool[i].m_bounds);
	}


}

void BVH::TraceRay(const Ray& ray)
{

	std::vector<HitData> hitData;
	m_root->Traverse(*this, ray, hitData, 0);
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

				if (doesIntrsct)
				{
					minDistance = std::min(minDistance, distance);
					nearestTriIdx = triIdx;
					triangleAABBidx = hd.nodeIdx;
				}
			}
		}

		if (nearestTriIdx > -1)
		{
			if (InputManager::m_isClicked)
			{
				Triangle& tri = triangles[nearestTriIdx];
				DrawTriangle(tri.A, tri.B, tri.C);
				m_pool[triangleAABBidx].m_bounds.Draw(*Camera::GetMain(), { 1.0f,0.2f,0.8f, 1.0f });
				for (auto& rangeIdx : TriangleBuffer::GetIndexRangeBuffer())
				{
					if (nearestTriIdx >= rangeIdx.startIdx && nearestTriIdx <= rangeIdx.endIdx)
					{ 
						ImGui::LabelText("selected object is: " , rangeIdx.modelPtr->name.c_str());
					}
				}
			}
		}
	}
}

//draw triangles in world space by updating buffer data;
//TODO: try transforming with a  matrix?
void BVH::DrawTriangle(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C) const
{
	auto& shader = ShaderManager::GetShader("singleTriangle");
	shader.Bind();
	Camera* cam = Camera::GetMain();
	//shader.SetUniformMat4f("model", model);
	shader.SetUniformMat4f("view", cam->GetViewMatrix());
	shader.SetUniformMat4f("projection", cam->GetProjectionMatrix());
	shader.SetUniform4f("u_color", 1, 1, 0.0f, 1.0f);
	glBindBuffer(GL_ARRAY_BUFFER, m_triangleVBO);
	const std::vector<glm::vec3 > newBuffer = { A,B,C };
	//glBufferSubData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 3, &newBuffer[0], GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(glm::vec3), &newBuffer[0]);
	glBindVertexArray(m_triangleVAO);
	glDrawArrays(GL_TRIANGLES, 0, 3); //plane!

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