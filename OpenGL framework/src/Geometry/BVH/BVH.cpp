#include "precomp.h"
#include "BVH.h"
#include "GameObject/Camera.h"
#include "GameObject/EntityManager.h" 
#include "Geometry/BVH/BVHNode.h"
#include "misc/InputManager.h"
#include "Rendering/Renderer.h"
#include "Rendering/ShaderManager.h" 

BVH::BVH(std::vector<unsigned> indices, std::vector<BVHNode> pool, BVHNode* root, const int poolPtr) :
	m_bvh_ssbo(0),
	m_root(root),
	m_poolPtr(poolPtr),
	m_indices(std::move(indices)),
	m_pool(std::move(pool))
{
}

void BVH::BuildBVH(const Renderer& renderer)
{
	fmt::print("Building BVH... \n");

	InitTriangleRenderer(); //TODO:: relocate it to renderer

	const std::vector<Triangle>& triangles = TriangleBuffer::GetTriangleBuffer();
	if(triangles.empty())
	{
		fmt::print("Error, Triangle list is empty! Cancelling build");
		return;
	}

	std::vector<AABB> triAABBs;
	triAABBs.reserve(triangles.size());
	for (const auto& tri : triangles)
		triAABBs.emplace_back(
			std::min(std::min(tri.A.x, tri.B.x), tri.C.x),
			std::min(std::min(tri.A.y, tri.B.y), tri.C.y),
			std::min(std::min(tri.A.z, tri.B.z), tri.C.z),
			std::max(std::max(tri.A.x, tri.B.x), tri.C.x),
			std::max(std::max(tri.A.y, tri.B.y), tri.C.y),
			std::max(std::max(tri.A.z, tri.B.z), tri.C.z)
		);

	const uint32_t N = triangles.size();	//WARNING: max is 4G tris!
	m_indices.resize(N);
	std::iota(m_indices.begin(), m_indices.end(), 0);

	m_pool.resize(N * 2 - 1);	//reserve max size, treat like array. Afterwards, resize downwards
	m_poolPtr = 1;
	m_root = &m_pool[0];

	const double startTime = glfwGetTime();

	m_root->Subdivide(*this, triAABBs, triangles, 0, N);

	m_pool.resize(m_poolPtr);
 
	const double endTime = glfwGetTime();
	double time = endTime - startTime;
	fmt::print("{0} triangles needed {1} recursions\n", N, count);
	fmt::print("Bvh size: {0} kb\n", sizeof(m_pool[0]) * m_poolPtr / 1024);
	fmt::print("seconds: {0}\n", time);
	fmt::print("triangles per second: {0}\n", N / time);

	CreateBuffers();
	m_isBuilt = true;
}

//void BVH::InitBVHRendererOld()
//{
//	m_aabbMatrices.reserve(m_localBounds.size());
//	for (auto& b : m_localBounds)
//		m_aabbMatrices.emplace_back(b.GetTransform());
//
//	unsigned int AABB_matBuffer;
//	GLCall(glGenBuffers(1, &AABB_matBuffer));
//	GLCall(glBindBuffer(GL_ARRAY_BUFFER, AABB_matBuffer));
//	GLCall(glBufferData(GL_ARRAY_BUFFER, m_aabbMatrices.size() * sizeof(glm::mat4), &m_aabbMatrices[0], GL_STATIC_DRAW));
//	Model* m_wireCube = &EntityManager::GetEntity("WireCube");
//
//	const unsigned int cubeVAO = m_wireCube->getMesh(0).GetVAO();
//	glBindVertexArray(cubeVAO);
//	// set attribute pointers for matrix (4 times vec4)
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
//	glEnableVertexAttribArray(2);
//	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
//	glEnableVertexAttribArray(3);
//	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
//	glEnableVertexAttribArray(4);
//	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
//
//	glVertexAttribDivisor(1, 1);
//	glVertexAttribDivisor(2, 1);
//	glVertexAttribDivisor(3, 1);
//	glVertexAttribDivisor(4, 1);
//
//	glBindVertexArray(0);
//	
//}
//void BVH::DrawOld(const Camera& camera)
//{
//	if (m_localBounds.size() <= 0) return; //has been initialized?
//	static   float bvhColor[] = { 1,0,0,1 };
//	ImGui::ColorEdit4("bvh color", bvhColor);
//	auto& aabbshader = EntityManager::GetEntity("WireCube").GetShader();
// 	aabbshader.Bind();
//	//aabbshader.SetUniformMat4f("model", spyro.model);
//	aabbshader.SetUniformMat4f("u_view", camera.GetViewMatrix());
//	aabbshader.SetUniformMat4f("u_projection", camera.GetProjectionMatrix());
//	aabbshader.SetVec4f("u_color", { bvhColor[0] , bvhColor[1] ,bvhColor[2] , bvhColor[3] });
//	Model*  wireCube = &EntityManager::GetEntity("WireCube");
//	const unsigned int vcount =  wireCube->getMesh(0).GetVertexCount();
//	glBindVertexArray( wireCube->getMesh(0).GetVAO());
//	glDrawArraysInstanced(GL_LINES, 0, vcount, m_localBounds.size());
//	glBindVertexArray(0);
//}

void BVH::Draw(const Camera& camera, Renderer& renderer) const
{
	if (!IsBuilt()) return; //has been initialized?
	static   glm::vec4 bvhColor = { 1, 0.3 , 0.6, 0.4 };
	ImGui::ColorEdit4("bvh color", &bvhColor[0]);
	auto& aabbShader = ShaderManager::GetShader("AABB_instanced_SSBO");

	aabbShader.Bind();
	aabbShader.SetUniformMat4f("u_view", camera.GetViewMatrix());
	aabbShader.SetUniformMat4f("u_projection", camera.GetProjectionMatrix());
	aabbShader.SetVec4f("u_color", bvhColor);

	//const uint32_t instanceCount = m_localBounds.size();
	renderer.DrawInstancedCubes(GetBVHSize());
}

void BVH::CastRay(const Ray& ray)
{
	if (!InputManager::m_isClicked) return;
	if (ImGui::IsWindowHovered()) return;
	if (!m_isBuilt) return;

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
			const int nrTris = node.m_bounds.m_count;
			//node.m_bounds.Draw(*Camera::GetMain(), { 1.0f, 1.0f, 1.0f, 1.0f });

			for (int i = 0; i < nrTris; i++)
			{
				const int j = node.m_bounds.m_leftFirst + i;
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
				if (nearestTriIdx >= rangeIdx.startIdx && nearestTriIdx < rangeIdx.endIdx)
				{
					ImGui::LabelText("selected object is: ", rangeIdx.modelPtr->m_name.c_str());
					ImGui::LabelText("Tri Idx: ", std::to_string(nearestTriIdx).c_str());
				}
			}

		}
	}
}

//draw triangles in world space by updating buffer data TODO: try transforming with a  matrix?
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
	shader.SetVec4f("u_color", 1, 1, 0.0f, 1.0f);
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_triangleVBO));

	GLCall(glBindVertexArray(m_triangleVAO));
	std::vector<glm::vec3 > newBuffer = { A + normal, B + normal,C + normal };
	GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(glm::vec3), &newBuffer[0]));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 3)); //plane!

	//newBuffer = { A - normal,B - normal,C - normal };
	//glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(glm::vec3), &newBuffer[0]);
	//glDrawArrays(GL_TRIANGLES, 0, 3); //plane!

	glBindVertexArray(0);
}
void BVH::InitTriangleRenderer()
{
	std::vector<glm::vec3> triangleBuffer = { {-0.5,-1.5, 0.5},{1, -1.5f,	0.5},{1, -2, -1} }; //world pos

	unsigned int  triangleVAO, triangleVBO;
	GLCall(glGenVertexArrays(1, &triangleVAO));
	GLCall(glGenBuffers(1, &triangleVBO));
	GLCall(glBindVertexArray(triangleVAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, triangleVBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 3, &triangleBuffer[0], GL_DYNAMIC_DRAW));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));

	m_triangleVAO = triangleVAO;
	m_triangleVBO = triangleVBO;

	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}
void BVH::CreateBVHTextures()
{
	//split bvh node into 2 textures
	//tex 1: indices:  4 ints
	//tex  2: min vecs: 3 floats
	//tex 3: max vecs : 3 floats
	const unsigned int aabbCount = m_poolPtr;
	unsigned int intTexture;
	//std::vector<glm::ivec4> indicesPart;
	//std::copy( m_pool ,std::)
	for (uint32_t i = 0; i < m_poolPtr; i++)
	{
		//const int start = m_pool[i].m_bounds.m_start;
		//const int end = m_pool[i].m_end;
		//const int leftFirst = m_pool[i].m_leftFirst;
		//const int count = m_pool[i].m_count;

		//indicesPart.emplace_back(glm::ivec4(start, end, leftFirst, count));
	}

	//std::cout << "nodes tex size: " << indicesPart.size() * sizeof(indicesPart[0]) / 1024 << "kb" << "\n";

	//aabb nodes
	m_aabbNodesTexture = Texture1D(aabbCount, 4, dataType::INT32, &m_indices[0], false);
	intTexture = m_aabbNodesTexture.GetID();

	std::vector<glm::vec3> minvec;
	auto GetMin = [](AABB& aabb) { return aabb.min; };

	//std::transform(std::begin(m_localBounds), std::end(m_localBounds), std::back_inserter(minvec), GetMin);



	m_minTexture = Texture1D(aabbCount, 3, dataType::FLOAT32, &minvec[0], false);
	//minVecTexture = m_minVecTex.GetID();

	//max bounds
	//auto Getmax = [](AABB& aabb) { return aabb.m_max.v; };

	std::vector<glm::vec3> maxVec;
	//std::transform(std::begin(m_localBounds), std::end(m_localBounds), std::back_inserter(maxVec), Getmax);

	m_maxTexture = Texture1D(aabbCount, 3, dataType::FLOAT32, &maxVec[0], false);

	//triangle texture
	const std::vector<Triangle>& triangles = TriangleBuffer::GetTriangleBuffer();
	const uint32_t triangleCount = static_cast<uint32_t>(triangles.size());

	std::vector<glm::vec3> triBuffer;

	for (const auto& t : triangles)
	{
		triBuffer.emplace_back(t.A);
		triBuffer.emplace_back(t.B);
		triBuffer.emplace_back(t.C);
	}

	m_triangleTexture = Texture1D(triBuffer.size(), 3, dataType::FLOAT32, &triBuffer[0], false);
	m_triangleIdxTexture = Texture1D(triangleCount, 1, dataType::UINT32, &m_indices[0], false);
}
void BVH::DrawSingleAABB(Camera& cam, const uint32_t index)
{
	if (index < 0 || index >= m_poolPtr)
	{
		std::cout << "Bvh draw index out of range!\n";
		return;
	}

	AABB& mat = m_pool[index].m_bounds; //m_aabbMatrices???
	mat.Draw(cam, { 1.0f, 0.2f,0.3f,1.0f });
}
void BVH::CreateBuffers()
{
	GLuint m_bvh_ssbo = 0;
	const unsigned int poolSize = sizeof(BVHNode) * m_poolPtr;
	glGenBuffers(1, &m_bvh_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_bvh_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, poolSize, &m_pool[0], GL_STATIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_bvh_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	std::vector<Triangle>& triangles = TriangleBuffer::GetTriangleBuffer();
	GLuint m_triangles_ssbo = 0;
	glGenBuffers(1, &m_triangles_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_triangles_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, triangles.size() * sizeof(Triangle), &triangles[0], GL_STATIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_triangles_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	GLuint m_tri_index_ssbo = 0;
	glGenBuffers(1, &m_tri_index_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_tri_index_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_tri_index_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}