#pragma once
#include "VertexArray.h"

//class VertexBuffer;
//class VertexBufferLayout;

class VertexArraySub : public VertexArray
{
public:
	VertexArraySub();
	~VertexArraySub();

 
	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout) override;

};



