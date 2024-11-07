#pragma once

#include "pch.h"

MESH_EDITOR_NAMESPACE_BEGIN

enum class MeshElementType
{
    None,
    Face,   // Vertices are duplicated for each face. Each vertex uses the face normal.
    Vertex, // Vertices are not duplicated. Each vertex uses the vertex normal.
    Edge,   // Vertices are duplicated. Each vertex uses the vertex normal.
};

inline const std::vector AllMeshElementTypes{MeshElementType::Vertex, MeshElementType::Edge, MeshElementType::Face};

struct MeshElementIndex
{
public:
    MeshElementIndex() : ElementType(MeshElementType::None), Index(-1) {}
    MeshElementIndex(MeshElementType ElementType, int Index) : ElementType(ElementType), Index(Index) {}

    auto operator <(const MeshElementIndex &Other) const
    {
        if (ElementType != Other.ElementType) {
            return ElementType < Other.ElementType;
        }
        return Index < Other.Index;
    }

    bool IsValid() const { return Index >= 0; }
    int Idx() const { return Index; }

public:
    MeshElementType ElementType;
    int Index;
};

struct MeshElementIndexHash
{
    size_t operator()(const MeshElementIndex& InMeshElementIndex) const
    {
        return std::hash<int>{}(static_cast<int>(InMeshElementIndex.ElementType)) ^ (std::hash<int>{}(InMeshElementIndex.Index) << 1);
    }
};

MESH_EDITOR_NAMESPACE_END
