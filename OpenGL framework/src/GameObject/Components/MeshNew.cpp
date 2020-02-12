#include "precomp.h"

void MeshNew::Draw(GPUShader& shader)
{
	// bind appropriate textures
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;

	int i = 0;
	for (const Texture2D& tex : m_textures)
	{
		glActiveTexture(GL_TEXTURE0 + i);

		std::string number;
		std::string name = tex.GetType();
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++);	// transfer unsigned int to stream
		else if (name == "texture_normal")
			number = std::to_string(normalNr++);	// transfer unsigned int to stream
		else if (name == "texture_height")
			number = std::to_string(heightNr++);	// transfer unsigned int to stream

		// now set the sampler to the correct texture unit
		GLCall(glUniform1i(glGetUniformLocation(shader.m_RendererID, (name + number).c_str()), i));
		// and finally bind the texture
		GLCall(glBindTexture(GL_TEXTURE_2D, tex.GetID()));
		i++;
	}

	/*int count;
	glGetProgramiv(shader.m_RendererID, GL_ACTIVE_UNIFORMS, &count);
	printf("Active Uniforms: %d\n", count);

	for (i = 0; i < count; i++)
	{
		glGetActiveUniform(program, (GLuint)i, bufSize, &length, &size, &type, name);

		printf("Uniform #%d Type: %u Name: %s\n", i, type, name);
	}*/

	int count;
	GLCall(glGetProgramiv(shader.m_RendererID, GL_ACTIVE_UNIFORMS, &count));
	//printf("Active Uniforms: %d\n", count);
	GLint maxLength;
	GLCall(glGetProgramiv(shader.m_RendererID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength));

	std::vector < std::string> names;

	for (int i = 0; i < count; i++)
	{
		GLchar name[GL_ACTIVE_UNIFORM_MAX_LENGTH + 1];
		int length, size;
		GLenum type;

		glGetActiveUniform(shader.m_RendererID, i, maxLength, &length, &size, &type, name);
		names.emplace_back(name);
		//const GLubyte* ss = glGetString(type);
		//printf("Uniform #%d Type: %u Name: %s\n", i, type, name);
		//names.emplace_back(name);
	}

	std::vector<glm::mat4> boneMatrices;
	if (m_animator.m_bones.size() > 0) {
		int k = 0;
		for (auto& m : m_animator.m_bones)
		{
			//boneMatrices.emplace_back(glm::rotate(glm::mat4(1.0f), glm::radians(-45.f * k++), glm::vec3(1, 0, 0)));
			boneMatrices.emplace_back(m.pose_transform);
		}

		auto idx = shader.GetUniformLocation("mBones[0]");
		GLCall(glUniformMatrix4fv((GLint)idx, (GLsizei)20, GL_FALSE, (const GLfloat*)&boneMatrices[0])); // Passing 20 matrices
	}

	//for(int i = 0; i < bones.size()l i++)
	//shader.SetUniformMat4f("boneMatrices[")
	//	Matrix4fv
	//	  glUniformMatrix4fv(GLint location,
	//		GLsizei count,
	//		GLboolean transpose,
	//		const GLfloat * value);


	// draw mesh
	glBindVertexArray(VAO);
	if (indices.size() > 0)
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
	else glDrawArrays(GL_TRIANGLES, 0, GetVertexCount()); //plane!
	glBindVertexArray(0);

	// always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);

}

MeshNew::MeshNew(
	const std::vector<float>& vertices,
	const std::vector<unsigned>& indices,
	const std::vector<Texture2D>& textures,
	const std::vector<bool>& bools,
	const Animator& animator,
	const VertexBufferLayout& vbl)
{
	this->vertices = vertices;
	this->indices = indices;
	this->m_textures = textures;
	this->m_animator = animator;
	this->m_VertexBufferLayout = vbl;
	//TODO: MAKE VERTEX BUFFER DYNAMIC!
	pos_loaded = bools[0];
	normals_loaded = bools[1];
	UVs_loaded = bools[2];
	tangents_loaded = bools[3];
	animation_loaded = bools[4];

	setupMesh();

}
void MeshNew::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	const GLsizeiptr bufferSize = sizeof(vertices[0]) * vertices.size();
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, bufferSize, &vertices[0], GL_STATIC_DRAW));

	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW));

	unsigned int i = 0;
	const unsigned int stride = m_VertexBufferLayout.GetStride();
	for (const VertexBufferElement& vbe : m_VertexBufferLayout.GetElements())
	{
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, vbe.count, vbe.type, vbe.normalized, stride, (const void*)vbe.vertexIndex);
		i++;
	}


	//const GLsizeiptr bufferSize = sizeof(vertices[0]) * vertices.size();
	//GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	//GLCall(glBufferData(GL_ARRAY_BUFFER, bufferSize, &vertices[0], GL_STATIC_DRAW));

	//GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
	//GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
	//	indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW));


	//GLint stride = 3 * sizeof(float); //pos
	//unsigned int normalSize = 3 * sizeof(float);
	//unsigned int uvSize = 2 * sizeof(float);
	//unsigned int tangentSize = 6 * sizeof(float);
	//unsigned int bonesSize = 6 * sizeof(float);


	//if (hasNormals()) stride += normalSize; //xyz normal
	//if (hasUVs()) stride += uvSize;  //uv tex
	//if (hasTangents()) stride += tangentSize; //xyz xyz bitangents
	//if (hasAnimation()) stride += bonesSize; //xyz xyz bitangents

 //  //const GLint stride = sizeof(VertexNew);
 //  //const GLint normalPos = offsetof(VertexNew, Normal);
 //  //const GLint uvPos = offsetof(VertexNew, TexCoords);
 //  //const GLint tangentPos = offsetof(VertexNew, Tangent);
 //  //const GLint btangentPos = offsetof(VertexNew, Bitangent);
 //  //
	//unsigned int offset = 0;

	//if (hasPositions()) {
	//	glEnableVertexAttribArray(0);
	//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
	//}

	//if (hasNormals()) {
	//	offset += normalSize;
	//	glEnableVertexAttribArray(1);
	//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(offset));
	//}
	//if (hasUVs()) {
	//	offset += uvSize;
	//	glEnableVertexAttribArray(2);
	//	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)offset);
	//}

	//if (hasTangents()) {
	//	offset += tangentSize / 2;
	//	glEnableVertexAttribArray(3);
	//	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);

	//	offset += tangentSize / 2;
	//	glEnableVertexAttribArray(4);
	//	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
	//}

	//if (hasAnimation()) {
	//	offset += 3 * sizeof(float);
	//	glEnableVertexAttribArray(5);
	//	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);

	//	offset += 3 * sizeof(float);
	//	glEnableVertexAttribArray(6);
	//	glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);

	//}

	glBindVertexArray(0);
}

MeshNew MeshNew::CreateCube() {
	MeshNew mesh;
	float cubeVertices[] = {
		// positions          // texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	// cube VAO
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	mesh.VAO = cubeVAO;
	mesh.VAO = cubeVAO;
	mesh.vertexCount = 36;
	return mesh;
}
//eshNew::~MeshNew()
//
//	std::cout << "destructor called meshnew" << "\n";
//	//GLCall(glDeleteBuffers(1, &VAO));
//	//GLCall(glDeleteBuffers(1, &VBO));
//	//GLCall(glDeleteBuffers(1, &EBO));
//


MeshNew MeshNew::CreatePlane() {

	MeshNew mesh;
	float planeVertices[] = {
		// positions          // texture Coords 
		 5.0f, -0.5f,  5.0f,  1.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 1.0f,

		 5.0f, -0.5f,  5.0f,  1.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 1.0f,
		 5.0f, -0.5f, -5.0f,  1.0f, 1.0f
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


	// MeshNew* mesh = new MeshNew();
	mesh.VBO = planeVBO;
	mesh.VAO = planeVAO;
	mesh.vertexCount = 6;

	return mesh;

}