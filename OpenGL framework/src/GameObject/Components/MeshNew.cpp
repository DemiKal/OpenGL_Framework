#include "precomp.h"

static int highest = 0;
void MeshNew::Draw(Shader& shader)
{
	// bind appropriate textures
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
		// retrieve texture number (the N in diffuse_textureN)
		std::string number;
		std::string name = textures[i].GetType();
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++); // transfer unsigned int to stream
		else if (name == "texture_normal")
			number = std::to_string(normalNr++); // transfer unsigned int to stream
		else if (name == "texture_height")
			number = std::to_string(heightNr++); // transfer unsigned int to stream

		// now set the sampler to the correct texture unit
		GLCall(glUniform1i(glGetUniformLocation(shader.m_RendererID, (name + number).c_str()), i));
		// and finally bind the texture
		GLCall(glBindTexture(GL_TEXTURE_2D, textures[i].GetID()));

		if (diffuseNr  > highest) highest = diffuseNr;
		if (specularNr > highest) highest = specularNr;
		if (normalNr > highest)  highest = normalNr;
		if (heightNr > highest)  highest = heightNr;
	}

	// draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);
}

MeshNew::MeshNew()
{
}

MeshNew::MeshNew(
	std::vector<VertexNew>& vertices,
	std::vector<unsigned>& indices,
	std::vector<Texture2D>& textures,
	std::vector<bool>& bools)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	pos_loaded = bools[0];
	normals_loaded = bools[1];
	UVs_loaded = bools[2];
	tangents_loaded = bools[3];

	setupMesh();

}
void MeshNew::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	// load data into vertex buffers
	// A great thing about structs is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.


	//float* buffer = new float[vertices.size()];


	//std::vector<float> buffer;
	//buffer.reserve(vertices.size() * 56);

	int ss = 4 * 14 * vertices.size();
	int s2 = sizeof(float) * 14 * vertices.size();
	float* bufferfloat = new float[ss];




	for (size_t i = 0; i < vertices.size(); i++) 
	{
	   auto& v = vertices[i];

	   bufferfloat[i * 14 + 0] = (v.Position.x);
	   bufferfloat[i * 14 + 1] = (v.Position.y);
	   bufferfloat[i * 14 + 2] = (v.Position.z);

	   bufferfloat[i * 14 + 3] = (v.Normal.x);
	   bufferfloat[i * 14 + 4] = (v.Normal.y);
	   bufferfloat[i * 14 + 5] = (v.Normal.z);

	   bufferfloat[i * 14 + 6] = (v.TexCoords.x);
	   bufferfloat[i * 14 + 7] = (v.TexCoords.y);

	   bufferfloat[i * 14 + 8] = (v.Tangent.x);
	   bufferfloat[i * 14 + 9] = (v.Tangent.y);
	   bufferfloat[i * 14 + 10] = (v.Tangent.z);

	   bufferfloat[i * 14 + 11] = (v.Bitangent.x);
	   bufferfloat[i * 14 + 12] = (v.Bitangent.y);
	   bufferfloat[i * 14 + 13] = (v.Bitangent.z);
	}

		//int sizef = sizeof(*bufferfloat);
		//const GLsizeiptr bufferSize =    2* sizeof(Vertex) * vertices.size();
	const GLsizeiptr bufferSize = 11 * sizeof(float) * vertices.size();
	//int vertexbytes = sizeof(Vertex) *
	const unsigned int mb = bufferSize / 1024 / 1024;
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, ss, bufferfloat, GL_STATIC_DRAW));

	//delete []buffer;

	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW));

	// set the vertex attribute pointers
	// vertex Positions


	const GLint stride = sizeof(VertexNew);
	const GLint normalPos = offsetof(VertexNew, Normal);
	const GLint uvPos = offsetof(VertexNew, TexCoords);
	const GLint tangentPos = offsetof(VertexNew, Tangent);
	const GLint btangentPos = offsetof(VertexNew, Bitangent);

	if (hasPositions()) {
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	}

	if (hasNormals()) {
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)normalPos);
		// vertex texture coords
	}
	if (hasUVs()) {
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)uvPos);
		// vertex tangent
	}

	if (hasTangents()) {
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)tangentPos);
		// vertex bitangent

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (void*)btangentPos);
	}

	glBindVertexArray(0);

}

MeshNew::~MeshNew()
{
}


