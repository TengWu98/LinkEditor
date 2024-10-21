#include "Mesh.h"

using namespace om;

Mesh::Mesh(const fs::path& InMeshFilePath)
{
    Load(InMeshFilePath, M);

    M = DeduplicateVertices();

    M.request_vertex_normals();
    M.request_face_normals();
    M.request_face_colors();
    SetFaceColor(DefaultFaceColor);
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
    size_t operator()(const Point& p) const {
        return std::hash<float>{}(p[0]) ^ std::hash<float>{}(p[1]) ^ std::hash<float>{}(p[2]);
    }
};

Mesh::PolyMesh Mesh::DeduplicateVertices()
{
    PolyMesh Deduped;
    
    std::unordered_map<Point, VH, VertexHash> UniqueVertices;
    // Add unique vertices.
    for (auto VertexIter = M.vertices_begin(); VertexIter != M.vertices_end(); ++VertexIter) {
        const auto Point = M.point(*VertexIter);
        if (auto [Iter, bIsInserted] = UniqueVertices.try_emplace(Point, VH()); bIsInserted) {
            Iter->second = Deduped.add_vertex(Point);
        }
    }
    
    // Add faces.
    for (const auto& FH : M.faces()) {
        std::vector<VH> NewFace;
        NewFace.reserve(M.valence(FH));
        for (const auto& VH : M.fv_range(FH)) NewFace.emplace_back(UniqueVertices.at(M.point(VH)));
        Deduped.add_face(NewFace);
    }
    
    return Deduped;
}
