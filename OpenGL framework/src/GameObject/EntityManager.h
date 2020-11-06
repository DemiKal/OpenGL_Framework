#pragma once
#include "Components/Model.h"
#include "Geometry/TriangleBuffer.h"

class Model;

class EntityManager
{
public:
	std::vector<Model*> Entities;

	static std::vector<Model*>& GetEntities();
	static std::optional<std::reference_wrapper<Model>> GetEntity(const std::string& ent_name);
	static void Init() { GetInstance()._Init(); };
	void _Init();
	void _Update();
	static void AddEntity(Model& model);

private:
	static EntityManager& GetInstance();

	EntityManager operator=(EntityManager& other) = delete; //no copy!

	entt::registry m_Registry = entt::registry{};
};

inline EntityManager& EntityManager::GetInstance()
{
	static EntityManager instance;
	return instance;
}

