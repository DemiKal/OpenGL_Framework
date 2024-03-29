#pragma once

#include "GameObject/Components/AABB.h"
#include "Rendering/Buffer/VertexBufferLayout.h"
#include "Animation/Animator.h"

class AABB;
class Shader;
class Texture2D;
class Animator;
struct MeshComponent;
enum class Meshtype
{
	Regular, Skybox
};

class Mesh
{

protected:
	uint32_t m_VAO{ 0 }, m_VBO{ 0 }, m_EBO{ 0 };
	bool m_AnimationLoaded = false;
	GLenum m_ElemDrawType = GL_TRIANGLES;
	std::string m_Directory = "Not from directory";
	std::string m_Filename = "Name not initialized";
	
	entt::hashed_string m_Tag = "";
	float m_LineThickness = 0.1f;

	void CreateBuffers();
public:
	Meshtype m_Meshtype = Meshtype::Regular;

	const std::string_view GetFilename() const;
	const std::string_view GetDirectory() const;
	std::vector<float> m_Vertices; //TODO make it dynamic for int and others
	std::vector<std::array< glm::vec2, 3>>  m_UVs; //TODO make it dynamic for int and others

	std::vector<unsigned int> m_Indices;
	std::vector<Texture2D> m_Textures;
	Animator m_animator;
	VertexBufferLayout m_VertexBufferLayout;
	std::vector<glm::vec4> m_PositionVertices; //m_Vertices, but only the positions for other purposes
	AABB m_aabb;	//TODO remove and delegate to a component
	AABB m_aabb_OG;

	Mesh() {}
	void LoadAnimations(const aiScene* scene, const aiMesh* mesh, const std::unordered_map<std::string, unsigned>& pairs, const std::vector<Joint>& bones);
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

	//Mesh(std::vector<float>& vertices, VertexBufferLayout vbl);

	~Mesh();

	const uint32_t GetIndicesCount() const { return static_cast<uint32_t>(m_Indices.size()); }
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

	[[nodiscard]] const uint32_t GetVAO()const;
	[[nodiscard]] const uint32_t GetVBO()const;
	[[nodiscard]] const uint32_t GetEBO()const;
	[[nodiscard]] const uint32_t GetVertexCount() const;
	[[nodiscard]] const uint32_t GetTriangleCount() const;

	[[nodiscard]] bool HasAnimation() const;
	[[nodiscard]] bool HasFaceIndices() const;

	void MakeWireFrame();
	void DrawWireFrame(const Camera& camera, const MeshComponent& mc, const glm::mat4& modelMatrix) const;
	void DrawNormals(const Camera& camera, const glm::mat4& modelMatrix, const glm::vec4& color, float magnitude); //TODO:: just pass mesh component?
	void AddTexture(const Texture2D& tex);
	void LoadBoneData(const aiScene* scene, const aiMesh* mesh, std::unordered_map<std::string, unsigned>& boneNames, std::vector<Joint>& bones);
	void LoadMaterialTextures(const aiMaterial* material, aiTextureType type, const std::string& typeName, const aiScene* scene);
	void Draw(const Camera& camera, const glm::mat4& transform, Shader& shader);
	void Draw(const glm::mat4& proj, const glm::mat4& view, Shader& shader);
	void Draw(Shader& shader);
	void DrawInstanced(int instanceCount);

};



