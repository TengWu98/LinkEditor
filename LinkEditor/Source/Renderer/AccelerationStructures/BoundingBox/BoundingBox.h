#pragma once
#include "pch.h"
#include "Renderer/Ray/Ray.h"

LINK_EDITOR_NAMESPACE_BEGIN

struct BoundingBox
{
    glm::vec3 Min, Max;

    BoundingBox(glm::vec3 min, glm::vec3 max) : Min(min), Max(max) {}
    BoundingBox() : Min(std::numeric_limits<float>::max()), Max(-std::numeric_limits<float>::max()) {}
    
    bool IsValid() const { return Min.x <= Max.x && Min.y <= Max.y && Min.z <= Max.z; }
    
    BoundingBox operator*(const glm::mat4 &transform) const
    {
        const auto corners = Corners();
        BoundingBox BoundingBox;
        for (const glm::vec3 &corner : corners)
        {
            const glm::vec3 transformed = transform * glm::vec4(corner, 1);
            BoundingBox.Min = glm::min(BoundingBox.Min, transformed);
            BoundingBox.Max = glm::max(BoundingBox.Max, transformed);
        }
        return BoundingBox;
    }
    
    std::array<glm::vec3, 8> Corners() const {
        return {{
            {Min.x, Min.y, Min.z},
            {Min.x, Min.y, Max.z},
            {Min.x, Max.y, Min.z},
            {Min.x, Max.y, Max.z},
            {Max.x, Min.y, Min.z},
            {Max.x, Min.y, Max.z},
            {Max.x, Max.y, Min.z},
            {Max.x, Max.y, Max.z},
        }};
    }
    
    inline static const std::array<uint, 24> EdgeIndices = {
        // clang-format off
        0, 1, 1, 3, 3, 2, 2, 0,
        4, 5, 5, 7, 7, 6, 6, 4,
        0, 4, 1, 5, 3, 7, 2, 6,
        // clang-format on
    };
    
    uint MaxAxis() const {
        const glm::vec3 diff = Max - Min;
        if (diff.x > diff.y && diff.x > diff.z) return 0;
        if (diff.y > diff.z) return 1;
        return 2;
    }
    
    glm::vec3 Center() const { return (Min + Max) * 0.5f; }
    float DiagonalLength() const { return glm::length(Max - Min); }
    
    static BoundingBox UnionAll(std::vector<BoundingBox> boxes)
    {
        return std::accumulate(boxes.begin(), boxes.end(), BoundingBox{}, [](const auto &acc, const auto &box)
        {
            return acc.Union(box);
        });
    }
    
    BoundingBox Union(const BoundingBox &o) const
    {
        return {glm::min(Min, o.Min), glm::max(Max, o.Max)};
    }
    
    glm::vec3 Normal(const glm::vec3 &p) const
    {
        static const float eps = 1e-4f;
        
        if (std::abs(p.x - Min.x) < eps) return {-1, 0, 0};
        if (std::abs(p.x - Max.x) < eps) return {1, 0, 0};
        if (std::abs(p.y - Min.y) < eps) return {0, -1, 0};
        if (std::abs(p.y - Max.y) < eps) return {0, 1, 0};
        if (std::abs(p.z - Min.z) < eps) return {0, 0, -1};
        if (std::abs(p.z - Max.z) < eps) return {0, 0, 1};
        
        return {0, 0, 0}; // Failed to find a normal.
    }
    
    // Returns the intersection distance of the ray with the box, or `std::nullopt` if there is no intersection.
    std::optional<float> Intersect(const Ray &ray) const
    {
        float t0 = 0, t1 = std::numeric_limits<float>::max();
        for (int i = 0; i < 3; ++i)
        {
            const float inv_ray_dir = 1.f / ray.Direction[i];
            float t_near = (Min[i] - ray.Origin[i]) * inv_ray_dir;
            float t_far = (Max[i] - ray.Origin[i]) * inv_ray_dir;
            if (t_near > t_far) std::swap(t_near, t_far);
            t_far *= 1 + 2 * std::numeric_limits<float>::epsilon();
            
            t0 = std::max(t0, t_near);
            t1 = std::min(t1, t_far);
            if (t0 > t1) return std::nullopt;
        }
        return t0;
    }
};

LINK_EDITOR_NAMESPACE_END
