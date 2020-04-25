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


	static inline LightManager& GetInstance()
	{
		static LightManager instance;
		return instance;
	}

public:
	static void AddLight(const glm::vec3 position, const glm::vec3 color)
	{
		GetInstance().m_lights.emplace_back(Light(position, color));
	}

	static std::vector<Light>& GetLights()
	{
		return GetInstance().m_lights;
	}

	static Light& GetLight(const unsigned int idx)
	{
		return GetInstance().m_lights[idx];
	}

	static void debug_render(const Camera& cam)
	{
		for (Light& l : GetLights())
			l.draw(cam);
	}

	static glm::vec3& GetDirectionalLight()
	{
		return GetInstance().m_directionalLight;
	}

	static float& GetAmbientLight()
	{
		return GetInstance().m_ambient;
	}
};

