#include "precomp.h"
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

void Model::SetShader(const std::string& shadername)
{
	shaderIdx = ShaderManager::getShaderIdx(shadername);
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

void Model::loadModel(const std::string& path)
{
	Assimp::Importer import;
	const auto flags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace;
	const aiScene* scene = import.ReadFile(path, flags);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	std::shared_ptr <Armature > armature = std::make_shared<Armature>();
	armature->name = scene->mRootNode->mName.C_Str();
	armature->mat = AI2GLMMAT(scene->mRootNode->mTransformation);
	armature->parent = nullptr;

	CreateHierarchy(armature, scene->mRootNode);

	processNode(scene->mRootNode, scene, armature);
}


void Model::processNode(aiNode* node, const aiScene* scene, std::shared_ptr<Armature> armature)
{
	//auto mat = node->mTransformation;
	//auto glmmat = AI2GLMMAT(mat);
	//auto name = node->mName;

	//Armature new_armature;
	//new_armature.name = name.C_Str();

	//armature.children.emplace_back(new_armature);

	// process all the node's meshes (if any)
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
	std::unordered_map<std::string, unsigned int > bonesDict) {

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

MeshNew Model::processMesh(aiMesh* mesh, const aiScene* scene, std::shared_ptr<Armature>  armature)
{
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	const unsigned int BONESPERVERTEX = 3;

	std::vector<bool> boolsarray;
	boolsarray.emplace_back(mesh->HasPositions());
	boolsarray.emplace_back(mesh->HasNormals());
	boolsarray.emplace_back(mesh->HasTextureCoords(0));
	boolsarray.emplace_back(mesh->HasTangentsAndBitangents());
	boolsarray.emplace_back(mesh->HasBones());

	//if (scene->HasAnimations()) {
	//	for (size_t i = 0; i <  scene->mNumAnimations; i++)
	//	{
	//		aiAnimation* ai_animation = scene->mAnimations[i];
	//		ai_animation





	// Walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		//VertexNew vertex{};
		//unsigned int size = sizeof(vertex);
		glm::vec3 vector;

		// positions
		if (mesh->HasPositions()) {

			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertices.emplace_back(vector.x);
			vertices.emplace_back(vector.y);
			vertices.emplace_back(vector.z);
		}
		// normals
		if (mesh->HasNormals()) {
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertices.emplace_back(vector.x);
			vertices.emplace_back(vector.y);
			vertices.emplace_back(vector.z);
		}
		else { vertices.emplace_back(0); vertices.emplace_back(0); vertices.emplace_back(0); }

		// texture coordinates
		//
		if (mesh->HasTextureCoords(0)) // TODO: check what this indx 0 is for
		{
			glm::vec2 vec;

			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertices.emplace_back(vec.x);
			vertices.emplace_back(vec.y);
		}
		else { vertices.emplace_back(0); vertices.emplace_back(0); }

		if (mesh->HasTangentsAndBitangents())
		{
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertices.emplace_back(vector.x);
			vertices.emplace_back(vector.y);
			vertices.emplace_back(vector.z);
			// bitangent
			glm::vec3 vv(0, 0, 0);
			vv.x = mesh->mBitangents[i].x;
			vv.y = mesh->mBitangents[i].y;
			vv.z = mesh->mBitangents[i].z;
			vertices.emplace_back(vv.x);
			vertices.emplace_back(vv.y);
			vertices.emplace_back(vv.z);
		}
		else
		{
			vertices.emplace_back(0), vertices.emplace_back(0), vertices.emplace_back(0);
			vertices.emplace_back(0), vertices.emplace_back(0), vertices.emplace_back(0);
		}
		if (mesh->HasBones()) {
			//3IDs, 3Weights (ints)
			vertices.emplace_back(0); vertices.emplace_back(0); vertices.emplace_back(0);
			vertices.emplace_back(0); vertices.emplace_back(0); vertices.emplace_back(0);
		}
		//vertices.emplace_back(vertex);
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
			Joint joint(boneIdx, boneName, AI2GLMMAT(ai_bone->mOffsetMatrix));
			
			FindChildren(armature, joint, bonesDict);
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

			if (fabs(sum - 1) > 0.01) //renormalize values!
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
			for (auto& bm : bonemapping[i]) {
				vertices[20 * i + 14 + j] = bm.first;
				vertices[20 * i + 14 + 3 + j] = bm.second;
				j++;
				if (j >= BONESPERVERTEX) break;
			}
		}
	}



	if (scene->HasAnimations()) {
		for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
			aiAnimation* anim = scene->mAnimations[i];
			const std::string name = anim->mName.C_Str();
			float ticks = anim->mTicksPerSecond;
			float duration = anim->mDuration;
			animator.m_duration = duration;
			animator.m_ticks = ticks;


			std::vector<AnimationChannel> AnimationChannels;

			for (size_t j = 0; j < anim->mNumChannels; j++)
			{
				const aiNodeAnim* ai_channel = anim->mChannels[j];
				const std::string channelName = ai_channel->mNodeName.C_Str();

				std::vector< std::pair<float, glm::vec3>> positionkeys;
				std::vector< std::pair<float, glm::quat>> rotationKeys;
				std::vector< std::pair<float, glm::vec3>> scalingKeys;

				for (size_t k = 0; k < ai_channel->mNumPositionKeys; k++)
				{
					aiVectorKey ai_key = ai_channel->mPositionKeys[k];
					std::pair<float, glm::vec3> positionKey
						= { ai_key.mTime , glm::vec3{ai_key.mValue.x,ai_key.mValue.y,ai_key.mValue.z } };

					positionkeys.emplace_back(positionKey);
				}

				for (size_t k = 0; k < ai_channel->mNumRotationKeys; k++)
				{
					aiQuatKey ai_key = ai_channel->mRotationKeys[k];
					std::pair<float, glm::quat> rotationKey
						= { ai_key.mTime, glm::quat{ ai_key.mValue.w, ai_key.mValue.x,ai_key.mValue.y,ai_key.mValue.z  } };

					rotationKeys.emplace_back(rotationKey);
				}

				for (size_t k = 0; k < ai_channel->mNumScalingKeys; k++)
				{
					aiVectorKey ai_key = ai_channel->mScalingKeys[k];
					std::pair<float, glm::vec3>
						scalingKey = { ai_key.mTime,
						glm::vec3(ai_key.mValue.x,ai_key.mValue.y,ai_key.mValue.z) };

					scalingKeys.emplace_back(scalingKey);
				}

				AnimationChannels.emplace_back(AnimationChannel(channelName, positionkeys, rotationKeys, scalingKeys));

			}

			Animation animation(duration, AnimationChannels);
			animator.current =  animation ;
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
	return MeshNew(vertices, indices, textures, boolsarray, animator);
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
	GPUShader& shader = ShaderManager::getShaderIdx(shaderIdx);
	shader.Bind();
	const unsigned int shaderID = shader.m_RendererID;

	//int count;
	//GLCall(glGetProgramiv(shaderID, GL_ACTIVE_UNIFORMS, &count));
	////printf("Active Uniforms: %d\n", count);
	//GLint maxLength;
	//GLCall(glGetProgramiv(shaderID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength));

	std::vector < std::string> names;

	//for (int i = 0; i < count; i++)
	//{
	//	GLchar name[GL_ACTIVE_UNIFORM_MAX_LENGTH + 1];
	//	int length, size;
	//	GLenum type;
	//
	//	glGetActiveUniform(shaderID, i, maxLength, &length, &size, &type, name);
	//
	//	//const GLubyte* ss = glGetString(type);
	//	//printf("Uniform #%d Type: %u Name: %s\n", i, type, name);
	//	//names.emplace_back(name);
	//}

	 

	const glm::mat4 view = cam.GetViewMatrix();
	const glm::mat4 proj = cam.GetProjectionMatrix();

	shader.SetUniformMat4f("model", model);
	shader.SetUniformMat4f("view", view);
	shader.SetUniformMat4f("projection", proj);

	for (auto& mesh : meshes)
		mesh.Draw(shader);

	shader.Unbind();
}

Model Model::CreateCube() {
	Model model;
	MeshNew mesh = MeshNew::CreateCube();
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
