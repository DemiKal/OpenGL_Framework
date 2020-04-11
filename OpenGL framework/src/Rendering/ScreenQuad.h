#pragma once
class ScreenQuad
{
public:
	ScreenQuad();
	void Bind();
	void UnBind();
	void Draw();
private:
		unsigned int m_VAO, m_VBO;
};

