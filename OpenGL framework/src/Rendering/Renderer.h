#pragma once

class Renderer
{
private: bool alphaBlending = true;
public:
	void Clear();
	void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader);
	void SetAlphaBlending(bool set);
	bool GetAlphaBlending() const { return alphaBlending; }
};