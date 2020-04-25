#pragma once
#include "Components/Model.h"
#include "Geometry/TriangleBuffer.h"

class Model; 
class TriangleBuffer;

class EntityManager
{
public:
	std::vector<Model*> Entities;

	static std::vector<Model*>& GetEntities()
	{
		return GetInstance().Entities;
	}

	static void AddEntity(Model& model)
	{
		auto& instance = GetInstance();
		instance.Entities.emplace_back(&model);
		if(model.m_name != "WireCube") 
			TriangleBuffer::AddTriangles(model);
	}

	static Model& GetEntity(const std::string& ent_name);
private:
	static EntityManager& GetInstance()
	{
		static EntityManager instance;
		return instance;
	}

	EntityManager operator=(EntityManager& other) = delete; //no copy!


};

