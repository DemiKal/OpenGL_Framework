#pragma once
//uniform sampler2D u_Texture;
class Model
{
public:

	struct  Armature {
		std::string name;
		int id;
		std::vector<std::shared_ptr<Armature>> children;
		std::shared_ptr< Armature> parent;
		glm::mat4 mat;

	};
	
	std::shared_ptr <Armature > armature;
	glm::mat4 model;
	std::vector<MeshNew> meshes;
	std::string directory;
	std::vector<Texture2D> textures_loaded;
	glm::mat4 inverse_root;
	glm::vec3 m_velocity = { 0.001f,0,0 };
	glm::vec3 m_position = { 0,0,0 };
	glm::vec3 m_rotation = { 0,0,0 };
	glm::vec3 m_scale = { 1,1,1 };

	unsigned int shaderIdx;

	//VertexBufferLayout vbl;
	Model() : model(glm::mat4(1.0f)), meshes(), directory(""),
		textures_loaded(), shaderIdx(0), inverse_root(glm::mat4(1.0f)) {}

	~Model() = default;

	Model(const std::string& path, const aiPostProcessSteps loadFlags) :
		model(glm::mat4(1.0f)), meshes(), directory(""), textures_loaded(), shaderIdx(0)
	{
		loadModel(path, loadFlags);
	}

	glm::mat4 GetModelMatrix() { return model; }
	void SetModelMatrix(const glm::mat4& mat) { model = mat; }
	void SetShader(const std::string& shadername);
	void Draw(const Camera& cam);
	GPUShader& GetShader() const;
	static Model CreateCube();
	static Model CreateCubeWireframe();
	void loadModel(const std::string& path, const aiPostProcessSteps LoadFlags);
	static Model CreatePlane();
	void processNode(aiNode* node, const aiScene* scene, std::shared_ptr<Armature>  armature);
	void UpdateModelMatrix();
	void Update(float deltaTime);

	void AddWeight(std::vector<float>& vertices, unsigned int vertex_index,
		unsigned int bone_index, GLuint bone_id, GLfloat weight);

	MeshNew processMesh(aiMesh* mesh, const aiScene* scene, std::shared_ptr <Armature> armature);
	inline MeshNew& getMesh(const unsigned int idx) { return meshes[idx]; }
	std::vector<Texture2D> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);



};


