#include "BVH.h"

MESH_EDITOR_NAMESPACE_BEGIN

BVH::BVH(std::vector<BoundingBox>& leaf_boxes) : LeafBoxes(leaf_boxes) {
    std::vector<uint> indices(LeafBoxes.size());
    std::iota(indices.begin(), indices.end(), 0);
    Build(indices);
}

uint BVH::Build(std::vector<uint>& indices) {
    if (indices.size() == 1) {
        Nodes.emplace_back(indices.front());
        return Nodes.size() - 1;
    }

    // Partially sort the indices array around the median element,
    // based on the box centers along the longest axis of the encompassing box.
    BoundingBox box = std::accumulate(indices.begin(), indices.end(), BoundingBox{}, [&](const BoundingBox &acc, uint i) {
        return acc.Union(LeafBoxes[i]);
    });
    const auto split_axis = box.MaxAxis();
    const uint mid = indices.size() / 2;
    std::nth_element(indices.begin(), indices.begin() + mid, indices.end(), [this, split_axis](auto a, auto b) {
        return LeafBoxes[a].Center()[split_axis] < LeafBoxes[b].Center()[split_axis];
    });

    // Add an internal parent node encompassing both children.
    Nodes.emplace_back(
        Build(std::vector<uint>{indices.begin(), indices.begin() + mid}),
        Build(std::vector<uint>{indices.begin() + mid, indices.end()}),
        std::move(box)
    );

    return Nodes.size() - 1;
}

std::vector<BoundingBox> BVH::CreateInternalBoxes() const {
    std::vector<BoundingBox> internal_boxes;
    
    for (const auto& node : Nodes) {
        if (node.IsInternal()) {
            internal_boxes.push_back(node.Internal->Box);
        }
    }

    return internal_boxes;
}

std::optional<uint> BVH::Intersect(const Ray &ray, const std::function<bool(uint)> &callback) const {
    if (Nodes.empty()) return std::nullopt;

    const uint root_index = Nodes.size() - 1;
    return IntersectNode(root_index, ray, callback);
}

std::optional<uint> BVH::IntersectNode(uint node_index, const Ray &ray, const std::function<bool(uint)> &callback) const {
    const auto& node = Nodes[node_index];
    if (node.IsLeaf())
    {
        return LeafBoxes[*node.BoxIndex].Intersect(ray) && callback(*node.BoxIndex) ? node.BoxIndex : std::nullopt;
    }

    // Internal node.
    if (!node.Internal->Box.Intersect(ray))
    {
        return std::nullopt;
    }
    
    if (auto left_hit = IntersectNode(node.Internal->Left, ray, callback))
    {
        return left_hit;
    }
    
    return IntersectNode(node.Internal->Right, ray, callback);
}


MESH_EDITOR_NAMESPACE_END
