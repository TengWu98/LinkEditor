#include "Mesh.h"

MESH_EDITOR_NAMESPACE_BEGIN

using namespace om;

Mesh::Mesh(const fs::path& InMeshFilePath)
{
    Load(InMeshFilePath, M);

    M = DeduplicateVertices();

    M.request_vertex_normals();
    M.request_face_normals();
    M.request_face_colors();
    SetFaceColor(FaceColor);
    M.update_normals();
}

Mesh::~Mesh()
{
    M.release_vertex_normals();
    M.release_face_normals();
    M.release_face_colors();
}

bool Mesh::Load(const fs::path& InMeshFilePath, PolyMesh& OutMesh)
{
    OpenMesh::IO::Options ReadOptions;
    if (!OpenMesh::IO::read_mesh(OutMesh, InMeshFilePath.string(), ReadOptions)) {
        std::cerr << "Error loading mesh: " << InMeshFilePath << "\n";
        return false;
    }
    return true;
}

struct VertexHash {
    size_t operator()(const Point& p) const
    {
        return std::hash<float>{}(p[0]) ^ std::hash<float>{}(p[1]) ^ std::hash<float>{}(p[2]);
    }
};

Mesh::PolyMesh Mesh::DeduplicateVertices()
{
    PolyMesh Deduped;
    
    std::unordered_map<Point, VH, VertexHash> UniqueVertices;
    // Add unique vertices.
    for (auto VertexIter = M.vertices_begin(); VertexIter != M.vertices_end(); ++VertexIter)
    {
        const auto Point = M.point(*VertexIter);
        if (auto [Iter, bIsInserted] = UniqueVertices.try_emplace(Point, VH()); bIsInserted) {
            Iter->second = Deduped.add_vertex(Point);
        }
    }
    
    // Add faces.
    for (const auto& FaceHandle : M.faces())
    {
        std::vector<VH> NewFace;
        
        NewFace.reserve(M.valence(FaceHandle));
        for (const auto& VertexHandle : M.fv_range(FaceHandle))
        {
            NewFace.emplace_back(UniqueVertices.at(M.point(VertexHandle)));
        }
        
        Deduped.add_face(NewFace);
    }
    
    return Deduped;
}

struct VerticesHandle {
    Mesh::ElementIndex Parent; // A vertex can belong to itself, an edge, or a face.
    std::vector<Mesh::VH> VHs;
};

std::vector<Vertex3D> Mesh::CreateVertices(MeshElementType RenderElementType, const ElementIndex& Highlight) const
{
    std::vector<VerticesHandle> Handles;
    
    if (RenderElementType == MeshElementType::Vertex)
    {
        Handles.reserve(M.n_vertices());
        for (const auto& VertexHandle : M.vertices())
        {
            VerticesHandle VerticesHandle = {VertexHandle, std::vector<VH>{VertexHandle}};
            Handles.emplace_back(VerticesHandle);
        }
    }
    else if (RenderElementType == MeshElementType::Edge)
    {
        Handles.reserve(M.n_edges() * 2);
        for (const auto& EdgeHandle : M.edges())
        {
            const auto HalfEdgeHandle = M.halfedge_handle(EdgeHandle, 0);
            VerticesHandle VerticesHandle = {EdgeHandle, std::vector<VH>{M.from_vertex_handle(HalfEdgeHandle), M.to_vertex_handle(HalfEdgeHandle)}};
            Handles.emplace_back(VerticesHandle);
        }
    }
    else if (RenderElementType == MeshElementType::Face)
    {
        Handles.reserve(M.n_faces() * 3); // Lower bound assuming all faces are triangles.
        for (const auto& FaceHandle : M.faces())
        {
            for (const auto& VertexHandle : M.fv_range(FaceHandle))
            {
                VerticesHandle VerticesHandle = {FaceHandle, std::vector<VH>{VertexHandle}};
                Handles.emplace_back(VerticesHandle);
            }
        }
    }

    static std::unordered_set<ElementIndex, MeshElementIndexHash> AllHighlights;
    AllHighlights.clear();
    AllHighlights.insert(HighlightedElements.begin(), HighlightedElements.end());
    AllHighlights.emplace(Highlight);

    std::vector<Vertex3D> Vertices;
    for (const auto& Handle : Handles)
    {
        const auto& Parent = Handle.Parent;
        const auto Normal = ToGlm(RenderElementType == MeshElementType::Vertex || RenderElementType == MeshElementType::Edge ? M.normal(Handle.VHs[0]) : M.normal(FH(Handle.Parent)));
        
        for(const auto& VertexHandle : Handle.VHs)
        {
            const bool bIsHighLight = false;

            glm::vec4 Color;
            if(bIsHighLight)
            {
                Color = HighlightColor;
            }
            else
            {
                if(RenderElementType == MeshElementType::Vertex)
                {
                    Color = VertexColor;
                }
                else if(RenderElementType == MeshElementType::Edge)
                {
                    Color = EdgeColor;
                }
                else
                {
                    // ToGlm(M.color(FH(Parent)));
                    Color = FaceColor;
                }
            }
            
            Vertex3D CurrentVertex = {GetPosition(VertexHandle), Normal, Color};
            Vertices.emplace_back(CurrentVertex);
        }
    }

    return Vertices;
}

std::vector<uint> Mesh::CreateIndices(MeshElementType RenderElementType) const
{
    switch (RenderElementType) {
    case MeshElementType::Face: return CreateTriangulatedFaceIndices();
    case MeshElementType::Vertex: return CreateTriangleIndices();
    case MeshElementType::Edge: return CreateEdgeIndices();
    case MeshElementType::None: return {};
    }
}

std::vector<uint> Mesh::CreateTriangleIndices() const
{
    std::vector<uint> Indices;
    
    for (const auto& FaceHandle : M.faces())
    {
        auto FaceVertexIter = M.cfv_iter(FaceHandle);
        const VH V0 = *FaceVertexIter++;
        VH V1 = *FaceVertexIter++;
        VH V2;
        for (; FaceVertexIter.is_valid(); ++FaceVertexIter)
        {
            V2 = *FaceVertexIter;
            Indices.insert(Indices.end(), {static_cast<uint>(V0.idx()), static_cast<uint>(V1.idx()), static_cast<uint>(V2.idx())});
            V1 = V2;
        }
    }
    
    return Indices;
}

std::vector<uint> Mesh::CreateTriangulatedFaceIndices() const
{
    std::vector<uint> Indices;
    
    uint Index = 0;
    for (const auto& FaceHandle : M.faces())
    {
        const auto Valence = M.valence(FaceHandle);
        for (uint i = 0; i < Valence - 2; ++i)
        {
            Indices.insert(Indices.end(), {Index, Index + i + 1, Index + i + 2});
        }
        
        Index += Valence;
    }
    
    return Indices;
}

std::vector<uint> Mesh::CreateEdgeIndices() const
{
    std::vector<uint> Indices;
    
    Indices.reserve(M.n_edges() * 2);
    for (uint i = 0; i < M.n_edges(); ++i)
    {
        Indices.push_back(2 * i);
        Indices.push_back(2 * i + 1);
    }
    
    return Indices;
}

MESH_EDITOR_NAMESPACE_END
