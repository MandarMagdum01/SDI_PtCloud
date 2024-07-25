
// SDI_PtCloudView.cpp : implementation of the CSDIPtCloudView class
//

#include "pch.h"

#include "Triangulation\Vector3D.h"
#include "Triangulation\Triangle.h"
#include "Triangulation/DelaunayTriangulation.h"
#include "Renderer.h"
#include "PointCloudManager.h"
#include "framework.h"

// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "SDI_PtCloud.h"
#endif

#include "SDI_PtCloudDoc.h"
#include "SDI_PtCloudView.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSDIPtCloudView

IMPLEMENT_DYNCREATE(CSDIPtCloudView, CView)

BEGIN_MESSAGE_MAP(CSDIPtCloudView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_SIZE()
    ON_WM_PAINT()
    ON_WM_KEYDOWN()
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEWHEEL()
    ON_COMMAND(ID_FILE_OPEN, &CSDIPtCloudView::OnFileOpen)
    ON_COMMAND(ID_CREATE_MESH, &CSDIPtCloudView::onCreateMesh)
END_MESSAGE_MAP()

void CSDIPtCloudView::onCreateMesh()
{
    if (m_Vectors.size() == 0)
    {
        AfxMessageBox(_T("Please open pts file to view mesh..."));
        return;
    }
    m_nMeshCreation = GL_TRIANGLES;
    PT_CLOUD::DelaunayTriangulation* triangulation = new PT_CLOUD::DelaunayTriangulation();
    m_Mesh = triangulation->GetTriangulationResult(m_Vectors);

    Invalidate();
}

void CSDIPtCloudView::OnFileOpen()
{
    m_nMeshCreation = GL_POINTS;
    m_zoomFactor = 45.0f;
    UpdateProjection();

    m_Cx = 0;
    m_Cy = 0;
    m_Cz = -3;
    CFileDialog dlg(TRUE); // TRUE for open dialog
    if (dlg.DoModal() == IDOK)
    {
        CString filePath = dlg.GetPathName();
       bool bIsValid =  m_pointCloudManager.LoadPTSFile(filePath,m_points, m_Vectors);
       if (!bIsValid)
           return;


        // Get reference to CPropertiesWnd instance
        CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
        CPropertiesWnd* pPropertiesWnd = &(pMainFrame->GetPropertiesWnd());
        pPropertiesWnd->UpdateProperties(m_points);
    }
    Invalidate();
}



void CSDIPtCloudView::DrawPoints(int inDrawMethod)
{
    glLoadIdentity();
    gluLookAt(m_Cx, m_Cy, m_Cz, 0, 0, 0, 0, 1, 0);

    if(inDrawMethod == GL_POINTS)
        m_glRenderer.RenderPoints(m_points, m_selectedPointId);

    m_glRenderer.RenderMesh(m_Mesh);

}


BOOL CSDIPtCloudView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // Handle mouse wheel scroll to zoom in or out
    if (zDelta > 0)
    {
        // Zoom in
        m_zoomFactor -= 1.0f;
        if (m_zoomFactor < 1.0f)
            m_zoomFactor = 1.0f;
    }
    else if (zDelta < 0)
    {
        // Zoom out
        m_zoomFactor += 1.0f;
        if (m_zoomFactor > 250.0f)
            m_zoomFactor = 250.0f;
    }

    // Update projection
    UpdateProjection();

    // Redraw the view
    Invalidate();

    return CView::OnMouseWheel(nFlags, zDelta, pt);
}
void CSDIPtCloudView::Translate(GLfloat dx, GLfloat dy, GLfloat dz)
{
    m_translateX += dx;
    m_translateY += dy;

    // Redraw the view after translation
    Invalidate();
}

void CSDIPtCloudView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // Handle key press
    switch (nChar)
    {
    case VK_LEFT:
        Translate(-0.1f, 0.0f, 0.0f);
        break;
    case VK_RIGHT:
        Translate(0.1f, 0.0f, 0.0f);
        break;
    case VK_UP:
        Translate(0.0f, 0.1f, 0.0f);
        break;
    case VK_DOWN:
        Translate(0.0f, -0.1f, 0.0f);
        break;
    case 'x':
        m_Cx -= 0.5;
        break;
    case 'y':
        m_Cy -= 0.5;
        break;
    case 'z':
        m_Cz -= 0.5;
        break;
    case 'X':
        m_Cx += 0.5;
        break;
    case 'Y':
        m_Cy += 0.5;
        break;
    case 'Z':
        m_Cz += 0.5;
        break;

    default:
        break;
    }

    // Redraw the view after handling key press
    Invalidate();

    CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CSDIPtCloudView::UpdateProjection()
{
    CRect clientRect;
    GetClientRect(&clientRect);

    int cx = clientRect.Width();
    int cy = clientRect.Height();

    ::glMatrixMode(GL_PROJECTION);
    ::glLoadIdentity();
    ::gluPerspective(m_zoomFactor, (GLfloat)cx / (GLfloat)cy, 0.1f, 2050.0f);
    ::glMatrixMode(GL_MODELVIEW);
}
// CSDIPtCloudView construction/destruction

CSDIPtCloudView::CSDIPtCloudView() : m_translateX(0.0f), m_translateY(0.0f), m_translateZ(0.0f)
{
    m_nMeshCreation = GL_POINTS;
}

CSDIPtCloudView::~CSDIPtCloudView()
{
}

BOOL CSDIPtCloudView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

// CSDIPtCloudView drawing

void CSDIPtCloudView::OnDraw(CDC* /*pDC*/)
{
	CSDIPtCloudDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}

void CSDIPtCloudView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CSDIPtCloudView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CSDIPtCloudView diagnostics

#ifdef _DEBUG
void CSDIPtCloudView::AssertValid() const
{
	CView::AssertValid();
}

void CSDIPtCloudView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSDIPtCloudDoc* CSDIPtCloudView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSDIPtCloudDoc)));
	return (CSDIPtCloudDoc*)m_pDocument;
}
#endif //_DEBUG

int CSDIPtCloudView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;

    // Setup OpenGL context
    HDC hdc = ::GetDC(m_hWnd);

    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 32;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int nPixelFormat = ::ChoosePixelFormat(hdc, &pfd);
    ::SetPixelFormat(hdc, nPixelFormat, &pfd);

    m_hrc = ::wglCreateContext(hdc);
    ::wglMakeCurrent(hdc, m_hrc);

    ::ReleaseDC(m_hWnd, hdc);

    return 0;
}

void CSDIPtCloudView::OnDestroy()
{
    if (m_hrc)
    {
        ::wglMakeCurrent(NULL, NULL);
        ::wglDeleteContext(m_hrc);
        m_hrc = NULL;
    }

    CView::OnDestroy();
}

void CSDIPtCloudView::OnSize(UINT nType, int cx, int cy)
{
    CView::OnSize(nType, cx, cy);

    m_glRenderer.Resize(cx, cy);
}

void CSDIPtCloudView::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    // Set up OpenGL for 3D rendering
    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ::glLoadIdentity();

    DrawPoints(m_nMeshCreation);

    // Swap buffers to display the rendered image
    ::SwapBuffers(dc.m_ps.hdc);
}


void CSDIPtCloudView::ScreenToWorld(int x, int y, GLfloat worldCoord[3])
{
    GLdouble modelview[16];
    GLdouble projection[16];
    GLint viewport[4];

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    GLfloat winX = (float)x;
    GLfloat winY = (float)viewport[3] - (float)y;

    GLfloat winZ;
    glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

    GLdouble Wc1 = 0.0;
    GLdouble Wc2 = 0.0;
    GLdouble Wc3 = 0.0;

    gluUnProject(winX, winY, winZ, modelview, projection, viewport, &Wc1, &Wc2, &Wc3);
    worldCoord[0] = Wc1;
    worldCoord[1] = Wc2;
    worldCoord[2] = Wc3;
}

void CSDIPtCloudView::OnLButtonDown(UINT nFlags, CPoint point)
{
    // Convert screen coordinates to world coordinates
    GLfloat worldCoord[3];
    ScreenToWorld(point.x, point.y, worldCoord);
    m_selectedPointId = -1;
    const float pickThreshold = 0.01f;  // Adjust as needed
    for (const auto& point : m_points) {
        float dist = sqrt((point.x - worldCoord[0]) * (point.x - worldCoord[0]) +
            (point.y - worldCoord[1]) * (point.y - worldCoord[1]) +
            (point.z - worldCoord[2]) * (point.z - worldCoord[2])
        );

        if (dist < pickThreshold) {
            m_selectedPointId = point.m_nId;
            
            break;  // Stop checking once a point is selected
        }
    }

    if (m_selectedPointId > 0)
    {
        DrawPoints(m_nMeshCreation);
        Invalidate();
    }

    CView::OnLButtonDown(nFlags, point);
}

// SDI_PtCloudView.cpp
BOOL CSDIPtCloudView::PreTranslateMessage(MSG* pMsg) {
    switch (pMsg->message) {
    case WM_MBUTTONDOWN:
        GetCursorPos(&m_lastMiddleMousePressScreen); // Capture screen position
        ScreenToClient(&m_lastMiddleMousePressScreen); // Convert to client coordinates if needed
        SetCapture();
        m_totalDistance = 0.0f; // Reset total distance
        return TRUE;

    case WM_MOUSEMOVE:
        if (GetCapture() == this) {
            CPoint currentPos;
            GetCursorPos(&currentPos); // Capture current screen position
            ScreenToClient(&currentPos); // Convert to client coordinates if needed

            CSize delta = currentPos - m_lastMiddleMousePressScreen;
            m_totalDistance = sqrt(static_cast<float>(delta.cx * delta.cx + delta.cy * delta.cy));

           // m_lastMiddleMousePressScreen = currentPos; // Update last position
        }
        return TRUE;

    case WM_MBUTTONUP:
        GetCursorPos(&m_lastMiddleMouseReleaseScreen); // Capture screen position
        ScreenToClient(&m_lastMiddleMouseReleaseScreen); // Convert to client coordinates if needed
        ReleaseCapture();
        // Optionally use m_totalDistance here as needed
        return TRUE;
    }

    return CView::PreTranslateMessage(pMsg);
}


void CSDIPtCloudView::UpdateCameraPosition(CSize delta) {
    // Example: Adjust camera position based on delta
    GLfloat sensitivity = 0.1f; // Adjust sensitivity as needed
    m_Cx += delta.cx * sensitivity;
    m_Cy += delta.cy * sensitivity;
    Invalidate(); // Redraw the view
}