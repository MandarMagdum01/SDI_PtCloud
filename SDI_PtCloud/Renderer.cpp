#include "pch.h"


#include "Triangulation\Vector3D.h"
#include "Triangulation\Triangle.h"
#include "Renderer.h"
#include "PointCloudManager.h"

CRenderer::CRenderer()
{
    // Constructor
}

CRenderer::~CRenderer()
{
    // Destructor
}

void CRenderer::Initialize(HWND hWnd)
{
    // OpenGL initialization code
}

void CRenderer::Resize(int cx, int cy)
{
	if (cy == 0)
		cy = 1;

	::glViewport(0, 0, cx, cy);
	::glClearColor(255, 255, 255, 1);
	::glEnable(GL_DEPTH_TEST);
	::glMatrixMode(GL_PROJECTION);
	::glLoadIdentity();
	::glFrustum(-1, 1, -1, 1, 2, 5);
	::glMatrixMode(GL_MODELVIEW);
}

void CRenderer::RenderPoints(const std::vector<Point3D>& points, int selectedPointId)
{
	GLfloat pointSize = 5.0f; // Adjust this value as needed
	glPointSize(pointSize);

	glBegin(GL_POINTS);
	for (size_t i = 0; i < points.size(); ++i)
	{
		const Point3D& point = points[i];
		float r = static_cast<float>(point.value2) / 255.0f;
		float g = static_cast<float>(point.value3) / 255.0f;
		float b = static_cast<float>(point.value4) / 255.0f;
		if (point.m_nId == selectedPointId) {
			// Highlighted color (e.g., yellow)
			glColor3f(1.0f, 1.0f, 0.0f);  // Yellow
		}
		else
			glColor3f(r, g, b); // Red color for points

		glVertex3f(point.x , point.y, point.z);
	}
	glEnd();
}

void CRenderer::RenderMesh(const std::vector<PT_CLOUD::Triangle*>& mesh)
{

	// Iterate through each triangle in m_Mesh
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Render triangles as lines
	for (size_t i = 0; i < mesh.size(); ++i)
	{
		PT_CLOUD::Triangle* triangle = mesh[i];
		if (NULL == triangle)
			continue;

		glBegin(GL_TRIANGLES);

		// Render each vertex of the triangle
		for (int j = 0; j < 3; ++j)
		{
			PT_CLOUD::Vector3D* vertex = triangle->Vertex[j];
			glColor3f(vertex->R / 255.0, vertex->G / 255.0, vertex->B / 255.0);
			glVertex3f(vertex->X, vertex->Y, vertex->Z);
		}

		glEnd();
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Restore polygon mode to fill

}
