#include "precomp.h"
#include "Mesh.h"
#include "Rendering/Buffer/VertexBufferLayout.h"
#include "GameObject/Components/Texture2D.h"
#include "Rendering/ShaderManager.h"
#include "GameObject/Camera.h"
#include "Animation/Joint.h"
#include "Animation/Animator.h"
#include "GameObject/Components/AABB.h"

//static std::vector<Mesh> m_Meshes;
std::vector<Mesh> Mesh::m_Meshes;

void Mesh::LoadMaterialTextures(
	const aiMaterial* mat,
	const aiTextureType type,
	const std::string& typeName)
{
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString fileName;
		auto r = mat->GetTexture(type, i, &fileName);
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

void Mesh::ProcessNode(
	const aiNode* node,
	const aiScene* scene,
	const std::string& directory
/*, std::shared_ptr<Armature> armature*/)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_Meshes.emplace_back(mesh, scene, directory);// /*, armature*/));
	}

	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, directory); /*, armature*/
	}
}

std::optional<uint32_t> Mesh::LoadFromFile(const std::string& path, const aiPostProcessSteps loadFlags)
{
	//const aiMesh* mesh, const aiScene* scene, std::shared_ptr<Armature>  armature)

	Assimp::Importer import;
	const auto standardFlags = aiProcess_GenSmoothNormals | aiProcess_GenBoundingBoxes;
	const auto flagsComposed = standardFlags | loadFlags;
	const aiScene* scene = import.ReadFile(path, flagsComposed);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return std::nullopt;
	}

	std::string directory = path.substr(0, path.find_last_of('/'));

	int idx = Mesh::m_Meshes.size();
	ProcessNode(scene->mRootNode, scene, directory);
	//TODO: delete scene;
	return idx; //TODO: use std optional?
}

Mesh::Mesh(
	const aiMesh* mesh,
	const aiScene* scene,
	const std::string& directory)
{
	m_Directory = directory;
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
	if (hasBones)
	{
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
		if (hasBones)
		{
			vertices.push_back(0);
			vertices.push_back(0);
			vertices.push_back(0);
			vertices.push_back(0);
			vertices.push_back(0);
			vertices.push_back(0);
		}

	}


	// now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex m_Indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all m_Indices of the face and store them in the m_Indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	//TODO: fix! animator.m_inverse_root = m_inverseRoot;

#pragma region bones
	Animator  animator;
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

			//TODO FIX:
			//FindChildren(armature, joint, bonesDict);
			//AssignMatrices(armature, joint);

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
#pragma endregion memelord 

#pragma region anim
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
	/*std::vector<Texture2D> diffuseMaps = */ LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	//textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	/* std::vector<Texture2D> specularMaps = */ LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	//textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	/* std::vector<Texture2D> normalMaps = */ LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	//textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	/* std::vector<Texture2D> heightMaps = */  LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	//textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());


	aiAABB  ai_aabb = mesh->mAABB;

	m_aabb = AABB({ ai_aabb.mMin.x, ai_aabb.mMin.y, ai_aabb.mMin.z },
		{ ai_aabb.mMax.x, ai_aabb.mMax.y, ai_aabb.mMax.z });
	m_aabb_OG = m_aabb;

	//for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	//	meshnew.positionVertices.emplace_back(
	//		glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
	for (const int v_idx : indices)
	{
		auto& ai_v = mesh->mVertices[v_idx];
		glm::vec3   v = { ai_v.x, ai_v.y, ai_v.z };
		positionVertices.emplace_back(v);
	}
}

void Mesh::Draw(const Camera& camera, const glm::mat4& transform,  Shader& shader )
{
	shader.Bind();
	//shader.SetInt("texture_diffuse1", 0);
	shader.SetUniformMat4f("u_model", transform);
	shader.SetUniformMat4f("u_view", camera.GetViewMatrix());
	shader.SetUniformMat4f("u_projection", camera.GetProjectionMatrix());
	Draw(shader);
}

void Mesh::Draw(Shader& shader)
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;

	for (int i = 0; i < m_Textures.size(); i++)
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
		//GLCall(glUniform1i(glGetUniformLocation(shader.m_RendererID, (name + number).c_str()), i));
		// and finally bind the texture
		GLCall(glBindTexture(GL_TEXTURE_2D, tex.GetID()));
	}

	if (!m_animator.m_bones.empty()) {
		std::vector<glm::mat4> boneMatrices;
		for (auto& m : m_animator.m_bones)
			boneMatrices.emplace_back(m.m_pose_transform);

		const auto idx = shader.GetUniformLocation("mBones[0]");
		GLCall(glUniformMatrix4fv((GLint)idx, (GLsizei)20, GL_FALSE, (const GLfloat*)&boneMatrices[0])); // Passing 20 matrices
	}

	// draw mesh
	glBindVertexArray(VAO);
	if (!m_Indices.empty())
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_Indices.size()), GL_UNSIGNED_INT, nullptr);
	else glDrawArrays(GetElemDrawType(), 0, static_cast<GLsizei>(GetVertexCount())); //plane!

	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

bool Mesh::HasAnimation() const
{
	return animation_loaded;
}

unsigned Mesh::GetVAO()
{
	return VAO;
}

unsigned Mesh::GetVBO()
{
	return VBO;
}

unsigned Mesh::GetEBO()
{
	return EBO;
}

Mesh::Mesh(
	const std::vector<float>& vertices,
	const std::vector<unsigned>& indices,
	const std::vector<Texture2D>& textures,
	const Animator& animator,
	const VertexBufferLayout& vbl)
{
	this->vertices = vertices;
	this->m_Indices = indices;
	this->m_Textures = textures;
	this->m_animator = animator;
	this->m_VertexBufferLayout = vbl;

	//get aabb
	//if (!positionVertices.empty())
	//	for (int i = 0; i < vertices.size(); i += vbl.GetStride())
	//	{
	//		positionVertices.emplace_back(glm::vec3{ vertices[i], vertices[i + 1], vertices[i + 2] });
	//		//m_aabb.CalcBounds(positionVertices);
	//	}

	setupMesh();
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	const GLsizeiptr bufferSize = sizeof(vertices[0]) * vertices.size();
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, bufferSize, &vertices[0], GL_STATIC_DRAW));

	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int),
		&m_Indices[0], GL_STATIC_DRAW));

	unsigned int i = 0;
	const unsigned int stride = m_VertexBufferLayout.GetStride();
	for (const VertexBufferElement& vbe : m_VertexBufferLayout.GetElements())
	{
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, vbe.count, vbe.type, vbe.normalized, stride,
			reinterpret_cast<const void*>(vbe.vertexIndex));
		i++;
	}

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
	std::copy(cubeVertices, cubeVertices + size, std::back_inserter(mesh.vertices));

	VertexBufferLayout vbl;
	vbl.Push<float>(3, VertexType::POSITION);

	for (int i = 0; i < size; i += 3)
	{
		mesh.positionVertices.emplace_back(
			glm::vec3(cubeVertices[i], cubeVertices[i + 1], cubeVertices[i + 2]));
	}

	//AABB aabb;
	mesh.m_aabb.CalcBounds(mesh.positionVertices);
	mesh.m_aabb_OG = mesh.m_aabb;

	mesh.m_VertexBufferLayout = vbl;

	// cube VAO
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.vertices.size(), &mesh.vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)nullptr);

	mesh.VAO = cubeVAO;
	mesh.VBO = cubeVBO;

	return mesh;
}

GLenum Mesh::GetElemDrawType() const
{
	return m_elemDrawType;
}

void Mesh::SetElemDrawType(const GLenum enm)
{
	m_elemDrawType = enm;
}

Mesh Mesh::CreateCube()
{
	Mesh mesh;
	float cubeVertices[] = {
		// positions          // texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	const unsigned int size = sizeof(cubeVertices) / sizeof(float);
	std::copy(cubeVertices, cubeVertices + 6 * 5 * 6, std::back_inserter(mesh.vertices));

	for (int i = 0; i < mesh.vertices.size(); i += 5)
		mesh.positionVertices.emplace_back(glm::vec3(
			mesh.vertices[i], mesh.vertices[i + 1], mesh.vertices[i + 2]));

	mesh.m_aabb.CalcBounds(mesh.positionVertices);
	mesh.m_aabb_OG = mesh.m_aabb;

	VertexBufferLayout vbl;
	vbl.Push<float>(3, VertexType::POSITION);
	vbl.Push<float>(2, VertexType::TEXCOORD);

	mesh.m_VertexBufferLayout = vbl;
	// cube VAO
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.vertices.size(), &mesh.vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	mesh.VAO = cubeVAO;
	mesh.VAO = cubeVAO;

	return mesh;
}

bool Mesh::HasFaceIndices() const
{
	return m_Indices.size() > 0;
}

unsigned Mesh::GetVertexCount() const
{
	return static_cast<unsigned int>(vertices.size());
}

void Mesh::AddTexture(const Texture2D& tex)
{
	m_Textures.emplace_back(tex);
}

void Mesh::MakeWireFrame()
{
	if (positionVertices.empty()) { std::cout << "no position vertices!" << std::endl; return; }

	glm::vec3 A(1, 0, 0);
	glm::vec3 B(0, 1, 0);
	glm::vec3 C(0, 0, 1);
	glm::vec3 bary[] = { A, B, C };
	std::vector<glm::vec3> newBuffer;
	int barycentricIdx = 0;
	for (auto& v : positionVertices)
	{
		//this->m_Indices
		newBuffer.emplace_back(v);
		newBuffer.emplace_back(bary[barycentricIdx]);
		barycentricIdx = (barycentricIdx + 1) % 3;
	}

	unsigned int wireVAO, wireVBO;
	glGenVertexArrays(1, &wireVAO);
	glGenBuffers(1, &wireVBO);
	glBindVertexArray(wireVAO);
	glBindBuffer(GL_ARRAY_BUFFER, wireVBO);
	glBufferData(GL_ARRAY_BUFFER, newBuffer.size() * sizeof(glm::vec3), &newBuffer[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	m_wireVAO = wireVAO;
	m_wireVBO = wireVBO;




}

void Mesh::DrawWireFrame(const Camera& camera, const glm::mat4& model_matrix) const
{
	glm::mat4 model = model_matrix;
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = camera.GetProjectionMatrix();

	auto& shader = ShaderManager::GetShader("wireframe");
	shader.Bind();
	shader.SetFloat("lineThickness", lineThickness);
	shader.SetUniformMat4f("model", model);
	shader.SetUniformMat4f("view", view);
	shader.SetUniformMat4f("projection", projection);

	glBindVertexArray(m_wireVAO);

	GLCall(glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(positionVertices.size())));
}

Mesh Mesh::CreatePlane() {
	Mesh mesh;
	float planeVertices[] = {
		// positions          // texture Coords 
		 5.0f, -0.5f,  5.0f,  1.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 1.0f,

		 5.0f, -0.5f,  5.0f,  1.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 1.0f,
		 5.0f, -0.5f, -5.0f,  1.0f, 1.0f
	};

	std::copy(planeVertices, planeVertices + 30, std::back_inserter(mesh.vertices));

	for (int i = 0; i < mesh.vertices.size(); i += 5)
	{
		const glm::vec3 v(mesh.vertices[i], mesh.vertices[i + 1], mesh.vertices[i + 2]);
		mesh.positionVertices.emplace_back(v);
	}

	mesh.m_aabb.CalcBounds(mesh.positionVertices);
	mesh.m_aabb_OG = mesh.m_aabb;

	VertexBufferLayout vbl;
	vbl.Push<float>(3, VertexType::POSITION);
	vbl.Push<float>(2, VertexType::TEXCOORD);
	mesh.m_VertexBufferLayout = vbl;

	unsigned int planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.vertices.size(), &mesh.vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));

	mesh.VBO = planeVBO;
	mesh.VAO = planeVAO;

	return mesh;
}