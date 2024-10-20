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
        std::cerr << "Error loading mesh: " << InMeshFilePath << std::endl;
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
    PolyMesh deduped;
    std::unordered_map<Point, VH, VertexHash> unique_vertices;

    // Add unique vertices.
    for (auto v_it = M.vertices_begin(); v_it != M.vertices_end(); ++v_it) {
        const auto p = M.point(*v_it);
        if (auto [it, inserted] = unique_vertices.try_emplace(p, VH()); inserted) {
            it->second = deduped.add_vertex(p);
        }
    }
    // Add faces.
    for (const auto &fh : M.faces()) {
        std::vector<VH> new_face;
        new_face.reserve(M.valence(fh));
        for (const auto &vh : M.fv_range(fh)) new_face.emplace_back(unique_vertices.at(M.point(vh)));
        deduped.add_face(new_face);
    }
    return deduped;
}
