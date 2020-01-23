#include "precomp.h"

void MeshNew::Draw(const GPUShader& shader)
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
	}

	// draw mesh
	glBindVertexArray(VAO);
	if (indices.size() > 0)
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
	else glDrawArrays(GL_TRIANGLES, 0, 6); //plane!
	glBindVertexArray(0);

	// always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);

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

	const GLsizeiptr bufferSize = sizeof(VertexNew) * vertices.size();
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, bufferSize, &vertices[0], GL_STATIC_DRAW));

	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW));


	//GLint stride = sizeof(VertexNew);
	//if (!hasNormals()) stride -= 3 * sizeof(float); //xyz
	//if (!hasUVs()) stride -= 2 * sizeof(float);  //uv
	//if (!hasTangents()) stride -= 6 * sizeof(float); //xyz xyz

	const GLint stride = sizeof(VertexNew);
	const GLint normalPos = offsetof(VertexNew, Normal);
	const GLint uvPos = offsetof(VertexNew, TexCoords);
	const GLint tangentPos = offsetof(VertexNew, Tangent);
	const GLint btangentPos = offsetof(VertexNew, Bitangent);

	int offset = 0;
	//if (hasPositions()) {
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
	offset += stride;
	//}

	//if (hasNormals()) {
		// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(normalPos));
	offset += stride;
	//}
	//if (hasUVs()) {
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)uvPos);
	//}

	//if (hasTangents()) {
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)tangentPos);
	//bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (void*)btangentPos);
	//}

	glBindVertexArray(0);
	//vertices.clear(); clear can be done in static draw
}


MeshNew& MeshNew::CreatePlane() {
	float planeVertices[] = {
		// positions          // texture Coords 
		 5.0f, -0.5f,  5.0f,  1.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 1.0f,

		 5.0f, -0.5f,  5.0f,  1.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 1.0f,
		 5.0f, -0.5f, -5.0f, 1.0f,1.0f
	};
	unsigned int planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	 

	MeshNew mesh;
	mesh.VBO = planeVBO;
	mesh.VAO = planeVAO;
	mesh.indices.clear();
	return mesh;





}


//eshNew::~MeshNew()
//
//	std::cout << "destructor called meshnew" << "\n";
//	//GLCall(glDeleteBuffers(1, &VAO));
//	//GLCall(glDeleteBuffers(1, &VBO));
//	//GLCall(glDeleteBuffers(1, &EBO));
//


