#pragma once

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
		TriangleBuffer::AddTriangles(model);
	}

private:
	static EntityManager& GetInstance()
	{
		static EntityManager instance;
		return instance;
	}

	EntityManager operator=(EntityManager& other) = delete; //no copy!


};

