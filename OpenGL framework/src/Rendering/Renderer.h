#pragma once

class GPUShader;
class IndexBuffer;
class VertexArray;

class Renderer
{
private: bool alphaBlending = true;
public:
	void Clear();
	void Draw(const VertexArray& va, const IndexBuffer& ib, const GPUShader& shader);
	void SetAlphaBlending(bool set);
	bool GetAlphaBlending() const { return alphaBlending; }
};
