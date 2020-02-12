#pragma once
enum  class VertexType
{
	POSITION,
	COLOR,
	TEXCOORD,
	BONE_INDEX,
	BONE_WEIGHT,
	NORMAL,
	TANGENT,
	BI_TANGENT,
};

struct VertexBufferElement
{
	unsigned int type;
	unsigned int count;
	unsigned char normalized;
	VertexType vertexType;
	unsigned int vertexIndex; //Get the index of this element

	VertexBufferElement(int classtype, int _count, char _normalized, VertexType semanticType, const unsigned int idx)
		:
		type(classtype), count(_count), normalized(_normalized), vertexType(semanticType), vertexIndex(idx) {}


	GLenum GetType()
	{

	}

	static unsigned int GetSizeOfType(unsigned int type)
	{
		switch (type)
		{
		case GL_FLOAT: return 4;
		case GL_UNSIGNED_INT: return 4;
		case GL_UNSIGNED_BYTE: return 1;

		}

		ASSERT(false);
		return 0;
	}
};
class VertexBufferLayout
{

private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned m_Stride;

public:
	VertexBufferLayout() : m_Stride(0) {}
	VertexBufferLayout(const VertexBufferLayout& o) : m_Stride(o.m_Stride), m_Elements(o.m_Elements) {}
	//~VertexBufferLayout() {};

	template<typename T>
	void Push(unsigned int count, VertexType vt)
	{
		static_assert(false);
	}

	template<>
	void Push<float>(unsigned int count, VertexType vt)
	{
		m_Elements.push_back(VertexBufferElement(GL_FLOAT, count, GL_FALSE, vt, m_Stride));
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
		//m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
		//m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
	}

	template<>
	void Push<unsigned int>(unsigned int count, VertexType vt)
	{
		m_Elements.push_back(VertexBufferElement(GL_UNSIGNED_INT, count, GL_FALSE, vt, m_Stride));
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
	}

	template<>
	void Push<unsigned char>(unsigned int count, VertexType vt)
	{
		m_Elements.push_back(VertexBufferElement(GL_UNSIGNED_INT, count, GL_TRUE, vt, m_Stride));
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
	}

	const unsigned int GetElementIndex(const unsigned int vertexIndex, const unsigned int subIdx, VertexType vt) {
		for (auto& vbe : GetElements()) {
			if (vbe.vertexType == vt)
			{
				const unsigned int elementIdx = vbe.vertexIndex;
				const unsigned int strideNoByte = vbe.vertexIndex / vbe.GetSizeOfType(vbe.type);
				return strideNoByte* vertexIndex + elementIdx + subIdx; 
			}
		}
	}




	inline unsigned int GetStride() const { return m_Stride; }
	inline const std::vector<VertexBufferElement> GetElements() const& { return m_Elements; }
};
