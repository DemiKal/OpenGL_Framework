#include "precomp.h"

Cube::Cube(const std::string& name) : GameObject(name)
{

	float positions[] = { //note: duplicate with meshverts but whatever
		-1, -1, 1,
		1, -1, 1,
		1, 1, 1,
		-1, 1, 1,

		1, -1, 1,
		1, -1, -1,
		1, 1, -1,
		1, 1, 1,

		1, -1, -1,
		-1, -1, -1,
		-1, 1, -1,
		1, 1, -1,

		-1, -1, -1,
		-1, -1, 1,
		-1, 1, 1,
		-1, 1, -1,

		-1,  1,  1,
		 1,  1,  1,
		 1,  1, -1,
		-1,  1, -1,

		1,  -1, -1,
		-1, -1, -1,
		-1, -1,  1,
		 1, -1,  1
	};

	float UVs[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

	float posAndUVArray[] = { //note: duplicate with meshverts but whatever
		-1, -1, 1,
		1, -1, 1,
		1, 1, 1,
		-1, 1, 1,

		1, -1, 1,
		1, -1, -1,
		1, 1, -1,
		1, 1, 1,

		1, -1, -1,
		-1, -1, -1,
		-1, 1, -1,
		1, 1, -1,

		-1, -1, -1,
		-1, -1, 1,
		-1, 1, 1,
		-1, 1, -1,

		-1,  1,  1,
		 1,  1,  1,
		 1,  1, -1,
		-1,  1, -1,

		1,  -1, -1,
		-1, -1, -1,
		-1, -1,  1,
		 1, -1,  1,
	 
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};
	  
	//std::vector<float> vecpos = std::vector<float>()
	mesh_vertices = std::vector<float>(
		std::begin(positions), std::end(positions));

	unsigned int indices[]{
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4,
		8, 9, 10, 10, 11, 8,
		12, 13, 14, 14, 15, 12,
		16, 17, 18, 18, 19, 16,
		22, 21, 20, 20, 23, 22,
	};

	mesh_indices = std::vector<unsigned int>(
		std::begin(indices), std::end(indices));

	std::vector<float> UVs_vec = std::vector<float>(
		std::begin(UVs), std::end(UVs));

	CalcMeshTriangles();

	std::vector<float> posAndUVs;
	posAndUVs.reserve(mesh_vertices.size() + UVs_vec.size());
	posAndUVs.insert(posAndUVs.end(), mesh_vertices.begin(), mesh_vertices.end());
	posAndUVs.insert(posAndUVs.end(), UVs_vec.begin(), UVs_vec.end());

	m_va = new VertexArraySub();
	m_vb = new VertexBuffer(posAndUVArray, 6 * 5 * 4 * sizeof(float));;
	m_vb->BufferSubData(mesh_vertices, UVs_vec);
 
	
	m_layout = new VertexBufferLayout();

	m_layout->Push<float>(mesh_vertices.size());
	m_layout->Push<float>(UVs_vec.size());

	m_va->AddBuffer(*m_vb, *m_layout);

	m_ib = new IndexBuffer(indices, 6 * 6);
	m_shader = new GPUShader("res/shaders/basic.shader");
	m_shader->Bind();
	m_shader->SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 0.0f);

	m_texture = new Texture("res/textures/mario.png", Texture::DIFFUSE);
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

