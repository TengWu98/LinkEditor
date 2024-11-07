#pragma once

#include "pch.h"

MESH_EDITOR_NAMESPACE_BEGIN

class IndexBuffer
{
public:
    IndexBuffer(uint32_t* Indices, uint32_t Count);
    ~IndexBuffer();

    void Bind() const;
    void Unbind() const;

    uint32_t GetCount() const { return Count; }
private:
    uint32_t RendererID;
    uint32_t Count;
};

MESH_EDITOR_NAMESPACE_END
