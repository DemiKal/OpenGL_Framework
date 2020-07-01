#pragma once
class ScreenQuad
{
friend class Renderer;
private:
	ScreenQuad();
	void Init();
	void Bind() const;
	static void UnBind();
	static void Draw();
	unsigned int m_VAO, m_VBO;
};

