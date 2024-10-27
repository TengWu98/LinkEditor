#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
{
    switch (type)
    {
    case ShaderDataType::Float:    return GL_FLOAT;
    case ShaderDataType::Float2:   return GL_FLOAT;
    case ShaderDataType::Float3:   return GL_FLOAT;
    case ShaderDataType::Float4:   return GL_FLOAT;
    case ShaderDataType::Mat3:     return GL_FLOAT;
    case ShaderDataType::Mat4:     return GL_FLOAT;
    case ShaderDataType::Int:      return GL_INT;
    case ShaderDataType::Int2:     return GL_INT;
    case ShaderDataType::Int3:     return GL_INT;
    case ShaderDataType::Int4:     return GL_INT;
    case ShaderDataType::Bool:     return GL_BOOL;
    }

    MESH_EDITOR_CORE_ASSERT(false, "Unknown ShaderDataType!");
    return 0;
}

VertexArray::VertexArray()
{
    glCreateVertexArrays(1, &RendererID);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &RendererID);
}

void VertexArray::Bind() const
{
    glBindVertexArray(RendererID);
}

void VertexArray::Unbind() const
{
    glBindVertexArray(0);
}

void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& InVertexBuffer)
{
    glBindVertexArray(RendererID);
    InVertexBuffer->Bind();

    const auto& Layout = InVertexBuffer->GetLayout();
    for (const auto& Element : Layout)
    {
        switch (Element.Type)
        {
        case ShaderDataType::Float:
        case ShaderDataType::Float2:
        case ShaderDataType::Float3:
        case ShaderDataType::Float4:
            {
                glEnableVertexAttribArray(VertexBufferIndex);
                glVertexAttribPointer(VertexBufferIndex,
                    Element.GetComponentCount(),
                    ShaderDataTypeToOpenGLBaseType(Element.Type),
                    Element.Normalized ? GL_TRUE : GL_FALSE,
                    Layout.GetStride(),
                    (const void*)Element.Offset);
                VertexBufferIndex++;
                break;
            }
        case ShaderDataType::Int:
        case ShaderDataType::Int2:
        case ShaderDataType::Int3:
        case ShaderDataType::Int4:
        case ShaderDataType::Bool:
            {
                glEnableVertexAttribArray(VertexBufferIndex);
                glVertexAttribIPointer(VertexBufferIndex,
                    Element.GetComponentCount(),
                    ShaderDataTypeToOpenGLBaseType(Element.Type),
                    Layout.GetStride(),
                    (const void*)Element.Offset);
                VertexBufferIndex++;
                break;
            }
        case ShaderDataType::Mat3:
        case ShaderDataType::Mat4:
            {
                uint8_t count = Element.GetComponentCount();
                for (uint8_t i = 0; i < count; i++)
                {
                    glEnableVertexAttribArray(VertexBufferIndex);
                    glVertexAttribPointer(VertexBufferIndex,
                        count,
                        ShaderDataTypeToOpenGLBaseType(Element.Type),
                        Element.Normalized ? GL_TRUE : GL_FALSE,
                        Layout.GetStride(),
                        (const void*)(Element.Offset + sizeof(float) * count * i));
                    glVertexAttribDivisor(VertexBufferIndex, 1);
                    VertexBufferIndex++;
                }
                break;
            }
        default:
            MESH_EDITOR_CORE_ASSERT(false, "Unknown ShaderDataType!");
        }
    }

    VertexBuffers.push_back(InVertexBuffer); 
}

void VertexArray::SetIndexBuffer(const std::shared_ptr<::IndexBuffer>& InIndexBuffer)
{
    glBindVertexArray(RendererID);
    InIndexBuffer->Bind();

    IndexBuffer = InIndexBuffer;
}
