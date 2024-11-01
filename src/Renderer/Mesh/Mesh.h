#pragma once

#include "pch.h"
#include <filesystem>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

#include "Vertex/Vertex.h"
#include "MeshElement.h"

namespace om {
    using PolyMesh = OpenMesh::PolyMesh_ArrayKernelT<>;
    using VH = OpenMesh::VertexHandle;
    using FH = OpenMesh::FaceHandle;
    using EH = OpenMesh::EdgeHandle;
    using HH = OpenMesh::HalfedgeHandle;
    using Point = OpenMesh::Vec3f;
}; // namespace om

inline om::Point ToOpenMesh(glm::vec3 Vertex) { return {Vertex.x, Vertex.y, Vertex.z}; }
inline OpenMesh::Vec3uc ToOpenMesh(glm::vec4 Color) {
    const auto cc = OpenMesh::color_cast<OpenMesh::Vec3uc>(OpenMesh::Vec3f{Color.r, Color.g, Color.b});
    return {cc[0], cc[1], cc[2]};
}

class Mesh
{
    using PolyMesh = om::PolyMesh;
    using VH = om::VH;
    using FH = om::FH;
    using EH = om::EH;
    using HH = om::HH;
    using Point = om::Point;

    // Adds OpenMesh handle comparison/conversion to `MeshElementIndex`.
    struct ElementIndex : MeshElementIndex {
        using MeshElementIndex::MeshElementIndex;
        ElementIndex(const MeshElementIndex &other) : MeshElementIndex(other) {}
        ElementIndex(VH vh) : MeshElementIndex(MeshElement::Vertex, vh.idx()) {}
        ElementIndex(EH eh) : MeshElementIndex(MeshElement::Edge, eh.idx()) {}
        ElementIndex(FH fh) : MeshElementIndex(MeshElement::Face, fh.idx()) {}

        bool operator==(ElementIndex other) const { return Element == other.Element && Index == other.Index; }

        bool operator==(VH vh) const { return Element == MeshElement::Vertex && Index == vh.idx(); }
        bool operator==(EH eh) const { return Element == MeshElement::Edge && Index == eh.idx(); }
        bool operator==(FH fh) const { return Element == MeshElement::Face && Index == fh.idx(); }

        // Implicit conversion to OpenMesh handles.
        operator VH() const { return Element == MeshElement::Vertex ? VH(Index) : VH(-1); }
        operator EH() const { return Element == MeshElement::Edge ? EH(Index) : EH(-1); }
        operator FH() const { return Element == MeshElement::Face ? FH(Index) : FH(-1); }
    };
    
public:
    static constexpr glm::vec4 DefaultFaceColor = {0.7, 0.7, 0.7, 1};
    
    Mesh(const fs::path& InMeshFilePath);
    ~Mesh();

    static bool Load(const fs::path& InMeshFilePath, PolyMesh& OutMesh);

    PolyMesh DeduplicateVertices();

    uint GetVertexCount() const { return M.n_vertices(); }
    uint GetEdgeCount() const { return M.n_edges(); }
    uint GetFaceCount() const { return M.n_faces(); }
    bool Empty() const { return GetVertexCount() == 0; }

    void SetFaceColor(FH FH, glm::vec4 Color) { M.set_color(FH, ToOpenMesh(Color)); }
    void SetFaceColor(glm::vec4 Color) {
        for (const auto& FH : M.faces()) SetFaceColor(FH, Color);
    }

    std::vector<Vertex3D> CreateVertices(MeshElement InMeshElement, const ElementIndex& Highlight = {}) const;
    std::vector<uint> CreateIndices(MeshElement InMeshElement) const;

private:
    PolyMesh M;
};
