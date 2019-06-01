#include "precomp.h"

void Model::SetShader(const std::string& shadername)
{
	//const auto& shadermanager = ShaderManager::getInstance();
	shaderIdx = ShaderManager::getShaderIdx(shadername);
}



void Model::loadModel(const std::string& path)
{
	Assimp::Importer import;
	const auto flags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace;
	const aiScene *scene = import.ReadFile(path, flags);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);

}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}

}

MeshNew Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	// Data to fill
		// data to fill
	std::vector<VertexNew> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture2D> textures;

	std::vector<bool> boolsarray;
	boolsarray.emplace_back(mesh->HasPositions());
	boolsarray.emplace_back(mesh->HasNormals());
	boolsarray.emplace_back(mesh->HasTextureCoords(0));
	boolsarray.emplace_back(mesh->HasTangentsAndBitangents());

	// Walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		VertexNew vertex{};
		unsigned int size = sizeof(vertex);
		glm::vec3 vector;

		// positions
		if (mesh->HasPositions()) {

			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;
		}
		// normals
		if (mesh->HasNormals()) {
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}

		// texture coordinates
		//
		if (mesh->HasTextureCoords(0)) // TODO: check what this indx 0 is for
		{
			glm::vec2 vec;

			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		// tangent
		//auto& x = mesh->mBitangents[0];

		if (mesh->HasTangentsAndBitangents())
		{
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
			// bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
		}
		else
		{
			int asdsad = 1;
		}

		vertices.push_back(vertex);
	}
	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

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
	return MeshNew(vertices, indices, textures, boolsarray);


}




std::vector<Texture2D> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type,
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

	int count;
	GLCall(glGetProgramiv(shaderID, GL_ACTIVE_UNIFORMS, &count));
	//printf("Active Uniforms: %d\n", count);
	GLint maxLength;
	GLCall(glGetProgramiv(shaderID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength));

	std::vector < std::string> names;

	for (int i = 0; i < count; i++)
	{
		GLchar name[GL_ACTIVE_UNIFORM_MAX_LENGTH + 1];
		int length, size;
		GLenum type;

		glGetActiveUniform(shaderID, i, maxLength, &length, &size, &type, name);

		//const GLubyte* ss = glGetString(type);
		//printf("Uniform #%d Type: %u Name: %s\n", i, type, name);
		//names.emplace_back(name);
	}

	const glm::mat4 view = cam.GetViewMatrix();
	const glm::mat4 proj = cam.GetProjectionMatrix();
	const glm::vec3 pos = { 1,0,0 };
	const glm::vec3 fw = { -1,0,0 };
	
	const float aspect = (float)SCREENWIDTH / (float)SCREENHEIGHT;
	const auto zx =   cam.PositionRead();

	Camera cam2({ 2,0,0 }, 70, aspect, 0.1f, 200.0f);
	static float time = 0;
	time += 0.01f;
	cam2.RotateYlocal(time);
	const glm::mat4 view2 = cam2.GetViewMatrix();//glm::lookAt(pos, pos + fw, { 0,1,0 });
	const glm::mat4 proj2 = cam2.GetProjectionMatrix();


	shader.SetUniformMat4f("model", model);
	shader.SetUniformMat4f("view", view);
	shader.SetUniformMat4f("projection", proj);
	
	shader.SetUniformMat4f("view2", view2);
	shader.SetUniformMat4f("proj2", proj2);

	for (auto& mesh : meshes)
		mesh.Draw(shader);

	shader.Unbind();
}