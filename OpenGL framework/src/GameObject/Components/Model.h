#pragma once
//uniform sampler2D u_Texture;
class Model
{
public:
	enum LoadType {
		OBJLOAD,
		PLANE,
		CUBE
	};

	Model() = default;
	~Model() = default;

	Model(const std::string& path, LoadType type) :
		model(glm::mat4(1.0f)), meshes(), directory(""), textures_loaded(), shaderIdx(0)
	{
		switch (type) {
			case OBJLOAD: loadModel(path);
			case PLANE: LoadPlane();
			case CUBE: ;
		}
	}

	void LoadPlane();
	glm::mat4 GetModelMatrix() { return model; }
	void SetModelMatrix(const glm::mat4& mat) { model = mat; }
	void SetShader(const std::string& shadername);
	void Draw(const Camera& cam);
	GPUShader& GetShader() const { return   ShaderManager::getShaderIdx(shaderIdx); }
	static 	Model& CreatePlane();

 
	glm::mat4 model;
	std::vector<MeshNew> meshes;
	std::string directory;
	std::vector<Texture2D> textures_loaded;
	unsigned int shaderIdx;

	void loadModel(const std::string& path);

	void processNode(aiNode *node, const aiScene *scene);
	MeshNew processMesh(aiMesh *mesh, const aiScene *scene);

	std::vector<Texture2D> loadMaterialTextures(
		aiMaterial* mat, aiTextureType type, const std::string& typeName);
};


