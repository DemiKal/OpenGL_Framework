#include "precomp.h"

void MeshNew::Draw(GPUShader& shader)
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;

	for (int i = 0; i < m_textures.size(); i++)
	{
		const Texture2D& tex = m_textures[i];
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
		shader.SetUniform1i(name + number, i);
		//GLCall(glUniform1i(glGetUniformLocation(shader.m_RendererID, (name + number).c_str()), i));
		// and finally bind the texture
		GLCall(glBindTexture(GL_TEXTURE_2D, tex.GetID()));
	}

	if (!m_animator.m_bones.empty()) {
		std::vector<glm::mat4> boneMatrices;
		for (auto& m : m_animator.m_bones)
			boneMatrices.emplace_back(m.pose_transform);

		const auto idx = shader.GetUniformLocation("mBones[0]");
		GLCall(glUniformMatrix4fv((GLint)idx, (GLsizei)20, GL_FALSE, (const GLfloat*)&boneMatrices[0])); // Passing 20 matrices
	}

	// draw mesh
	glBindVertexArray(VAO);
	if (!indices.empty())
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
	else glDrawArrays(GetElemDrawType(), 0, GetVertexCount()); //plane!

	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

MeshNew::MeshNew(
	const std::vector<float>& vertices,
	const std::vector<unsigned>& indices,
	const std::vector<Texture2D>& textures,
	const Animator& animator,
	const VertexBufferLayout& vbl)
{
	this->vertices = vertices;
	this->indices = indices;
	this->m_textures = textures;
	this->m_animator = animator;
	this->m_VertexBufferLayout = vbl;

	//get aabb
	if (!positionVertices.empty())
		for (int i = 0; i < vertices.size(); i += vbl.GetStride())
		{
			positionVertices.emplace_back(glm::vec3{ vertices[i], vertices[i + 1], vertices[i + 2] });
			//m_aabb.CalcBounds(positionVertices);
		}

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
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
		&indices[0], GL_STATIC_DRAW));

	unsigned int i = 0;
	const unsigned int stride = m_VertexBufferLayout.GetStride();
	for (const VertexBufferElement& vbe : m_VertexBufferLayout.GetElements())
	{
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, vbe.count, vbe.type, vbe.normalized, stride,
			reinterpret_cast<const void*>(vbe.vertexIndex));
		i++;
	}

	glBindVertexArray(0);
}

MeshNew MeshNew::CreateCubeWireframe()
{
	MeshNew mesh;
	mesh.SetElemDrawType(GL_LINES);

	float cubeVertices[] = {
		//front face
		-0.5f, -0.5f, -0.5f, //ll-          
		0.5f, -0.5f, -0.5f,	 //lr-
		0.5f, -0.5f, -0.5f,	//lr-
		0.5f,  0.5f, -0.5f,	//tr-
		0.5f,  0.5f, -0.5f, //tr-
		-0.5f,  0.5f, -0.5f, //tl-
		-0.5f,  0.5f, -0.5f, //tl-
		-0.5f, -0.5f, -0.5f, //ll-

		//back face
		-0.5f, -0.5f, 0.5f,   //ll+          
		0.5f, -0.5f,  0.5f,	   //lr+
		0.5f, -0.5f,  0.5f,	   //tr+
		0.5f,  0.5f,  0.5f,	   //tr+
		0.5f,  0.5f,  0.5f,    //lr+
		-0.5f,  0.5f, 0.5f,   //tl+
		-0.5f,  0.5f, 0.5f,   //tl+
		-0.5f, -0.5f, 0.5f,   //ll+

		//connecting edges
		 -0.5f, -0.5f,  -0.5f,	//ll-
		 -0.5f, -0.5f,  0.5f,	//ll+
		 0.5f, -0.5f, -0.5f,		//lr-
		 0.5f, -0.5f,  0.5f,		//lr+
		 0.5f,  0.5f, -0.5f,		//tr-
		 0.5f,  0.5f, 0.5f,		//tr+
		 -0.5f,  0.5f,  -0.5f,	//tl-
		 -0.5f,  0.5f,  0.5f		//tl+
	}; //8*3*3

	int size = sizeof(cubeVertices) / sizeof(float);
	std::copy(cubeVertices, cubeVertices + size, std::back_inserter(mesh.vertices));

	VertexBufferLayout vbl;
	vbl.Push<float>(3, VertexType::POSITION);

	for (int i = 0; i < size; i += 3)
	{
		mesh.positionVertices.emplace_back(
			glm::vec3(cubeVertices[i], cubeVertices[i + 1], cubeVertices[i + 2]));
	}
	 
	//AABB aabb;
	mesh.m_aabb.CalcBounds(mesh.positionVertices);
	mesh.m_aabb_OG = mesh.m_aabb;
	
	mesh.m_VertexBufferLayout = vbl;

	// cube VAO
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.vertices.size(), &mesh.vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)nullptr);

	mesh.VAO = cubeVAO;
	mesh.VBO = cubeVBO;

	return mesh;
}

MeshNew MeshNew::CreateCube()
{
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

	const unsigned int size = sizeof(cubeVertices) / sizeof(float);
	std::copy(cubeVertices, cubeVertices + 6 * 5 * 6, std::back_inserter(mesh.vertices));

	for (int i = 0; i < mesh.vertices.size(); i += 5)
		mesh.positionVertices.emplace_back(glm::vec3(
			mesh.vertices[i], mesh.vertices[i + 1], mesh.vertices[i + 2]));

	mesh.m_aabb.CalcBounds(mesh.positionVertices);
	mesh.m_aabb_OG = mesh.m_aabb;

	VertexBufferLayout vbl;
	vbl.Push<float>(3, VertexType::POSITION);
	vbl.Push<float>(2, VertexType::TEXCOORD);

	mesh.m_VertexBufferLayout = vbl;
	// cube VAO
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.vertices.size(), &mesh.vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	mesh.VAO = cubeVAO;
	mesh.VAO = cubeVAO;

	return mesh;
}

void MeshNew::MakeWireFrame()
{
	if (positionVertices.empty()) { std::cout << "no position vertices!" << std::endl; return; }

	glm::vec3 A(1, 0, 0);
	glm::vec3 B(0, 1, 0);
	glm::vec3 C(0, 0, 1);
	glm::vec3 bary[] = { A, B, C };
	std::vector<glm::vec3> newBuffer;
	int barycentricIdx = 0;
	for (auto& v : positionVertices)
	{
		//this->indices
		newBuffer.emplace_back(v);
		newBuffer.emplace_back(bary[barycentricIdx]);
		barycentricIdx = (barycentricIdx + 1) % 3; 
	}

	unsigned int wireVAO, wireVBO;
	glGenVertexArrays(1, &wireVAO);
	glGenBuffers(1, &wireVBO);
	glBindVertexArray(wireVAO);
	glBindBuffer(GL_ARRAY_BUFFER, wireVBO);
	glBufferData(GL_ARRAY_BUFFER, newBuffer.size() * 2 * sizeof(glm::vec3), &newBuffer[0], GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);					 
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	m_wireVAO = wireVAO;
	m_wireVBO = wireVBO;




}

void MeshNew::DrawWireFrame(const Camera& camera, const glm::mat4& model_matrix)
{
	glm::mat4 model = model_matrix;
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = camera.GetProjectionMatrix();

	auto& shader = ShaderManager::GetShader("wireframe");
	shader.Bind();
	shader.SetFloat("lineThickness", lineThickness);
	shader.SetUniformMat4f("model", model);
	shader.SetUniformMat4f("view", view);
	shader.SetUniformMat4f("projection", projection);

	glBindVertexArray(m_wireVAO);

	GLCall(glDrawArrays(GL_TRIANGLES, 0, positionVertices.size()));

}

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

	std::copy(planeVertices, planeVertices + 30, std::back_inserter(mesh.vertices));

	for (int i = 0; i < mesh.vertices.size(); i += 5)
	{
		const glm::vec3 v(mesh.vertices[i], mesh.vertices[i + 1], mesh.vertices[i + 2]);
		mesh.positionVertices.emplace_back(v);
	}

	mesh.m_aabb.CalcBounds(mesh.positionVertices);
	mesh.m_aabb_OG = mesh.m_aabb;

	VertexBufferLayout vbl;
	vbl.Push<float>(3, VertexType::POSITION);
	vbl.Push<float>(2, VertexType::TEXCOORD);
	mesh.m_VertexBufferLayout = vbl;

	unsigned int planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.vertices.size(), &mesh.vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));

	mesh.VBO = planeVBO;
	mesh.VAO = planeVAO;

	return mesh;
}