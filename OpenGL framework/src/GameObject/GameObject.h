#pragma once


class GameObject
{
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
public:
	Renderer* renderer;
	VertexArray* m_va;
	VertexBuffer* m_vb;
	VertexBufferLayout* m_layout;
	IndexBuffer* m_ib;
	Shader* m_shader;
	Texture* m_texture;
	Transform* m_transform;

	std::string Name() const { return  m_name; }
	//VertexArray* GetVertexArray() { return &m_va; }

	void Rename(const std::string& rename)
	{
		m_name = rename;
	}

	void Draw() { if(enabled) renderer->Draw(*m_va, *m_ib, *m_shader); }
};
