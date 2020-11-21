#include "precomp.h"
#include "MeshManager.h"


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
