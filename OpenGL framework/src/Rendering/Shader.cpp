#include "precomp.h"
#include "Shader.h"

//TODO: extra shader type for manually constructed shader
//Shader::Shader(const std::string& path, const std::string& vertexSrc, const std::string& fragSrc)
//	:
//	m_Name(std::filesystem::path(path).stem().string()),
//	m_FilePath(path)
//{
//	m_RendererID = CreateShader(vertexSrc, fragSrc, std::string());	//, sps.FragmentSource);
//	SetupUniforms();
//	GetAttributes();
//}

Shader::Shader(const std::string& path, const std::string& vertexSrc, const std::string& fragSrc, const std::string& geomSrc)
	:
	m_Name(std::filesystem::path(path).stem().string()),
	m_FilePath(path)
{
	m_RendererID = CreateShader(vertexSrc, fragSrc, geomSrc);	//, sps.FragmentSource);
	SetupUniforms();
	GetAttributes();
}



Shader::Shader(const std::string& filepath, const ShaderType shaderType)
	:
	m_RendererID(0),
	m_ShaderType(shaderType),
	m_Name("not yet loaded"),
	m_FilePath(filepath)
{

	//m_RendererID = CreateShader(sourceCode, );//, sps.FragmentSource);
	const  std::string nm = std::filesystem::path(filepath).stem().string();
	m_Name = nm;

	SetupUniforms();
	GetAttributes();
}

std::string Shader::GetName() const	//TODO: use string view?
{
	return m_Name;
}

unsigned Shader::GetID() const
{
	return m_RendererID;
}

void Shader::GetAttributes()
{
	GLint attribCount = 0;
	glGetProgramiv(m_RendererID, GL_ACTIVE_ATTRIBUTES, &attribCount);

	if (attribCount != 0)
	{
		GLint 	max_name_len = 0;
		GLsizei length = 0;
		GLsizei count = 0;
		GLenum 	type = GL_NONE;
		glGetProgramiv(m_RendererID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_name_len);

		const auto attribName = std::make_unique<char[]>(max_name_len);

		//std::unordered_map<std::string, uniform_info_t> m_uniformsInfo;

		for (GLint i = 0; i < attribCount; ++i)
		{
			glGetActiveAttrib(m_RendererID, i, max_name_len, &length, &count, &type, attribName.get());


			AttribInfo attribInfo = {};
			attribInfo.location = glGetAttribLocation(m_RendererID, attribName.get());
			attribInfo.count = count;
			attribInfo.type = type;

			m_AttribInfo.emplace(std::make_pair(std::string(attribName.get(), length), attribInfo));

		}
	}
}
void Shader::SetupUniforms() {

	GLint uniform_count = 0;
	glGetProgramiv(m_RendererID, GL_ACTIVE_UNIFORMS, &uniform_count);

	if (uniform_count != 0)
	{
		GLint 	max_name_len = 0;
		GLsizei length = 0;
		GLsizei count = 0;
		GLenum 	type = GL_NONE;
		glGetProgramiv(m_RendererID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len);

		const auto uniform_name = std::make_unique<char[]>(max_name_len);

		//std::unordered_map<std::string, uniform_info_t> m_uniformsInfo;

		for (GLint i = 0; i < uniform_count; ++i)
		{
			glGetActiveUniform(m_RendererID, i, max_name_len, &length, &count, &type, uniform_name.get());

			uniform_info_t uniform_info = {};
			uniform_info.location = glGetUniformLocation(m_RendererID, uniform_name.get());
			uniform_info.count = count;

			m_uniformsInfo.emplace(std::make_pair(std::string(uniform_name.get(), length), uniform_info));

		}
	}
}	//todo: call it get or set?

void Shader::Bind() const
{
	GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind()
{
	GLCall(glUseProgram(0));
}


void Shader::SetInt(const std::string& name, const int value)
{
	GLCall(const int loc = GetUniformLocation(name));
	GLCall(glUniform1i(loc, value));
}


//TODO: ADD 4fv!
//void Shader::SetUniform4V(const std::string& name, float v0, float v1, float v2, float v3)
//{
//	GLCall(int loc = GetUniformLocation(name));
//	GLCall(glUniform4f(loc, v0, v1, v2, v3));
//}
int Shader::GetUniformLocation(const std::string& name)
{
	if (m_uniformsInfo.find(name) != m_uniformsInfo.end())
		return m_uniformsInfo[name].location;

	GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));

	if (location == -1)
		std::cout << "location is not set of uniform " << name << std::endl;

	uniform_info_t ut;
	ut.location = location;
	ut.count = 1; //? ?
	m_uniformsInfo[name] = ut;

	return location;
}

unsigned int Shader::CompileShader(const unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		auto message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		//std::cout << "Failed to compile shader" <<
		//	(type == GL_VERTEX_SHADER ? "fragment" : "vertex") << " shader" << std::endl;
		fmt::print("Failed to compile {0} shader\n", type == GL_VERTEX_SHADER ? "fragment" : "vertex");
		fmt::print("{}\n", message);

		glDeleteShader(id);
		return 0;

	}
	return id;
}
unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader)
{
	const unsigned int program = glCreateProgram();
	const unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	const unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
	unsigned int gs = 0;

	if (!geometryShader.empty())
		gs = CompileShader(GL_GEOMETRY_SHADER, geometryShader);

	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));

	if (!geometryShader.empty())
		glAttachShader(program, gs);

	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));

	if (!geometryShader.empty())
		GLCall(glDeleteShader(fs))

		return program;
}

void Shader::SetUniformMat4f(const char* name, const glm::mat4& mat)
{
	const int location = GetUniformLocation(name);
	GLCall(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat)))
}

void Shader::SetVec4f(const std::string& name, const float v0, const float v1, const float v2, const float v3)
{
	const int loc = GetUniformLocation(name);
	GLCall(glUniform4f(loc, v0, v1, v2, v3));
}

void Shader::SetVec4f(const std::string& name, const glm::vec4& value)
{
	const int location = GetUniformLocation(name);
	GLCall(glUniform4fv(location, 1, glm::value_ptr(value)))
}



void Shader::SetVec3f(const std::string& name, const glm::vec3& value)
{
	const int location = GetUniformLocation(name);
	GLCall(glUniform3fv(location, 1, &value[0]));
}
void Shader::SetFloat(const std::string& name, const float value)
{
	const int location = GetUniformLocation(name);
	GLCall(glUniform1f(location, value));
}

//void Shader::SetUniformStruct(const std::string& name, void* myStruct)
//{
//	const int location = GetUniformLocation(name);
//	GLCall(glUniform4fv(location, 1, &value[0]));
//}

void Shader::Destroy() const
{
	GLCall(glDeleteProgram(m_RendererID));
}

