#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(uint32_t Size)
{
    glCreateBuffers(1, &RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, RendererID);
    glBufferData(GL_ARRAY_BUFFER, Size, nullptr, GL_DYNAMIC_DRAW);
}

VertexBuffer::VertexBuffer(float* Vertices, uint32_t Size)
{
    glCreateBuffers(1, &RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, RendererID);
    glBufferData(GL_ARRAY_BUFFER, Size, Vertices, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &RendererID);
}

void VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, RendererID);
}

void VertexBuffer::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::SetData(const void* Data, uint32_t Size)
{
    glBindBuffer(GL_ARRAY_BUFFER, RendererID);
    glBufferSubData(GL_ARRAY_BUFFER, 0, Size, Data);
}
