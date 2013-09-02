
// OBJViewerDlg.h : 헤더 파일
//

#include "OpenGLView.h"
#include "ObjParser.h"

#pragma once


// COBJViewerDlg 대화 상자
class COBJViewerDlg : public CDialog
{
// 생성입니다.
public:
	COBJViewerDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_OBJVIEWER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	CRect rect;

	COpenGLView* m_pOpenGLView;
	CObjParser* m_pObjParser;

	CStatic m_lblFilename;
	CComboBox m_BasicShape;

	// options
	BOOL m_blnMouseMode;
	BOOL m_blnVertex;
	BOOL m_blnTexture;
	BOOL m_blnCW;
	BOOL m_blnFlat;
	BOOL m_blnBGR;
	BOOL m_blnDepth;
	BOOL m_blnBlend;
	BOOL m_blnCullface;
	BOOL m_blnMipmap;

	CString m_sFPS;

public:
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedFileopenButton();

	afx_msg void OnBnClickedBgcolorButton();
	afx_msg void OnBnClickedObjcolorButton();
	afx_msg void OnBnClickedResetButton();

	afx_msg void OnBnClickedRadioObj();
	afx_msg void OnBnClickedRadioView();

	afx_msg void OnBnClickedCheckVertex();
	afx_msg void OnBnClickedCheckTexture();

	afx_msg void OnBnClickedRadioCcw();
	afx_msg void OnBnClickedRadioCw();

	afx_msg void OnBnClickedRadioSmooth();
	afx_msg void OnBnClickedRadioFlat();

	afx_msg void OnBnClickedRadioRgb();
	afx_msg void OnBnClickedRadioBgr();

	afx_msg void OnBnClickedCheckDepth();
	afx_msg void OnBnClickedCheckBlend();
	afx_msg void OnBnClickedCheckCullface();
	afx_msg void OnBnClickedCheckMipmap();
	
	afx_msg void OnCbnSelchangeComboShape();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
