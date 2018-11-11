#pragma once
  
 
class Cube : public GameObject
{  
	 
public:
	
	Cube(const std::string& name);
	~Cube();

	std::vector<float>& GetMeshVertices() { return mesh_vertices; }
	std::vector<unsigned int>& GetMeshIndices() { return mesh_indices; }
 
};

