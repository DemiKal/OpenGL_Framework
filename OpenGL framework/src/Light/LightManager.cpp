#include "precomp.h"
#include "LightManager.h"

LightManager::LightManager(): m_lights(), m_directionalLight(glm::vec3(0,1,0)), m_ambient(1)
{
}  
