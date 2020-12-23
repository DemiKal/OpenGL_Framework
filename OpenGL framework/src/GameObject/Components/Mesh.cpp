#pragma once
#include "precomp.h"
#include "Mesh.h"
#include "GameObject/Components/Texture2D.h"
#include "Rendering/ShaderManager.h"
#include "GameObject/Camera.h"
#include "Animation/Joint.h"
#include "Texture2D.h"
#include "GameObject/Components/EntityComponents.h"

glm::mat4 AI2GLMMAT(aiMatrix4x4& ai_mat)
{
	return glm::transpose(glm::make_mat4(ai_mat[0]));
}
//class glm::vec3;



struct  Armature2
{
	std::string name;
	int id{};
	std::vector< Armature2*> children;
	Armature2* parent{};
	glm::mat4 mat{};
	std::vector<uint32_t> indices;
	~Armature2() noexcept
	{
		//delete this;
		std::cout << "lmao cyaa\n";
		for (auto* child : children)
			delete child;
	}
};

Armature2* FindBone(Armature2* curr, const std::string& name)
{
	for (auto* child : curr->children)
		FindBone(child, name);

	if (std::strcmp(curr->name.c_str(), name.c_str()) == 0)
	{
		return curr;
	}
}

void CreateArmature(Armature2* parentArmature, aiNode* node, uint32_t  iter)
{
	Armature2* me = new Armature2;
	me->name = node->mName.C_Str();
	me->mat = glm::make_mat4(node->mTransformation[0]);

	if (iter > 0) me->parent = parentArmature;

	parentArmature->children.push_back(me);

	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		CreateArmature(me, node->mChildren[i], ++iter);
	}
}
const std::string_view Mesh::GetFilename() const
{
	return m_Filename;
};
const std::string_view Mesh::GetDirectory() const
{
	return m_Directory;
};
void CreateHierarchy(Armature2* parent, uint32_t& idx)
{
	parent->id = idx;

	for (Armature2* child : parent->children)
		CreateHierarchy(child, ++idx);

	for (Armature2* child : parent->children)
		parent->indices.emplace_back(child->id);

}

void CreateList(Armature2* parent, std::unordered_map< std::string, std::vector  < std::tuple < std::string, uint32_t>>>& dict)
{
	for (Armature2* child : parent->children)
		CreateList(child, dict);
	for (Armature2* child : parent->children)
		dict[parent->name].push_back({ child->name, child->id });
}
//void GetChildIndices(Armature2* parent, const std::unordered_map<std::string)

//TODO: ADD  PARENT POINTER AND FETCH LIST OF TRANSFORMS THAT WAY
void Mesh::LoadBoneData(
	const aiScene* scene,
	const aiMesh* mesh,
	std::unordered_map<std::string, unsigned int>& boneNames,
	std::vector<Joint>& bones)
{
	Armature2* first = new Armature2;
	CreateArmature(first, scene->mRootNode, 0);
	std::vector<Armature2*> armatureVec;
	std::vector<Armature2*> kids;

	armatureVec.push_back(first);

	while (!armatureVec.empty())
	{
		Armature2* current = armatureVec.back();
		armatureVec.pop_back();
		for (Armature2* child : current->children)
		{
			kids.emplace_back(child);
			armatureVec.push_back(child);
		}


	}

	Armature2* torso = nullptr;
	for (Armature2* a : kids)
		if (std::strcmp(a->name.c_str(), "Torso") == 0)
			torso = a;

	std::unordered_map< std::string, std::vector  < std::tuple < std::string, uint32_t>>>  boneMapping2;
	uint32_t idxCounter = 0;
	CreateHierarchy(torso, idxCounter);
	CreateList(torso, boneMapping2);


	//make lookup table for bone names
	for (unsigned int boneIdx = 0; boneIdx < mesh->mNumBones; boneIdx++)
	{
		aiBone* ai_bone = mesh->mBones[boneIdx];
		std::string boneName = ai_bone->mName.C_Str();
		Joint joint(boneIdx, boneName, AI2GLMMAT(ai_bone->mOffsetMatrix));
		boneNames[boneName] = boneIdx;

		bones.emplace_back(joint);
	}

	//map vertex weights to bones
	std::vector<std::vector<std::tuple<float, float>>> bonemapping(mesh->mNumVertices);
	for (unsigned int boneIdx = 0; boneIdx < mesh->mNumBones; boneIdx++)
	{
		aiBone* ai_bone = mesh->mBones[boneIdx];
		const unsigned int numWeights = mesh->mBones[boneIdx]->mNumWeights;
		for (unsigned int j = 0; j < numWeights; j++)
		{
			aiVertexWeight vw = ai_bone->mWeights[j];
			const unsigned int v_idx = vw.mVertexId;
			const float v_w = vw.mWeight;
			bonemapping[v_idx].emplace_back(std::make_tuple(boneIdx, v_w));
		}
	}


	for (Joint& joint : bones)
	{
		auto& children = boneMapping2[joint.m_Name];
		for (auto& [nm, it] : children)
			joint.m_ChildrenIndices.push_back(it);
	}

	auto stride1 = m_VertexBufferLayout.GetStride() / 4;
	auto boneIndexElem = m_VertexBufferLayout.GetElement(VertexType::BONE_INDEX);
	auto subStride = boneIndexElem.vertexIndex / 4;

	for (uint32_t i = 0; i < mesh->mNumVertices; i++)
	{
		auto& as = bonemapping[i];
		for (uint32_t j = 0; j < as.size(); j++)
		{
			auto& [boneIdx, weight] = as[j];
			uint32_t idxB = stride1 * i + subStride + j; //m_VertexBufferLayout.GetElementIndex(i, j, VertexType::BONE_INDEX);
			uint32_t idxW = idxB + boneIndexElem.count;

			m_Vertices[idxB] = boneIdx;
			m_Vertices[idxW] = weight;

		}
	}

}
void Mesh::LoadMaterialTextures(
	const aiMaterial* material,
	const aiTextureType type,
	const std::string& typeName,
	const aiScene* scene)
{
	//aiMaterial* material = scene->mMaterials[0];
	bool isEmbedded = false;
	const unsigned int texCount = material->GetTextureCount(type);

	for (unsigned int i = 0; i < texCount; i++)
	{
		aiString texture_file;
		aiReturn ret = material->Get(AI_MATKEY_TEXTURE(type, i), texture_file);
		if (ret != aiReturn_SUCCESS) continue;

		if (const aiTexture* texture = scene->GetEmbeddedTexture(texture_file.C_Str()))
		{
			//returned pointer is not null, read texture from memory
			isEmbedded = true;
			if (texture->mHeight == 0)
			{
				m_Textures.emplace_back(texture, typeName);
			}
			else
			{
				fmt::print("Texture is embedded but height is not 0!");
				//image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth * texture->mHeight, &width, &height, &components_per_pixel, 0);
				//m_Textures.emplace_back(fullpath, typeName);
			}
		}
	}


	//regular file, check if it exists and read it

	if (!isEmbedded)
	{

		for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
		{
			aiString fileName;
			auto r = material->GetTexture(type, i, &fileName);
			// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			bool skip = false;
			for (auto& j : m_Textures)
			{
				if (std::strcmp(j.GetPath().data(), fileName.C_Str()) == 0)
				{
					//textures.push_back(j);
					skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
					break;
				}
			}
			if (!skip)
			{   // if texture hasn't been loaded already, load it
				const std::string fullpath = m_Directory + '/' + fileName.C_Str();
				m_Textures.emplace_back(fullpath, typeName);
			}

		}
	}
}

void Mesh::LoadAnimations(const aiScene* scene, const aiMesh* mesh, const std::unordered_map<std::string, unsigned>& pairs, const std::vector<Joint>& bones)
{
	if (scene->HasAnimations())
	{
		Animator animator;
		animator.m_Bones.resize(bones.size());
		std::copy(bones.begin(), bones.end(), animator.m_Bones.begin());
		std::unordered_map<std::string, uint32_t> boneNames;	//todo: this has already been done b4 but whatever
		for (Joint& j : animator.m_Bones)
		{
			j.m_PoseTransform = glm::mat4(1.0f);
			boneNames[j.m_Name] = j.m_Index;
		}
		for (unsigned int i = 0; i < scene->mNumAnimations; i++)
		{
			aiAnimation* anim = scene->mAnimations[i];
			const std::string anim_name = anim->mName.C_Str();
			float ticks = 1;	//;
			float duration = static_cast<float>(anim->mDuration);
			animator.m_Duration = duration;
			animator.m_Ticks = ticks;
			double test = static_cast<double>(animator.m_Ticks) / 1000.0;
			double test2 = anim->mTicksPerSecond;


			std::vector<AnimationChannel> AnimationChannels;
			AnimationChannels.resize(anim->mNumChannels); //resize to sync with bones

			for (uint32_t j = 0; j < anim->mNumChannels; j++)
			{
				const aiNodeAnim* ai_channel = anim->mChannels[j];
				const std::string channelName = ai_channel->mNodeName.C_Str();

				std::vector<PositionKey> positionkeys;
				std::vector<RotationKey> rotationKeys;
				std::vector<ScaleKey> scalingKeys;

				for (uint32_t k = 0; k < ai_channel->mNumPositionKeys; k++)
				{
					aiVectorKey ai_key = ai_channel->mPositionKeys[k];
					positionkeys.emplace_back(ai_key.mTime, ai_key.mValue.x, ai_key.mValue.y, ai_key.mValue.z);
				}

				for (uint32_t k = 0; k < ai_channel->mNumRotationKeys; k++)
				{
					aiQuatKey ai_key = ai_channel->mRotationKeys[k];
					rotationKeys.emplace_back(ai_key.mTime, ai_key.mValue.w, ai_key.mValue.x, ai_key.mValue.y, ai_key.mValue.z);
				}

				for (uint32_t k = 0; k < ai_channel->mNumScalingKeys; k++)
				{
					aiVectorKey ai_key = ai_channel->mScalingKeys[k];
					scalingKeys.emplace_back(ai_key.mTime, ai_key.mValue.x, ai_key.mValue.y, ai_key.mValue.z);
				}

				//AnimationChannels.emplace_back(AnimationChannel(channelName, positionkeys, rotationKeys, scalingKeys));
				unsigned int channel_idx = boneNames[channelName];
				AnimationChannels[channel_idx] = AnimationChannel(channelName, positionkeys, rotationKeys, scalingKeys);

			}

			Animation animation(duration, AnimationChannels);
			animator.current = animation; //TODO add multiple animations!
		}

		m_animator = animator;
		m_animator.m_inverse_root = glm::mat4(1.0f);
		m_AnimationLoaded = true;
	}
}


Mesh::Mesh(
	const aiMesh* mesh,
	const aiScene* scene,
	const std::string& directory)
{
	m_Directory = directory;
	constexpr unsigned int BONESPERVERTEX = 4;

	//if (scene->HasTextures())
	//{
	//	
	//	for (int i = 0; i < scene->mNumTextures; i++)
	//	{
	//		scene->HasMaterials()
	//		//scene->mMaterials[0]->GetTexture()
	//		const aiTexture* texture = scene->GetEmbeddedTexture("*" + i);
	//		texture->pcData;
	//		
	//		if (texture->mHeight == 0)
	//		{
	//			//auto image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth, &width, &height, &components_per_pixel, 0);
	//		}
	//	}
	//}

	//VertexBufferLayout vbLayout;
	unsigned int stride = 0;
	//this->m_VertexBufferLayout
	const bool hasPositions = mesh->HasPositions();
	if (hasPositions) m_VertexBufferLayout.Push<float>(3, VertexType::POSITION);

	const bool hasNormals = mesh->HasNormals();
	if (hasNormals)
		m_VertexBufferLayout.Push<float>(3, VertexType::NORMAL);

	const bool hasTexCoords = mesh->HasTextureCoords(0);
	if (hasTexCoords) m_VertexBufferLayout.Push<float>(2, VertexType::TEXCOORD);

	const bool hasTangents = mesh->HasTangentsAndBitangents();
	if (hasTangents)
	{
		m_VertexBufferLayout.Push<float>(3, VertexType::TANGENT);
		m_VertexBufferLayout.Push<float>(3, VertexType::BITANGENT);
	}

	//TODO colors
	bool colors = mesh->HasVertexColors(1);

	const bool hasBones = mesh->HasBones();
	if (hasBones)
	{
		m_VertexBufferLayout.Push<float>(BONESPERVERTEX, VertexType::BONE_INDEX);	//bone idx
		m_VertexBufferLayout.Push<float>(BONESPERVERTEX, VertexType::BONE_WEIGHT);	//bone weight
	}

	const unsigned int morphTargetCount = mesh->mNumAnimMeshes;

	if (morphTargetCount)
	{
		for (unsigned int i = 0; i < morphTargetCount; i++)
		{
			aiAnimMesh* morph = mesh->mAnimMeshes[i];
			if (morph->HasPositions())
				m_VertexBufferLayout.Push<float>(3, VertexType::POSITION);

			if (morph->mNumVertices != mesh->mNumVertices)
				std::cout << " morph target vertex m_Count != original vertex m_Count!";
		}
	}

	//finally create vertex buffer
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		m_Vertices.push_back(mesh->mVertices[i].x);
		m_Vertices.push_back(mesh->mVertices[i].y);
		m_Vertices.push_back(mesh->mVertices[i].z);

		if (hasNormals)
		{
			m_Vertices.push_back(mesh->mNormals[i].x);
			m_Vertices.push_back(mesh->mNormals[i].y);
			m_Vertices.push_back(mesh->mNormals[i].z);
		}

		if (hasTexCoords)
		{
			m_Vertices.push_back(mesh->mTextureCoords[0][i].x);
			m_Vertices.push_back(mesh->mTextureCoords[0][i].y);
		}
		//manage morph animation by interleaving
		for (unsigned int j = 0; j < morphTargetCount; j++)
		{
			aiAnimMesh* morph = mesh->mAnimMeshes[j];
			//TODO: check for each vertex element to see if they are in animation
			if (morph->HasPositions())
				m_Vertices.push_back(morph->mVertices[i].x);
			m_Vertices.push_back(morph->mVertices[i].y);
			m_Vertices.push_back(morph->mVertices[i].z);
		}

		if (hasTangents)
		{
			m_Vertices.push_back(mesh->mTangents[i].x);
			m_Vertices.push_back(mesh->mTangents[i].y);
			m_Vertices.push_back(mesh->mTangents[i].z);
			// bitangemplace_back( 
			m_Vertices.push_back(mesh->mBitangents[i].x);
			m_Vertices.push_back(mesh->mBitangents[i].y);
			m_Vertices.push_back(mesh->mBitangents[i].z);
		}
		if (hasBones)
		{
			for (uint32_t b = 0; b < BONESPERVERTEX; b++)
			{
				m_Vertices.push_back(0);
				m_Vertices.push_back(0);
			}
		}

	}

	// now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex m_Indices.
	for (uint32_t i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all m_Indices of the face and store them in the m_Indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			m_Indices.push_back(face.mIndices[j]);
		}
	}



	if (hasTexCoords)
	{
		const uint32_t indxSize = m_Indices.size() / 3;
		//for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		for (uint32_t i = 0; i < indxSize; i++)
		{
			auto a = i * 3;
			auto b = i * 3 + 1u;
			auto c = i * 3 + 2u;

			uint32_t idx1 = m_Indices[a];
			uint32_t idx2 = m_Indices[b];
			uint32_t idx3 = m_Indices[c];

			float u1 = mesh->mTextureCoords[0][idx1].x;
			float v1 = mesh->mTextureCoords[0][idx1].y;
			glm::vec2 uv1(u1, v1);

			float u2 = mesh->mTextureCoords[0][idx2].x;
			float v2 = mesh->mTextureCoords[0][idx2].y;
			glm::vec2 uv2(u2, v2);

			float u3 = mesh->mTextureCoords[0][idx3].x;
			float v3 = mesh->mTextureCoords[0][idx3].y;
			glm::vec2 uv3(u3, v3);
			//std::array< glm::vec2, 3>> face = {uv1, uv2, uv3};
			std::array< glm::vec2, 3> face = { uv1, uv2, uv3 };
			m_UVs.emplace_back(face);

		}
	}

	//TODO: fix! animator.m_inverse_root = m_inverseRoot;
	if (hasBones)
	{
		std::unordered_map<std::string, unsigned int> boneNames;
		std::vector<Joint> bones;

		LoadBoneData(scene, mesh, boneNames, bones);
		LoadAnimations(scene, mesh, boneNames, bones);
	}

	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	auto a = material->GetTextureCount(aiTextureType_DIFFUSE);
	auto b = material->GetTextureCount(aiTextureType_SPECULAR);
	auto e = material->GetTextureCount(aiTextureType_AMBIENT);
	auto k = material->GetTextureCount(aiTextureType_EMISSIVE);
	auto c = material->GetTextureCount(aiTextureType_HEIGHT);
	auto i = material->GetTextureCount(aiTextureType_NORMALS);
	auto o = material->GetTextureCount(aiTextureType_SHININESS);
	auto p = material->GetTextureCount(aiTextureType_OPACITY);
	auto paa = material->GetTextureCount(aiTextureType_DISPLACEMENT);
	auto zzp = material->GetTextureCount(aiTextureType_LIGHTMAP);
	auto f = material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION);
	auto m = material->GetTextureCount(aiTextureType_REFLECTION);

	auto rg = material->GetTextureCount(aiTextureType_BASE_COLOR);
	auto s = material->GetTextureCount(aiTextureType_NORMAL_CAMERA);
	auto t = material->GetTextureCount(aiTextureType_EMISSION_COLOR);
	auto u = material->GetTextureCount(aiTextureType_METALNESS);
	auto v = material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS);
	//auto g = material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS);
	//auto h = material->GetTextureCount(aiTextureType_METALNESS);
	//auto j = material->GetTextureCount(aiTextureType_EMISSION_COLOR);

	auto aal = material->GetTextureCount(aiTextureType_UNKNOWN);
	auto pss = material->GetTextureCount(aiTextureType_NONE);

	LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
	LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", scene);
	LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height", scene);
	LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_ambient", scene);
	LoadMaterialTextures(material, aiTextureType_EMISSIVE, "texture_emissive", scene);
	LoadMaterialTextures(material, aiTextureType_NORMALS, "texture_normals", scene);
	LoadMaterialTextures(material, aiTextureType_LIGHTMAP, "texture_lightmap", scene);
	LoadMaterialTextures(material, aiTextureType_AMBIENT_OCCLUSION, "texture_ambient_occlusion", scene);
	LoadMaterialTextures(material, aiTextureType_REFLECTION, "texture_reflection", scene);

	LoadMaterialTextures(material, aiTextureType_BASE_COLOR, "texture_base_color", scene);
	LoadMaterialTextures(material, aiTextureType_NORMAL_CAMERA, "texture_normal_camera", scene);
	LoadMaterialTextures(material, aiTextureType_EMISSION_COLOR, "texture_emission_color", scene);
	LoadMaterialTextures(material, aiTextureType_METALNESS, "texture_metalness", scene);
	LoadMaterialTextures(material, aiTextureType_DIFFUSE_ROUGHNESS, "texture_diffuse_roughness", scene);
	LoadMaterialTextures(material, aiTextureType_UNKNOWN, "texture_unknown", scene);
	LoadMaterialTextures(material, aiTextureType_NONE, "texture_none", scene);


	aiAABB  ai_aabb = mesh->mAABB;

	m_aabb = AABB({ ai_aabb.mMin.x, ai_aabb.mMin.y, ai_aabb.mMin.z },
		{ ai_aabb.mMax.x, ai_aabb.mMax.y, ai_aabb.mMax.z });
	m_aabb_OG = m_aabb;

	//for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	//	meshnew.m_PositionVertices.emplace_back(
	//		glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
	for (const int v_idx : m_Indices)
	{
		auto& ai_v = mesh->mVertices[v_idx];
		glm::vec4 v{ ai_v.x, ai_v.y, ai_v.z, 0 }; //added 0 for padding with gpu
		m_PositionVertices.emplace_back(v);

	}

	CreateBuffers();
}

void Mesh::Draw(const Camera& camera, const glm::mat4& transform, Shader& shader)
{
	shader.Bind();
	//shader.SetInt("texture_diffuse1", 0);
	shader.SetUniformMat4f("u_Model", transform);
	shader.SetUniformMat4f("u_View", camera.GetViewMatrix());
	shader.SetUniformMat4f("u_Projection", camera.GetProjectionMatrix());
	shader.SetVec4f("u_Color", 1, 0, 0, 1);

	Draw(shader);
}

//for skybox
void Mesh::Draw(const glm::mat4& proj, const glm::mat4& view, Shader& shader)
{
	shader.Bind();
	shader.SetUniformMat4f("u_View", view);
	shader.SetUniformMat4f("u_Projection", proj);

	Draw(shader);
}

void Mesh::Draw(Shader& shader)
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;

	for (size_t i = 0; i < m_Textures.size(); i++)
	{
		const Texture2D& tex = m_Textures[i];
		glActiveTexture(GL_TEXTURE0 + i);

		std::string number;
		std::string name = tex.GetType();
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++);	// transfer unsigned int to stream
		else if (name == "texture_normal")
			number = std::to_string(normalNr++);	// transfer unsigned int to stream
		else if (name == "texture_height")
			number = std::to_string(heightNr++);	// transfer unsigned int to stream
				// now set the sampler to the correct texture unit
		shader.SetInt(name + number, i);
		//glUniform1i(glGetUniformLocation(shader.m_RendererID, (name + number).c_str()), i));
		// and finally bind the texture
		GLenum type = tex.GetGLType();
		glBindTexture(type, tex.GetID());
	}
	glBindVertexArray(m_VAO);

	if (!m_animator.m_Bones.empty())
	{
		std::vector<glm::mat4> boneMatrices;
		for (auto& m : m_animator.m_Bones)	//TODO: cache this at an animator or somsething
			boneMatrices.emplace_back(m.m_PoseTransform);
		//boneMatrices.emplace_back(glm::mat4(1.0f));

		const auto idx = shader.GetUniformLocation("mBones[0]");
		glUniformMatrix4fv(static_cast<GLint>(idx), static_cast<GLsizei>(20), GL_FALSE, reinterpret_cast<const GLfloat*>(&boneMatrices[0])); // Passing 20 matrices
	}
	auto elemtype = GetElemDrawType();

	// draw mesh
	if (!m_Indices.empty())
	{
		glDrawElements(elemtype, static_cast<GLsizei>(m_Indices.size()), GL_UNSIGNED_INT, nullptr);
	}
	else
	{
		auto tricount = static_cast<GLsizei>(GetTriangleCount());
		glDrawArrays(elemtype, 0, tricount);   //plane!
	}

	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::DrawInstanced(int instanceCount)
{
	//TODO: add binding for correct ssbo interface
	glBindVertexArray(m_VAO);
	const GLsizei indxSize = static_cast<GLsizei>(m_Indices.size());
	glDrawElementsInstanced(GL_LINES, indxSize, GL_UNSIGNED_INT, nullptr, instanceCount);

	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);

}

bool Mesh::HasAnimation() const
{
	return m_AnimationLoaded;
}

const uint32_t Mesh::GetVAO() const
{
	return m_VAO;
}

const uint32_t Mesh::GetVBO() const
{
	return m_VBO;
}

const uint32_t Mesh::GetEBO() const
{
	return m_EBO;
}

Mesh::Mesh(
	const std::vector<float>& vertices,
	const std::vector<unsigned>& indices,
	const std::vector<Texture2D>& textures,
	const Animator& animator,
	const VertexBufferLayout& vbl)
{
	this->m_Vertices = vertices;
	this->m_Indices = indices;
	this->m_Textures = textures;
	this->m_animator = animator;
	this->m_VertexBufferLayout = vbl;

	//get aabb
	//if (!m_PositionVertices.empty())
	//	for (int i = 0; i < m_Vertices.size(); i += vbl.GetStride())
	//	{
	//		m_PositionVertices.emplace_back(glm::vec3{ m_Vertices[i], m_Vertices[i + 1], m_Vertices[i + 2] });
	//		//m_aabb.CalcBounds(m_PositionVertices);
	//	}

	CreateBuffers();
}

Mesh::Mesh(const std::vector<float>& p_vertices, const VertexBufferLayout& vbl)
{
	m_Vertices = p_vertices;
	m_VertexBufferLayout = vbl;
	CreateBuffers();
}

Mesh::~Mesh()
{
	fmt::print("Mesh {0} deconstructed!\n", m_Directory);
}

Mesh::Mesh(std::vector<float>& vertices, std::vector<unsigned int>& indices, const VertexBufferLayout& vbl)
{
	m_Vertices = vertices;
	m_Indices = indices;
	m_VertexBufferLayout = vbl;
	CreateBuffers();
}

void Mesh::CreateBuffers()
{
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);

	const GLsizeiptr bufferSize = sizeof(m_Vertices[0]) * m_Vertices.size();
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, bufferSize, &m_Vertices[0], GL_STATIC_DRAW);

	if (!m_Indices.empty())
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);
	}

	unsigned int i = 0;
	const unsigned int stride = m_VertexBufferLayout.GetStride();
	for (const VertexBufferElement& vbe : m_VertexBufferLayout.GetElements())
	{
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, vbe.count, vbe.type, vbe.normalized, stride,
			reinterpret_cast<const void*>(vbe.vertexIndex));
		i++;
	}
	//glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

Mesh Mesh::CreateCubeWireframe()
{
	Mesh mesh;
	mesh.SetElemDrawType(GL_LINES);

	float cubeVertices[] = {
		//front face
		-0.5f, -0.5f, -0.5f, //ll-          
		0.5f, -0.5f, -0.5f,	 //lr-
		0.5f, -0.5f, -0.5f,	//lr-
		0.5f,  0.5f, -0.5f,	//tr-
		0.5f,  0.5f, -0.5f, //tr-
		-0.5f,  0.5f, -0.5f, //tl-
		-0.5f,  0.5f, -0.5f, //tl-
		-0.5f, -0.5f, -0.5f, //ll-

		//back face
		-0.5f, -0.5f, 0.5f,   //ll+          
		0.5f, -0.5f,  0.5f,	   //lr+
		0.5f, -0.5f,  0.5f,	   //tr+
		0.5f,  0.5f,  0.5f,	   //tr+
		0.5f,  0.5f,  0.5f,    //lr+
		-0.5f,  0.5f, 0.5f,   //tl+
		-0.5f,  0.5f, 0.5f,   //tl+
		-0.5f, -0.5f, 0.5f,   //ll+

		//connecting edges
		 -0.5f, -0.5f,  -0.5f,	//ll-
		 -0.5f, -0.5f,  0.5f,	//ll+
		 0.5f, -0.5f, -0.5f,		//lr-
		 0.5f, -0.5f,  0.5f,		//lr+
		 0.5f,  0.5f, -0.5f,		//tr-
		 0.5f,  0.5f, 0.5f,		//tr+
		 -0.5f,  0.5f,  -0.5f,	//tl-
		 -0.5f,  0.5f,  0.5f		//tl+
	}; //8*3*3

	const int size = sizeof(cubeVertices) / sizeof(float);
	std::copy(cubeVertices, cubeVertices + size, std::back_inserter(mesh.m_Vertices));

	VertexBufferLayout vbl;
	vbl.Push<float>(3, VertexType::POSITION);

	//for (int i = 0; i < size; i += 3)
	//{
	//	mesh.m_PositionVertices.emplace_back(
	//		glm::vec3(cubeVertices[i], cubeVertices[i + 1], cubeVertices[i + 2]));
	//}

	//AABB aabb;
	//mesh.m_aabb.CalcBounds(mesh.m_PositionVertices);
	mesh.m_aabb_OG = mesh.m_aabb;

	mesh.m_VertexBufferLayout = vbl;

	// cube m_VAO
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.m_Vertices.size(), &mesh.m_Vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)nullptr);

	mesh.m_VAO = cubeVAO;
	mesh.m_VBO = cubeVBO;

	return mesh;
}

GLenum Mesh::GetElemDrawType() const
{
	return m_ElemDrawType;
}

void Mesh::SetElemDrawType(const GLenum enm)
{
	m_ElemDrawType = enm;
}

Mesh Mesh::CreateCube()
{
	Mesh mesh;
	return mesh;
}


bool Mesh::HasFaceIndices() const
{
	return !m_Indices.empty();
}

const uint32_t Mesh::GetVertexCount() const
{
	return static_cast<unsigned int>(m_Vertices.size());
}

const uint32_t Mesh::GetTriangleCount() const
{
	const auto t = GetVertexCount();
	const auto stride = m_VertexBufferLayout.GetStride();
	const auto strideBytes = stride / sizeof(float);
	const auto triCount = t / strideBytes;
	return triCount;
}

void Mesh::AddTexture(const Texture2D& tex)
{
	m_Textures.emplace_back(tex);
}

//void Mesh::MakeWireFrame()
//{
//	if (m_PositionVertices.empty()) { std::cout << "no position m_Vertices!" << std::endl; return; }
//
//	glm::vec3 A(1, 0, 0);
//	glm::vec3 B(0, 1, 0);
//	glm::vec3 C(0, 0, 1);
//	glm::vec3 bary[] = { A, B, C };
//	std::vector<glm::vec3> newBuffer;
//	int barycentricIdx = 0;
//	for (auto& v : m_PositionVertices)
//	{
//		//this->m_Indices
//		//newBuffer.emplace_back(v);
//		//newBuffer.emplace_back(bary[barycentricIdx]);
//		//barycentricIdx = (barycentricIdx + 1) % 3;
//	}
//
//	unsigned int wireVAO, wireVBO;
//	glGenVertexArrays(1, &wireVAO);
//	glGenBuffers(1, &wireVBO);
//	glBindVertexArray(wireVAO);
//	glBindBuffer(GL_ARRAY_BUFFER, wireVBO);
//	glBufferData(GL_ARRAY_BUFFER, newBuffer.size() * sizeof(glm::vec3), &newBuffer[0], GL_STATIC_DRAW);
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
//
//	m_WireVAO = wireVAO;
//	m_WireVBO = wireVBO;
//
//}

void Mesh::DrawWireFrame(const Camera& camera, const MeshComponent& mc, const glm::mat4& modelMatrix) const
{
	Shader& shader = ShaderManager::GetShader("wireframeGS");
	shader.Bind();
	const uint32_t tris = GetTriangleCount();

	shader.SetFloat("u_Thickness", mc.WireframeThickness);
	shader.SetVec4f("u_Color", mc.WireFrameColor);
	shader.SetUniformMat4f("u_Model", modelMatrix);
	shader.SetUniformMat4f("u_View", camera.GetViewMatrix());
	shader.SetUniformMat4f("u_Projection", camera.GetProjectionMatrix());

	glBindVertexArray(m_VAO);
	//for (auto& vblElem : m_VertexBufferLayout.GetElements())
	//{
	//	if (vblElem.vertexType != VertexType::POSITION)
	//	{
	//		auto strideIndex = vblElem.vertexIndex;
	//		auto idx = strideIndex / stride;
	//		toDisable.push_back(i);
	//	}
	//
	//	i++;
	//}

	//for (uint32_t id : toDisable)
	//	glDisableVertexArrayAttrib(0,id);
	//glDisablevertexAttrib
	const unsigned int stride = m_VertexBufferLayout.GetStride();
	auto  elements = m_VertexBufferLayout.GetElements();
	auto count = elements.size();
	int newStride = 3 * 4;
	for (int i = 1; i < count; ++i)
	{
		//auto& vbe = elements[i];
		//if (vbe.vertexType == VertexType::POSITION)
		glDisableVertexAttribArray(i);
		//else glDisableVertexAttribArray(i);
		//
		//glVertexAttribPointer(i, vbe.count, vbe.type, vbe.normalized, newStride,
		//	reinterpret_cast<const void*>(vbe.vertexIndex));
		//i++;
	}

	//glBindVertexArray(0);

   //glBindVertexArray(m_VAO);

	if (!m_Indices.empty()) glDrawElements(GetElemDrawType(), static_cast<GLsizei>(m_Indices.size()), GL_UNSIGNED_INT, nullptr);
	else glDrawArrays(GetElemDrawType(), 0, static_cast<GLsizei>(GetTriangleCount())); //plane!

	 //for (int i = 0; i < count; ++i)
	 //{
	 //	auto& vbe = elements[i];
	 //	if (vbe.vertexType == VertexType::POSITION);
	 //	glEnableVertexAttribArray(i);
	 //	glVertexAttribPointer(i, vbe.count, vbe.type, vbe.normalized, stride,
	 //		reinterpret_cast<const void*>(vbe.vertexIndex));
	 //	i++;
	 //}
	//  for (uint32_t id : toDisable)
		//glEnableVertexAttribArray(id);
	//	glEnableVertexArrayAttrib(0,id);
	for (int i = 0; i < count; ++i)
	{
		//auto& vbe = elements[i];
		//if (vbe.vertexType == VertexType::POSITION)
		glEnableVertexAttribArray(i);
	}
	glBindVertexArray(0);
	shader.Unbind();
}

void Mesh::DrawNormals(const Camera& camera, const glm::mat4& modelMatrix, const glm::vec4& color, float magnitude)
{
	Shader& shader = ShaderManager::GetShader("normalsGS");
	shader.Bind();
	const uint32_t tris = GetTriangleCount();

	shader.SetUniformMat4f("u_Model", modelMatrix);
	shader.SetUniformMat4f("u_View", camera.GetViewMatrix());
	shader.SetUniformMat4f("u_Projection", camera.GetProjectionMatrix());
	shader.SetVec4f("u_Color", color);
	shader.SetFloat("u_Magnitude", magnitude);


	glBindVertexArray(m_VAO);
	int i = 0;
	auto stride = m_VertexBufferLayout.GetStride();

	std::vector<uint32_t> toDisable;

	for (auto& vblElem : m_VertexBufferLayout.GetElements())
	{
		if (vblElem.vertexType != VertexType::POSITION &&
			vblElem.vertexType != VertexType::NORMAL)
		{
			auto strideIndex = vblElem.vertexIndex;
			auto idx = strideIndex / stride;
			toDisable.push_back(i);
		}

		i++;
	}

	for (uint32_t id : toDisable)
		glDisableVertexAttribArray(id);

	if (!m_Indices.empty()) glDrawElements(GetElemDrawType(), static_cast<GLsizei>(m_Indices.size()), GL_UNSIGNED_INT, nullptr);
	else glDrawArrays(GetElemDrawType(), 0, static_cast<GLsizei>(GetTriangleCount())); //plane!

	for (uint32_t id : toDisable)
		glEnableVertexAttribArray(id);

	glBindVertexArray(0);

	shader.Unbind();
}


Mesh Mesh::CreatePlane() {
	Mesh mesh;
	return mesh;
}
// 	float planeVertices[] = {
//		// positions          // texture Coords 
//		 5.0f, -0.5f,  5.0f,  1.0f, 0.0f,
//		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
//		-5.0f, -0.5f, -5.0f,  0.0f, 1.0f,
//
//		 5.0f, -0.5f,  5.0f,  1.0f, 0.0f,
//		-5.0f, -0.5f, -5.0f,  0.0f, 1.0f,
//		 5.0f, -0.5f, -5.0f,  1.0f, 1.0f
//	};
//
//	std::copy(planeVertices, planeVertices + 30, std::back_inserter(mesh.m_Vertices));
//
//	for (int i = 0; i < mesh.m_Vertices.size(); i += 5)
//	{
//		const glm::vec3 v(mesh.m_Vertices[i], mesh.m_Vertices[i + 1], mesh.m_Vertices[i + 2]);
//	//	mesh.m_PositionVertices.emplace_back(v);
//	}
//
//	//mesh.m_aabb.CalcBounds(mesh.m_PositionVertices);
//	mesh.m_aabb_OG = mesh.m_aabb;
//
//	VertexBufferLayout vbl;
//	vbl.Push<float>(3, VertexType::POSITION);
//	vbl.Push<float>(2, VertexType::TEXCOORD);
//	mesh.m_VertexBufferLayout = vbl;
//
//	unsigned int planeVAO, planeVBO;
//	glGenVertexArrays(1, &planeVAO);
//	glGenBuffers(1, &planeVBO);
//	glBindVertexArray(planeVAO);
//	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.m_Vertices.size(), &mesh.m_Vertices[0], GL_STATIC_DRAW);
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), static_cast<void*>(nullptr));
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
//
//	mesh.m_VBO = planeVBO;
//	mesh.m_VAO = planeVAO;
//
//	return mesh;
//}
