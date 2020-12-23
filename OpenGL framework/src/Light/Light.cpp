#include "precomp.h"
#include "Light.h"

#include "GameObject/Camera.h"
#include "Rendering/ShaderManager.h"
#include "Rendering/Shader.h"

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

Light::Light(const glm::vec3 pos, const glm::vec3 col):
	m_position(pos),
	m_color(col),
	m_VAO(0),
	m_VBO(0)
{
	create_buffer_object();
}

void Light::Draw(const Camera& cam )  
{
	const glm::mat4  model = glm::translate(glm::mat4(1.0f), m_position);
	const glm::mat4& view = cam.GetViewMatrix();
	const glm::mat4& projection = cam.GetProjectionMatrix();
	Shader& shader = ShaderManager::GetShader("lightShader");
	shader.Bind();
	shader.SetUniformMat4f("model", model);
	shader.SetUniformMat4f("view", view);
	shader.SetUniformMat4f("projection", projection);
	shader.SetVec3f("color", GetColor());
	glEnable(GL_PROGRAM_POINT_SIZE);
	glPointSize(15);

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_POINTS, 0, 3);
	shader.Unbind();
}

glm::vec3& Light::GetPosition()
{
	return m_position;
}

glm::vec3& Light::GetColor()
{
	return m_color;
}

unsigned Light::GetVAO() const
{
	return m_VAO;
}

unsigned int Light::GetVBO() const
{
	return m_VBO;
}
