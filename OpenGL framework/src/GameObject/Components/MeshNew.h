#pragma once
class MeshNew
{

protected:
	unsigned int VAO, VBO, EBO;
	//vertex bools
	bool animation_loaded = false;
	GLenum elemDrawType = GL_TRIANGLES;
	
	/*  Functions    */
	void setupMesh();


public:
	static MeshNew CreateCubeWireframe();
	inline GLenum GetElemDrawType() { return elemDrawType; };
	void SetElemDrawType(const GLenum enm) { elemDrawType = enm; }
	
	std::vector<float> vertices; //TODO make it dynamic for ints and others
	std::vector<unsigned int> indices;
	std::vector<Texture2D> m_textures;
	Animator m_animator;
	VertexBufferLayout m_VertexBufferLayout;
	AABB m_aabb;

	MeshNew() : indices(), m_textures(), VAO(0), VBO(0), EBO(0) {};

	~MeshNew() = default;
	//MeshNew(MeshNew& a) = default;
	MeshNew(
		const std::vector<float>& vertices,
		const std::vector <unsigned int>& indices,
		const std::vector <Texture2D>& textures,
		const Animator& animator,
		const VertexBufferLayout& vbl,
		const AABB& aabb);

	void Draw(GPUShader& shader);
	//bool hasPositions() const { return pos_loaded; }
	//bool hasNormals() const { return normals_loaded; }
	//bool hasUVs() const { return UVs_loaded; }
	//bool hasTangents() const { return tangents_loaded; }
	bool hasAnimation() const { return animation_loaded; }
	unsigned int GetVAO() { return VAO; }
	unsigned int GetVBO() { return VBO; }
	unsigned int GetEBO() { return EBO; }

	static MeshNew CreatePlane();

	static MeshNew CreateCube();

	inline bool HasFaceIndices() { return indices.size() > 0; }
	inline size_t GetVertexCount() { return vertices.size(); }

	//void SetVAO(unsigned int val) { VAO = val; }
	//void SetVBO(unsigned int val) { VBO = val; }
	//void SetEBO(unsigned int val) { EBO = val; }
	void addTexture(const Texture2D& tex) 
	{
		m_textures.emplace_back(tex);
	}

	//void AddWeight(std::vector<float>& vertices, unsigned int vertex_index, unsigned int bone_index,
	//	GLuint bone_id, GLfloat weight);

	//void AddWeight(std::vector<float> vertices&, unsigned int vertex_index, unsigned int bone_index, GLuint bone_id, GLfloat weight);
	//void AddTexture(const Texture2D& tex) { textures.emplace_back(tex); } 
};



