#include "precomp.h"
#include "EntityManager.h"
#include "Components/Model.h"

std::vector<Model*>& EntityManager::GetEntities()
{
	return GetInstance().Entities;
}

void EntityManager::AddEntity(Model& model)
{
	auto& instance = GetInstance();
	instance.Entities.emplace_back(&model);
	TriangleBuffer::AddTriangles(model);
}

std::optional<std::reference_wrapper<Model>> EntityManager::GetEntity(const std::string& ent_name)
{  
	const auto& instance = GetInstance();
	const auto& entities = instance.GetEntities();

	for (Model* m : entities )
	{
		if (m->m_name == ent_name)
			return *m;
	}

	fmt::print("Could not find entity of name {}\n", ent_name);
	ASSERT(false);
	return {};
}
