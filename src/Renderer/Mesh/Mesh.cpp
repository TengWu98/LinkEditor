#include "Mesh.h"
#include "Renderer/Ray/Ray.h"
#include "Renderer/AccelerationStructures/BoundingBox/BoundingBox.h"
#include "Renderer/AccelerationStructures/BVH/BVH.h"

MESH_EDITOR_NAMESPACE_BEGIN

using namespace om;

static float SquaredDistanceToLineSegment(const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &p) {
    const glm::vec3 edge = v2 - v1;
    const float t = glm::clamp(glm::dot(p - v1, edge) / glm::dot(edge, edge), 0.f, 1.f);
    const glm::vec3 closest_p = v1 + t * edge;
    const glm::vec3 diff = p - closest_p;
    return glm::dot(diff, diff);
}

Mesh::Mesh(const fs::path& InMeshFilePath)
{
    Load(InMeshFilePath, M);

    M = DeduplicateVertices();

    M.request_vertex_normals();
    M.request_face_normals();
    M.request_face_colors();
    M.request_vertex_texcoords2D();

    SetFaceColor(FaceColor);
    M.update_normals();

    MeshBBox = ComputeBbox();
    MeshBVH = std::make_shared<BVH>(CreateFaceBoundingBoxes());
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
            const bool bIsHighLight = AllHighlights.find(VertexHandle) != AllHighlights.end()
                || AllHighlights.find(Parent) != AllHighlights.end()
                || (RenderElementType == MeshElementType::Face && VertexBelongsToFaceEdge(VertexHandle, Parent, Highlight))
                || (RenderElementType == MeshElementType::Vertex && (VertexBelongsToFace(Parent, Highlight) || VertexBelongsToEdge(Parent, Highlight)))
                || (RenderElementType == MeshElementType::Edge && EdgeBelongsToFace(Parent, Highlight));

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

void Mesh::SetTextureCoordinates(const std::vector<glm::vec2>& InTexCoords)
{
    if (InTexCoords.size() != M.n_vertices())
    {
        throw std::runtime_error("Texture coordinates count does not match vertex count");
    }

    size_t i = 0;
    for (auto VertexHandle : M.vertices())
    {
        M.set_texcoord2D(VertexHandle, OpenMesh::Vec2f(InTexCoords[i].x, InTexCoords[i].y));
        ++i;
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

BoundingBox Mesh::ComputeBbox() const
{
    BoundingBox bbox;
    for (const auto &vh : M.vertices()) {
        const auto v = ToGlm(M.point(vh));
        bbox.Min = glm::min(bbox.Min, v);
        bbox.Max = glm::max(bbox.Max, v);
    }
    return bbox;
}

std::vector<BoundingBox> Mesh::CreateFaceBoundingBoxes() const {
    std::vector<BoundingBox> boxes;
    boxes.reserve(M.n_faces());
    for (const auto &fh : M.faces()) {
        BoundingBox box;
        for (const auto &vh : M.fv_range(fh)) {
            const auto &point = M.point(vh);
            box.Min = glm::min(box.Min, ToGlm(point));
            box.Max = glm::max(box.Max, ToGlm(point));
        }
        boxes.push_back(box);
    }
    return boxes;
}

// Moller-Trumbore ray-triangle intersection algorithm.
// Returns true if the ray intersects the given triangle.
// If ray intersects, sets `distance_out` to the distance along the ray to the intersection point, and sets `intersect_point_out`, if not null.
bool RayIntersectsTriangle(const Mesh::PolyMesh &m, const Ray &ray, VH v1, VH v2, VH v3, float *distance_out, glm::vec3 *intersect_point_out) {
    static const float eps = 1e-7f; // Floating point error tolerance.

    const Point ray_origin = ToOpenMesh(ray.Origin), ray_dir = ToOpenMesh(ray.Direction);
    const Point &p1 = m.point(v1), &p2 = m.point(v2), &p3 = m.point(v3);
    const Point edge1 = p2 - p1, edge2 = p3 - p1;
    const Point h = ray_dir % edge2;
    const float a = edge1.dot(h); // Barycentric coordinate
    if (a > -eps && a < eps) return false; // Check if the ray is parallel to the triangle.

    // Check if the intersection point is inside the triangle (in barycentric coordinates).
    const Point s = ray_origin - p1;
    const float f = 1.0 / a, u = f * s.dot(h);
    if (u < 0.0 || u > 1.0) return false;

    const Point q = s % edge1;
    const float v = f * ray_dir.dot(q);
    if (v < 0.0 || u + v > 1.0) return false;

    // Calculate the intersection point's distance along the ray and verify it's ahead of the ray's origin.
    const float distance = f * edge2.dot(q);
    if (distance > eps) {
        if (distance_out) *distance_out = distance;
        if (intersect_point_out) *intersect_point_out = ray(distance);
        return true;
    }
    return false;
}

bool Mesh::RayIntersectsFace(const Ray &ray, FH fh, float *distance_out, glm::vec3 *intersect_point_out) const {
    auto fv_it = M.cfv_iter(fh);
    const VH v0 = *fv_it++;
    VH v1 = *fv_it++, v2;
    for (; fv_it.is_valid(); ++fv_it) {
        v2 = *fv_it;
        if (RayIntersectsTriangle(M, ray, v0, v1, v2, distance_out, intersect_point_out)) return true;
        v1 = v2;
    }
    return false;
}

FH Mesh::FindNearestIntersectingFace(const Ray &local_ray, glm::vec3 *nearest_intersect_point_out) const {
    float distance = 0, min_distance = std::numeric_limits<float>::max();
    glm::vec3 intersect_point;
    FH nearest_face{};
    MeshBVH->Intersect(local_ray, [&](uint fi) {
        if (RayIntersectsFace(local_ray, FH{int(fi)}, &distance, &intersect_point) && distance < min_distance) {
            min_distance = distance;
            nearest_face = FH{int(fi)};
            if (nearest_intersect_point_out) *nearest_intersect_point_out = intersect_point;
        }
        return false; // We want the nearest face, not just any intersecting face.
    });
    return nearest_face;
}

bool Mesh::VertexBelongsToFace(VH VertexHandle, FH FaceHandle) const
{
    return VertexHandle.is_valid()
        && FaceHandle.is_valid()
        && std::any_of(M.fv_range(FaceHandle).begin(), M.fv_range(FaceHandle).end(), [&](const VH& vh_o)
        {
            return vh_o == VertexHandle;
        });
}

bool Mesh::VertexBelongsToEdge(VH VertexHandle, EH EdgeHandle) const
{
    return VertexHandle.is_valid()
        && EdgeHandle.is_valid()
        && std::any_of(M.voh_range(VertexHandle).begin(), M.voh_range(VertexHandle).end(), [&](const auto& heh)
        {
            return M.edge_handle(heh) == EdgeHandle;
        });
}

bool Mesh::VertexBelongsToFaceEdge(VH VertexHandle, FH FaceHandle, EH EdgeHandle) const
{
    return FaceHandle.is_valid()
        && EdgeHandle.is_valid()
        && std::any_of(M.voh_range(VertexHandle).begin(), M.voh_range(VertexHandle).end(), [&](const auto &heh)
        {
           return M.edge_handle(heh) == EdgeHandle && (M.face_handle(heh) == FaceHandle || M.face_handle(M.opposite_halfedge_handle(heh)) == FaceHandle);
        });
}

bool Mesh::EdgeBelongsToFace(EH EdgeHandle, FH FaceHandle) const
{
    return EdgeHandle.is_valid()
        && FaceHandle.is_valid()
        && std::any_of(M.fh_range(FaceHandle).begin(), M.fh_range(FaceHandle).end(), [&](const auto &heh)
        {
            return M.edge_handle(heh) == EdgeHandle;
        });
}

std::optional<float> Mesh::Intersect(const Ray &local_ray) const {
    float distance = 0, min_distance = std::numeric_limits<float>::max();
    MeshBVH->Intersect(local_ray, [&](uint fi) {
        if (RayIntersectsFace(local_ray, FH{int(fi)}, &distance) && distance < min_distance) {
            min_distance = distance;
        }
        return false; // We want the nearest intersection, not just any intersection.
    });
    return min_distance < std::numeric_limits<float>::max() ? std::make_optional(min_distance) : std::nullopt;
}
bool Mesh::RayIntersects(const Ray &local_ray) const {
    auto callback = [this, &local_ray](uint fi) { return RayIntersectsFace(local_ray, FH{int(fi)}); };
    return MeshBVH->Intersect(local_ray, callback).has_value();
}

VH Mesh::FindNearestVertex(glm::vec3 WorldPoint) const
{
    VH ClosestVertex;
    
    float MinDistanceSquare = std::numeric_limits<float>::max();
    for(const auto& VertexHandle : M.vertices())
    {
        const glm::vec3 Diff = GetPosition(VertexHandle) - WorldPoint;
        const float DistanceSquare = glm::dot(Diff, Diff);
        if(DistanceSquare < MinDistanceSquare)
        {
            MinDistanceSquare = DistanceSquare;
            ClosestVertex = VertexHandle;
        }
    }

    return ClosestVertex;
}

VH Mesh::FindNearestVertex(const Ray& LocalRay) const
{
    glm::vec3 intersection_point;
    const auto face = FindNearestIntersectingFace(LocalRay, &intersection_point);
    if (!face.is_valid()) return VH{};

    VH closest_vertex;
    float min_distance_sq = std::numeric_limits<float>::max();
    for (const auto &vh : M.fv_range(face)) {
        const glm::vec3 diff = GetPosition(vh) - intersection_point;
        const float distance_sq = glm::dot(diff, diff);
        if (distance_sq < min_distance_sq) {
            min_distance_sq = distance_sq;
            closest_vertex = vh;
        }
    }

    return closest_vertex; 
}

EH Mesh::FindNearestEdge(const Ray& WorldRay) const
{
    glm::vec3 intersection_point;
    const auto face = FindNearestIntersectingFace(WorldRay, &intersection_point);
    if (!face.is_valid()) return Mesh::EH{};

    Mesh::EH closest_edge;
    float min_distance_sq = std::numeric_limits<float>::max();
    for (const auto &heh : M.fh_range(face)) {
        const auto &edge_handle = M.edge_handle(heh);
        const auto &p1 = GetPosition(M.from_vertex_handle(M.halfedge_handle(edge_handle, 0)));
        const auto &p2 = GetPosition(M.to_vertex_handle(M.halfedge_handle(edge_handle, 0)));
        const float distance_sq = SquaredDistanceToLineSegment(p1, p2, intersection_point);
        if (distance_sq < min_distance_sq) {
            min_distance_sq = distance_sq;
            closest_edge = edge_handle;
        }
    }

    return closest_edge;
}

MESH_EDITOR_NAMESPACE_END
