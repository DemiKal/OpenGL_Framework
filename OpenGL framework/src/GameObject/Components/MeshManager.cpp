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
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return std::nullopt;
	}

	const std::string directory = path.substr(0, path.find_last_of('/'));

	MeshManager& instance = GetInstance();
	uint32_t idx = instance.m_Meshes.size();
	instance.ProcessNode(scene->mRootNode, scene, directory);

	//TODO: delete scene;
	return idx; //TODO: use std optional?
}

MeshManager::MeshManager()
{
	CreateLinePrimitive();
	CreateTrianglePrimitive();
	CreatePlanePrimitive();
	CreateCubePrimitive();
}

MeshManager& MeshManager::GetInstance()
{
	static MeshManager instance;
	return instance;
}

void MeshManager::CreateLinePrimitive()
{
}

void MeshManager::CreateCubePrimitive()
{
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

std::vector<Mesh> MeshManager::GetMeshes()
{
	return GetInstance().m_Meshes;
}
