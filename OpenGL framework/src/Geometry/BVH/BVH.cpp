#include "precomp.h"
#include "BVH.h"
#include "GameObject/Camera.h"
#include "GameObject/EntityManager.h" 
#include "misc/InputManager.h"
#include "Rendering/Renderer.h"
#include "Rendering/ShaderManager.h" 
#include "Geometry/TriangleBuffer.h"
//#include "GameObject/Components/Texture1D.h"
#include "Geometry/Ray.h"
#include "GameObject/Components/MeshManager.h"

BVH::BVH(std::vector<unsigned> indices, std::vector<BVHNode> pool, BVHNode* root, const int poolPtr) :
	m_BVH_SSBO(0),
	//m_Root(root),
	m_PoolPtr(poolPtr),
	m_Indices(std::move(indices)),
	m_Pool(std::move(pool))
{
}


void BVH::BuildTopLevelBVH(
	const std::vector<AABB>& originalAABBs, 
	const std::vector<TopNodeRef>& nodeRefs)
{
	if (nodeRefs.empty())
	{
		fmt::print("Error, Triangle list is empty! Cancelling build\n");
		return;
	}

	const uint32_t N = static_cast<uint32_t>(nodeRefs.size());	//WARNING: max is 4G tris!


	m_Indices.resize(N);
	std::iota(m_Indices.begin(), m_Indices.end(), 0);

	m_Pool.resize(N * 2 - 1);	//reserve max size, treat like array. Afterwards, resize downwards
	m_PoolPtr = 1;
	//m_Root = &m_Pool[0];
	m_AABBS.reserve(N);
	//for (const auto& node : nodes)
	const float h = 0.5f;
	for (int i = 0; i < N; i++)
	{
		//const BVHNode& node = nodes[i];
		const TopNodeRef& nodeRef = nodeRefs[i];
		AABB copy;
		copy.Update(glm::inverse(nodeRef.InverseMat), originalAABBs[i]  );

		m_AABBS.emplace_back(copy);
		m_TriangleCenters.emplace_back(copy.GetCenter());
	}

	const double startTime = glfwGetTime();
	auto start = std::chrono::steady_clock::now();
	uint32_t idx = 0;
	m_Pool[0].Subdivide(*this, 0, N, idx);

	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double, std::milli> build_ms = end - start;
	m_Pool.resize(m_PoolPtr);	//TODO: check careful with this!
}

void BVH::BuildBVH(const std::vector<glm::vec4>& tris, const uint32_t bufferOffset, const uint32_t triangleOffset)
{

	fmt::print("Building BVH... \n");

	const std::vector<Triangle>& triangles = *(reinterpret_cast<const std::vector<Triangle>*>(&tris));

	if (triangles.empty())
	{
		fmt::print("Error, Triangle list is empty! Cancelling build");
		return;
	}

	m_GlobalOffset = bufferOffset;
	m_TriangleOffset = triangleOffset;
	const uint32_t N = static_cast<uint32_t>(triangles.size());	//WARNING: max is 4G tris!

	m_AABBS.reserve(N);
	for (const auto& tri : triangles)
		m_AABBS.emplace_back(
			std::min(std::min(tri.A.x, tri.B.x), tri.C.x),
			std::min(std::min(tri.A.y, tri.B.y), tri.C.y),
			std::min(std::min(tri.A.z, tri.B.z), tri.C.z),
			std::max(std::max(tri.A.x, tri.B.x), tri.C.x),
			std::max(std::max(tri.A.y, tri.B.y), tri.C.y),
			std::max(std::max(tri.A.z, tri.B.z), tri.C.z)
		);

	//std::vector < glm::vec3 > triangleCenters;
	m_TriangleCenters.reserve(N);
	for (AABB& aabb : m_AABBS)
		m_TriangleCenters.emplace_back(aabb.GetCenter());


	m_Indices.resize(N);
	std::iota(m_Indices.begin(), m_Indices.end(), 0);

	m_Pool.resize(static_cast<size_t>(N) * 2 - 1);	//reserve max size, treat like array. Afterwards, resize downwards
	m_PoolPtr = 1;
	//m_Root = &m_Pool[0];

	const double startTime = glfwGetTime();
	auto start = std::chrono::steady_clock::now();
	uint32_t idx = 0;
	
	m_Pool[0].Subdivide(*this, 0, N, idx);

	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double, std::milli> build_ms = end - start;
	m_Pool.resize(m_PoolPtr);

	
	const double endTime = glfwGetTime();
	double time = endTime - startTime;
	auto sizeInKB = sizeof(m_Pool[0]) * m_PoolPtr / 1024;
	fmt::print("{0} triangles needed {1} recursions\n", N, m_Count);
	fmt::print("Bvh size: {0} kb\n", sizeInKB);
	fmt::print("Time: {0} ms\n", build_ms.count());
	fmt::print("triangles per second: {0}\n", N / (build_ms.count() * 1000.0f));
	fmt::print("triangles per ms: {0}\n", (N / build_ms.count()));

	for (auto& node : m_Pool)
	{
		auto lf = node.GetLeftFirst();
		node.SetLeftFirst(lf + bufferOffset);

		if(node.GetCount() <= m_LeafCount)
			node.SetLeftFirst(triangleOffset + lf);
	}

	int i = 0;
	std::map<uint32_t, BVHNode> m;
	fmt::print("all\n");
	for (auto& node : m_Pool)
	{
		uint32_t count = node.GetCount();
		if (count <= 2)
		{
			m[node.GetLeftFirst()] = node;
		}

		fmt::print("idx {}, leftfirst: {}, count: {}\n", i, node.GetLeftFirst(), count);

		i++;
	}
	fmt::print("yabadabadoooooooooo\n");
	i = 0;
	for (auto& [k, v] : m)
	{
		fmt::print("[lf: {}]. Node: idx {}, leftfirst: {}, count: {}\n", k, i++, v.GetLeftFirst(), v.GetCount());
	}

	CreateBuffers(triangles);
	m_IsBuilt = true;
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
//	const unsigned int cubeVAO = m_wireCube->GetMesh(0).GetVAO();
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
//	const unsigned int vcount =  wireCube->GetMesh(0).GetVertexCount();
//	glBindVertexArray( wireCube->GetMesh(0).GetVAO());
//	glDrawArraysInstanced(GL_LINES, 0, vcount, m_localBounds.size());
//	glBindVertexArray(0);
//}

//the offset is determined by the start position of this bvh in the buffer
void BVH::Draw(const Camera& camera, const glm::mat4& transform, const glm::vec4& color, int offset, uint32_t size = 0) const
{
	//if (!IsBuilt()) return; //has been initialized?
	//static glm::vec4 bvhColor = { 1, 0.3 , 0.6, 1.0 };
	//ImGui::ColorEdit4("bvh color", &bvhColor[0]);
	auto& aabbShader = ShaderManager::GetShader("AABB_instanced_SSBO");

	aabbShader.Bind();
	aabbShader.SetUniformMat4f("u_Model", transform);
	aabbShader.SetUniformMat4f("u_View", camera.GetViewMatrix());
	aabbShader.SetUniformMat4f("u_Projection", camera.GetProjectionMatrix());
	aabbShader.SetVec4f("u_Color", color);
	aabbShader.SetInt("u_Offset", offset);

	Mesh& cube = MeshManager::GetMesh(1);

	cube.DrawInstanced(size == 0 ? m_PoolPtr : size);
}

void BVH::CastRay(const Ray& ray)
{
	if (!InputManager::m_isClicked) return;
	//if (ImGui::IsWindowHovered()) return;
	if (!m_IsBuilt) return;

	std::vector<HitData> hitData;
	m_Pool[0].Traverse(*this, ray, hitData, 0);
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
			BVHNode& node = m_Pool[hd.nodeIdx];
			const uint32_t nrTris = node.GetCount();
			//node.m_bounds.Draw(*Camera::GetMain(), { 1.0f, 1.0f, 1.0f, 1.0f });

			for (uint32_t i = 0; i < nrTris; i++)
			{
				const uint32_t j = node.GetLeftFirst() + i;
				const uint32_t triIdx = m_Indices[j];
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
			m_Pool[triangleAABBidx].m_bounds.Draw(*Camera::GetMain(), { 1.0f,0.2f,0.8f, 1.0f });
			auto& triangleMap = TriangleBuffer::GetIndexRangeBuffer();
			for (auto& rangeIdx : triangleMap)
			{
				if (nearestTriIdx >= rangeIdx.startIdx && nearestTriIdx < rangeIdx.endIdx)
				{
					//ImGui::LabelText("selected object is: ", rangeIdx.modelPtr->m_Name.c_str());
					//ImGui::LabelText("Tri Idx: ", std::to_string(nearestTriIdx).c_str());
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
	//ImGui::Text("Dot: %f", dot);
	//ImGui::Text("Normal: {%f.3, %f.3, %f.3)", normal.x, normal.y, normal.z);
	if (dot < 0)
		normal = -normal;

	normal *= 0.01f;
	auto& shader = ShaderManager::GetShader("singleTriangle");
	shader.Bind();
	//shader.SetUniformMat4f("model", model);
	shader.SetUniformMat4f("view", cam->GetViewMatrix());
	shader.SetUniformMat4f("projection", cam->GetProjectionMatrix());
	shader.SetVec4f("u_color", 1, 1, 0.0f, 1.0f);
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_TriangleVBO));

	GLCall(glBindVertexArray(m_TriangleVAO));
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

	m_TriangleVAO = triangleVAO;
	m_TriangleVBO = triangleVBO;

	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}
void BVH::CreateBVHTextures()
{
	////split bvh node into 2 textures
	////tex 1: m_Indices:  4 ints
	////tex  2: min vecs: 3 floats
	////tex 3: max vecs : 3 floats
	//const unsigned int aabbCount = m_PoolPtr;
	//unsigned int intTexture;
	////std::vector<glm::ivec4> indicesPart;
	////std::copy( m_Pool ,std::)
	//for (uint32_t i = 0; i < m_PoolPtr; i++)
	//{
	//	//const int start = m_Pool[i].m_bounds.m_start;
	//	//const int end = m_Pool[i].m_end;
	//	//const int leftFirst = m_Pool[i].m_leftFirst;
	//	//const int m_Count = m_Pool[i].m_count;

	//	//indicesPart.emplace_back(glm::ivec4(start, end, leftFirst, m_Count));
	//}

	////std::cout << "nodes tex size: " << indicesPart.size() * sizeof(indicesPart[0]) / 1024 << "kb" << "\n";

	////aabb nodes
	//m_aabbNodesTexture = Texture1D(aabbCount, 4, dataType::INT32, &m_Indices[0], false);
	//intTexture = m_aabbNodesTexture.GetID();

	//std::vector<glm::vec3> minvec;
	////auto GetMin = [](AABB& aabb) { return aabb.min; };

	////std::transform(std::begin(m_localBounds), std::end(m_localBounds), std::back_inserter(minvec), GetMin);



	//m_minTexture = Texture1D(aabbCount, 3, dataType::FLOAT32, &minvec[0], false);
	////minVecTexture = m_minVecTex.GetID();

	////max bounds
	////auto Getmax = [](AABB& aabb) { return aabb.m_max.v; };

	//std::vector<glm::vec3> maxVec;
	////std::transform(std::begin(m_localBounds), std::end(m_localBounds), std::back_inserter(maxVec), Getmax);

	//m_maxTexture = Texture1D(aabbCount, 3, dataType::FLOAT32, &maxVec[0], false);

	////triangle texture
	//const std::vector<Triangle>& triangles = TriangleBuffer::GetTriangleBuffer();
	//const uint32_t triangleCount = static_cast<uint32_t>(triangles.size());

	//std::vector<glm::vec3> triBuffer;

	//for (const auto& t : triangles)
	//{
	//	triBuffer.emplace_back(t.A);
	//	triBuffer.emplace_back(t.B);
	//	triBuffer.emplace_back(t.C);
	//}
	//const uint32_t triBuffSize = static_cast<uint32_t>(triBuffer.size());
	//m_triangleTexture = Texture1D(triBuffSize, 3, dataType::FLOAT32, &triBuffer[0], false);
	//m_triangleIdxTexture = Texture1D(triangleCount, 1, dataType::UINT32, &m_Indices[0], false);
}
void BVH::DrawSingleAABB(Camera& cam, const uint32_t index)
{
	if (index < 0 || index >= m_PoolPtr)
	{
		std::cout << "Bvh draw index out of range!\n";
		return;
	}

	AABB& mat = m_Pool[index].m_bounds; //m_aabbMatrices???
	mat.Draw(cam, { 1.0f, 0.2f, 0.3f, 1.0f });
}

void BVH::CreateBuffers(const std::vector<Triangle>& tris)
{
	//GLuint m_bvh_ssbo = 0;
	//const unsigned int poolSize = sizeof(BVHNode) * m_PoolPtr;
	//glGenBuffers(1, &m_BVH_SSBO);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BVH_SSBO);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, poolSize, &m_Pool[0], GL_STATIC_COPY);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_BVH_SSBO);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	//
	////std::vector<Triangle>& triangles = TriangleBuffer::GetTriangleBuffer();
	////GLuint m_triangles_ssbo = 0;
	//glGenBuffers(1, &m_Tri_SSBO);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_Tri_SSBO);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, tris.size() * sizeof(Triangle), &tris[0], GL_STATIC_COPY);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_Tri_SSBO);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	//
	////GLuint m_tri_index_ssbo = 0;
	//glGenBuffers(1, &m_TriIdx_SSBO);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_TriIdx_SSBO);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_COPY);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_TriIdx_SSBO);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

bool BVH::IsBuilt() const
{
	return m_IsBuilt;
}

uint32_t BVH::GetBVHSize() const
{
	return m_PoolPtr;
}
