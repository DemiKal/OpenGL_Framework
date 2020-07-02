#pragma once


class Gbuffer;
class Camera;

class PostProcessing
{
public:
	static void  ShadowCastGLSL(Camera& cam, Gbuffer& gBuffer);
};

