#include "precomp.h"


void MeshNew::Draw(Shader shader)
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
		// retrieve texture number (the N in diffuse_textureN)
		std::string number;
		const std::string name = textures[i].GetTypeName();
		
		if (name == "texture_diffuse")	number = std::to_string(diffuseNr++);
		else if (name == "texture_specular") number = std::to_string(specularNr++);

		const std::string uniformName = "material." + name + number;
		shader.SetFloat(uniformName, i);
		glBindTexture(GL_TEXTURE_2D, textures[i].GetID());
	}

	glActiveTexture(GL_TEXTURE0);

	// draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

MeshNew::MeshNew()
{
}

MeshNew::MeshNew(
	std::vector<Vertex>& vertices,
	std::vector<unsigned> indices,
	std::vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	setupMesh();

}
void MeshNew::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
		&indices[0], GL_STATIC_DRAW);

	const GLsizei vertexSize = sizeof(Vertex);
	const GLsizei normalOffset = offsetof(Vertex, Normal);
	const GLsizei uvOffset = offsetof(Vertex, TexCoords);


	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*)0);
	// vertex normals								 
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*)normalOffset);
	// vertex texture coords					 
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertexSize, (void*)uvOffset);

	glBindVertexArray(0);
}

MeshNew::~MeshNew()
{
}


