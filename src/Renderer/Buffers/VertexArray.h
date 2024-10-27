#pragma once

#include "pch.h"

class VertexBuffer;
class IndexBuffer;

class VertexArray
{
public:
    VertexArray();
    ~VertexArray();

    void Bind() const;
    void Unbind() const;

    void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& InVertexBuffer);
    void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& InIndexBuffer);

    std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() { return VertexBuffers; }
    const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const { return IndexBuffer; }

private:
    uint32_t RendererID;
    uint32_t VertexBufferIndex = 0;
    std::vector<std::shared_ptr<VertexBuffer>> VertexBuffers;
    std::shared_ptr<IndexBuffer> IndexBuffer;
};
