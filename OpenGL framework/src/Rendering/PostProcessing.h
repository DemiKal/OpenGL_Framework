#pragma once
#include "GameObject/Camera.h"
#include "Buffer/Gbuffer.h"

class PostProcessing
{
	static void ShadowCastGLSL(Camera& cam, Gbuffer& gBuffer);
};

