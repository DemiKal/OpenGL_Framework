#pragma once
#include "GameObject/Components/Mesh.h"

class MeshManager
{
	std::vector<Mesh> m_Meshes;
	MeshManager();

	void ProcessNode(const aiNode* node, const aiScene* scene, const std::string& directory);

	static MeshManager& GetInstance();
	void CreateLinePrimitive();
	void CreateTrianglePrimitive();
	void CreatePlanePrimitive();
	void CreateCubePrimitive();
	void CreateCubeWireframePrimitive();
public:
	static std::vector<Mesh>& GetMeshes();
	static Mesh& GetMesh(int i);
	static std::optional<uint32_t> LoadFromFile(const std::string& path, aiPostProcessSteps loadFlags);

	static void Init();
};

