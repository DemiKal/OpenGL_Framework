#pragma once
//uniform sampler2D u_Texture;
class Model
{
public:
	enum class LoadType {
		OBJLOAD,
		PLANE,
		CUBE
	};

	struct  Armature {
		std::string name;
		int id;
		std::vector<std::shared_ptr<Armature>> children;
		std::shared_ptr< Armature> parent;
		glm::mat4 mat;
		
	};

	glm::mat4 model;
	std::vector<MeshNew> meshes;
	std::string directory;
	std::vector<Texture2D> textures_loaded;
	unsigned int shaderIdx;

	//VertexBufferLayout vbl;
	Model() : model(glm::mat4(1.0f)), meshes(), directory(""), textures_loaded(), shaderIdx(0) {}
	~Model() = default;

	Model(const std::string& path, LoadType type) :
		model(glm::mat4(1.0f)), meshes(), directory(""), textures_loaded(), shaderIdx(0)
	{
		loadModel(path);
	}

	glm::mat4 GetModelMatrix() { return model; }
	void SetModelMatrix(const glm::mat4& mat) { model = mat; }
	void SetShader(const std::string& shadername);
	void Draw(const Camera& cam);
	GPUShader& GetShader() const { return   ShaderManager::getShaderIdx(shaderIdx); }
	static Model CreateCube();
	void loadModel(const std::string& path);
	static Model CreatePlane();
	void processNode(aiNode *node, const aiScene *scene, std::shared_ptr<Armature>  armature  );
	void AddWeight(std::vector<float>& vertices, unsigned int vertex_index, unsigned int bone_index, GLuint bone_id, GLfloat weight);
	MeshNew processMesh(aiMesh *mesh, const aiScene *scene, std::shared_ptr <Armature> armature);

	std::vector<Texture2D> loadMaterialTextures(
		aiMaterial* mat, aiTextureType type, const std::string& typeName);
};


