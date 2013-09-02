#pragma once


// COpenGLView view

class COpenGLView : public CView
{
	DECLARE_DYNCREATE(COpenGLView)

public:
	COpenGLView();           // protected constructor used by dynamic creation
	virtual ~COpenGLView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
private:
	HGLRC m_hglrc;
	HDC m_hdc;
	int m_width;
	int m_height;

protected:
	DECLARE_MESSAGE_MAP()

private:
	// Texture Data
	BYTE * m_texRawData;

	// Eye Position
	float m_fltEyeX, m_fltEyeY, m_fltEyeZ;
	
	// View
	float m_fltDistance;
	int m_nAltitude, m_nAzimuth;

	// Mouse Button
	bool m_blnMousingLeftBTN;
	bool m_blnMousingRightBTN;

	bool m_blnFirstLeftClick;
	bool m_blnFirstRightClick;

	int m_nMousingstep;

	// Object
	float m_fltObjX_P, m_fltObjZ_P;
	float m_fltObjScale;
	int m_nObjR;

	// Texture
	int m_texWidth , m_texHeight;

	// Screen
	LONG m_lngScreenX, m_lngScreenY;
	LONG m_lngScreenX_Old, m_lngScreenY_Old;

	// Options
	bool m_blnMouseChk;
	bool m_blnVertexChk;
	bool m_blnTextureChk;
	bool m_blnCwChk;
	bool m_blnFlatChk;
	bool m_blnBGRChk;
	bool m_blnDepthChk;
	bool m_blnBlendChk;
	bool m_blnCullfaceChk;
	bool m_blnMipmapChk;

	// Color
	int m_nClear_color_R, m_nClear_color_G, m_nClear_color_B;
	int m_nObj_color_R, m_nObj_color_G, m_nObj_color_B;

	// Basic Shape
	int m_nBasicShape;

public:
	int SetupPixelFormat(HDC hdc);
	void LoadTexture(CString sFileName);
	void InitOptionChk();
	void GetScreenPoint(CPoint point);
	void ResetOBJPosition();

	void SetModel(bool reset);
	void SetEyePoint();
	void SetOldScreenPoint();
	void SetTexture();

	void SetBasicShape(int type);

	// Color
	int GetClear_R_Color();
	int GetClear_G_Color();
	int GetClear_B_Color();
	int GetObj_R_Color();
	int GetObj_G_Color();
	int GetObj_B_Color();

	void SetClearColor(int R, int G, int B);
	void SetObjColor(int R, int G, int B);

	// Check
	void SetMouseChk(bool setting);
	void SetVertexChk(bool setting);
	void SetTextureChk(bool setting);
	void SetCwChk(bool setting);
	void SetFlatChk(bool setting);
	void SetBGRChk(bool setting);
	void SetDepthChk(bool setting);
	void SetBlendChk(bool setting);
	void SetCullfaceChk(bool setting);
	void SetMipmapChk(bool setting);

	// Options
	void ChkVertex_N_Draw();
	void ChkTexture_N_Draw();
	void ChkCw_Mode();
	void ChkFlat_Mode();
	void ChkDepth_Mode();
	void ChkBlend_Mode();
	void ChkCullface_Mode();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	void CheckFPS();
};


