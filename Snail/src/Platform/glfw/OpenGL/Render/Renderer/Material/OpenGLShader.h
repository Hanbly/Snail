#pragma once

#include "Snail/Core/Core.h"
#include "Snail/Core/Macro.h"

#include "Snail/Render/Renderer/Material/Shader.h"

namespace Snail {

    class OpenGLShader : public Shader
    {
    public:
        // 构造函数：读取文件并编译
        OpenGLShader(const std::string& filePath);
        // 构造函数：读取文件并编译 (允许自定义命名)
        OpenGLShader(const std::string& customName, const std::string& filePath);
        // 构造函数：直接传入源码（调试用）
        OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
        ~OpenGLShader();

        inline virtual uint32_t GetRendererId() const override { return m_RendererID; }
        inline virtual const std::string& GetName() const override { return m_Name; }

        void Bind() const;
        void Unbind() const;

        // --- Uniform 设置工具 (向 Hazel 看齐，名字更短更好用) ---
        virtual void SetInt(const std::string& name, int value) override;
        virtual void SetIntArray(const std::string& name, int* values, uint32_t count) override;
        virtual void SetFloat(const std::string& name, float value) override;
        virtual void SetFloat2(const std::string& name, const glm::vec2& value) override;
        virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
        virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
        virtual void SetMat4(const std::string& name, const glm::mat4& value) override;

    private:
        // 读取文件内容
        virtual std::string ReadFile(const std::string& filePath) override;
        // 分割源码
        virtual std::unordered_map<GLenum, std::string> PreProcess(const std::string& source) override;
        // 编译核心
        virtual void Compile(const std::unordered_map<GLenum, std::string>& shaderSources) override;
        // 获取 Uniform 位置 (带缓存)
        virtual GLint GetUniformLocation(const std::string& name) const override;

    private:
        uint32_t m_RendererID;
        std::string m_FilePath;
        std::string m_Name;

        // 缓存 Uniform 位置，避免每帧重复查询 OpenGL
        mutable std::unordered_map<std::string, GLint> m_UniformLocationCache;
    };
}