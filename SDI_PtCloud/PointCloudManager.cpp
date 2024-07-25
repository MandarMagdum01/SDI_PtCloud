#include "pch.h"
#include "Triangulation/Vector3D.h"
#include "Triangulation/Triangle.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "PointCloudManager.h"

CPointCloudManager::CPointCloudManager()
{
    // Constructor
}

CPointCloudManager::~CPointCloudManager()
{
    // Destructor
}

bool CPointCloudManager::LoadPTSFile(const CString& filename, std::vector<Point3D>& points, std::vector<PT_CLOUD::Vector3D*>& vectors)
{
    std::ifstream file(filename);
    if (!file) {
        // Handle file loading error
        std::cerr << "Error opening file: " << filename.GetString() << std::endl;
        return false;
    }

    int dotIndex = filename.ReverseFind(_T('.'));
    if (dotIndex == -1)
    {
        // No dot found, handle error (invalid filename format)
        CString errorMessage;
        errorMessage.Format(_T("Error: Filename '%s' is missing an extension."), filename);
        AfxMessageBox(errorMessage, MB_ICONERROR);
        return false;
    }

    // Get file extension from the filename
    CString ext = filename.Mid(dotIndex);

    // Compare the file extension (case-insensitive)
    if (ext.CompareNoCase(_T(".pts")) != 0)
    {
        // Handle incorrect file extension
        CString errorMessage;
        errorMessage.Format(_T("Error: Incorrect file extension. Expected .pts file. %s"), filename);
        AfxMessageBox(errorMessage, MB_ICONERROR);
        return false;
    }

    points.clear();
    vectors.clear();
    std::string line;
    int nId = 1;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        float x, y, z;
        float value1, value2, value3, value4;

        // Attempt to read values from the line
        if (!(ss >> x >> y >> z >> value1 >> value2 >> value3 >> value4)) {
            std::cerr << "Skipping invalid line: " << line << std::endl;
            continue; // Skip this line if parsing fails or doesn't match expected format
        }

        // Check if exactly 7 values were successfully read
        std::string remaining;
        if (ss >> remaining) {
            std::cerr << "Skipping line with extra values: " << line << std::endl;
            continue; // Skip this line if there are extra values
        }

        // Store the valid point
        points.push_back({ x, y, z, value1, value2, value3, value4,nId });
        PT_CLOUD::Vector3D* dot = new  PT_CLOUD::Vector3D(x, y, z, value2, value3, value4);
        vectors.push_back(dot);
        nId++;
    }

    file.close();
    return true;
}
