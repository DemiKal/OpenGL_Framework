#pragma once
class MeshNew
{

	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};

public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	void Draw(Shader shader);

	MeshNew();
	MeshNew(std::vector<Vertex>& vertices,
		std::vector <unsigned int> indices,
		std::vector <Texture> textures);

	~MeshNew();

private:
	unsigned int VAO, VBO, EBO;
	/*  Functions    */
	void setupMesh();

};

