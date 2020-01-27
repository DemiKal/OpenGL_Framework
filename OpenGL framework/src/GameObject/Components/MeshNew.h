#pragma once
class MeshNew
{

protected:
	unsigned int VAO, VBO, EBO, vertexCount;
	//vertex bools
	bool pos_loaded, normals_loaded, UVs_loaded, tangents_loaded;

	/*  Functions    */
	void setupMesh();

public:
	std::vector<float> vertices; //TODO make it dynamic for ints and others
	std::vector<unsigned int> indices;
	std::vector<Texture2D> m_textures;

	MeshNew() : indices(), m_textures() {};
	~MeshNew() = default;
	
	MeshNew(
		std::vector<float>& vertices,
		std::vector <unsigned int>& indices,
		std::vector <Texture2D>& textures,
		std::vector<bool>& bools);
	
	void Draw(const GPUShader& shader);
	bool hasPositions() const { return pos_loaded; }
	bool hasNormals() const { return normals_loaded; }
	bool hasUVs() const { return UVs_loaded; }
	bool hasTangents() const { return tangents_loaded; }
	unsigned int GetVAO() { return VAO; }
	unsigned int GetVBO() { return VBO; }
	unsigned int GetEBO() { return EBO; }

	static MeshNew CreatePlane();

	static MeshNew CreateCube();
	inline unsigned int GetVertexCount() { return vertexCount; }

	void SetVAO(unsigned int val) { VAO = val; }
	void SetVBO(unsigned int val) { VBO = val; }
	void SetEBO(unsigned int val) { EBO = val; }
	//void AddTexture(const Texture2D& tex) { textures.emplace_back(tex); } 
};



