#include "precomp.h"
#include "ShaderManager.h"
#include "Shader.h"

void ShaderManager::Destroy()
{
	auto& inst = GetInstance();
	for (auto& s : inst.shaders)
		s.Destroy();
}
//from https://www.programmersought.com/article/96663621079/
void AddCommonShaderFile(const std::string& path, const std::string& fileName)
{
	if (fileName.empty())
		return;



	//std::string shaderFolder = m_context->GetConfigParams("ShaderPath");
	//std::string shaderPath = shaderFolder + "Common/" + fileName + ".glsl";

	/// Read the common shader file
	std::string code;
	std::ifstream shaderFile;
	shaderFile.exceptions(std::ifstream::badbit);
	try
	{
		/// open a file 
		shaderFile.open(path);
		std::stringstream shaderStream;
		/// Read the buffer content of the file into the stream
		shaderStream << shaderFile.rdbuf();
		/// Close the file
		shaderFile.close();
		/// Convert stream to GLchar array
		code = shaderStream.str();
	}
	catch (std::ifstream::failure e) {
		fmt::print("init shader error: failed to read shader file");
	}

	/// Pass the common shader file into GL's virtual file system through glNamedStringARB function
	/// For example, the incoming shader file is named commonFunction.glsl
	/// Then the second parameter of glNamedStringARB is: "/commonFunction.glsl", "/" is necessary
	/// The fourth parameter is the string of code
	std::string fullFileName = "/" + fileName + ".glsl";
	glNamedStringARB(GL_SHADER_INCLUDE_ARB, fullFileName.size(), fullFileName.c_str(), code.size(), code.c_str());
}

void ShaderManager::LoadShaders(const std::string& shaderDirectory)
{
	for (const auto& entry : std::filesystem::directory_iterator(shaderDirectory))
	{
		std::cout << entry.path() << std::endl;
		const std::string p = entry.path().string();
		const std::string name = std::filesystem::path(p).stem().string();


		//get extension
		std::string filePath = entry.path().string();
		size_t dot = p.find_last_of('.');
		
		std::string extension;
		if (dot != std::string::npos)
			extension = p.substr(dot + 1);

		size_t lastSlash = p.find_last_of('/');
		std::string fileName;
		if (lastSlash != std::string::npos)
			fileName = p.substr(lastSlash + 1, dot +1);


		Shader::ShaderType type = Shader::ShaderType::NONE;
		const std::string sourceCode = ParseShader(filePath);

		if (extension == "vert")
		{
			type = Shader::ShaderType::VERTEX;
			m_vertexShaderSources[name] = std::tie(type, sourceCode);
		}

		if (extension == "frag")
		{
			type = Shader::ShaderType::FRAGMENT;
			m_fragmentShaderSources[name] = std::tie(type, sourceCode);
		}
		if (extension == "glsl")
		{
			int iii = 12;
			AddCommonShaderFile(filePath, name);
			//m_CommonShaderFiles.emplace_back(name);
			//type = Shader::ShaderType::FRAGMENT;
			//m_fragmentShaderSources[name] = std::tie(type, sourceCode);
		}
		//if (extension = "vert") type = ShaderType::VERTEX;
		//ShaderSources.emplace_back(  );
		//shaders.emplace_back(Shader(p, type));
	}

	for (auto& [vertKey, vertVal] : m_vertexShaderSources)
	{
		auto& [type, vertSrc] = vertVal;

		if (m_fragmentShaderSources.find(vertKey) != m_fragmentShaderSources.end())
		{
			auto& [name2, fragSrc] = m_fragmentShaderSources[vertKey];

			shaders.emplace_back(Shader(vertKey, vertSrc, fragSrc));
		}
		else continue;
	}
}

Shader& ShaderManager::GetShader(const std::string& name)
{
	const unsigned int idx = GetShaderIdx(name);
	auto& inst = GetInstance();
	return inst.shaders[idx];
}

//Need better datastructure for this
unsigned int ShaderManager::GetShaderIdx(const std::string& name)
{
	const auto& inst = GetInstance();
	for (unsigned int i = 0; i < inst.shaders.size(); i++)
	{
		if (inst.shaders[i].GetName() == name)
		{
			return  i;
		}
	}

	return 0;
}

void ShaderManager::Init(const std::string& shaderDirectory)
{
	auto& instance = GetInstance();
	instance.LoadShaders(shaderDirectory);
	instance.initialized = true;
}

ShaderManager& ShaderManager::GetInstance()
{
	static auto instance = std::make_unique<ShaderManager>();
	return *instance;
}

Shader& ShaderManager::GetShader(const unsigned idx)
{
	auto& inst = GetInstance();
	return inst.shaders[idx];
}

std::string ShaderManager::ParseShader(const std::string& path)
{
	std::ifstream stream(path);
	std::string shaderString((std::istreambuf_iterator<char>(stream)),
		(std::istreambuf_iterator<char>()));

	return shaderString;
}
