#include "precomp.h"

GameObject::GameObject(const std::string& name) : m_name(name)
{
}


GameObject::~GameObject()
{
}


void GameObject::LoadMesh(const std::string& filename)
{
	OBJModel obj(filename);
	  idxmdl = obj.ToIndexedModel();
	//mesh_indices = idxmdl.indices;

	std::vector<float> vertices_vec;

	for (const auto v : obj.vertices)
	{
		vertices_vec.push_back(v.x);
		vertices_vec.push_back(v.y);
		vertices_vec.push_back(v.z);
	}

	mesh_vertices = vertices_vec;

	std::vector<unsigned int> _indices;

	for (auto obji : obj.OBJIndices)
	{
		_indices.push_back(obji.vertexIndex);
	}

	// mesh_indices = _indices;
	mesh_indices = idxmdl.indices;
	//std::vector<float>(std::begin(idxmdl.positions),
	//std::end(idxmdl.positions));

	//CalcMeshTriangles();

	std::vector<float> UVs_vec;
	std::vector<glm::vec2> uvReorder{ obj.uvs };

	//for (int i = 0; i < mesh_indices.size(); i++)
	//{
	//	unsigned int idx = mesh_indices[i];
	//	auto uv = obj.uvs[idx];
	//	UVs_vec.push_back(uv.x);
	//	UVs_vec.push_back(1.f- uv.y);
	//}
	//for (const auto& v : obj.OBJIndices)
	//{
	//	////size_t idx = mesh_indices[i];
	//	//unsigned int idx = v.uvIndex;;
	//	//
	//	//UVs_vec.push_back(obj.uvs[idx].x);
	//	//UVs_vec.push_back(obj.uvs[idx].y);
	//}

	//std::vector<float> pos_uv;

	//pos_uv.reserve(mesh_vertices.size() + UVs_vec.size());
	//pos_uv.insert(pos_uv.end(), mesh_vertices.begin(), mesh_vertices.end());
	//pos_uv.insert(pos_uv.end(), UVs_vec.begin(), UVs_vec.end());
	//std::reverse(pos_uv.begin() + mesh_vertices.size(), pos_uv.end());

	//pos_uv.reserve(mesh_vertices.size() + UVs_vec.size());
	//pos_uv.insert(pos_uv.end(), mesh_vertices.begin(), mesh_vertices.end());
	//pos_uv.insert(pos_uv.end(), UVs_vec.begin(), UVs_vec.end());

	std::vector<float> pos_uv_interl;

	//for (unsigned int i = 0; i  < idxmdl.indices.size(); i++)
	int k = 0;
	for (int i = 0 ; i < idxmdl.positions.size(); i++)
	{
		const glm::vec3 p = idxmdl.positions[i];	//obj.vertices[pi];
		const glm::vec2 uv = idxmdl.texCoords[i];	//obj.uvs[uvi];
		const glm::vec3 n = idxmdl.normals[i];	//obj.normals[ni];

		//pos
		pos_uv_interl.push_back(p.x);
		pos_uv_interl.push_back(p.y);
		pos_uv_interl.push_back(p.z);
		
		//normal
		pos_uv_interl.push_back(n.x);
		pos_uv_interl.push_back(n.y);
		pos_uv_interl.push_back(n.z);
		
		//uv
		pos_uv_interl.push_back(uv.x);
		pos_uv_interl.push_back(uv.y);

		k++;
		//.
		//int idx = idxmdl.indices[i];
		//const auto p = idxmdl.positions[idx];
		//const auto tc = idxmdl.texCoords[idx];
		//
		//pos_uv_interl.push_back(p.x);
		//pos_uv_interl.push_back(p.y);
		//pos_uv_interl.push_back(p.z);
		//pos_uv_interl.push_back(tc.x);
		//pos_uv_interl.push_back(tc.y);

	}
	//m_shader = new Shader("res/shaders/basic.glsl");
	//m_shader->Bind();
	//m_shader->SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 0.0f);
	//
	//m_texture = new Texture("res/meshes/Spyro/spyro_tex.png", Texture::DIFFUSE);
	//m_texture->Bind();
	//m_shader->SetUniform1i("texture_diffuse1", 0);


	m_vb = new VertexBuffer(&pos_uv_interl[0], pos_uv_interl.size() * sizeof(float));

	//if (!UVs_vec.empty())
	//	m_vb->BufferSubData(mesh_vertices, UVs_vec);

	m_layout = new VertexBufferLayout();

	m_layout->Push<float>(3); //pos vert
	m_layout->Push<float>(3); //normie
	m_layout->Push<float>(2); //uv

	m_va = new VertexArraySub();
	m_va->AddBuffer(*m_vb, *m_layout);

	m_ib = new IndexBuffer(&mesh_indices[0], mesh_indices.size());




	//m_texture = new Texture("res/textures/uvtest.png", Texture::DIFFUSE);
	

	m_transform = new Transform;


	m_va->Unbind();
	m_vb->UnBind();
	m_ib->UnBind();
	m_shader->Unbind();

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



