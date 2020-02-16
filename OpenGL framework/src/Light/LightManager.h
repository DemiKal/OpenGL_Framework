#pragma once
class light_manager
{
private:
	std::vector<Light> m_lights;

	light_manager();

public:
	static inline light_manager& get_instance()
	{
		static light_manager instance;
		return instance;
	}

	static void add_light(const glm::vec3 position, const glm::vec3 color)
	{
		get_instance().m_lights.emplace_back(Light( position , color));
	}

	static std::vector<Light>& GetLights()
	{
		 return get_instance().m_lights;
	}

	static Light& GetLight(const unsigned int idx) 
	{
		return get_instance().m_lights[idx];
	}

	static void debug_render(const Camera& cam)
	{
		for (Light& l : GetLights())
			l.draw(cam);
	}
};

