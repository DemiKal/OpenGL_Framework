#pragma once
//uniform sampler2D u_Texture;
class Model
{
public:

	Model() = default;

	/*  Functions   */
	Model(const std::string& path)
	{
		loadModel(path);
	}

	void Draw(const Camera& cam)
	{
		//auto d = shader.get();
		shader.Bind();
		const int shaderID = shader.m_RendererID;

		int count;
		glGetProgramiv(shaderID, GL_ACTIVE_UNIFORMS, &count);
		printf("Active Uniforms: %d\n", count);
		GLint maxLength;
		glGetProgramiv(shaderID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);

		std::vector < std::string> names;

		for (unsigned int i = 0; i < count; i++)
		{
			GLchar name[GL_ACTIVE_UNIFORM_MAX_LENGTH + 1];
			int length, size;
			GLenum type;

			glGetActiveUniform(shaderID,
				i, maxLength, &length, &size, &type, name);

			const GLubyte* ss = glGetString(type);
			printf("Uniform #%d Type: %u Name: %s\n", i, type, name);
			names.emplace_back(name);
		}

		const glm::mat4 view = cam.GetViewMatrix();
		const glm::mat4 proj = cam.GetProjectionMatrix();

		shader.SetUniformMat4f("model", model);
		shader.SetUniformMat4f("view", view);
		shader.SetUniformMat4f("projection", proj);

		for (auto& mesh : meshes)
			mesh.Draw(shader);

		shader.Unbind();
	}

	glm::mat4 model = glm::mat4(1.0f);

	Shader GetShader() const { return  shader; }
	~Model();
	Shader  shader;

private:
	/*  Model Data  */
	std::vector<MeshNew> meshes;
	std::string directory;
	std::vector<Texture2D> textures_loaded;

	/*  Functions   */
	void loadModel(const std::string& path);

	void processNode(aiNode *node, const aiScene *scene);
	MeshNew processMesh(aiMesh *mesh, const aiScene *scene);

	std::vector<Texture2D> loadMaterialTextures(
		aiMaterial* mat, aiTextureType type, const std::string& typeName);

	//std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
	//	const std::string& typeName);
};


