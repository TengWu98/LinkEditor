#include "MeshSegementation.h"

LINK_EDITOR_NAMESPACE_BEGIN

MeshSegmentationInfo MeshSegmentationManager::CurrentSegmentationInfo;

Eigen::VectorXi MeshSegmentationManager::LoadSegLabels(const std::string& InSegFilePath)
{
    if (!std::ifstream(InSegFilePath).good())
    {
        LOG_ERROR("File not found: {0}", InSegFilePath);
    }
    
    std::ifstream File(InSegFilePath);
    if (!File.is_open())
    {
        LOG_ERROR("Failed to open file: {0}", InSegFilePath);
    }

    std::vector<int> Labels;
    int Label;
    while (File >> Label)
    {
        Labels.push_back(Label);
    }
    
    File.close();
    
    Eigen::VectorXi SegLabels(Labels.size());
    for (size_t i = 0; i < Labels.size(); ++i)
    {
        SegLabels[i] = Labels[i];
    }

    CurrentSegmentationInfo.SegLabelsFilePath = InSegFilePath;
    CurrentSegmentationInfo.SegLabels = SegLabels;

    return SegLabels;
}

void MeshSegmentationManager::SaveSegLabels(const Eigen::VectorXi& SegLabels, const std::string& InFilePath)
{
    std::ofstream File(InFilePath);
    if (!File.is_open())
    {
        LOG_ERROR("Failed to open file: {0}", InFilePath);
    }

    for (int i = 0; i < SegLabels.size(); ++i)
    {
        File << SegLabels(i) << "\n";
    }

    File.close();
}

LINK_EDITOR_NAMESPACE_END
