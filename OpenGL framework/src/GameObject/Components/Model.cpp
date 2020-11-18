#include "precomp.h"
#include "GameObject/Components/Model.h"
#include "Rendering/Buffer/VertexBufferLayout.h"
#include "GameObject/Components/Texture2D.h"
#include "Rendering/Shader.h"
#include "Rendering/ShaderManager.h"
#include "GameObject/Camera.h"
#include "Animation/AnimationChannel.h"
#include "Animation/Joint.h"
#include "Animation/Animation.h"
#include "Animation/Animator.h"
#include "GameObject/Components/AABB.h"
#include "GameObject/Components/mesh.h"
#include "Light/LightManager.h"
#include "Rendering/Shader.h"


//from assimpviewer
glm::mat4 AI2GLMMAT(aiMatrix4x4  ai_mat) {
	glm::mat4 result;
	aiMatrix4x4 transposed = ai_mat.Transpose();
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++)
		{
			result[i][j] = transposed[i][j];
		}
	}
	return result;
}

Model::Model(const std::string& path, const aiPostProcessSteps loadFlags = aiProcess_GenBoundingBoxes)
	:
	m_modelMatrix(glm::mat4(1.0f)),
	m_meshes(),
	m_directory(""),
	m_textures_loaded(),
	m_shaderIdx(0)
{
	LoadModel(path, loadFlags);
}

glm::mat4 Model::GetModelMatrix() const
{
	return m_modelMatrix;
}

void Model::SetModelMatrix(const glm::mat4& mat)
{
	m_modelMatrix = mat;
}

void Model::SetShader(const std::string& shadername)
{
	m_shaderIdx = ShaderManager::GetShaderIdx(shadername);
}


void CreateHierarchy(std::shared_ptr<Model::Armature> parentArmature, aiNode* node)
{
	std::shared_ptr<Model::Armature> me = std::make_shared<Model::Armature>();
	me->name = node->mName.C_Str();
	me->mat = AI2GLMMAT(node->mTransformation);
	me->parent = parentArmature;

	parentArmature->children.push_back(me);

	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		CreateHierarchy(me, node->mChildren[i]);
	}
}

aiNode* FindRootNode(aiNode* node, const std::string& name) {
	if (node->mName.C_Str() == name)
		return node;
	else {
		for (unsigned int i = 0; i < node->mNumChildren; i++)
			FindRootNode(node->mChildren[i], name);
	}
	return nullptr;
}

void Model::LoadModel(const std::string& path, const aiPostProcessSteps loadFlags)
{
	Assimp::Importer import;
	const auto standardFlags = aiProcess_GenSmoothNormals | aiProcess_GenBoundingBoxes;
	const auto flagsComposed = standardFlags | loadFlags;
	const aiScene* scene = import.ReadFile(path, flagsComposed);


	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	m_directory = path.substr(0, path.find_last_of('/'));

	//aiNode* root_node = FindRootNode(scene->mRootNode, "Torso");
	m_armature = std::make_shared<Armature>();
	m_armature->name = scene->mRootNode->mName.C_Str();
	m_armature->mat = AI2GLMMAT(scene->mRootNode->mTransformation);
	m_armature->parent = nullptr;
	this->m_inverseRoot = AI2GLMMAT(scene->mRootNode->mTransformation);

	CreateHierarchy(m_armature, scene->mRootNode);

	ProcessNode(scene->mRootNode, scene, m_armature);
}


void Model::ProcessNode(aiNode* node, const aiScene* scene, std::shared_ptr<Armature> armature)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(ProcessMesh(mesh, scene, armature));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, armature);
	}
}

void Model::UpdateModelMatrix()
{
	const auto id = glm::mat4(1.0f);
	const auto scale = glm::scale(id, m_scale);
	const glm::mat4 rotation = glm::eulerAngleYXZ(
		glm::radians(m_rotation.y),
		glm::radians(m_rotation.x),
		glm::radians(m_rotation.z));

	const glm::mat4 translation = glm::translate(id, m_position);
	m_modelMatrix = translation * rotation * scale;
}

void Model::Update(float deltaTime)
{
	UpdateModelMatrix();

	for (Mesh& mesh : m_meshes)
		mesh.m_aabb.UpdateArvo(m_modelMatrix, mesh.m_aabb_OG);
}

void FindChildren(
	std::shared_ptr<Model::Armature> arma,
	Joint& joint,
	std::unordered_map<std::string, unsigned int> bonesDict)
{
	if (arma->name == joint.GetName())
	{
		for (int i = 0; i < arma->children.size(); i++)
		{
			joint.m_childrenPair.emplace_back(
				std::make_pair(arma->children[i]->name, bonesDict[arma->children[i]->name]));
		}
		return;
	}
	for (auto& i : arma->children) FindChildren(i, joint, bonesDict);
}

void AssignMatrices(std::shared_ptr<Model::Armature> armature, Joint& joint)
{
	if (armature->name == joint.m_name)
	{
		joint.m_nodeTransform = armature->mat;
		return;
	}
	else for (auto& children : armature->children)	AssignMatrices(children, joint);
}
Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene, std::shared_ptr<Armature>  armature)
{
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	const unsigned int BONESPERVERTEX = 3;

	VertexBufferLayout vbLayout;
	unsigned int stride = 0;

	const bool hasPositions = mesh->HasPositions();
	if (hasPositions) vbLayout.Push<float>(3, VertexType::POSITION);

	const bool hasNormals = mesh->HasNormals();
	if (hasNormals)
		vbLayout.Push<float>(3, VertexType::NORMAL);

	const bool hasTexCoords = mesh->HasTextureCoords(0);
	if (hasTexCoords) vbLayout.Push<float>(2, VertexType::TEXCOORD);

	const bool hasTangents = mesh->HasTangentsAndBitangents();
	if (hasTangents)
	{
		vbLayout.Push<float>(3, VertexType::TANGENT);
		vbLayout.Push<float>(3, VertexType::BITANGENT);
	}

	//TODO colors
	bool colors = mesh->HasVertexColors(1);

	const bool hasBones = mesh->HasBones();
	if (hasBones) {
		vbLayout.Push<float>(BONESPERVERTEX, VertexType::BONE_INDEX);	//bone idx
		vbLayout.Push<float>(BONESPERVERTEX, VertexType::BONE_WEIGHT);	//bone weight
	}

	const unsigned int morphTargetCount = mesh->mNumAnimMeshes;

	if (morphTargetCount)
	{
		for (unsigned int i = 0; i < morphTargetCount; i++)
		{
			aiAnimMesh* morph = mesh->mAnimMeshes[i];
			if (morph->HasPositions())
				vbLayout.Push<float>(3, VertexType::POSITION);

			if (morph->mNumVertices != mesh->mNumVertices)
				std::cout << " morph target vertex count != original vertex count!";
		}
	}

	//finally create vertex buffer
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		vertices.push_back(mesh->mVertices[i].x);
		vertices.push_back(mesh->mVertices[i].y);
		vertices.push_back(mesh->mVertices[i].z);

		if (hasNormals)
		{
			vertices.push_back(mesh->mNormals[i].x);
			vertices.push_back(mesh->mNormals[i].y);
			vertices.push_back(mesh->mNormals[i].z);
		}

		if (hasTexCoords)
		{
			vertices.push_back(mesh->mTextureCoords[0][i].x);
			vertices.push_back(mesh->mTextureCoords[0][i].y);
		}
		//manage morph animation by interleaving
		for (unsigned int j = 0; j < morphTargetCount; j++)
		{
			aiAnimMesh* morph = mesh->mAnimMeshes[j];
			//TODO: check for each vertex element to see if they are in animation
			if (morph->HasPositions())
				vertices.push_back(morph->mVertices[i].x);
			vertices.push_back(morph->mVertices[i].y);
			vertices.push_back(morph->mVertices[i].z);
		}

		if (hasTangents)
		{
			vertices.push_back(mesh->mTangents[i].x);
			vertices.push_back(mesh->mTangents[i].y);
			vertices.push_back(mesh->mTangents[i].z);
			// bitangemplace_back( 
			vertices.push_back(mesh->mBitangents[i].x);
			vertices.push_back(mesh->mBitangents[i].y);
			vertices.push_back(mesh->mBitangents[i].z);
		}
		if (hasBones) {
			vertices.push_back(0);
			vertices.push_back(0);
			vertices.push_back(0);
			vertices.push_back(0);
			vertices.push_back(0);
			vertices.push_back(0);
		}

	}


	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	Animator  animator;
	animator.m_inverse_root = m_inverseRoot;

	std::unordered_map<std::string, unsigned int> bonesMapping;
	if (mesh->HasBones())
	{
		std::vector<Joint> bones;
		std::vector< std::unordered_map<unsigned int, float>> bonemapping;
		bonemapping.resize(mesh->mNumVertices);

		std::unordered_map<std::string, unsigned int> bonesDict;
		for (unsigned int boneIdx = 0; boneIdx < mesh->mNumBones; boneIdx++) {
			aiBone* ai_bone = mesh->mBones[boneIdx];
			std::string boneName = ai_bone->mName.C_Str();
			bonesDict[boneName] = boneIdx;
		}

		for (unsigned int boneIdx = 0; boneIdx < mesh->mNumBones; boneIdx++) {
			aiBone* ai_bone = mesh->mBones[boneIdx];
			std::string boneName = ai_bone->mName.C_Str();

			//glm::mat4 a1 = glm::make_mat4(ai_bone->mOffsetMatrix[0]);
			//glm::mat4 a2 = AI2GLMMAT(ai_bone->mOffsetMatrix);

			Joint joint(boneIdx, boneName, AI2GLMMAT(ai_bone->mOffsetMatrix));
			bonesMapping[boneName] = boneIdx;
			FindChildren(armature, joint, bonesDict);
			AssignMatrices(armature, joint);

			bones.emplace_back(joint);

			const unsigned int numWeights = mesh->mBones[boneIdx]->mNumWeights;

			for (unsigned int j = 0; j < numWeights; j++) {
				aiVertexWeight vw = ai_bone->mWeights[j];
				const unsigned int v_idx = vw.mVertexId;
				const float v_w = vw.mWeight;

				if (bonemapping[v_idx].find(boneIdx) == bonemapping[v_idx].end())
				{
					bonemapping[v_idx][boneIdx] = v_w;
				}
				else
				{
					ASSERT(false);
				}
			}
		}

		//copy bones
		animator.m_bones.resize(bones.size());
		std::copy(bones.begin(), bones.end(), animator.m_bones.begin());

		const unsigned int bmapSize = static_cast<unsigned int>(bonemapping.size());
		for (unsigned int i = 0; i < bmapSize; i++) {

			float sum = 0;
			for (auto& bm : bonemapping[i]) sum += bm.second;

			if (fabs(sum - 1) > 0.01) //renormalize values! shouldn't happen with assimp postprocessing
			{
				std::vector<std::pair<unsigned int, float>> vec;
				std::for_each(bonemapping[i].begin(), bonemapping[i].end(),
					[&](const std::pair<unsigned int, float>& element) {
						vec.push_back(element);
					});

				std::sort(vec.begin(), vec.end(),
					[](const std::pair<unsigned int, float>& L,
						const std::pair<unsigned int, float>& R) {
							return L.second < R.second;
					});

				for (auto& bm : bonemapping[i]) {
					bm.second = bm.second / sum;
				}

			}

			unsigned int j = 0;
			for (auto& bm : bonemapping[i])
			{
				const unsigned int elementIdxV1 = vbLayout.GetElementIndex(i, j, VertexType::BONE_INDEX);
				const unsigned int elementIdxV2 = vbLayout.GetElementIndex(i, j, VertexType::BONE_WEIGHT);
				vertices[elementIdxV1] = bm.first;
				vertices[elementIdxV2] = bm.second;
				j++;
				if (j >= BONESPERVERTEX) break;
			}
		}
	}


	if (scene->HasAnimations()) {
		for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
			aiAnimation* anim = scene->mAnimations[i];
			const std::string anim_name = anim->mName.C_Str();
			float ticks = 1;	//anim->mTicksPerSecond;
			float duration = static_cast<float>(anim->mDuration);
			animator.m_duration = duration;
			animator.m_ticks = ticks;


			std::vector<AnimationChannel> AnimationChannels;
			AnimationChannels.resize(anim->mNumChannels); //resize to sync with bones

			for (size_t j = 0; j < anim->mNumChannels; j++)
			{
				const aiNodeAnim* ai_channel = anim->mChannels[j];
				const std::string channelName = ai_channel->mNodeName.C_Str();

				std::vector< PositionKey> positionkeys;
				std::vector< RotationKey > rotationKeys;
				std::vector< ScaleKey> scalingKeys;

				for (size_t k = 0; k < ai_channel->mNumPositionKeys; k++)
				{
					aiVectorKey ai_key = ai_channel->mPositionKeys[k];
					positionkeys.emplace_back(ai_key.mTime, ai_key.mValue.x, ai_key.mValue.y, ai_key.mValue.z);
				}

				for (size_t k = 0; k < ai_channel->mNumRotationKeys; k++)
				{
					aiQuatKey ai_key = ai_channel->mRotationKeys[k];
					rotationKeys.emplace_back(ai_key.mTime, ai_key.mValue.w, ai_key.mValue.x, ai_key.mValue.y, ai_key.mValue.z);
				}

				for (size_t k = 0; k < ai_channel->mNumScalingKeys; k++)
				{
					aiVectorKey ai_key = ai_channel->mScalingKeys[k];
					scalingKeys.emplace_back(ai_key.mTime, ai_key.mValue.x, ai_key.mValue.y, ai_key.mValue.z);
				}

				//AnimationChannels.emplace_back(AnimationChannel(channelName, positionkeys, rotationKeys, scalingKeys));
				unsigned int channel_idx = bonesMapping[channelName];
				AnimationChannels[channel_idx] = AnimationChannel(channelName, positionkeys, rotationKeys, scalingKeys);

			}

			Animation animation(duration, AnimationChannels);
			animator.current = animation;
		}
	}

	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN
	std::vector<Texture2D> textures;
	//auto& shader = GetShader();
	// 1. diffuse maps
	std::vector<Texture2D> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	std::vector<Texture2D> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<Texture2D> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<Texture2D> heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	// return a mesh object created from the extracted mesh data
	Mesh meshnew(vertices, indices, textures, animator, vbLayout);

	aiAABB& ai_aabb = mesh->mAABB;

	meshnew.m_aabb = AABB({ ai_aabb.mMin.x, ai_aabb.mMin.y, ai_aabb.mMin.z },
		{ ai_aabb.mMax.x, ai_aabb.mMax.y, ai_aabb.mMax.z });
	meshnew.m_aabb_OG = meshnew.m_aabb;

	//for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	//	meshnew.positionVertices.emplace_back(
	//		glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
	for (const int v_idx : indices)
	{
		auto& ai_v = mesh->mVertices[v_idx];
		glm::vec3   v = { ai_v.x, ai_v.y, ai_v.z };
		meshnew.positionVertices.emplace_back(v);
	}

	return	 meshnew;
}

Mesh& Model::GetMesh(const unsigned idx)
{
	return m_meshes[idx];
}


std::vector<Texture2D> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type,
	const std::string& typeName)
{
	std::vector<Texture2D> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString fileName;
		auto r = mat->GetTexture(type, i, &fileName);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (auto& j : m_textures_loaded)
		{
			if (std::strcmp(j.GetPath().data(), fileName.C_Str()) == 0)
			{
				textures.push_back(j);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it

			const std::string fullpath = m_directory + '/' + fileName.C_Str();
			Texture2D texture(fullpath, typeName);

			textures.push_back(texture);
			m_textures_loaded.emplace_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}

void Model::Draw(const glm::mat4& projection, Shader& shader)
{
	static long idx = 1;
	shader.Bind();

	shader.SetUniformMat4f("u_model", m_modelMatrix);
	shader.SetUniformMat4f("u_lightProj", projection);
	shader.SetVec3f("u_color", idx++ % 2 ? glm::vec3(1.0f, 0, 0) : glm::vec3(0, 1, 0));

	for (auto& mesh : m_meshes)
		mesh.Draw(shader);

	shader.Unbind();
}

void Model::Draw(const Camera& cam, Shader& shader)
{
	shader.Bind();

	const glm::mat4 view = cam.GetViewMatrix();
	const glm::mat4 proj = cam.GetProjectionMatrix();

	shader.SetUniformMat4f("u_model", m_modelMatrix);
	shader.SetUniformMat4f("u_view", view);
	shader.SetUniformMat4f("u_projection", proj);

	for (auto& mesh : m_meshes)
		mesh.Draw(shader);

	shader.Unbind();
}

void Model::Draw(const Camera& cam)
{
	Shader& shader = ShaderManager::GetShader(m_shaderIdx);
	Draw(cam, shader);
}

Shader& Model::GetShader() const
{
	return   ShaderManager::GetShader(m_shaderIdx);
}

Model Model::CreateCube() {
	Model model;
	Mesh mesh = Mesh::CreateCube();
	model.m_meshes.emplace_back(mesh);
	return model;
}

Model Model::CreateCubeWireframe() {
	Model model;
	Mesh mesh = Mesh::CreateCubeWireframe();
	model.m_meshes.emplace_back(mesh);
	return model;
}

Model Model::CreatePlane()
{
	Model model;
	Mesh mesh = Mesh::CreatePlane();

	model.m_meshes.emplace_back(mesh);
	return model;
}
