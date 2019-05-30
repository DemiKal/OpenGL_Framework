#include "precomp.h"





Model::~Model()
{
}



void Model::loadModel(const std::string& path)
{
	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate  );

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

//MeshNew Model::processMaterials(aiMesh* mesh, const aiScene* scene)
//{
//	if (mesh->mMaterialIndex >= 0)
//	{
//		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
//		// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
//		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
//		// Same applies to other texture as the following list summarizes:
//		// Diffuse: texture_diffuseN
//		// Specular: texture_specularN
//		// Normal: texture_normalN
//
//		// 1. Diffuse maps
//		std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
//		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
//
//		// 2. Specular maps
//		std::vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
//		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
//		// 3. Reflection maps (Note that ASSIMP doesn't load reflection maps properly from wavefront objects, so we'll cheat a little by defining the reflection maps as ambient maps in the .obj file, which ASSIMP is able to load)
//		std::vector<Texture> reflectionMaps = this->loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_reflection");
//		textures.insert(textures.end(), reflectionMaps.begin(), reflectionMaps.end());
//	}
//}

MeshNew Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	// Data to fill
	std::vector<VertexNew> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture*> textures;

	// Walk through each of the mesh's vertices
	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		VertexNew vertex{};
		glm::vec3 vector; // We declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
		// Positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		// Normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;
		// Texture Coordinates
		if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		vertices.push_back(vertex);
	}
	// Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// Retrieve all indices of the face and store them in the indices vector
		for (GLuint j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// Process materials
 	if (mesh->mMaterialIndex >= 0)
 	{
 		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
 		// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
 		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
 		// Same applies to other texture as the following list summarizes:
 		// Diffuse: texture_diffuseN
 		// Specular: texture_specularN
 		// Normal: texture_normalN
 
 		// 1. Diffuse maps
 		
 		//std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
 		//textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
 
 		// 2. Specular maps
 		//std::vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
 		//textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
 		//// 3. Reflection maps (Note that ASSIMP doesn't load reflection maps properly from wavefront objects, so we'll cheat a little by defining the reflection maps as ambient maps in the .obj file, which ASSIMP is able to load)
 		//std::vector<Texture> reflectionMaps = this->loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_reflection");
 		//textures.insert(textures.end(), reflectionMaps.begin(), reflectionMaps.end());
 	}
	textures.emplace_back(new Texture("res/meshes/Spyro/spyro_tex.png", Texture::DIFFUSE));
	// Return a mesh object created from the extracted mesh data
	return MeshNew(vertices, indices, textures);

}


GLuint TextureFromFile(const char* path, const std::string& directory, Texture& texture)
{
	//Generate texture ID and load texture data 
	std::string filename = std::string(path);
	filename = directory + '/' + filename;
	GLuint textureID;
	//glGenTextures(1, &textureID);
	int width, height, mBPP;
	//unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
	unsigned char* image = stbi_load(path, &width, &height, &mBPP, 4);
	stbi_set_flip_vertically_on_load(true);

	ASSERT(width > 0 && height > 0);
	GLCall(glGenTextures(1, &textureID));
	GLCall(glBindTexture(GL_TEXTURE_2D, textureID));
	GLCall(glPixelStorei(GL_PACK_ALIGNMENT, 1));
	GLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
		0, GL_RGB, GL_UNSIGNED_BYTE, image));

	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	// Assign texture to ID
	//glBindTexture(GL_TEXTURE_2D, textureID);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	//glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	///GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	//GLCall(glBindTexture(GL_TEXTURE_2D, 0));
   ///


	//SOIL_free_image_data(image);
	if (image)
		stbi_image_free(image);

	texture.m_LocalBuffer = image;
	return textureID;
}

std::vector<Texture*> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type,
	const std::string& typeName) const
{
	std::vector<Texture*> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString filename;
		auto x = mat->GetTexture(type, i, &filename);

		const std::string fullpath = directory + '/' + filename.C_Str();

		
		textures.emplace_back(new Texture(fullpath, Texture::DIFFUSE));

		//const std::string p = "res/textures/uvtest.png";
		//Texture texture;
		//texture.m_RendererID = TextureFromFile(p.c_str(), directory, texture);
		//texture.type = Texture::GetType(typeName);
		//texture.m_FilePath = p;
		//textures.push_back(texture);
	}
	return textures;
}
