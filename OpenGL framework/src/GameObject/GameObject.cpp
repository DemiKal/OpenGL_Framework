#include "precomp.h"

GameObject::GameObject(const std::string& name)	: m_name(name)
{
}


GameObject::~GameObject()
{
}

void GameObject::LoadMesh(const std::string& filename)
{
	//OBJModel obj(filename);
	//auto idxmdl = obj.ToIndexedModel();
	//mesh_indices =  idxmdl.indices;
	//
	//CalcMeshTriangles();
	//mesh_vertices = std::vector<float>(std::begin(idxmdl.positions),
	//	std::end(idxmdl.positions));

	////nr verts (3) * nr uvs (5)
	//const int pos_uv_size = idxmdl.texCoords.size() * sizeof(float) ;
	//m_vb = new VertexBuffer(positions, 6 * 5 * 4 * sizeof(float));;
	//m_layout = new VertexBufferLayout();

	//m_layout->Push<float>(3);
	//m_layout->Push<float>(2);

	//m_va = new VertexArray();
	//m_va->AddBuffer(*m_vb, *m_layout);

	//m_ib = new IndexBuffer(indices, 6 * 6);
	//m_shader = new Shader("res/shaders/basic.shader");
	//m_shader->Bind();
	//m_shader->SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 0.0f);

	//m_texture = new Texture("res/textures/mario.png");
	//m_transform = new Transform;

	//m_texture->Bind();
	//m_shader->SetUniform1i("u_Texture", 0);

	//m_va->Unbind();
	//m_vb->UnBind();
	//m_ib->UnBind();
	//m_shader->Unbind(); 

}

 

void GameObject::CalcMeshTriangles()
{

	for (int i = 0; i < mesh_indices.size(); i += 3)
	{
		int const mesh_idx1 = mesh_indices[i];
		int const mesh_idx2 = mesh_indices[i + 1];
		int const mesh_idx3 = mesh_indices[i + 2];

		//v1
		float const v1_x = mesh_vertices[3 * mesh_idx1 + 0];
		float const v1_y = mesh_vertices[3 * mesh_idx1 + 1];
		float const v1_z = mesh_vertices[3 * mesh_idx1 + 2];

		//v2	 				    
		float const v2_x = mesh_vertices[3 * mesh_idx2 + 0];
		float const v2_y = mesh_vertices[3 * mesh_idx2 + 1];
		float const v2_z = mesh_vertices[3 * mesh_idx2 + 2];

		//v3	 			   
		float const v3_x = mesh_vertices[3 * mesh_idx3 + 0];
		float const v3_y = mesh_vertices[3 * mesh_idx3 + 1];
		float const v3_z = mesh_vertices[3 * mesh_idx3 + 2];

		glm::vec3 v1(v1_x, v1_y, v1_z);
		glm::vec3 v2(v2_x, v2_y, v2_z);
		glm::vec3 v3(v3_x, v3_y, v3_z);

		Triangle tri(v1, v2, v3);
		mesh_triangles.emplace_back(tri);
	}
}

 

