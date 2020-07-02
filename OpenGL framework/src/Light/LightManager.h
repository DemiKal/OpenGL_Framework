#pragma once
#include "Light.h"

class LightManager
{
private:
	std::vector<Light> m_lights;
	glm::vec3 m_directionalLight;
	float m_ambient;


	LightManager()
		:
		m_lights(),
		m_directionalLight({ 0, 1, 0 }),
		m_ambient(0.1f)
	{

	}


	static inline LightManager& GetInstance();

public:
	static float& GetAmbientLight();

	static std::vector<Light>& GetLights();

	static Light& GetLight(const unsigned int idx);

	static void DebugRender(const Camera& cam);
	static void AddLight(const glm::vec3 position, const glm::vec3 color);

	static glm::vec3& GetDirectionalLight();

};

