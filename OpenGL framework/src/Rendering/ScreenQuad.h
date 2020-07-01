#pragma once
class ScreenQuad
{
public:
	ScreenQuad();
	void Bind() const;
	static void UnBind();
	static void Draw();
private:
	unsigned int m_VAO, m_VBO;
};

