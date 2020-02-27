#include "precomp.h"
#include "VertexBufferLayout.h"
#include "VertexArraySub.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

VertexArraySub::VertexArraySub() : ::VertexArray()
{

}

void  VertexArraySub::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	Bind();
	vb.Bind();

	//glEnableVertexAttribArray(0);    // We like submitting vertices on stream 0 for no spe
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void*)0);
	//glEnableVertexAttribArray(1);    // We like submitting normals on stream 1 for no spec
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void*)12);
	//GLCall(glEnableVertexAttribArray(0));
	//auto elems = layout.GetElements();
	//const auto first = elems[0];
	//const int startingPosUV = first.count * VertexBufferElement::GetSizeOfType(first.type);
	//
	////0 = vertex pos
	//GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (const GLvoid*)(0)));
	//
	////7b. Link buffer data to ub attribute location  
	//if (elems.size() > 1)
	//{
	//	GLCall(glEnableVertexAttribArray(1));
	//
	//	GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const GLvoid*)(startingPosUV)));
	//}
	//

	const auto& elements = layout.GetElements();
	int stride = layout.GetStride();
	unsigned int offset = 0;

	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		GLCall(glEnableVertexAttribArray(i));
		GLCall(glVertexAttribPointer(i, element.count,
			element.type, element.normalized, stride, (const void*)offset));

		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}


}