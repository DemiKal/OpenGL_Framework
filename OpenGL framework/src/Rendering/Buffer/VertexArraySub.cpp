#include "precomp.h"


VertexArraySub::VertexArraySub() : VertexArray()
{

}

void  VertexArraySub::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	Bind();
	vb.Bind();

	GLCall(glEnableVertexAttribArray(0));
	GLCall(glEnableVertexAttribArray(1));

	//0 = vertex pos
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)(0)));

	//7b. Link buffer data to ub attribute location  
	GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)(72*4)));

 

	/*const auto& elements = layout.GetElements();
	unsigned int offset = 0;

	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		int stride = layout.GetStride();
		GLCall(glEnableVertexAttribArray(i));
		GLCall(glVertexAttribPointer(i, element.count,
			element.type, element.normalized, 0 , (const void*)offset));

		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}*/


}