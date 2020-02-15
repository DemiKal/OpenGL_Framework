#pragma once
class light_manager
{
	std::vector<Light> m_lights_;

	light_manager();

public:
	static light_manager& get_instance()
	{
		static light_manager instance;
		return instance;
	}

	static void add_light(const glm::vec3 position, const glm::vec3 color)
	{
		get_instance().m_lights_.emplace_back(Light( position , color));
	}

	static std::vector<Light>& get_lights()
	{
		 return get_instance().m_lights_;
	}
	static void debug_render(const Camera& cam)
	{
		//light_manager& instance = get_instance();
		for (Light& l :   get_lights())
			l.draw(cam);
	}
};

