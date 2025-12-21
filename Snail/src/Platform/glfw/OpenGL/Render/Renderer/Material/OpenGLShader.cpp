#include "SNLpch.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "OpenGLShader.h"

namespace Snail {

    static GLenum ShaderTypeFromString(const std::string& type)
    {
        SNL_PROFILE_FUNCTION();


        if (type == "vertex") return GL_VERTEX_SHADER;
        if (type == "fragment" || type == "pixel") return GL_FRAGMENT_SHADER;
        SNL_CORE_ASSERT(false, "Unknown shader type!");
        return 0;
    }

    OpenGLShader::OpenGLShader(const std::string& filePath)
        : m_FilePath(filePath)
    {
        SNL_PROFILE_FUNCTION();


        // 1. 读取文件
        std::string source = ReadFile(filePath);
        // 2. 预处理分割
        auto shaderSources = PreProcess(source);
        // 3. 编译
        Compile(shaderSources);

        // 从路径提取名字 (例如 assets/shaders/Texture.glsl -> Texture)
        std::filesystem::path path = filePath; 
        m_Name = path.stem().string();          // 返回除去扩展名的文件名
    }

    OpenGLShader::OpenGLShader(const std::string& customName, const std::string& filePath)
        : m_Name(customName), m_FilePath(filePath)
    {
        SNL_PROFILE_FUNCTION();


        std::string source = ReadFile(filePath);
        auto shaderSources = PreProcess(source);
        Compile(shaderSources);
    }

    OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
        : m_Name(name)
    {
        SNL_PROFILE_FUNCTION();


        std::unordered_map<GLenum, std::string> sources;
        sources[GL_VERTEX_SHADER] = vertexSrc;
        sources[GL_FRAGMENT_SHADER] = fragmentSrc;
        Compile(sources);
    }

    OpenGLShader::~OpenGLShader()
    {
        SNL_PROFILE_FUNCTION();


        glDeleteProgram(m_RendererID);
    }

    std::string OpenGLShader::ReadFile(const std::string& filePath)
    {
        SNL_PROFILE_FUNCTION();


        std::string result;
        std::ifstream in(filePath, std::ios::in | std::ios::binary);
        if (in)
        {
            in.seekg(0, std::ios::end);
            size_t size = in.tellg();
            if (size != -1)
            {
                result.resize(size);
                in.seekg(0, std::ios::beg);
                in.read(&result[0], size);
            }
            else {
                SNL_CORE_ERROR("无法读取文件 '{0}'", filePath);
            }
        }
        else {
            SNL_CORE_ERROR("无法打开文件 '{0}'", filePath);
        }
        return result;
    }

    // --- 借用 Hazel 的解析逻辑，非常标准 ---
    std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
    {
        SNL_PROFILE_FUNCTION();


        std::unordered_map<GLenum, std::string> shaderSources;

        const char* typeToken = "#type";
        size_t typeTokenLength = strlen(typeToken);
        size_t pos = source.find(typeToken, 0);
        while (pos != std::string::npos)
        {
            size_t eol = source.find_first_of("\r\n", pos);
            SNL_CORE_ASSERT(eol != std::string::npos, "Syntax error");
            size_t begin = pos + typeTokenLength + 1;
            std::string type = source.substr(begin, eol - begin);
            SNL_CORE_ASSERT(ShaderTypeFromString(type), "不可用的shader类型");

            size_t nextLinePos = source.find_first_not_of("\r\n", eol);
            SNL_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
            pos = source.find(typeToken, nextLinePos);

            shaderSources[ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
        }
        return shaderSources;
    }

    void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
    {
        SNL_PROFILE_FUNCTION();


        const uint8_t shaderSources_maxCount = 2;
        GLuint program = glCreateProgram();
        // 因为可能有多于2个着色器(比如Geometry Shader)，存一下ID方便后面删除
        SNL_CORE_ASSERT((shaderSources.size() <= shaderSources_maxCount), "OpenGLShader: 资源内(单文件) shader 多于限定数量 '{0}'", shaderSources_maxCount);
        std::array<GLenum, shaderSources_maxCount> glShaderIDs;
        int glShaderIDIndex = 0;

        for (auto& kv : shaderSources)
        {
            GLenum type = kv.first;
            const std::string& source = kv.second;

            GLuint shaderId = glCreateShader(type);

            const GLchar* sourceCStr = source.c_str();
            glShaderSource(shaderId, 1, &sourceCStr, 0);
            glCompileShader(shaderId);

            GLint isCompiled = 0;
            glGetShaderiv(shaderId, GL_COMPILE_STATUS, &isCompiled);
            if (isCompiled == GL_FALSE)
            {
                GLint maxLength = 0;
                glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);

                std::vector<GLchar> infoLog(maxLength);
                glGetShaderInfoLog(shaderId, maxLength, &maxLength, &infoLog[0]);

                glDeleteShader(shaderId); // 失败了要清理

                SNL_CORE_ERROR("{0}", infoLog.data());
                SNL_CORE_ASSERT(false, "着色器编译失败!");
                break;
            }

            glAttachShader(program, shaderId);
            glShaderIDs[glShaderIDIndex++] = shaderId;
        }

        // Link Program
        glLinkProgram(program);

        GLint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

            glDeleteProgram(program);
            for (auto id : glShaderIDs) glDeleteShader(id);

            SNL_CORE_ERROR("{0}", infoLog.data());
            SNL_CORE_ASSERT(false, "Shader link failure!");
            return;
        }

        // Detach and delete shaders after linking
        for (auto id : glShaderIDs) {
            glDetachShader(program, id);
            glDeleteShader(id);
        }

        m_RendererID = program;
    }

    void OpenGLShader::Bind() const
    {
        SNL_PROFILE_FUNCTION();


        glUseProgram(m_RendererID);
    }

    void OpenGLShader::Unbind() const
    {
        SNL_PROFILE_FUNCTION();


        glUseProgram(0);
    }

    // --- Uniform Set 设置 ---

    void OpenGLShader::SetInt(const std::string& name, int value)
    {
        glUniform1i(GetUniformLocation(name), value);
    }
    void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count)
    {
        glUniform1iv(GetUniformLocation(name), count, values);
    }
    void OpenGLShader::SetFloat(const std::string& name, float value)
    {
        glUniform1f(GetUniformLocation(name), value);
    }
    void OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& value)
    {
        glUniform2f(GetUniformLocation(name), value.x, value.y);
    }
    void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
    {
        glUniform3f(GetUniformLocation(name), value.x, value.y, value.z);
    }
    void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
    {
        glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w);
    }
    void OpenGLShader::SetMat3(const std::string& name, const glm::mat3& value)
    {
        glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
    }
    void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
    {
        glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
    }

    GLint OpenGLShader::GetUniformLocation(const std::string& name) const
    {
        if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
            return m_UniformLocationCache[name];

        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location == -1)
            SNL_CORE_WARN("Warning: Uniform '{0}' 不存在!", name);

        m_UniformLocationCache[name] = location;
        return location;
    }

}