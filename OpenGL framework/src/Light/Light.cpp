#include "precomp.h"
#include "Light.h"

#include "GameObject/Camera.h"
#include "Rendering/ShaderManager.h"
#include "Rendering/GPUShader.h"

void Light::create_buffer_object()
{
	float dotVerts[] = {0, 0, 0}; //xyz pos

	//unsigned int dotVao, dotVBO;
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(dotVerts), &dotVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void Light::draw(const Camera& cam )  
{
	const glm::mat4  model = glm::translate(glm::mat4(1.0f), m_position);
	const glm::mat4& view = cam.GetViewMatrix();
	const glm::mat4& projection = cam.GetProjectionMatrix();
	GPUShader& shader = ShaderManager::GetShader("lightShader");
	shader.Bind();
	shader.SetUniformMat4f("model", model);
	shader.SetUniformMat4f("view", view);
	shader.SetUniformMat4f("projection", projection);
	shader.setVec3("color", get_color());
	glEnable(GL_PROGRAM_POINT_SIZE);
	glPointSize(15);

	GLCall(glBindVertexArray(m_VAO));
	GLCall(glDrawArrays(GL_POINTS, 0, 3));
	shader.Unbind();
}

unsigned int Light::get_vbo() const
{
	return m_VBO;
}
