
// SDI_PtCloudView.h : interface of the CSDIPtCloudView class
//

#pragma once

////struct Point3D {
//	float x, y, z;
//	float value1, value2, value3, value4;
//};


class CSDIPtCloudView : public CView
{
protected:
	void RotateScene(float angleX, float angleY);
	void OnFileOpen();
	void onCreateMesh();
	void DrawPoints(int nDrawMethod);
	void ClearAndRedraw();
	BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	void Translate(float dx, float dy, float dz);
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void UpdateProjection();
	CPoint m_lastMiddleMousePressScreen;   // Screen position when middle mouse button is pressed
	CPoint m_lastMiddleMouseReleaseScreen; // Screen position when middle mouse button is released
	float m_totalDistance;                 // Total distance traveled
	// create from serialization only
	CSDIPtCloudView();
	DECLARE_DYNCREATE(CSDIPtCloudView)
private:

	CRenderer m_glRenderer;
	CPointCloudManager m_pointCloudManager;
	std::vector<Point3D> m_points;
	std::vector<PT_CLOUD::Vector3D*> m_Vectors;
	std::vector<PT_CLOUD::Triangle*> m_Mesh;
	int m_selectedPointId;
// Attributes
public:
	CSDIPtCloudDoc* GetDocument() const;

	int OnCreate(LPCREATESTRUCT lpCreateStruct);

	void OnDestroy();

	void OnSize(UINT nType, int cx, int cy);

	void OnPaint();
	void ScreenToWorld(int x, int y, float worldCoord[3]);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnMouseMove(UINT nFlags, CPoint point);
	BOOL PreTranslateMessage(MSG* pMsg);
	void UpdateCameraPosition(CSize delta);
	HGLRC m_hrc; // OpenGL rendering context

	float m_translateX;
	float m_translateY;
	float m_translateZ;
	//std::vector<Point3D> m_points;
	//std::vector<PT_CLOUD::Triangle*> m_Mesh;
	float m_Cx = 0;
	float m_Cy = 0;
	float m_Cz = -3;
	float m_zoomFactor = 45.0f; // Initial zoom factor (FOV angle)
	//int m_selectedPointId = -1;
	int m_nMeshCreation = -1;
// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CSDIPtCloudView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SDI_PtCloudView.cpp
inline CSDIPtCloudDoc* CSDIPtCloudView::GetDocument() const
   { return reinterpret_cast<CSDIPtCloudDoc*>(m_pDocument); }
#endif

