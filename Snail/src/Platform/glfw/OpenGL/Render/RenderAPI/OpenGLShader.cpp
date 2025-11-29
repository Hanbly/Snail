#include "SNLpch.h"

#include "OpenGLShader.h"

namespace Snail {

	OpenGLShader::OpenGLShader(const std::string& filePath)
		: m_RendererId(0)
	{
		ShaderProgramSource basicShader = LoadShaderSource(filePath);

		unsigned int program = glCreateProgram();

		unsigned int vshader = CompileShader(GL_VERTEX_SHADER, basicShader.VertexShader);
		unsigned int fshader = CompileShader(GL_FRAGMENT_SHADER, basicShader.FragmentShader);

		glAttachShader(program, vshader);
		glAttachShader(program, fshader);
		glLinkProgram(program);
		glValidateProgram(program);

		int result;
		glGetProgramiv(program, GL_LINK_STATUS, &result);
		if (result == GL_FALSE) {
			int length;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
			
			std::vector<char> message(length);
			glGetProgramInfoLog(program, length, &length, &message[0]);
			SNL_CORE_ERROR("链接着色器程序失败! Error: ");
			SNL_CORE_ERROR("{0}", message.data());
			SNL_CORE_ASSERT(false, "");

			glDeleteProgram(program);
			return;
		}

		glDeleteShader(vshader);
		glDeleteShader(fshader);

		m_RendererId = program;
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_RendererId);
	}

	void OpenGLShader::SetUniform4f(const std::string& name, const float& v0, const float& v1, const float& v2, const float& v3) const
	{
		this->Bind();
		int location = GetUniformLocation(name);
		glUniform4f(location, v0, v1, v2, v3);
	}

	void OpenGLShader::SetUniform1i(const std::string& name, const int& value) const
	{
		this->Bind();
		int location = GetUniformLocation(name);
		glUniform1i(location, value);
	}

	void OpenGLShader::SetUniformMatrix4fv(const std::string& name, const glm::mat4& mat4) const
	{
		this->Bind();
		int location = GetUniformLocation(name);
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat4));
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(m_RendererId);
	}

	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}

	ShaderProgramSource OpenGLShader::LoadShaderSource(const std::string& filePath) const
	{
		std::ifstream stream(filePath);

		std::string line;
		std::stringstream result[2];
		ShaderType currentShaderType = ShaderType::UNKNOWN;

		while (std::getline(stream, line)) {
			if (line.find("#SHADER") != std::string::npos) {
				if (line.find("vertex") != std::string::npos) {
					currentShaderType = ShaderType::VERTEX;
				}
				else if (line.find("fragment") != std::string::npos) {
					currentShaderType = ShaderType::FRAGMENT;
				}
			}
			else {
				if (currentShaderType != ShaderType::UNKNOWN)
					result[(int)currentShaderType] << line << "\n";
			}
		}
		return { result[(int)ShaderType::VERTEX].str(), result[(int)ShaderType::FRAGMENT].str() };
	}

	uint32_t OpenGLShader::CompileShader(const uint32_t& shaderType, const std::string& shaderSource) const
	{
		uint32_t shaderId = glCreateShader(shaderType);
		const char* srcPtr = shaderSource.c_str();

		glShaderSource(shaderId, 1, &srcPtr, nullptr);
		glCompileShader(shaderId);

		int result;
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE) {
			int length;
			glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
			
			std::vector<char> message(length);
			glGetProgramInfoLog(shaderId, length, &length, &message[0]);
			SNL_CORE_ERROR(" 编译着色器 [", 
				(shaderType == GL_VERTEX_SHADER ? "vertex" : shaderType == GL_FRAGMENT_SHADER ? "fragment" : "unknown"),
				"] 失败! Error : ");
			SNL_CORE_ERROR(message.data());
			SNL_CORE_ASSERT(false, "");

			glDeleteShader(shaderId);
			return 0;
		}

		return shaderId;
	}

	int OpenGLShader::GetUniformLocation(const std::string& name) const
	{
		if (m_UniformNameMap.find(name) != m_UniformNameMap.end()) {
			return m_UniformNameMap[name];
		}
		int location = glGetUniformLocation(m_RendererId, name.c_str());
		if (location == -1) {
			SNL_CORE_ASSERT(false, "Uniform Location does not EXIST !");
		} else {
			m_UniformNameMap.insert({ name, location });
		}
		return location;
	}

}