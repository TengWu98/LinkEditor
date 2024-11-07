#include "UniformBuffer.h"

MESH_EDITOR_NAMESPACE_BEGIN

UniformBuffer::UniformBuffer(uint32_t Size, uint32_t Binding)
{
    glCreateBuffers(1, &RendererID);
    glNamedBufferData(RendererID, Size, nullptr, GL_DYNAMIC_DRAW); // TODO: investigate usage hint
    glBindBufferBase(GL_UNIFORM_BUFFER, Binding, RendererID);
}

UniformBuffer::~UniformBuffer()
{
    glDeleteBuffers(1, &RendererID);
}

void UniformBuffer::SetData(const void* Data, uint32_t Size, uint32_t Offset)
{
    glNamedBufferSubData(RendererID, Offset, Size, Data);
}

MESH_EDITOR_NAMESPACE_END
