#include "precomp.h"
#include "LightManager.h"

LightManager& LightManager::GetInstance()
{
	static LightManager instance;
	return instance;
}

void LightManager::AddLight(const glm::vec3 position, const glm::vec3 color)
{
	GetInstance().m_lights.emplace_back(Light(position, color));
}

std::vector<Light>& LightManager::GetLights()
{
	return GetInstance().m_lights;
}

Light& LightManager::GetLight(const unsigned idx)
{
	return GetInstance().m_lights[idx];
}

void LightManager::DebugRender(const Camera& cam)
{
	for (Light& l : GetLights())
		l.Draw(cam);
}

glm::vec3& LightManager::GetDirectionalLight()
{
	return GetInstance().m_directionalLight;
}

float& LightManager::GetAmbientLight()
{
	return GetInstance().m_ambient;
}
