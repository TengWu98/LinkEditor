#pragma once

#include "pch.h"

class Shader
{
public:
    Shader(const std::string& FilePath);
    Shader(const std::string& Name, const std::string& VertexSrc, const std::string& FragmentSrc);
    ~Shader();

    void Bind() const;
    void Unbind() const;

    void SetInt(const std::string& Name, int Value);
    void SetIntArray(const std::string& Name, int* Values, uint32_t Count);
    void SetFloat(const std::string& Name, float Value);
    void SetFloat2(const std::string& Name, const glm::vec2& Value);
    void SetFloat3(const std::string& Name, const glm::vec3& Value);
    void SetFloat4(const std::string& Name, const glm::vec4& Value);
    void SetMat4(const std::string& Name, const glm::mat4& Value);

    virtual const std::string& GetName() const { return Name; }

    void UploadUniformInt(const std::string& Name, int Value);
    void UploadUniformIntArray(const std::string& Name, int* Values, uint32_t Count);

    void UploadUniformFloat(const std::string& Name, float Value);
    void UploadUniformFloat2(const std::string& Name, const glm::vec2& Value);
    void UploadUniformFloat3(const std::string& Name, const glm::vec3& Value);
    void UploadUniformFloat4(const std::string& Name, const glm::vec4& Value);

    void UploadUniformMat3(const std::string& Name, const glm::mat3& Matrix);
    void UploadUniformMat4(const std::string& Name, const glm::mat4& Matrix);

private:
    std::string ReadFile(const std::string& InFilePath);

    std::unordered_map<GLenum, std::string> PreProcess(const std::string& Source);
    
    void CompileOrGetOpenGLBinaries();
    void CreateProgram();
    void Reflect(GLenum Stage, const std::vector<uint32_t>& ShaderData);

private:
    uint32_t RendererID;
    std::string Name;
    std::string FilePath;

    std::unordered_map<GLenum, std::vector<uint32_t>> OpenGLSPIRV;
    std::unordered_map<GLenum, std::string> OpenGLSourceCode;
};
