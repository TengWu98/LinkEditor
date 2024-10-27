#pragma once

#include "pch.h"

enum class ShaderDataType
{
    None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
};

static uint32_t ShaderDataTypeSize(ShaderDataType type)
{
    switch (type)
    {
    case ShaderDataType::Float:    return 4;
    case ShaderDataType::Float2:   return 4 * 2;
    case ShaderDataType::Float3:   return 4 * 3;
    case ShaderDataType::Float4:   return 4 * 4;
    case ShaderDataType::Mat3:     return 4 * 3 * 3;
    case ShaderDataType::Mat4:     return 4 * 4 * 4;
    case ShaderDataType::Int:      return 4;
    case ShaderDataType::Int2:     return 4 * 2;
    case ShaderDataType::Int3:     return 4 * 3;
    case ShaderDataType::Int4:     return 4 * 4;
    case ShaderDataType::Bool:     return 1;
    }

    MESH_EDITOR_CORE_ASSERT(false, "Unknown ShaderDataType!");
    return 0;
}

struct VertexBufferElement
{
    std::string Name;
    ShaderDataType Type;
    uint32_t Size;
    size_t Offset;
    bool Normalized;

    VertexBufferElement() = default;

    VertexBufferElement(ShaderDataType Type, const std::string& Name, bool Normalized = false)
        : Name(Name), Type(Type), Size(ShaderDataTypeSize(Type)), Offset(0), Normalized(Normalized)
    {
    }

    uint32_t GetComponentCount() const
    {
        switch (Type)
        {
        case ShaderDataType::Float:   return 1;
        case ShaderDataType::Float2:  return 2;
        case ShaderDataType::Float3:  return 3;
        case ShaderDataType::Float4:  return 4;
        case ShaderDataType::Mat3:    return 3; // 3* float3
        case ShaderDataType::Mat4:    return 4; // 4* float4
        case ShaderDataType::Int:     return 1;
        case ShaderDataType::Int2:    return 2;
        case ShaderDataType::Int3:    return 3;
        case ShaderDataType::Int4:    return 4;
        case ShaderDataType::Bool:    return 1;
        }

        MESH_EDITOR_CORE_ASSERT(false, "Unknown ShaderDataType!");
        return 0;
    }
};

class VertexBufferLayout
{
public:
    VertexBufferLayout() : Stride(0) {}

    VertexBufferLayout(std::initializer_list<VertexBufferElement> InElements)
            : Elements(InElements)
    {
        CalculateOffsetsAndStride();
    }

    inline unsigned int GetStride() const { return Stride; }
    inline const std::vector<VertexBufferElement>& GetElements() const { return Elements; }

    std::vector<VertexBufferElement>::iterator begin() { return Elements.begin(); }
    std::vector<VertexBufferElement>::iterator end() { return Elements.end(); }
    std::vector<VertexBufferElement>::const_iterator begin() const { return Elements.begin(); }
    std::vector<VertexBufferElement>::const_iterator end() const { return Elements.end(); }

private:
    void CalculateOffsetsAndStride()
    {
        size_t Offset = 0;
        Stride = 0;
        for (auto& Element : Elements)
        {
            Element.Offset = Offset;
            Offset += Element.Size;
            Stride += Element.Size;
        }
    }
    
private:
    unsigned int Stride;
    std::vector<VertexBufferElement> Elements;
};

class VertexBuffer
{
public:
    VertexBuffer(uint32_t Size);
    VertexBuffer(float* Vertices, uint32_t Size);
    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;

    void SetData(const void* Data, uint32_t Size);

    const VertexBufferLayout& GetLayout() const { return Layout; }
    void SetLayout(const VertexBufferLayout& InLayout) { Layout = InLayout; }
private:
    uint32_t RendererID;
    VertexBufferLayout Layout;
};
