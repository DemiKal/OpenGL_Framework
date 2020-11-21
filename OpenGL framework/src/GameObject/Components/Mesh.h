#pragma once

#include "Texture2D.h"
#include "Animation/Animator.h"
#include "GameObject/Components/AABB.h"
#include "Rendering/Buffer/VertexBufferLayout.h"
#include "Rendering/Shader.h"

class Mesh
{
protected:
	unsigned int m_VAO, m_VBO, m_EBO;
	//vertex bools
	bool m_AnimationLoaded = false;
	GLenum m_ElemDrawType = GL_TRIANGLES;
	std::string m_Directory;
	unsigned int m_WireVAO, m_WireVBO;
	void CreateBuffers();
public:
	float m_LineThickness = 0.1f;

	std::vector<float> vertices; //TODO make it dynamic for ints and others
	std::vector<unsigned int> m_Indices;
	std::vector<Texture2D> m_Textures;
	Animator m_animator;
	VertexBufferLayout m_VertexBufferLayout;
	std::vector<glm::vec3> positionVertices; //vertices, but only the positions for other purposes
	AABB m_aabb;
	AABB m_aabb_OG;

	Mesh() : m_WireVAO(0), m_WireVBO(0), m_VAO(0), m_VBO(0), m_EBO(0), m_Indices(), m_Textures() {};
	Mesh(
		const aiMesh* mesh,
		const aiScene* scene,
		const std::string& directory);

	Mesh(
		const std::vector<float>& vertices,
		const std::vector <unsigned int>& indices,
		const std::vector <Texture2D>& textures,
		const Animator& animator,
		const VertexBufferLayout& vbl);

	~Mesh() = default;

	static Mesh CreatePlane();
	static Mesh CreateCube();
	static Mesh CreateCubeWireframe();
	static void ProcessNode(const aiNode* node, const aiScene* scene, const std::string& directory);
	[[nodiscard]] static std::optional<uint32_t> LoadFromFile(const std::string& path, const aiPostProcessSteps loadFlags);

	inline GLenum GetElemDrawType() const;
	void SetElemDrawType(const GLenum enm);

	unsigned int GetVAO();
	unsigned int GetVBO();
	unsigned int GetEBO();
	unsigned int GetVertexCount() const;

	bool HasAnimation() const;
	bool HasFaceIndices() const;

	void MakeWireFrame();
	void DrawWireFrame(const Camera& camera, const glm::mat4& model_matrix) const;
	void AddTexture(const Texture2D& tex);
	void LoadMaterialTextures(const aiMaterial* mat, const aiTextureType type, const std::string& typeName);
	void Draw(const Camera& camera, const glm::mat4& transform, Shader& shader);
	void Draw(Shader& shader);

};



