#include "precomp.h"
#include "ShaderManager.h"
#include "Shader.h"

void ShaderManager::Destroy()
{
	auto& inst = GetInstance();
	for (auto& s : inst.shaders)
		s.Destroy();
}

void ShaderManager::LoadShaders(const std::string& shaderDirectory)
{
	fmt::print("---Parsing shader files---\n");
	fmt::print("Parsing helper shaders\n");
	for (const auto& entry : std::filesystem::recursive_directory_iterator(shaderDirectory))
	{
		const std::string p = entry.path().string();
		const std::string name = std::filesystem::path(p).stem().string();

		//get extension
		std::string filePath = entry.path().string();
		size_t dot = p.find_last_of('.');
		std::string extension;

		if (dot != std::string::npos)
			extension = p.substr(dot + 1);

		if (extension == "glsl")
		{
			const std::string sourceCode = ParseShader(filePath);
			auto type = Shader::ShaderType::UTIL;
			m_ShaderHelperFiles[name] = std::tie(type, sourceCode);
		}
	}

	fmt::print("Parsing shaders (VERT/FRAG/TESS/GEOM)\n");
	for (const auto& entry : std::filesystem::recursive_directory_iterator(shaderDirectory))
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

		Shader::ShaderType type = Shader::ShaderType::NONE;
		const std::string sourceCode = ParseShader(filePath);

		if (extension == "glsl") continue;

		if (extension == "vert")
		{
			type = Shader::ShaderType::VERTEX;
			m_VertexShaderSources[name] = std::tie(type, sourceCode);
		}

		if (extension == "frag")
		{
			type = Shader::ShaderType::FRAGMENT;
			m_FragmentShaderSources[name] = std::tie(type, sourceCode);
		}

		if (extension == "geom")
		{
			type = Shader::ShaderType::GEOMETRY;
			m_GeometryShaderSources[name] = std::tie(type, sourceCode);
		}
		//if (extension = "vert") type = ShaderType::VERTEX;
		//ShaderSources.emplace_back(  );
		//shaders.emplace_back(Shader(p, type));
	}

	for (auto& [vertKey, vertVal] : m_VertexShaderSources)
	{
		auto& [type, vertSrc] = vertVal;

		//find fragment shader
		if (m_FragmentShaderSources.find(vertKey) != m_FragmentShaderSources.end())
		{
			auto& [fragType, fragSrc] = m_FragmentShaderSources[vertKey];

			//find geometry shader
			if (m_GeometryShaderSources.find(vertKey) != m_GeometryShaderSources.end())
			{
				auto& [geomType, geomSrc] = m_FragmentShaderSources[vertKey];
				shaders.emplace_back(Shader(vertKey, vertSrc, fragSrc, geomSrc));

			}
			else shaders.emplace_back(Shader(vertKey, vertSrc, fragSrc));
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


static inline void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
		}));
}

void ShaderManager::RemoveComments(std::string& shaderText)
{
	std::size_t pos = 0;
	auto oldCopy = shaderText;
	while (true)
	{
		pos = shaderText.find("//", pos);
		if (pos == std::string::npos) break;

		//std::string restOfFile = shaderText.substr(pos, shaderText.size() - 1);

		auto entireLine = 1 + shaderText.find_first_of('\n', pos);
		shaderText.erase(pos, entireLine - pos);
		pos = 0;
	}

	//TODO: remove /* */ comments!

}

//replace code in shader like #include like a preprocessor
std::string& ShaderManager::SearchAndReplace(std::string& shaderText, const std::string& replacedWord)
{
	auto& inst = ShaderManager::GetInstance();
	if (replacedWord.empty()) return shaderText;

	//for (std::size_t pos = 0; (pos = shaderText.find(replacedWord, pos) + 1); pos += to.size())
	std::size_t pos = 0;

	while (true)
	{
		pos = shaderText.find(replacedWord, pos);
		if (pos == std::string::npos) break;
		std::string restOfFile = shaderText.substr(pos, shaderText.size() - 1);
		auto start = restOfFile.find_first_of('\"');
		auto end = restOfFile.find_last_of('\"', start + 1);
		auto dot = restOfFile.find_first_of('.');
		auto entireLine = 1 + restOfFile.find_first_of('\n');
		std::string cpy = shaderText;


		//auto filename = restOfFile.substr(start , dot );
		//auto filename = std::string(&restOfFile[start], &restOfFile[dot]);
		//auto size = std::distance();
		std::string filename = restOfFile.substr(start + 1, dot - start - 1);

		std::istringstream iss{ shaderText };
		std::vector<std::string> words{ std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>() };
		//std::string filename = words[0];

		if (inst.m_ShaderHelperFiles.find(filename) != inst.m_ShaderHelperFiles.end())
		{
			auto& [key, replacementString] = inst.m_ShaderHelperFiles[filename];

			shaderText.replace(--pos, entireLine, replacementString);
			pos += replacementString.size();
		}


	}

	return shaderText;
}

std::string ShaderManager::ParseShader(const std::string& path)
{
	std::ifstream stream(path);
	std::string shaderString((std::istreambuf_iterator<char>(stream)), (std::istreambuf_iterator<char>()));
	auto& inst = GetInstance();


	inst.RemoveComments(shaderString);
	inst.SearchAndReplace(shaderString, "#include");

	return shaderString;
}
