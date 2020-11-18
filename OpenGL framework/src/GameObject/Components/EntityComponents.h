#pragma once
#include "precomp.h"
#include "GameObject/Components/Mesh.h"

struct PositionComponent
{
	glm::vec3 Position;
};

struct VelocityComponent
{
	glm::vec3 Velocity;
};

struct ScaleComponent
{
	glm::vec3 Scale;
};
struct OrientationVecComponent
{
	glm::vec3 OrientationVec;
};

struct OrientationQuatComponent
{
	glm::quat OrientationQuat;
};

struct  Armature
{
	std::string name;
	int id;
	std::vector<std::shared_ptr<Armature>> children;
	std::shared_ptr< Armature> parent;
	glm::mat4 mat;

};

struct TransformComponent
{
	TransformComponent(const glm::mat4& transform) : Transform(transform) {};
	TransformComponent() : Transform(glm::mat4(1.0f)) {};


	glm::mat4 Transform;
};

struct NameComponent
{
	entt::hashed_string Name;
};

struct DirectoryComponent
{
	entt::hashed_string directory;
};

//TODO fix
struct MeshComponent
{
	MeshComponent() = default;
	MeshComponent(const std::string& path, const aiPostProcessSteps postProcessing)
	{
		auto ret = Mesh::LoadFromFile(path, postProcessing);
		if (ret)
		{
			Initialized = true;
			MeshIdx = ret.value();
		}
	}

	uint32_t MeshIdx = 0;
	bool Initialized = false;
};

struct Texture2DComponent
{
	uint32_t texId = 0; //GLid for tex 
	bool loaded = false;
	//struct Texture2DComponent(const std::string& path)
	//{
	//		
	//}

};

struct CameraComponent
{

};

struct AABBComponent
{
	glm::vec3 min, max;
};

//std::shared_ptr <Armature> m_armature;
//glm::mat4 m_modelMatrix;
//std::vector<Mesh> m_meshes;
//std::string m_directory;
//std::vector<Texture2D> m_textures_loaded;
//glm::mat4 m_inverseRoot;
//glm::vec3 m_velocity = { 0.001f, 0, 0 };
//glm::vec3 m_position = { 0, 0, 0 };
//glm::vec3 m_rotation = { 0, 0, 0 };
//glm::vec3 m_scale = { 1,1,1 };
//std::string m_name;
//unsigned int m_shaderIdx; //TODO check with reference inside entt