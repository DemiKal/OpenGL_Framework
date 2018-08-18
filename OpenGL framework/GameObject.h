#pragma once
#include "VertexArray.h"
#include "IndexBuffer.h"
#include <string>
#include "Texture.h"

class GameObject
{
public:
	GameObject(const std::string& name);
	~GameObject();

	//	float  m_MeshVertices[ ];
   //	float m_UVs[ ];

private:
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;

public:
	
	Renderer* renderer;
	VertexArray* m_va;
	VertexBuffer* m_vb;
	VertexBufferLayout* m_layout;
	IndexBuffer* m_ib;
	std::string  m_name;
	Shader* m_shader;
	Texture* m_texture;
	static GameObject CreateCube();
	std::string* Name() { return  &m_name; }

	VertexArray* GetVertexArray() const { return m_va; }
	void Rename(const std::string& rename)
	{
		auto n = Name();
		*n = rename;

	}

	void SetShader(const std::string filepath)
	{
		
	}

	void Draw()
	{
		renderer->Draw(*m_va, *m_ib, *m_shader);
	}

};

