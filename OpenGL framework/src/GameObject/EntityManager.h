/*
#pragma once
#include "Components/Model.h"
#include "Geometry/TriangleBuffer.h"

class Model;

class EntityManager
{
public:
	std::vector<std::shared_ptr<Model>> Entities;

	//static std::vector<std::shared_ptr<Model>>& GetEntities();
	//static std::optional<std::shared_ptr<Model>> GetEntity(const std::string& ent_name);
	//static void Init() { GetInstance()._Init(); };
	//void _Init();
	//void _Update();
	static void AddEntity(Model& model);
	static void LoadModel(const std::string& path, const std::string& name);
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

*/