#pragma once

#include "Texture2D.h"
#include "Animation/Animator.h"
#include "GameObject/Components/AABB.h"
#include "Rendering/Buffer/VertexBufferLayout.h"
#include "Rendering/Shader.h"

class Mesh
{
protected:
	unsigned int VAO, VBO, EBO;
	//vertex bools
	bool animation_loaded = false;
	GLenum m_elemDrawType = GL_TRIANGLES;
	std::string m_Directory;
	unsigned int m_wireVAO, m_wireVBO;
	void setupMesh();
public:
	static std::vector<Mesh> m_Meshes;

	float lineThickness = 0.1f;
	static Mesh CreateCubeWireframe();
	inline GLenum GetElemDrawType() const;
	void SetElemDrawType(const GLenum enm);

	std::vector<float> vertices; //TODO make it dynamic for ints and others
	std::vector<unsigned int> m_Indices;
	std::vector<Texture2D> m_Textures;
	Animator m_animator;
	VertexBufferLayout m_VertexBufferLayout;
	std::vector<glm::vec3> positionVertices; //vertices, but only the positions for other purposes
	AABB m_aabb;
	AABB m_aabb_OG;

	Mesh() : m_wireVAO(0), m_wireVBO(0), VAO(0), VBO(0), EBO(0), m_Indices(), m_Textures() {};
	Mesh(
		const aiMesh* mesh, 
		const aiScene* scene,
		const std::string& directory);

	//Mesh(Mesh& a) = default;
	Mesh(
		const std::vector<float>& vertices,
		const std::vector <unsigned int>& indices,
		const std::vector <Texture2D>& textures,
		const Animator& animator,
		const VertexBufferLayout& vbl);

	~Mesh() = default;

	void LoadMaterialTextures(
		const aiMaterial* mat, 
		const aiTextureType type, 
		const std::string& typeName);
	
	static void ProcessNode(
		const aiNode* node, 
		const aiScene* scene, 
		const std::string& directory);
	//static bool ProcessMesh(aiMesh* mesh, aiScene* scene);

	[[nodiscard]] static std::optional<uint32_t> LoadFromFile(const std::string& path, const aiPostProcessSteps loadFlags);
	void Draw(Shader& shader);
	void Draw(const Camera& camera, const glm::mat4& transform, Shader& shader); 
	
	bool HasAnimation() const;
	unsigned int GetVAO();
	unsigned int GetVBO();
	unsigned int GetEBO();

	void MakeWireFrame();
	void DrawWireFrame(const Camera& camera, const glm::mat4& model_matrix) const;

	static Mesh CreatePlane();

	static Mesh CreateCube();

	bool HasFaceIndices() const;
	unsigned int GetVertexCount() const;

	//void SetVAO(unsigned int val) { VAO = val; }
	//void SetVBO(unsigned int val) { VBO = val; }
	//void SetEBO(unsigned int val) { EBO = val; }
	void AddTexture(const Texture2D& tex);

	//void AddWeight(std::vector<float>& vertices, unsigned int vertex_index, unsigned int bone_index,
	//	GLuint bone_id, GLfloat weight);

	//void AddWeight(std::vector<float> vertices&, unsigned int vertex_index, unsigned int bone_index, GLuint bone_id, GLfloat weight);
	//void AddTexture(const Texture2D& tex) { textures.emplace_back(tex); } 
};



