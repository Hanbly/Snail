#pragma once

#include "Snail/Core/Core.h"

#include "Snail/Render/RenderAPI/Shader.h"

namespace Snail {

	class OpenGLShader : public Shader {
	private:
		uint32_t m_RendererId;
		mutable std::unordered_map<std::string, int> m_UniformNameMap;
	public:
		OpenGLShader(const std::string& filePath);
		~OpenGLShader();

		inline virtual uint32_t GetRendererId() const override {
			return m_RendererId;
		}

		virtual void SetUniform4f(const std::string& name, const float& v0, const float& v1, const float& v2, const float& v3) const override;
		virtual void SetUniform1i(const std::string& name, const int& value) const override;
		virtual void SetUniformMatrix4fv(const std::string& name, const glm::mat4& mat4) const override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

	private:
		virtual ShaderProgramSource LoadShaderSource(const std::string& filePath) const override;
		virtual uint32_t CompileShader(const uint32_t& shaderType, const std::string& shaderSource) const override;
		virtual int GetUniformLocation(const std::string& name) const override;
	};

}