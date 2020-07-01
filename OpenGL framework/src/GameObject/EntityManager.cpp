#include "precomp.h"
#include "EntityManager.h"

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
