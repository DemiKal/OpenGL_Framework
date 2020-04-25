#include "precomp.h"
#include "EntityManager.h"

Model& EntityManager::GetEntity(const std::string& ent_name) 
{  
	const auto& instance = GetInstance();
	const auto& entities = instance.GetEntities();

	for (Model* m : entities )
	{
		if (m->m_name == ent_name)
			return *m;
	}

	std::cout << "Could not find entity of name, " << ent_name << "\n";
}
