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
	static void AddEntity(Model& model);

private:
	static EntityManager& GetInstance();

	EntityManager operator=(EntityManager& other) = delete; //no copy!


};

inline EntityManager& EntityManager::GetInstance()
{
	static EntityManager instance;
	return instance;
}

