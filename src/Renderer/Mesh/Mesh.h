#pragma once

#include "pch.h"
#include <filesystem>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

#include "Vertex/Vertex.h"
#include "MeshElement.h"
#include "Renderer/AccelerationStructures/BoundingBox/BoundingBox.h"
#include "Renderer/AccelerationStructures/BVH/BVH.h"

MESH_EDITOR_NAMESPACE_BEGIN

namespace om {
    using PolyMesh = OpenMesh::PolyMesh_ArrayKernelT<>;
    using VH = OpenMesh::VertexHandle;
    using FH = OpenMesh::FaceHandle;
    using EH = OpenMesh::EdgeHandle;
    using HH = OpenMesh::HalfedgeHandle;
    using Point = OpenMesh::Vec3f;
}; // namespace om

inline om::Point ToOpenMesh(glm::vec3 Vertex) { return {Vertex.x, Vertex.y, Vertex.z}; }
inline OpenMesh::Vec3uc ToOpenMesh(glm::vec4 Color)
{
    const auto cc = OpenMesh::color_cast<OpenMesh::Vec3uc>(OpenMesh::Vec3f{Color.r, Color.g, Color.b});
    return {cc[0], cc[1], cc[2]};
}

struct Ray;

class Mesh
{
public:
    using PolyMesh = om::PolyMesh;
    using VH = om::VH;
    using FH = om::FH;
    using EH = om::EH;
    using HH = om::HH;
    using Point = om::Point;
    
    // Adds OpenMesh handle comparison/conversion to `MeshElementIndex`.
    struct ElementIndex : MeshElementIndex
    {
        using MeshElementIndex::MeshElementIndex;
        ElementIndex(const MeshElementIndex &other) : MeshElementIndex(other) {}
        ElementIndex(VH vh) : MeshElementIndex(MeshElementType::Vertex, vh.idx()) {}
        ElementIndex(EH eh) : MeshElementIndex(MeshElementType::Edge, eh.idx()) {}
        ElementIndex(FH fh) : MeshElementIndex(MeshElementType::Face, fh.idx()) {}

        bool operator==(ElementIndex other) const { return ElementType == other.ElementType && Index == other.Index; }

        bool operator==(VH vh) const { return ElementType == MeshElementType::Vertex && Index == vh.idx(); }
        bool operator==(EH eh) const { return ElementType == MeshElementType::Edge && Index == eh.idx(); }
        bool operator==(FH fh) const { return ElementType == MeshElementType::Face && Index == fh.idx(); }

        // Implicit conversion to OpenMesh handles.
        operator VH() const { return ElementType == MeshElementType::Vertex ? VH(Index) : VH(-1); }
        operator EH() const { return ElementType == MeshElementType::Edge ? EH(Index) : EH(-1); }
        operator FH() const { return ElementType == MeshElementType::Face ? FH(Index) : FH(-1); }
    };
    
public:
    Mesh(const fs::path& InMeshFilePath);
    ~Mesh();

    static bool Load(const fs::path& InMeshFilePath, PolyMesh& OutMesh);

    PolyMesh DeduplicateVertices();

    glm::vec3 GetPosition(VH VertexHandle) const { return ToGlm(M.point(VertexHandle)); }

    uint GetVertexCount() const { return M.n_vertices(); }
    uint GetEdgeCount() const { return M.n_edges(); }
    uint GetFaceCount() const { return M.n_faces(); }
    bool Empty() const { return GetVertexCount() == 0; }

    void SetFaceColor(FH FH, glm::vec4 Color) { M.set_color(FH, ToOpenMesh(Color)); }
    void SetFaceColor(glm::vec4 Color) {
        for (const auto& FH : M.faces()) SetFaceColor(FH, Color);
    }

    std::vector<Vertex3D> CreateVertices(MeshElementType RenderElementType, const ElementIndex& Highlight = {}) const;
    std::vector<uint> CreateIndices(MeshElementType RenderElementType) const;

    std::vector<uint> CreateTriangleIndices() const; // Triangulated face indices.
    std::vector<uint> CreateTriangulatedFaceIndices() const; // Triangle fan for each face.
    std::vector<uint> CreateEdgeIndices() const;

    BoundingBox ComputeBbox() const;
    std::vector<BoundingBox> CreateFaceBoundingBoxes() const;

    std::optional<float> Intersect(const Ray& LocalRay) const;
    bool RayIntersects(const Ray& LocalRay) const;
    bool RayIntersectsFace(const Ray &, FH, float *distance_out = nullptr, glm::vec3 *intersect_point_out = nullptr) const;

    VH FindNearestVertex(glm::vec3 WorldPoint) const;
    VH FindNearestVertex(const Ray& LocalRay) const;
    EH FindNearestEdge(const Ray& WorldRay) const;
    FH FindNearestIntersectingFace(const Ray& LocalRay, glm::vec3 *NearestIntersectPointOut = nullptr) const;

    bool VertexBelongsToFace(VH VertexHandle, FH FaceHandle) const;
    bool VertexBelongsToEdge(VH VertexHandle, EH EdgeHandle) const;
    bool VertexBelongsToFaceEdge(VH VertexHandle, FH FaceHandle, EH EdgeHandle) const;
    bool EdgeBelongsToFace(EH EdgeHandle, FH FaceHandle) const;

public:
    inline static glm::vec4 FaceColor = glm::vec4{0.7, 0.7, 0.7, 1};
    inline static glm::vec4 VertexColor = glm::vec4{1, 1, 1, 1};
    inline static glm::vec4 EdgeColor = glm::vec4{0, 0, 0, 1};
    inline static glm::vec4 HighlightColor = glm::vec4{0.929, 0.341, 0, 1};                 // Blender's default `Preferences->Themes->3D Viewport->Object Selected`.
    inline static glm::vec4 FaceNormalIndicatorColor = glm::vec4{0.133, 0.867, 0.867, 1};   // Blender's default `Preferences->Themes->3D Viewport->Face Normal`.
    inline static glm::vec4 VertexNormalIndicatorColor = glm::vec4{0.137, 0.380, 0.867, 1}; // Blender's default `Preferences->Themes->3D Viewport->Vertex Normal`.
    inline static float NormalIndicatorLengthScale = 0.25;

private:
    PolyMesh M;
    BoundingBox MeshBBox;
    std::shared_ptr<BVH> MeshBVH;
    std::vector<ElementIndex> HighlightedElements; 
};

MESH_EDITOR_NAMESPACE_END
