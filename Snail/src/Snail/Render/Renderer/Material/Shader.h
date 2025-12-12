#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "glm/glm.hpp"

typedef unsigned int GLenum;
typedef int GLint;

namespace Snail {

	class SNAIL_API Shader {
	public:
		virtual ~Shader() {}

		virtual uint32_t GetRendererId() const = 0;
        virtual const std::string& GetName() const = 0;

        virtual void SetInt(const std::string& name, int value) = 0;
        virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;
        virtual void SetFloat(const std::string& name, float value) = 0;
        virtual void SetFloat2(const std::string& name, const glm::vec2& value) = 0;
        virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
        virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
        virtual void SetMat3(const std::string& name, const glm::mat3& value) = 0;
        virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

        static Refptr<Shader> Create(const std::string& filePath);
        static Refptr<Shader> Create(const std::string& customName, const std::string& filePath);
        static Refptr<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);

	private:
        // 读取文件内容
        virtual std::string ReadFile(const std::string& filePath) = 0;
        // 分割源码
        virtual std::unordered_map<GLenum, std::string> PreProcess(const std::string& source) = 0;
        // 编译核心
        virtual void Compile(const std::unordered_map<GLenum, std::string>& shaderSources) = 0;
        // 获取 Uniform 位置 (带缓存)
        virtual GLint GetUniformLocation(const std::string& name) const = 0;
	};

    class ShaderLibrary {
    public:
        void Add(const Refptr<Shader>& shader);

        Refptr<Shader> Load(const std::string& filePath);
        Refptr<Shader> Load(const std::string& customName, const std::string& filePath);

        Refptr<Shader> Get(const std::string& name);
    private:
        std::unordered_map<std::string, Refptr<Shader>> m_Shaders;

    };

}