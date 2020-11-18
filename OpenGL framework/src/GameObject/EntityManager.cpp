#include "precomp.h"
#include "EntityManager.h"
#include "Components/Model.h"
#include "Components/EntityComponents.h"

std::vector<std::shared_ptr<Model>>& EntityManager::GetEntities()
{
	return GetInstance().Entities;
}

void EntityManager::AddEntity(Model& model)
{
	auto& instance = GetInstance();
	instance.Entities.emplace_back(&model);
	TriangleBuffer::AddTriangles(model);
}

void EntityManager::LoadModel(const std::string& path, const std::string& name)
{
	const auto& instance = GetInstance();
	auto& vec = instance.GetEntities();
	vec.emplace_back(std::make_shared<Model>(path, aiProcess_Triangulate));
	vec.back()->m_name = name;
}

std::optional<std::shared_ptr<Model>> EntityManager::GetEntity(const std::string& ent_name)
{
	const auto& instance = GetInstance();
	const auto& entities = instance.GetEntities();

	for (auto m : entities)
	{
		if (m->m_name == ent_name)
			return m;
	}

	fmt::print("Could not find entity of name {}\n", ent_name);
	ASSERT(false);
	return {};
}

class ComponentManager
{
public:
	void Initialize(entt::registry&, entt::entity)
	{
		fmt::print("initializing\n");
	}
	void Update(entt::registry&, entt::entity)
	{
		fmt::print("updating\n");
	}
	ComponentManager() = default;
};

void InitializeAAA(entt::registry& a, entt::entity e)
{
	fmt::print("hi dad AAA\n");
}

void EntityManager::_Init()
{
	//std::unique_ptr<Mesh> quad = std::make_unique<Mesh>(Mesh::CreatePlane());

	//struct my_resource
	//{
	//	const int value;
	//	my_resource(int val) : value(val) {};
	//};
	// /
	//struct my_loader : entt::resource_loader<my_loader, my_resource>
	//{
	//	std::shared_ptr<my_resource> load(int value) const
	//	{
	//		return std::shared_ptr<my_resource>(new my_resource{ value });
	//	}
	//};
	// 
	//
	//using my_cache = entt::resource_cache<my_resource>;
	//my_cache cache;
	// //
	//cache.load < my_loader>("quadVBO"_hs, quad->GetVBO());
	//cache.load < my_loader>("bb"_hs, 2);
	//auto handle1 = cache.handle("aa"_hs);
	//auto handle2 = cache.handle("bb"_hs);
	//auto handle3 = cache.handle("aaac"_hs);


	std::uint64_t dt = 16;
	//Model
	ComponentManager cm;


	//m_Registry.assign<SpriteLocation>(player_entity, 0.0f, 0.0f);
	//m_Registry.assign<MovementComponent>(player_entity);
	//load_sprite("../assets/sprites/paddleBlu.png", m_Registry.get<SDL_RenderSprite>(player_entity));
}

void EntityManager::_Update()
{
	entt::registry  registry;
	auto view = registry.view<PositionComponent, VelocityComponent>();

	for (auto entity : view)
	{
		// gets only the components that are going to be used ...

		auto& vel = view.get<VelocityComponent>(entity).Velocity;

		vel.x = 0.;
		vel.y = 0.;

		// ...
	}


}