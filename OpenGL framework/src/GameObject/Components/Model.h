#pragma once
#include "GameObject/Components/Texture2D.h"
#include "GameObject/Components/Mesh.h"
#include "Rendering/Shader.h"

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

	std::shared_ptr <Armature> m_armature;
	glm::mat4 m_modelMatrix;
	std::vector<Mesh> m_meshes;
	std::string m_directory;
	std::vector<Texture2D> m_textures_loaded;
	glm::mat4 m_inverseRoot;
	glm::vec3 m_velocity = { 0.001f, 0, 0 };
	glm::vec3 m_position = { 0,0,0 };
	glm::vec3 m_rotation = { 0,0,0 };
	glm::vec3 m_scale = { 1,1,1 };
	std::string m_name;
	unsigned int m_shaderIdx;

	//VertexBufferLayout vbl;
	Model()
		:
		m_modelMatrix(glm::mat4(1.0f)),
		m_directory(""),
		m_inverseRoot(glm::mat4(1.0f)),
		m_shaderIdx(0)
	{}

	~Model() 
	{
		fmt::print("MODEL DESTRUCTED!");
	} 

	Model(const std::string& path, aiPostProcessSteps loadFlags);

	[[nodiscard]] glm::mat4 GetModelMatrix() const;
	void SetModelMatrix(const glm::mat4& mat);
	void SetShader(const std::string& shader_name);
	void Draw(const Camera& cam);
	Shader& GetShader() const;
	static Model CreateCube();
	static Model CreateCubeWireframe();
	void LoadModel(const std::string& path, const aiPostProcessSteps loadFlags);
	static Model CreatePlane();
	void ProcessNode(aiNode* node, const aiScene* scene, std::shared_ptr<Armature>  armature);
	void UpdateModelMatrix();
	void Update(float deltaTime);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, std::shared_ptr <Armature> armature);
	Mesh& GetMesh(const unsigned int idx);
	std::vector<Texture2D> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);
	void Draw(const glm::mat4& projection, Shader& shader);
	void Draw(const Camera& cam, Shader& shader);
	void AddWeight(std::vector<float>& vertices, unsigned int vertexIndex, unsigned int boneIndex, GLuint boneId, GLfloat weight);


};


