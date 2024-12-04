#pragma once
#include "pch.h"
#include "Renderer/Mesh/Mesh.h"

LINK_EDITOR_NAMESPACE_BEGIN

struct MeshSegmentationInfo
{
    MeshSegmentationInfo() {}

    std::string SegLabelsFilePath;
    Eigen::VectorXi SegLabels;
};

class MeshSegmentationManager
{
public:
    static Eigen::VectorXi LoadSegLabels(const std::string& InSegFilePath);
    static void SaveSegLabels(const Eigen::VectorXi& SegLabels, const std::string& InFilePath);
    
    static MeshSegmentationInfo CurrentSegmentationInfo;
};

LINK_EDITOR_NAMESPACE_END
