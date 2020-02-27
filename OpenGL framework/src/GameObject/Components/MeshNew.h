#pragma once

#include "Animation/Animator.h"
#include "GameObject/Components/AABB.h"
#include "Rendering/Buffer/VertexBufferLayout.h"
#include "Rendering/GPUShader.h"


class MeshNew
{
protected:
	unsigned int VAO, VBO, EBO;
	//vertex bools
	bool animation_loaded = false;
	GLenum m_elemDrawType = GL_TRIANGLES;

	unsigned int m_wireVAO, m_wireVBO;
	void setupMesh();
	float xxxxxxxxxxxxxxxxxxxxx = 1;
public:
	float lineThickness = 0.1f;
	static MeshNew CreateCubeWireframe();
	inline GLenum GetElemDrawType() const { return m_elemDrawType; };
	void SetElemDrawType(const GLenum enm) { m_elemDrawType = enm; }

	std::vector<float> vertices; //TODO make it dynamic for ints and others
	std::vector<unsigned int> indices;
	std::vector<Texture2D> m_textures;
	Animator m_animator;
	VertexBufferLayout m_VertexBufferLayout;
	std::vector<glm::vec3> positionVertices; //vertices, but only the positions for other purposes
	AABB m_aabb;
	AABB m_aabb_OG;

	MeshNew() : VAO(0), VBO(0), EBO(0), indices(), m_textures() {};

	~MeshNew() = default;
	//MeshNew(MeshNew& a) = default;
	MeshNew(
		const std::vector<float>& vertices,
		const std::vector <unsigned int>& indices,
		const std::vector <Texture2D>& textures,
		const Animator& animator,
		const VertexBufferLayout& vbl);

	void Draw(GPUShader& shader);
	bool hasAnimation() const { return animation_loaded; }
	unsigned int GetVAO() { return VAO; }
	unsigned int GetVBO() { return VBO; }
	unsigned int GetEBO() { return EBO; }

	void MakeWireFrame();
	void DrawWireFrame(const Camera& camera, const glm::mat4& model_matrix);

	static MeshNew CreatePlane();

	static MeshNew CreateCube();

	inline bool HasFaceIndices() { return indices.size() > 0; }
	inline unsigned int GetVertexCount() { return vertices.size(); }

	//void SetVAO(unsigned int val) { VAO = val; }
	//void SetVBO(unsigned int val) { VBO = val; }
	//void SetEBO(unsigned int val) { EBO = val; }
	void addTexture(const Texture2D& tex)
	{
		m_textures.emplace_back(tex);
	}

	//void AddWeight(std::vector<float>& vertices, unsigned int vertex_index, unsigned int bone_index,
	//	GLuint bone_id, GLfloat weight);

	//void AddWeight(std::vector<float> vertices&, unsigned int vertex_index, unsigned int bone_index, GLuint bone_id, GLfloat weight);
	//void AddTexture(const Texture2D& tex) { textures.emplace_back(tex); } 
};



