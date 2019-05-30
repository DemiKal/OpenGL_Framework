#pragma once
//uniform sampler2D u_Texture;
class Model
{
public:

	/*  Functions   */
	Model(const std::string& path)
	{
		loadModel(path);
	}
	
	void Draw(Shader& shader)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader);
	}

	~Model();

private:
	/*  Model Data  */
	std::vector<MeshNew> meshes;
	std::string directory;


	/*  Functions   */
	void loadModel(const std::string& path);
	
	void processNode(aiNode *node, const aiScene *scene);
	MeshNew processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture*> loadMaterialTextures(
		aiMaterial* mat, aiTextureType type, const std::string& typeName) const;

	//std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
	//	const std::string& typeName);
};


