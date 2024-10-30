#pragma once

#include "pch.h"

class UniformBuffer
{
public:
    UniformBuffer(uint32_t Size, uint32_t Binding);
    ~UniformBuffer();

    void SetData(const void* Data, uint32_t Size, uint32_t Offset = 0);
    
private:
    uint32_t RendererID;
};
