#pragma once
#include "precomp.h"

class RenderBufferObject
{
private:
	unsigned int m_renderID;

public:
	RenderBufferObject();
	void Bind() const;

	unsigned int GetRendererID() const;

};



