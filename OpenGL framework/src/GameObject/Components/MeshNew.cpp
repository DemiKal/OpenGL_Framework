#include "precomp.h"


void MeshNew::Draw(Shader& shader)
{
	//unsigned int diffuseNr = 1;
	//unsigned int specularNr = 1;
	////shader.Bind();
	//
	//glBindVertexArray(VAO);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//
	// for (unsigned int i = 0; i < textures.size(); i++)
	// {
	//// 	textures[i].Bind();
	// }
	//
	////1) bind 
	////GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	////GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
	//
	//
	//
	// for (unsigned int i = 0; i < textures.size(); i++)
	// {
	//	 //textures[i]->Bind();
	//	 //shader.SetUniform1i("texture_diffuse1", 0);
	//
	// 	  //GLCall(glActiveTexture(GL_TEXTURE0 + i)); // activate proper texture unit before binding
	// 	  //// retrieve texture number (the N in diffuse_textureN)
	// 	  //std::string number;
	// 	  //const std::string name = textures[i].GetTypeName();
	// 	  //
	// 	  //if (name == "texture_diffuse")	number = std::to_string(diffuseNr++);
	// 	  //else if (name == "texture_specular") number = std::to_string(specularNr++);
	// 	  //
	// 	  ////const std::string uniformName = "material." + name + number;
	// 	  ////shader.SetFloat(uniformName, i);
	// 	  //
	// 	  //const std::string uniform_name = (name + number);
	// 	  //shader.SetUniform1i(uniform_name, i);
	// 	  //
	// 	  ////const GLint uloc = glGetUniformLocation(shader.m_RendererID, uniform_name.c_str());
	// 	  ////GLCall(glUniform1i(uloc, i));
	// 	  //int texid = textures[i].GetID();
	// 	  ////GLCall(glBindTexture(GL_TEXTURE_2D, texid));
	// }
	//
	////glActiveTexture(GL_TEXTURE0);
	//
	//// draw mesh
	////glBindTexture(GL_TEXTURE_2D,1);
	////glBindVertexArray(VAO);
	//
	////glBindVertexArray(VAO);
	//GLCall(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (const void*)0));
	//GLCall(glBindVertexArray(0));
	//GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	//GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));



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
		std::string name = textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++); // transfer unsigned int to stream
		else if (name == "texture_normal")
			number = std::to_string(normalNr++); // transfer unsigned int to stream
		else if (name == "texture_height")
			number = std::to_string(heightNr++); // transfer unsigned int to stream

												 // now set the sampler to the correct texture unit
		glUniform1i(glGetUniformLocation(shader.m_RendererID, (name + number).c_str()), i);
		// and finally bind the texture
		glBindTexture(GL_TEXTURE_2D, textures[i].m_rendererID);
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
	std::vector<unsigned> indices,	//todo: pointer!
	std::vector<Texture2D> textures)
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

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexNew), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
		&indices[0], GL_STATIC_DRAW);

	//const GLsizei vertexSize = ;
	//const GLsizei normalOffset = ;
	//const GLsizei uvOffset = ;


	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNew), (const void*)0);
	// vertex normals								 
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNew), (const void*)offsetof(VertexNew, Normal));
	// vertex texture coords					 
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexNew), (const void*)offsetof(VertexNew, TexCoords));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

MeshNew::~MeshNew()
{
}


