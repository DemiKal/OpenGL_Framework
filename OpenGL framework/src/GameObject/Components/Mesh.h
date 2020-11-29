#pragma once

#include "Texture2D.h"
#include "Animation/Animator.h"
#include "GameObject/Components/AABB.h"
#include "Rendering/Buffer/VertexBufferLayout.h"
#include "Rendering/Shader.h"

class Mesh
{
protected:
	unsigned int m_VAO{ 0 }, m_VBO{ 0 }, m_EBO{ 0 };
	//vertex bools
	bool m_AnimationLoaded = false;
	GLenum m_ElemDrawType = GL_TRIANGLES;
	std::string m_Directory = "Directory Not initialized";
	std::string m_Filename = "Name not initialized";
	unsigned int m_WireVAO{ 0 }, m_WireVBO{ 0 };
	void CreateBuffers();
public:
	float m_LineThickness = 0.1f;

	std::vector<float> m_Vertices; //TODO make it dynamic for int and others
	std::vector<std::array< glm::vec2, 3>>  m_UVs; //TODO make it dynamic for int and others

	std::vector<unsigned int> m_Indices;
	std::vector<Texture2D> m_Textures;
	Animator m_animator;
	VertexBufferLayout m_VertexBufferLayout;
	std::vector<glm::vec3> m_PositionVertices; //m_Vertices, but only the positions for other purposes
	AABB m_aabb;	//TODO remove and delegate to a component
	AABB m_aabb_OG;

	Mesh() {}
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
	Mesh(const std::vector<float>& p_vertices, const VertexBufferLayout& vbl);

	Mesh(std::vector<float>& vertices, VertexBufferLayout vbl);

	~Mesh();

	//Mesh(std::vector<float>& vertices, std::vector<unsigned int>& indices, const VertexBufferLayout& vbl);
	Mesh(std::vector<float>& vertices, std::vector<unsigned int>& indices, const VertexBufferLayout& vbl);

	//Mesh(const Mesh& m) = delete;

	static Mesh CreatePlane();
	static Mesh CreateCube();
	static Mesh CreateCubeWireframe();
	//static void ProcessNode(const aiNode* node, const aiScene* scene, const std::string& directory);
	//[[nodiscard]] static std::optional<uint32_t> LoadFromFile(const std::string& path, const aiPostProcessSteps loadFlags);

	inline GLenum GetElemDrawType() const;
	void SetElemDrawType(const GLenum enm);

	[[nodiscard]] unsigned int GetVAO();
	[[nodiscard]] unsigned int GetVBO();
	[[nodiscard]] unsigned int GetEBO();
	[[nodiscard]] unsigned int GetVertexCount() const;

	[[nodiscard]] bool HasAnimation() const;
	[[nodiscard]] bool HasFaceIndices() const;

	void MakeWireFrame();
	void DrawWireFrame(const Camera& camera, const glm::mat4& model_matrix) const;
	void AddTexture(const Texture2D& tex);
	void LoadMaterialTextures(const aiMaterial* material, aiTextureType type, const std::string& typeName,const aiScene* scene);
	void Draw(const Camera& camera, const glm::mat4& transform, Shader& shader);
	void Draw(Shader& shader);

};



