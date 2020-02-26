#include "precomp.h"
#include <glm/gtc/type_ptr.hpp>

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

Model::Model(const std::string& path, const aiPostProcessSteps loadFlags) :
	model(glm::mat4(1.0f)), meshes(), directory(""), textures_loaded(), shaderIdx(0)
{
	loadModel(path, loadFlags);
}

void Model::SetShader(const std::string& shadername)
{
	shaderIdx = ShaderManager::GetShaderIdx(shadername);
}


void CreateHierarchy(std::shared_ptr<Model::Armature> parentArmature, aiNode* node)
{
	//Model::Armature* arma = new Model::Armature; //ptr?
	//arma.name = node->mName.C_Str();
	//arma.mat =	AI2GLMMAT(node->mTransformation);

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
		for (int i = 0; i < node->mNumChildren; i++)
			FindRootNode(node->mChildren[i], name);
	}
}

void Model::loadModel(const std::string& path, const aiPostProcessSteps LoadFlags = aiProcess_Triangulate)
{
	Assimp::Importer import;
	const auto standardFlags = aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_GenBoundingBoxes;
	const auto flagsComposed = standardFlags | LoadFlags;
	const aiScene* scene = import.ReadFile(path, flagsComposed);


	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	//aiNode* root_node = FindRootNode(scene->mRootNode, "Torso");
	armature = std::make_shared<Armature>();
	armature->name = scene->mRootNode->mName.C_Str();
	armature->mat = AI2GLMMAT(scene->mRootNode->mTransformation);
	armature->parent = nullptr;
	this->inverse_root = AI2GLMMAT(scene->mRootNode->mTransformation);

	CreateHierarchy(armature, scene->mRootNode);

	processNode(scene->mRootNode, scene, armature);
}


void Model::processNode(aiNode* node, const aiScene* scene, std::shared_ptr<Armature> armature)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene, armature));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, armature);
	}
}

void Model::UpdateModelMatrix()
{
	glm::mat4 id = glm::mat4(1.0f);
	glm::mat4 scale = glm::scale(id, m_scale);

	glm::mat4 rotationX = glm::rotate(id, glm::radians(m_rotation.x), glm::vec3{ 1,0,0 });
	glm::mat4 rotationY = glm::rotate(id, glm::radians(m_rotation.y), glm::vec3{ 0,1,0 });
	glm::mat4 rotationZ = glm::rotate(id, glm::radians(m_rotation.z), glm::vec3{ 0,0,1 });
	//glm::mat4 rotation = rotationX * rotationY * rotationZ;
	//glm::mat4 rotation = glm::orientate4(m_rotation);
	glm::mat4 rotation = glm::eulerAngleYXZ(
		glm::radians(m_rotation.y),
		glm::radians(m_rotation.x),
		glm::radians(m_rotation.z));

	glm::mat4 translation = glm::translate(id, m_position);

	model = translation * rotation * scale;

}

void Model::Update(float deltaTime)
{
	//m_rotation += glm::vec3(1, 1, 1) * 30.0f * deltaTime;

	UpdateModelMatrix();

	for (MeshNew& mesh : meshes)
	{
		double t0 = glfwGetTime();
		mesh.m_aabb.UpdateArvo(model, mesh.m_aabb_OG);
		double t1 = glfwGetTime();

		double diff = 1000 * (t1 - t0);
		std::cout << "diff ms: " << diff << "\n";



	}

}

void Model::AddWeight(
	std::vector<float>& vertices, unsigned int vertex_index, unsigned int bone_index,
	GLuint bone_id, GLfloat weight)
{
	unsigned int stride = 3 + 3 + 2 + 3 + 3 + 3 + 3;
	unsigned int idx = stride * vertex_index;
	unsigned int id_offset = 13;// elems to right *sizeof(float);
	vertices[idx + id_offset + bone_index] = bone_id;
	vertices[idx + id_offset + 3 + bone_index] = weight;
}

void FindChildren(std::shared_ptr<Model::Armature> arma, Joint& joint,
	std::unordered_map<std::string, unsigned int > bonesDict)
{
	if (arma->name == joint.GetName())
	{
		for (int i = 0; i < arma->children.size(); i++)
		{
			joint.childrenPair.emplace_back(
				std::make_pair(arma->children[i]->name, bonesDict[arma->children[i]->name]));
		}
		return;
	}
	for (int i = 0; i < arma->children.size(); i++)
		FindChildren(arma->children[i], joint, bonesDict);
}
void AssignMatrices(std::shared_ptr<Model::Armature> armature, Joint& joint)
{
	if (armature->name == joint.Name)
	{
		joint.nodeTransform = armature->mat;
		return;
	}
	else for (auto& children : armature->children)	AssignMatrices(children, joint);
}
MeshNew Model::processMesh(aiMesh* mesh, const aiScene* scene, std::shared_ptr<Armature>  armature)
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
		vbLayout.Push<float>(3, VertexType::BI_TANGENT);
	}

	//TODO colors
	bool colors = mesh->HasVertexColors(1);

	const bool hasBones = mesh->HasBones();
	if (hasBones) {
		vbLayout.Push<float>(BONESPERVERTEX, VertexType::BONE_INDEX);	//bone idx
		vbLayout.Push<float>(BONESPERVERTEX, VertexType::BONE_WEIGHT);	//bone weight
	}


	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		vertices.emplace_back(mesh->mVertices[i].x);
		vertices.emplace_back(mesh->mVertices[i].y);
		vertices.emplace_back(mesh->mVertices[i].z);


		if (hasNormals)
		{
			vertices.emplace_back(mesh->mNormals[i].x);
			vertices.emplace_back(mesh->mNormals[i].y);
			vertices.emplace_back(mesh->mNormals[i].z);
		}

		if (hasTexCoords)
		{
			vertices.emplace_back(mesh->mTextureCoords[0][i].x);
			vertices.emplace_back(mesh->mTextureCoords[0][i].y);
		}


		if (hasTangents)
		{
			vertices.emplace_back(mesh->mTangents[i].x);
			vertices.emplace_back(mesh->mTangents[i].y);
			vertices.emplace_back(mesh->mTangents[i].z);
			// bitangemplace_back( 
			vertices.emplace_back(mesh->mBitangents[i].x);
			vertices.emplace_back(mesh->mBitangents[i].y);
			vertices.emplace_back(mesh->mBitangents[i].z);
		}
		if (hasBones) {
			vertices.emplace_back(0);
			vertices.emplace_back(0);
			vertices.emplace_back(0);
			vertices.emplace_back(0);
			vertices.emplace_back(0);
			vertices.emplace_back(0);
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
	animator.m_inverse_root = inverse_root;

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

			for (int j = 0; j < numWeights; j++) {
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

		for (size_t i = 0; i < bonemapping.size(); i++) {

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

			size_t j = 0;
			for (auto& bm : bonemapping[i])
			{
				int elemidxV1 = vbLayout.GetElementIndex(i, j, VertexType::BONE_INDEX);
				int elemidxV2 = vbLayout.GetElementIndex(i, j, VertexType::BONE_WEIGHT);
				vertices[elemidxV1] = bm.first;
				vertices[elemidxV2] = bm.second;
				j++;
				if (j >= BONESPERVERTEX) break;
			}
		}
	}


	if (scene->HasAnimations()) {
		for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
			aiAnimation* anim = scene->mAnimations[i];
			const std::string anim_name = anim->mName.C_Str();
			float ticks = 1;//anim->mTicksPerSecond;
			float duration = anim->mDuration;
			animator.m_duration = duration;
			animator.m_ticks = ticks;


			std::vector<AnimationChannel> AnimationChannels;
			AnimationChannels.resize(anim->mNumChannels); //resize to sync with bones

			for (size_t j = 0; j < anim->mNumChannels; j++)
			{
				const aiNodeAnim* ai_channel = anim->mChannels[j];
				const std::string channelName = ai_channel->mNodeName.C_Str();

				std::vector<  PositionKey> positionkeys;
				std::vector< RotationKey > rotationKeys;
				std::vector< ScaleKey> scalingKeys;

				for (size_t k = 0; k < ai_channel->mNumPositionKeys; k++)
				{
					aiVectorKey ai_key = ai_channel->mPositionKeys[k];
					std::pair<float, glm::vec3> positionKey
						= { ai_key.mTime ,
						glm::vec3{ai_key.mValue.x,ai_key.mValue.y,ai_key.mValue.z } };

					positionkeys.emplace_back(positionKey);
				}

				for (size_t k = 0; k < ai_channel->mNumRotationKeys; k++)
				{
					aiQuatKey ai_key = ai_channel->mRotationKeys[k];
					std::pair<float, glm::quat> rotationKey
						= { ai_key.mTime  ,
						glm::quat{ ai_key.mValue.w, ai_key.mValue.x,ai_key.mValue.y,ai_key.mValue.z  } };

					rotationKeys.emplace_back(rotationKey);
				}

				for (size_t k = 0; k < ai_channel->mNumScalingKeys; k++)
				{
					aiVectorKey ai_key = ai_channel->mScalingKeys[k];
					std::pair<float, glm::vec3>
						scalingKey = { ai_key.mTime    ,
						glm::vec3(ai_key.mValue.x,ai_key.mValue.y,ai_key.mValue.z) };

					scalingKeys.emplace_back(scalingKey);
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
	std::vector<Texture2D> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	std::vector<Texture2D> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<Texture2D> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<Texture2D> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	// return a mesh object created from the extracted mesh data
	MeshNew meshnew(vertices, indices, textures, animator, vbLayout);

	aiAABB& ai_aabb = mesh->mAABB;

	meshnew.m_aabb = AABB({ ai_aabb.mMin.x, ai_aabb.mMin.y, ai_aabb.mMin.z },
		{ ai_aabb.mMax.x, ai_aabb.mMax.y, ai_aabb.mMax.z });
	meshnew.m_aabb_OG = meshnew.m_aabb;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		meshnew.positionVertices.emplace_back(
			glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));

	return	 meshnew;
}


std::vector<Texture2D> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type,
	const std::string& typeName)
{
	std::vector<Texture2D> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString fileName;
		auto r = mat->GetTexture(type, i, &fileName);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (auto& j : textures_loaded)
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

			const std::string fullpath = directory + '/' + fileName.C_Str();
			Texture2D texture(fullpath, typeName);

			textures.push_back(texture);
			textures_loaded.emplace_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}

void Model::Draw(const Camera& cam)
{
	//auto d = shader.get();
	GPUShader& shader = ShaderManager::GetShader(shaderIdx);
	shader.Bind();
	const unsigned int shaderID = shader.m_RendererID;

	std::vector < std::string> names;

	const glm::mat4 view = cam.GetViewMatrix();
	const glm::mat4 proj = cam.GetProjectionMatrix();

	Camera* camera = Camera::GetMain();
	glm::mat4 proj2 = camera->GetProjectionMatrix();
	glm::mat4 view2 = camera->GetViewMatrix();

	shader.SetUniformMat4f("model", model);
	shader.SetUniformMat4f("view", view);
	shader.SetUniformMat4f("projection", proj);

	for (auto& mesh : meshes)
		mesh.Draw(shader);

	shader.Unbind();

	for (auto& mesh : meshes)
		mesh.m_aabb.Draw(cam);
}

GPUShader& Model::GetShader() const { return   ShaderManager::GetShader(shaderIdx); }

Model Model::CreateCube() {
	Model model;
	MeshNew mesh = MeshNew::CreateCube();
	model.meshes.emplace_back(mesh);
	return model;
}

Model Model::CreateCubeWireframe() {
	Model model;
	MeshNew mesh = MeshNew::CreateCubeWireframe();
	model.meshes.emplace_back(mesh);
	return model;
}

Model Model::CreatePlane()
{
	Model model;
	MeshNew mesh = MeshNew::CreatePlane();

	model.meshes.emplace_back(mesh);
	return model;
}
