#pragma once
class Camera;

class Light
{
private:
	glm::vec3  m_position;
	glm::vec3  m_color;
	unsigned int m_VAO;
	unsigned int m_VBO;


	void create_buffer_object();
public:
	Light(const glm::vec3 pos, const glm::vec3 col);

	void Draw(const Camera& cam);
	glm::vec3& GetPosition();
	glm::vec3& GetColor();
	[[nodiscard]] unsigned int GetVAO() const;
	[[nodiscard]] unsigned int GetVBO() const;
};



