#pragma once
#include "pch.h"
#include "Renderer/AccelerationStructures/BoundingBox/BoundingBox.h"

MESH_EDITOR_NAMESPACE_BEGIN

struct BVH
{
    struct Node {
        // Leaf nodes have a valid `BoxIndex` pointing to the index of the box in `BVH::LeafBoxes`.
        // Internal nodes have a valid `Internal` field.
        struct InternalData {
            uint Left, Right; // Indices of child nodes in `BVH::Nodes`.
            BoundingBox Box; // Union of the child boxes.
        };
        std::optional<uint> BoxIndex;
        std::optional<InternalData> Internal;

        Node(uint index) : BoxIndex(index) {}
        Node(uint left, uint right, BoundingBox box) : Internal({left, right, box}) {}

        bool IsLeaf() const { return BoxIndex.has_value(); }
        bool IsInternal() const { return Internal.has_value(); }
    };
    
    BVH(std::vector<BoundingBox>& leaf_boxes);
    ~BVH() = default;

    std::optional<uint> Intersect(const Ray &, const std::function<bool(uint)> &callback) const;
    std::vector<BoundingBox> CreateInternalBoxes() const; // All non-leaf boxes, for debugging.

private:
    std::vector<BoundingBox> LeafBoxes;
    std::vector<Node> Nodes;

    uint Build(std::vector<uint>& indices);
    std::optional<uint> IntersectNode(uint node_index, const Ray &ray, const std::function<bool(uint)> &callback) const;
};

MESH_EDITOR_NAMESPACE_END