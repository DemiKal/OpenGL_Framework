#include "Cube.h"
#include "VertexBufferLayout.h"


Cube::Cube(const std::string& name) : GameObject(name)
{
	float positions[] = {
		-1,	-1,  1,		0.0f, 0.0f,
		1,	-1,  1,		1.0f, 0.0f,
		1,  1,	 1,		1.0f, 1.0f,
		-1,	 1,	 1,		0.0f, 1.0f,

		1,	-1,  1,		0.0f, 0.0f,
		1,	-1,  -1,	1.0f, 0.0f,
		1,  1 ,	 -1,	1.0f, 1.0f,
		1,	 1,	 1,		0.0f, 1.0f,

		1,	-1,  -1,	0.0f, 0.0f,
		-1,	-1,  -1,	1.0f, 0.0f,
		-1,	 1,	 -1,	1.0f, 1.0f,
		1,  1,	 -1,	0.0f, 1.0f,

		-1,	-1,  -1,	0.0f, 0.0f,
		-1,	-1,   1,	1.0f, 0.0f,
		-1,	 1,	  1,	1.0f, 1.0f,
		-1,  1 , -1,	0.0f, 1.0f,
	};

	unsigned int indices[]{
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4,
		8, 9, 10, 10, 11, 8,
		12, 13, 14, 14, 15, 12
	};

	VertexArray va;
	m_va = &va;

	VertexBuffer vb(positions, 4 * 5 * 4 * sizeof(float));
	m_vb = &vb;

	VertexBufferLayout layout;
	m_layout = &layout;

	layout.Push<float>(3);
	layout.Push<float>(2);
	va.AddBuffer(vb, layout);

	IndexBuffer ib(indices, 4 * 6);
	m_ib = &ib;

	Shader shader("res/shaders/basic.shader");
	m_shader = &shader;
	shader.Bind();
	shader.SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 0.0f);

	Texture texture("res/textures/mario.png");
	m_texture = &texture;


	texture.Bind();
	shader.SetUniform1i("u_Texture", 0);

	va.Unbind();
	vb.UnBind();
	ib.UnBind();
	shader.Unbind(); 
}


Cube::~Cube()
{
}
