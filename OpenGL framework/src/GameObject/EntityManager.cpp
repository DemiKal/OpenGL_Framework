#include "precomp.h"
#include "EntityManager.h"

Model& EntityManager::GetEntity(const std::string& ent_name) 
{  
	const auto& instance = GetInstance();
	const auto& entities = instance.GetEntities();

	for (Model* m : entities )
	{
		if (m->name == ent_name)
			return *m;
	}
}
