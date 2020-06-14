#pragma once
#include "GameObject/Camera.h"
#include "Buffer/Gbuffer.h"

class PostProcessing
{
public:
	static void  ShadowCastGLSL(Camera& cam, Gbuffer& gBuffer);
};

