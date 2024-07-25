#pragma once

class CPointCloudManager
{
public:
    CPointCloudManager();
    virtual ~CPointCloudManager();

    bool LoadPTSFile(const CString& filename, std::vector<Point3D>& points, std::vector<PT_CLOUD::Vector3D*>& vectors);

private:
    // Helper methods for parsing and handling files
};
