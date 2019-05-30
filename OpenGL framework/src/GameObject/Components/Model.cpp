#include "precomp.h"

Model::~Model()
{
}


void Model::loadModel(const std::string& path)
{
	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate  | aiProcess_FlipUVs);

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
	std::vector<VertexNew> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture2D> textures;

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
 		
 		 std::vector<Texture2D> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
 		 textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
 		 
 		////  2. Specular maps
 		 std::vector<Texture2D> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
 		 textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
 		///// 3. Reflection maps (Note that ASSIMP doesn't load reflection maps properly from wavefront objects, so we'll cheat a little by defining the reflection maps as ambient maps in the .obj file, which ASSIMP is able to load)
 		 std::vector<Texture2D> reflectionMaps = this->loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_reflection");
 		 textures.insert(textures.end(), reflectionMaps.begin(), reflectionMaps.end());
 	}
	//Texture* t = new Texture("res/meshes/Spyro/spyro_tex.png", Texture::DIFFUSE);
	//textures.push_back(t);
	// Return a mesh object created from the extracted mesh data
	return MeshNew(vertices, indices, textures);

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
	 		if (std::strcmp(j.path.data(), fileName.C_Str()) == 0)
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
