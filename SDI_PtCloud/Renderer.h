#pragma once

class CRenderer
{
public:
    CRenderer();
    virtual ~CRenderer();

    void Initialize(HWND hWnd); // Initialize OpenGL context
    void Resize(int cx, int cy); // Resize OpenGL viewport
    void RenderPoints(const std::vector<Point3D>& points, int selectedPointId);
    void RenderMesh(const std::vector<PT_CLOUD::Triangle*>& mesh);

private:
    HGLRC m_hrc; // OpenGL rendering context
    GLfloat m_zoomFactor;
    GLfloat m_Cx, m_Cy, m_Cz;

    // Other OpenGL related members as needed
};
