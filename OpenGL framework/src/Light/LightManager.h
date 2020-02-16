#pragma once
class LightManager
{
private:
	std::vector<Light> m_lights;

	LightManager();
	
	static inline LightManager& GetInstance()
	{
		static LightManager instance;
		return instance;
	}

public:
	static void AddLight(const glm::vec3 position, const glm::vec3 color)
	{
		GetInstance().m_lights.emplace_back(Light( position , color));
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
};

