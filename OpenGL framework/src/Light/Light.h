#pragma once
class Light
{
private:
	glm::vec3  m_position;
	glm::vec3  m_color;
	unsigned int m_VAO;
	unsigned int m_VBO;


	void create_buffer_object();
public:
	Light(const glm::vec3  pos, const glm::vec3  col)
		:
		m_position(pos), m_color(col), m_VAO(0), m_VBO(0)
	{	
		create_buffer_object();
	}

	void draw(const Camera& cam);
	glm::vec3& get_position() { return m_position; }
	glm::vec3& get_color() { return m_color; }
	[[nodiscard]] unsigned int get_vao() const { return m_VAO; }
	unsigned int get_vbo() const;
};



