#pragma once
#include "GameObject/Components/Mesh.h"


class MeshManager
{
 
	std::vector<Mesh> m_Meshes;
	MeshManager();

	static MeshManager& GetInstance();
	void CreateLinePrimitive();
	void CreateTrianglePrimitive();
	void CreatePlanePrimitive();
	void CreateCubePrimitive();
public:
	static std::vector<Mesh> GetMeshes();
	static Mesh& GetMesh(  int i);

	
};

