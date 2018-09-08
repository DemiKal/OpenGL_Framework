#pragma once
#include "GameObject.h"
#include <array>


class Cube : public GameObject
{

	class Triangle
	{
	public: glm::vec3  v1{};
			glm::vec3  v2{};
			glm::vec3  v3{};

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
	Cube(const std::string& name);
	~Cube();

	std::vector<float>& GetMeshVertices() { return mesh_vertices; }
	std::vector<unsigned int>& GetMeshIndices() { return mesh_indices; }
	std::vector<Triangle> GetMeshTriangles()
	{
		glm::mat4 model_matrix = m_transform->GetModelMatrix();
		std::vector<Triangle> world_space_triangles;
		/*std::transform(mesh_triangles.begin(), mesh_triangles.end(),
			world_space_triangles, [model_matrix](Triangle tri) {

			auto v1h = glm::vec3(model_matrix * glm::vec4(tri.v1, 1.0));
			auto v2h = glm::vec3(model_matrix * glm::vec4(tri.v2, 1.0));
			auto v3h = glm::vec3(model_matrix * glm::vec4(tri.v3, 1.0));
			return Triangle(v1h, v2h, v3h);

		});*/

		for(Triangle t : mesh_triangles)
		{
			Triangle t2 = t * model_matrix;
			world_space_triangles.emplace_back(t2);
		}


		return world_space_triangles;
	}


private:
	void CalcMeshTriangles();

	std::vector<float> mesh_vertices;
	std::vector<unsigned int> mesh_indices;
	std::vector<Triangle> mesh_triangles;
};

