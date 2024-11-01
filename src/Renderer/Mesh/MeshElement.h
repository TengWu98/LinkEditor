#pragma once

#include "pch.h"

enum class MeshElement
{
    None,
    Face,   // Vertices are duplicated for each face. Each vertex uses the face normal.
    Vertex, // Vertices are not duplicated. Each vertex uses the vertex normal.
    Edge,   // Vertices are duplicated. Each vertex uses the vertex normal.
};

struct MeshElementIndex
{
public:
    MeshElementIndex() : Element(MeshElement::None), Index(-1) {}
    MeshElementIndex(MeshElement Element, int Index) : Element(Element), Index(Index) {}

    auto operator <(const MeshElementIndex &Other) const
    {
        if (Element != Other.Element) {
            return Element < Other.Element;
        }
        return Index < Other.Index;
    }

    bool IsValid() const { return Index >= 0; }
    int Idx() const { return Index; }

public:
    MeshElement Element;
    int Index;
};

struct MeshElementIndexHash
{
    size_t operator()(const MeshElementIndex& InMeshElementIndex) const
    {
        return std::hash<int>{}(static_cast<int>(InMeshElementIndex.Element)) ^ (std::hash<int>{}(InMeshElementIndex.Index) << 1);
    }
};
