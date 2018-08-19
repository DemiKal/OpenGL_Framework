#include "Cube.h"
#include "VertexBufferLayout.h"


Cube::Cube(const std::string& name) : GameObject(name)
{
	float positions[] = {
		-1, -1, 1,	0.0f, 0.0f,
		1, -1, 1,	1.0f, 0.0f,
		1, 1, 1,	1.0f, 1.0f,
		-1, 1, 1,	0.0f, 1.0f,

		1, -1, 1,	0.0f, 0.0f,
		1, -1, -1,	1.0f, 0.0f,
		1, 1, -1,	1.0f, 1.0f,
		1, 1, 1,	0.0f, 1.0f,

		1, -1, -1,	0.0f, 0.0f,
		-1, -1, -1, 1.0f, 0.0f,
		-1, 1, -1,	1.0f, 1.0f,
		1, 1, -1,	0.0f, 1.0f,

		-1, -1, -1, 0.0f, 0.0f,
		-1, -1, 1,	1.0f, 0.0f,
		-1, 1, 1,	1.0f, 1.0f,
		-1, 1, -1,	0.0f, 1.0f,

		-1,  1,  1, 0.0f, 0.0f,
		 1,  1,  1, 1.0f, 0.0f,
		 1,  1, -1, 1.0f, 1.0f,
		-1,  1, -1, 0.0f, 1.0f,

		1,  -1, -1,	0.0f, 0.0f,
		-1, -1, -1, 1.0f, 0.0f,
		-1, -1,  1, 1.0f, 1.0f,
		 1, -1,  1,	0.0f, 1.0f, 
	};

	unsigned int indices[]{
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4,
		8, 9, 10, 10, 11, 8,
		12, 13, 14, 14, 15, 12,
		16, 17, 18, 18, 19, 16,
		22, 21, 20, 20, 23, 22, 
	};

	m_va = new VertexArray();
	m_vb = new VertexBuffer(positions, 6 * 5 * 4 * sizeof(float));;
	m_layout = new VertexBufferLayout();

	m_layout->Push<float>(3);
	m_layout->Push<float>(2);
	m_va->AddBuffer(*m_vb, *m_layout);

	m_ib = new IndexBuffer(indices, 6 * 6);
	m_shader = new Shader("res/shaders/basic.shader");
	m_shader->Bind();
	m_shader->SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 0.0f);

	m_texture = new Texture("res/textures/mario.png");
	m_transform = new Transform;

	m_texture->Bind();
	m_shader->SetUniform1i("u_Texture", 0);

	m_va->Unbind();
	m_vb->UnBind();
	m_ib->UnBind();
	m_shader->Unbind();
}


Cube::~Cube()
{
}
