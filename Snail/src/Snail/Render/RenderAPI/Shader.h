#pragma once

#include "Snail/Core.h"

#include "glm/glm.hpp"

namespace Snail {

	struct ShaderProgramSource
	{
		std::string VertexShader;
		std::string FragmentShader;
	};

	class SNAIL_API Shader {
	protected:
		const enum class ShaderType {
			UNKNOWN = -1, VERTEX = 0, FRAGMENT = 1
		};
	public:
		virtual ~Shader() {}

		virtual uint32_t GetRendererId() const = 0;

		virtual void SetUniform4f(const std::string& name, const float& v0, const float& v1, const float& v2, const float& v3) const = 0;
		virtual void SetUniform1i(const std::string& name, const int& value) const = 0;
		virtual void SetUniformMatrix4fv(const std::string& name, const glm::mat4& mat4) const = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static std::shared_ptr<Shader> CreateShader(const std::string& filePath);

	private:
		virtual ShaderProgramSource LoadShaderSource(const std::string& filePath) const = 0;
		virtual uint32_t CompileShader(const uint32_t& shaderType, const std::string& shaderSource) const = 0;
		virtual int GetUniformLocation(const std::string& name) const = 0;
	};

}