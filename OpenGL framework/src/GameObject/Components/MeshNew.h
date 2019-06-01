#pragma once

class MeshNew
{
private:
	unsigned int VAO, VBO, EBO;
	//vertex bools
	bool pos_loaded, normals_loaded, UVs_loaded, tangents_loaded;

	/*  Functions    */
	void setupMesh();

public:
	std::vector<VertexNew> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture2D> textures;
	void Draw(const GPUShader& shader);

	MeshNew() = default;
	~MeshNew() = default;
	MeshNew(std::vector<VertexNew>& vertices,
		std::vector <unsigned int>& indices,
		std::vector <Texture2D>& textures,
		std::vector<bool> & bools);

	bool hasPositions() const { return pos_loaded; }
	bool hasNormals() const { return normals_loaded; }
	bool hasUVs() const { return UVs_loaded; }
	bool hasTangents() const { return tangents_loaded; }


};

