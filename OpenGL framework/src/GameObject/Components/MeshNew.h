#pragma once
 


class MeshNew
{
public:
	std::vector<VertexNew> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture2D> textures;
	void Draw(Shader& shader);

	MeshNew();
	MeshNew(std::vector<VertexNew>& vertices,
		std::vector <unsigned int> indices,
		std::vector <Texture2D> textures);

	~MeshNew();

private:
	unsigned int VAO, VBO, EBO;
	/*  Functions    */
	void setupMesh();

};

