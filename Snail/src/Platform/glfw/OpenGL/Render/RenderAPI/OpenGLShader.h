#pragma once

#include "Snail/Core.h"

#include "Snail/Render/RenderAPI/Shader.h"

namespace Snail {

	class OpenGLShader : public Shader {
	private:
		uint32_t m_ShaderId;
	public:
		OpenGLShader(std::string vertexShaderSrc, std::string fragmentShaderSrc);
		~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;
	};

}