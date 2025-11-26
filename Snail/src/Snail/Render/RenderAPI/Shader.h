#pragma once

#include "Snail/Core.h"

namespace Snail {

	class SNAIL_API Shader {
	public:
		virtual ~Shader() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static std::shared_ptr<Shader> CreateShader(std::string vertexShaderSrc, std::string fragmentShaderSrc);
	};

}