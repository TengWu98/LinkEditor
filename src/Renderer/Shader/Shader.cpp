#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>

static GLenum ShaderTypeFromString(const std::string& Type)
{
    if (Type == "vertex")
        return GL_VERTEX_SHADER;
    if (Type == "fragment" || Type == "pixel")
        return GL_FRAGMENT_SHADER;

    MESH_EDITOR_CORE_ASSERT(false, "Unknown shader type!")
    
    return 0;
}

static const char* GetCacheDirectory()
{
    return "Cache/Shader/OpenGL";
}

static void CreateCacheDirectoryIfNeeded()
{
    std::string CacheDirectory = GetCacheDirectory();
    if (!std::filesystem::exists(CacheDirectory))
        std::filesystem::create_directories(CacheDirectory);
}

Shader::Shader(const std::string& FilePath)
{
    CreateCacheDirectoryIfNeeded();
    
    std::string Source = ReadFile(FilePath);
    auto ShaderSources = PreProcess(Source);
    
    CompileOrGetOpenGLBinaries();
    CreateProgram();

    auto LastSlash = FilePath.find_last_of("/\\");
    LastSlash = LastSlash == std::string::npos ? 0 : LastSlash + 1;
    auto LastDot = FilePath.rfind('.');
    auto Count = LastDot == std::string::npos ? FilePath.size() - LastSlash : LastDot - LastSlash;
    Name = FilePath.substr(LastSlash, Count);
}

Shader::Shader(const std::string& Name, const std::string& VertexSrc, const std::string& FragmentSrc)
{
    std::unordered_map<GLenum, std::string> Sources;
    Sources[GL_VERTEX_SHADER] = VertexSrc;
    Sources[GL_FRAGMENT_SHADER] = FragmentSrc;

    CompileOrGetOpenGLBinaries();
    CreateProgram();
}

Shader::~Shader()
{
    glDeleteProgram(RendererID);
}

void Shader::Bind() const
{
    glUseProgram(RendererID);
}

void Shader::Unbind() const
{
    glUseProgram(0);
}

void Shader::SetInt(const std::string& Name, int Value)
{
    UploadUniformInt(Name, Value);
}

void Shader::SetIntArray(const std::string& Name, int* Values, uint32_t Count)
{
    UploadUniformIntArray(Name, Values, Count);
}

void Shader::SetFloat(const std::string& Name, float Value)
{
    UploadUniformFloat(Name, Value);
}

void Shader::SetFloat2(const std::string& Name, const glm::vec2& Value)
{
    UploadUniformFloat2(Name, Value);
}

void Shader::SetFloat3(const std::string& Name, const glm::vec3& Value)
{
    UploadUniformFloat3(Name, Value);
}

void Shader::SetFloat4(const std::string& Name, const glm::vec4& Value)
{
    UploadUniformFloat4(Name, Value);
}

void Shader::SetMat4(const std::string& Name, const glm::mat4& Value)
{
    UploadUniformMat4(Name, Value);
}

void Shader::UploadUniformInt(const std::string& Name, int Value)
{
    GLint Location = glGetUniformLocation(RendererID, Name.c_str());
    glUniform1i(Location, Value);
}

void Shader::UploadUniformIntArray(const std::string& Name, int* Values, uint32_t Count)
{
    GLint Location = glGetUniformLocation(RendererID, Name.c_str());
    glUniform1iv(Location, Count, Values);
}

void Shader::UploadUniformFloat(const std::string& Name, float Value)
{
    GLint Location = glGetUniformLocation(RendererID, Name.c_str());
    glUniform1f(Location, Value);
}

void Shader::UploadUniformFloat2(const std::string& Name, const glm::vec2& Value)
{
    GLint Location = glGetUniformLocation(RendererID, Name.c_str());
    glUniform2f(Location, Value.x, Value.y);
}

void Shader::UploadUniformFloat3(const std::string& Name, const glm::vec3& Value)
{
    GLint Location = glGetUniformLocation(RendererID, Name.c_str());
    glUniform3f(Location, Value.x, Value.y, Value.z);
}

void Shader::UploadUniformFloat4(const std::string& Name, const glm::vec4& Value)
{
    GLint Location = glGetUniformLocation(RendererID, Name.c_str());
    glUniform4f(Location, Value.x, Value.y, Value.z, Value.w);
}

void Shader::UploadUniformMat3(const std::string& Name, const glm::mat3& Matrix)
{
    GLint Location = glGetUniformLocation(RendererID, Name.c_str());
    glUniformMatrix3fv(Location, 1, GL_FALSE, glm::value_ptr(Matrix));
}

void Shader::UploadUniformMat4(const std::string& Name, const glm::mat4& Matrix)
{
    GLint Location = glGetUniformLocation(RendererID, Name.c_str());
    glUniformMatrix4fv(Location, 1, GL_FALSE, glm::value_ptr(Matrix));
}

std::string Shader::ReadFile(const std::string& InFilePath)
{
    std::string Result;
    std::ifstream InFile(InFilePath, std::ios::in | std::ios::binary); // ifstream closes itself due to RAII
    if (InFile)
    {
        InFile.seekg(0, std::ios::end);
        size_t Size = InFile.tellg();
        if (Size != -1)
        {
            Result.resize(Size);
            InFile.seekg(0, std::ios::beg);
            InFile.read(&Result[0], Size);
        }
        else
        {
            LOG_ERROR("Could not read from file '{0}'", InFilePath);
        }
    }
    else
    {
        LOG_ERROR("Could not open file '{0}'", InFilePath);
    }

    return Result;
}

std::unordered_map<GLenum, std::string> Shader::PreProcess(const std::string& Source)
{
    std::unordered_map<GLenum, std::string> ShaderSources;

    const char* TypeToken = "#type";
    size_t TypeTokenLength = strlen(TypeToken);
    size_t Pos = Source.find(TypeToken, 0); //Start of shader type declaration line
    while (Pos != std::string::npos)
    {
        size_t EOL = Source.find_first_of("\r\n", Pos); //End of shader type declaration line
        MESH_EDITOR_CORE_ASSERT(EOL != std::string::npos, "Syntax error")
        size_t Begin = Pos + TypeTokenLength + 1; //Start of shader type name (after "#type " keyword)
        std::string Type = Source.substr(Begin, EOL - Begin);
        MESH_EDITOR_CORE_ASSERT(ShaderTypeFromString(Type), "Invalid shader type specified")

        size_t NextLinePos = Source.find_first_not_of("\r\n", EOL); //Start of shader code after shader type declaration line
        MESH_EDITOR_CORE_ASSERT(NextLinePos != std::string::npos, "Syntax error")
        Pos = Source.find(TypeToken, NextLinePos); //Start of next shader type declaration line

        ShaderSources[ShaderTypeFromString(Type)] = (Pos == std::string::npos) ? Source.substr(NextLinePos) : Source.substr(NextLinePos, Pos - NextLinePos);
    }

    return ShaderSources;
}

void Shader::CompileOrGetOpenGLBinaries()
{
    
}

void Shader::CreateProgram()
{
    GLuint Program = glCreateProgram();
}

void Shader::Reflect(GLenum Stage, const std::vector<uint32_t>& ShaderData)
{
}
