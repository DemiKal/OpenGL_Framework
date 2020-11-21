#pragma once
#include "precomp.h"
#include "GameObject/Components/Meshmanager.h"
#include "GameObject/Camera.h"

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
	//TransformComponent(const glm::mat4& transform) : Transform(transform) {};
	//TransformComponent() : Transform(glm::mat4(1.0f)) {};
	TransformComponent() = default;

	glm::vec3 Position{ 0,0,0 };
	glm::vec3 Rotation{ 0,0,0 }; //TODO ADD QUAT
	glm::vec3 Scale{ 1,1,1 };

	[[nodiscard]] glm::mat4 CalcMatrix() const
	{
		const glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), Scale);
		const glm::mat4 rotationMat = glm::eulerAngleXYZ(
			glm::radians(Rotation.x), 
			glm::radians(Rotation.y),
			glm::radians(Rotation.z));
		const glm::mat4 translMat = glm::translate(glm::mat4(1.0f), Position);
		return translMat * rotationMat * scaleMat;
	}
};

struct TagComponent
{
	TagComponent(const entt::hashed_string& name) : Name(name) {}
	entt::hashed_string Name;
	
};

struct CameraComponent 
{
	Camera camera;
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
		auto ret = MeshManager::LoadFromFile(path, postProcessing);
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