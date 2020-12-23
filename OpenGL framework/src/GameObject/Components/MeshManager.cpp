#include "precomp.h"
#include "MeshManager.h"

void MeshManager::ProcessNode(
	const aiNode* node,
	const aiScene* scene,
	const std::string& directory)
{

	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_Meshes.emplace_back(mesh, scene, directory);
	}

	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, directory);
	}
}

std::optional<uint32_t> MeshManager::LoadFromFile(const std::string& path, const aiPostProcessSteps loadFlags)
{
	Assimp::Importer import;
	const auto standardFlags = aiProcess_GenSmoothNormals | aiProcess_GenBoundingBoxes;
	const auto flagsComposed = standardFlags | loadFlags;

	const aiScene* scene = import.ReadFile(path, flagsComposed);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		fmt::print("ERROR::ASSIMP\n", import.GetErrorString());
		return std::nullopt;
	}

	const std::string directory = path.substr(0, path.find_last_of('/'));

	MeshManager& instance = GetInstance();
	const uint32_t idx = instance.m_Meshes.size();	//TODO: make indexing work with multiple meshes loaded
	instance.ProcessNode(scene->mRootNode, scene, directory);

	//TODO: delete scene?;
	return idx; //TODO: use std optional?
}

std::tuple<uint32_t, uint32_t> MeshManager::GetLineBuffer()
{
	auto& i = GetInstance();
	return { i.m_LineVAO, i.m_LineVBO };
}

MeshManager::MeshManager()
{
	CreateCubePrimitive();
	CreateCubeWireframePrimitive();
	CreateLinePrimitive();
	CreateSkyBoxCube();
}

void MeshManager::CreateSkyBoxCube()
{
	const std::vector<float> skyboxVertices = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	VertexBufferLayout vbl;
	vbl.Push<float>(3, VertexType::POSITION);
	Mesh mesh(skyboxVertices, vbl);
	mesh.m_Meshtype = Meshtype::Skybox;

	const std::vector<std::string> cubemapFiles
	{
		"Assets/textures/skyboxes/Ice/right.jpg",
		"Assets/textures/skyboxes/Ice/left.jpg",
		"Assets/textures/skyboxes/Ice/top.jpg",
		"Assets/textures/skyboxes/Ice/bottom.jpg",
		"Assets/textures/skyboxes/Ice/front.jpg",
		"Assets/textures/skyboxes/Ice/back.jpg"
	};
	Texture2D skybox;

	skybox.LoadCubemap(cubemapFiles);
	mesh.AddTexture(skybox);

	m_Meshes.emplace_back(std::move(mesh));
}

MeshManager& MeshManager::GetInstance()
{
	static MeshManager instance;

	return instance;
}

uint32_t MeshManager::GetLineVAO()
{
	return m_LineVAO;
}

void MeshManager::CreateLinePrimitive()
{
	glGenVertexArrays(1, &m_LineVAO);
	glGenBuffers(1, &m_LineVBO);
	glBindVertexArray(m_LineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 2, nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
	glVertexAttribDivisor(1, 1);
	//glGenBuffers(1, &cubeEBO));
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO));
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices.size() * sizeof(unsigned int), &cubeIndices[0], GL_STATIC_DRAW));
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MeshManager::Init()
{
	//TODO: fix for release mode?
	MeshManager& instance = GetInstance();
	fmt::print("Mesh manager Instance initialized!");
}

Mesh* MeshManager::GetMesh(const Meshtype t)
{
	auto& inst = GetInstance();
	for (auto& mesh : inst.m_Meshes)
		if (mesh.m_Meshtype == t)
			return &mesh;
	return nullptr;
}

void MeshManager::CreateCubeWireframePrimitive()
{
	std::vector<float> cubeVertices = {
	 -0.5f, -0.5f, 0.5f,
	 0.5f, -0.5f, 0.5f,
	 0.5f, 0.5f, 0.5f,
	 -0.5f,0.5f, 0.5f,
	 -0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, 0.5f, -0.5f,
	 -0.5f,0.5f, -0.5f,
	};

	std::vector<unsigned int> cubeIndices = {
	   0,1, 1,2, 2,3, 3,0,
	   1,5, 5,6, 6,2,
	   5,4, 6,7,
	   4,7, 0,4, 3,7
	};

	VertexBufferLayout vbl;
	vbl.Push<float>(3, VertexType::POSITION);

	m_Meshes.emplace_back(cubeVertices, cubeIndices, vbl);
	m_Meshes.back().SetElemDrawType(GL_LINES);
}

void MeshManager::CreateCubePrimitive()
{
	const std::vector<float> cubeVertices = {
		// positions          // texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	VertexBufferLayout vbl;
	vbl.Push<float>(3, VertexType::POSITION);
	vbl.Push<float>(2, VertexType::TEXCOORD);

	m_Meshes.emplace_back(cubeVertices, vbl);
}

void MeshManager::CreateTrianglePrimitive()
{

}

Mesh& MeshManager::GetMesh(const int i)
{
	return GetInstance().m_Meshes[i];
}

void MeshManager::CreatePlanePrimitive()
{
}

std::vector<Mesh>& MeshManager::GetMeshes()
{
	return GetInstance().m_Meshes;
}
