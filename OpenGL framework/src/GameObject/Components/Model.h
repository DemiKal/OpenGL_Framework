#pragma once
//uniform sampler2D u_Texture;
class Model
{
public:
	glm::mat4 model = glm::mat4(1.0f);


	Model() = default;
	~Model() = default;

	Model(const std::string& path) { loadModel(path); }

	void SetShader(const std::string& shadername);
	void Draw(const Camera& cam);
	GPUShader& GetShader() const { return   ShaderManager::getShaderIdx(shaderIdx); }

private:
	std::vector<MeshNew> meshes;
	std::string directory;
	std::vector<Texture2D> textures_loaded;
	unsigned int shaderIdx =0 ;

	void loadModel(const std::string& path);

	void processNode(aiNode *node, const aiScene *scene);
	MeshNew processMesh(aiMesh *mesh, const aiScene *scene);
	
	std::vector<Texture2D> loadMaterialTextures(
		aiMaterial* mat, aiTextureType type, const std::string& typeName);
};


