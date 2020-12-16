#pragma once
#include "precomp.h"
#include "GameObject/Components/Meshmanager.h"
#include "GameObject/Camera.h"
#include "Rendering/ShaderManager.h"

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

	glm::vec3 Position{ 0, 0, 0 };
	glm::vec3 Rotation{ 0, 0, 0 }; //TODO ADD QUAT
	glm::vec3 Scale{ 1, 1, 1 };

	[[nodiscard]] glm::mat4 CalcMatrix() const
	{
		const glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), Scale);
		//const glm::mat4 rotationMat = glm::eulerAngleYXZ(Rotation.y, Rotation.x, Rotation.z);
		glm::mat4 rotationMat = glm::toMat4(glm::quat(Rotation));
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
	enum class PrimitiveType
	{
		Cube, CubeWireframe, Plane, Triangle
	};
	MeshComponent() = delete;

	explicit MeshComponent(const PrimitiveType primitiveType)
	{
		switch (primitiveType)
		{
		case PrimitiveType::Cube:			MeshIdx = 0; break;
		case PrimitiveType::CubeWireframe:  MeshIdx = 1; break;
		case PrimitiveType::Plane:			MeshIdx = 2; break;
		case PrimitiveType::Triangle:		MeshIdx = 3; break;
		}
		Initialized = true;
	}

	MeshComponent(const std::string& path, const aiPostProcessSteps postProcessing)
	{
		const float h = 0.5f;
		BoundingBox = AABB(glm::vec3(-h, -h, -h), glm::vec3(h, h, h));

		auto ret = MeshManager::LoadFromFile(path, postProcessing);
		if (ret)
		{
			Initialized = true;
			MeshIdx = ret.value();
			ShaderIdx = ShaderManager::GetShaderIdx("basic"); //TODO: determine shader automatically
			Mesh& m = MeshManager::GetMesh(*ret);
			BoundingBox = m.m_aabb;
		}
	}
	uint32_t MeshIdx = 0;
	uint32_t ShaderIdx = 0;
	uint32_t Layer = 0; //layer for culling (TODO: future memes)
	AABB BoundingBox;
	bool Initialized = false;

	//DEBUG... TODO: Delegate this to somewhere else so it doesn't pollute this struct
	bool DrawWireFrame = false;
	bool DrawAABB = false;
	bool DrawNormals = false;
	bool Enabled = true;
	glm::vec4 WireFrameColor{ 1, 1, 1, 1 };
	glm::vec4 AABBcolor{ 1, 1, 0, 0.75f };
	glm::vec4 NormalsColor{ 0,1,1,1 };
	float NormalsMagnitude{ 0.2f };
};

struct Texture2DComponent
{
	uint32_t texId = 0; //GLid for tex 
	bool loaded = false;
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
//std::string m_Name;
//unsigned int m_shaderIdx; //TODO check with reference inside entt