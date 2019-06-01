#pragma once


class GameObject
{
protected: class Triangle
{
public: glm::vec3  v1 ;
		glm::vec3  v2 ;
		glm::vec3  v3 ;

		Triangle() = default;
		Triangle(glm::vec3& v1_, glm::vec3& v2_, glm::vec3& v3_)
		{
			v1 = v1_;
			v2 = v2_;
			v3 = v3_;
		}

		Triangle operator*(const glm::mat4& mat)  const
		{
			auto v1_ = glm::vec3(mat * glm::vec4(v1, 1));
			auto v2_ = glm::vec3(mat * glm::vec4(v2, 1));
			auto v3_ = glm::vec3(mat * glm::vec4(v3, 1));

			return { v1_, v2_, v3_ };
		}


		void ApplyTransform(glm::mat4& mat)
		{
			auto v1_ = mat * glm::vec4(v1, 1);
			auto v2_ = mat * glm::vec4(v2, 1);
			auto v3_ = mat * glm::vec4(v3, 1);

			v1.x = v1_.x;  v1.y = v1_.y; v1.z = v1_.z;
			v2.x = v2_.x;  v2.y = v2_.y; v2.z = v2_.z;
			v3.x = v3_.x;  v3.y = v3_.y; v3.z = v3_.z;

		};
};

public:
	GameObject(const std::string& name);
	~GameObject();
	void SetUniformMat4f(const std::string& str, glm::mat4&  mat);

private:
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;
	std::string  m_name;
	bool enabled = true;
protected:
	std::vector<float> mesh_vertices;
	std::vector<unsigned int> mesh_indices;
	std::vector<Triangle> mesh_triangles;
public:
	Renderer renderer;
	VertexArray* m_va;
	VertexBuffer* m_vb;
	VertexBufferLayout* m_layout;
	IndexBuffer* m_ib;
	GPUShader* m_shader;
	Texture* m_texture;
	Transform* m_transform;
	IndexedModel idxmdl;
	void LoadMesh(const std::string& filename);
	inline std::vector<Triangle> GetMeshTriangles()
	{
		glm::mat4 model_matrix = m_transform->GetModelMatrix();
		std::vector<Triangle> world_space_triangles;

		for (Triangle t : mesh_triangles)
		{
			Triangle t2 = t * model_matrix;
			world_space_triangles.emplace_back(t2);
		}

		return world_space_triangles;
	}


	void CalcMeshTriangles();



	std::string Name() const { return  m_name; }
	//VertexArray* GetVertexArray() { return &m_va; }

	void Rename(const std::string& rename)
	{
		m_name = rename;
	}

	void Draw() {
		if (enabled)
			renderer.Draw(*m_va, *m_ib, *m_shader);
	}
};
