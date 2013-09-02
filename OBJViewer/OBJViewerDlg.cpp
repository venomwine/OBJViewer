/* OBJViewer Copyright (C) 2013 Hong, Wooram */
// OBJViewerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "OBJViewer.h"
#include "OBJViewerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

// Obj File Data
vector	< Vert3D >		vertexArray;
vector	< TexUV >		textureArray;
vector	< Norm3D >		normal3DArray;
vector	< IndexVTN >	indexArray;
vector	< TexFile >		textureFileNameArray;

// Vertex, Texture Real Array
VectCoordType *vertices_Tri;
TexCoordType *textures;
NormCoordType *normals;

// Array Size
int g_nIndexSize, g_nVertexSize, g_nTextureSize;

// File Type
int g_nFileType;

// Vertex File Name Num
int g_nVertexFileNum;

// FirstVal -> Min = Max
BOOL g_blnFirstX, g_blnFirstY, g_blnFirstZ;

// FPS
DWORD g_dwFPS; 

// textureID
GLuint *g_textureID;

// TextureImagePath;
CString g_sTextureImagePath;

// COBJViewerDlg 대화 상자

COBJViewerDlg::COBJViewerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COBJViewerDlg::IDD, pParent)
	, m_blnVertex(FALSE)
	, m_blnTexture(TRUE)
	, m_blnCW(FALSE)
	, m_blnFlat(FALSE)
	, m_blnBGR(TRUE)
	, m_blnDepth(TRUE)
	, m_blnBlend(TRUE)
	, m_blnCullface(TRUE)
	, m_blnMipmap(TRUE)
	, m_blnMouseMode(TRUE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pOpenGLView = new COpenGLView;
}

void COBJViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LBLFILENAME, m_lblFilename);
	DDX_Radio(pDX, IDC_RADIO_OBJ, m_blnMouseMode);
	DDX_Check(pDX, IDC_CHECK_VERTEX, m_blnVertex);
	DDX_Check(pDX, IDC_CHECK_TEXTURE, m_blnTexture);
	DDX_Radio(pDX, IDC_RADIO_CCW, m_blnCW);
	DDX_Radio(pDX, IDC_RADIO_SMOOTH, m_blnFlat);
	DDX_Radio(pDX, IDC_RADIO_RGB, m_blnBGR);
	DDX_Check(pDX, IDC_CHECK_DEPTH, m_blnDepth);
	DDX_Check(pDX, IDC_CHECK_BLEND, m_blnBlend);
	DDX_Check(pDX, IDC_CHECK_CULLFACE, m_blnCullface);
	DDX_Check(pDX, IDC_CHECK_MIPMAP, m_blnMipmap);
	DDX_Control(pDX, IDC_COMBO_SHAPE, m_BasicShape);

}

BEGIN_MESSAGE_MAP(COBJViewerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_FILEOPEN_BUTTON, &COBJViewerDlg::OnBnClickedFileopenButton)
	ON_BN_CLICKED(IDC_BGCOLOR_BUTTON, &COBJViewerDlg::OnBnClickedBgcolorButton)
	ON_BN_CLICKED(IDC_RADIO_OBJ, &COBJViewerDlg::OnBnClickedRadioObj)
	ON_BN_CLICKED(IDC_RADIO_VIEW, &COBJViewerDlg::OnBnClickedRadioView)
	ON_BN_CLICKED(IDC_RESET_BUTTON, &COBJViewerDlg::OnBnClickedResetButton)
	ON_BN_CLICKED(IDC_OBJCOLOR_BUTTON, &COBJViewerDlg::OnBnClickedObjcolorButton)
	ON_BN_CLICKED(IDC_CHECK_VERTEX, &COBJViewerDlg::OnBnClickedCheckVertex)
	ON_BN_CLICKED(IDC_CHECK_TEXTURE, &COBJViewerDlg::OnBnClickedCheckTexture)
	ON_BN_CLICKED(IDC_RADIO_CCW, &COBJViewerDlg::OnBnClickedRadioCcw)
	ON_BN_CLICKED(IDC_RADIO_CW, &COBJViewerDlg::OnBnClickedRadioCw)
	ON_BN_CLICKED(IDC_RADIO_SMOOTH, &COBJViewerDlg::OnBnClickedRadioSmooth)
	ON_BN_CLICKED(IDC_RADIO_FLAT, &COBJViewerDlg::OnBnClickedRadioFlat)
	ON_BN_CLICKED(IDC_RADIO_RGB, &COBJViewerDlg::OnBnClickedRadioRgb)
	ON_BN_CLICKED(IDC_RADIO_BGR, &COBJViewerDlg::OnBnClickedRadioBgr)
	ON_BN_CLICKED(IDC_CHECK_DEPTH, &COBJViewerDlg::OnBnClickedCheckDepth)
	ON_BN_CLICKED(IDC_CHECK_BLEND, &COBJViewerDlg::OnBnClickedCheckBlend)
	ON_BN_CLICKED(IDC_CHECK_CULLFACE, &COBJViewerDlg::OnBnClickedCheckCullface)
	ON_BN_CLICKED(IDC_CHECK_MIPMAP, &COBJViewerDlg::OnBnClickedCheckMipmap)
	ON_CBN_SELCHANGE(IDC_COMBO_SHAPE, &COBJViewerDlg::OnCbnSelchangeComboShape)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// COBJViewerDlg 메시지 처리기

BOOL COBJViewerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	// 그릴 윈도우의 좌표 획득
	GetDlgItem(IDC_DISPLAY)->GetWindowRect(rect);
	ScreenToClient(rect);

	// view 생성
	m_pOpenGLView->Create(NULL, NULL, WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_VISIBLE,
		rect, this, 0);

	// 초기화
	m_lblFilename.SetWindowText(_T(""));

	m_BasicShape.SetCurSel(GL_TRIANGLES-1);	// 0 번 째 GL_POINTS 는 따로 구현함.
	m_pOpenGLView->SetBasicShape(GL_TRIANGLES-1);

	m_pOpenGLView->InitOptionChk();

	SetTimer(0, 200, NULL);
	UpdateData(FALSE);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void COBJViewerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR COBJViewerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void COBJViewerDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
	if(m_pOpenGLView) delete m_pOpenGLView;
}

void COBJViewerDlg::OnBnClickedFileopenButton()
{
	// TODO: Add your control notification handler code here

	TCHAR strFilter[] = { _T("OBJ Files (*.obj)|*.obj|MQO Files (*.mqo)|*.mqo|All Files (*.*)|*.*||") };
	CFileDialog FileDlg(TRUE, _T(".obj"), NULL, 0, strFilter);

	if( FileDlg.DoModal() == IDOK )
	{
		m_lblFilename.SetWindowText(FileDlg.GetPathName());

		// TESTTODO: Should Parse Obj File Here

		// Obj File 의 데이터 초기화
		normal3DArray.clear();
		textureArray.clear();
		vertexArray.clear();
		indexArray.clear();
		textureFileNameArray.clear();

		CStdioFile ObjFile;
		
		g_sTextureImagePath = FileDlg.GetPathName();

		if(!ObjFile.Open(g_sTextureImagePath, CFile::modeRead))
		{
			AfxMessageBox(_T("열 수 없는 파일"));
			return;
		}

		// Object 범위 초기화
		g_blnFirstX = g_blnFirstY = g_blnFirstZ = TRUE;

		// Array Size 초기화
		g_nIndexSize = g_nVertexSize = g_nTextureSize  = 0;
		g_nVertexFileNum = 0;

		CString sLineBuf;

		// Check OBJ File Type
		ObjFile.ReadString(sLineBuf);
		m_pObjParser->ChkFileType(sLineBuf);

		// Data Parsing
		while(ObjFile.ReadString(sLineBuf))
		{
			m_pObjParser->ObjDataParser(sLineBuf);
		}
		m_pObjParser->MakeDataArray();

		// Texture (이미지) Load
		if(g_nFileType != _MQO) 
			m_pOpenGLView->LoadTexture(m_pObjParser->MtlParser(g_sTextureImagePath));
		else
			m_pOpenGLView->LoadTexture(g_sTextureImagePath);

		ObjFile.Close();

		m_pOpenGLView->SetModel(TRUE);
		m_pOpenGLView->RedrawWindow();

	}
	else
		return;

}

void COBJViewerDlg::OnBnClickedBgcolorButton()
{
	// TODO: Add your control notification handler code here
	
	CColorDialog colorDlg(RGB(m_pOpenGLView->GetClear_R_Color(),m_pOpenGLView->GetClear_G_Color(),m_pOpenGLView->GetClear_B_Color()), CC_FULLOPEN);
	colorDlg.DoModal();
	COLORREF color = colorDlg.GetColor();

	m_pOpenGLView->SetClearColor(GetRValue(color), GetGValue(color), GetBValue(color));

	m_pOpenGLView->RedrawWindow();
}

void COBJViewerDlg::OnBnClickedObjcolorButton()
{
	// TODO: Add your control notification handler code here
	
	CColorDialog colorDlg(RGB(m_pOpenGLView->GetObj_R_Color(),m_pOpenGLView->GetObj_G_Color(),m_pOpenGLView->GetObj_B_Color()), CC_FULLOPEN);
	colorDlg.DoModal();
	COLORREF color = colorDlg.GetColor();

	m_pOpenGLView->SetObjColor(GetRValue(color), GetGValue(color), GetBValue(color));

	m_pOpenGLView->RedrawWindow();
}

void COBJViewerDlg::OnBnClickedRadioObj()
{
	// TODO: Add your control notification handler code here

	m_blnMouseMode = FALSE;

	m_pOpenGLView->SetMouseChk(FALSE);
	m_pOpenGLView->SetModel(FALSE);
	m_pOpenGLView->RedrawWindow();
}

void COBJViewerDlg::OnBnClickedRadioView()
{
	// TODO: Add your control notification handler code here

	m_blnMouseMode = TRUE;

	m_pOpenGLView->SetMouseChk(TRUE);
	m_pOpenGLView->SetModel(FALSE);
	m_pOpenGLView->RedrawWindow();
}

void COBJViewerDlg::OnBnClickedResetButton()
{
	// TODO: Add your control notification handler code here

	m_pOpenGLView->SetModel(TRUE);

	m_pOpenGLView->RedrawWindow();
}

void COBJViewerDlg::OnBnClickedCheckVertex()
{
	// TODO: Add your control notification handler code here

	if(m_blnVertex == TRUE)
	{
		m_blnVertex = FALSE;
		m_pOpenGLView->SetVertexChk(FALSE);
	}
	else if(m_blnVertex == FALSE)
	{
		m_blnVertex = TRUE;
		m_pOpenGLView->SetVertexChk(TRUE);
	}

	m_pOpenGLView->RedrawWindow();

}

void COBJViewerDlg::OnBnClickedCheckTexture()
{
	// TODO: Add your control notification handler code here

	if(m_blnTexture == TRUE)
	{
		m_blnTexture = FALSE;
		m_pOpenGLView->SetTextureChk(FALSE);
	}
	else if(m_blnTexture == FALSE)
	{
		m_blnTexture = TRUE;
		m_pOpenGLView->SetTextureChk(TRUE);
	}

	m_pOpenGLView->RedrawWindow();
}

void COBJViewerDlg::OnBnClickedRadioCcw()
{
	// TODO: Add your control notification handler code here
	
	m_blnCW = FALSE;

	m_pOpenGLView->SetCwChk(FALSE);
	m_pOpenGLView->SetModel(FALSE);
	m_pOpenGLView->RedrawWindow();
}

void COBJViewerDlg::OnBnClickedRadioCw()
{
	// TODO: Add your control notification handler code here

	m_blnCW = TRUE;

	m_pOpenGLView->SetCwChk(TRUE);
	m_pOpenGLView->SetModel(FALSE);
	m_pOpenGLView->RedrawWindow();
}

void COBJViewerDlg::OnBnClickedRadioSmooth()
{
	// TODO: Add your control notification handler code here

	m_blnFlat = FALSE;

	m_pOpenGLView->SetFlatChk(FALSE);
	m_pOpenGLView->SetModel(FALSE);
	m_pOpenGLView->RedrawWindow();
}

void COBJViewerDlg::OnBnClickedRadioFlat()
{
	// TODO: Add your control notification handler code here

	m_blnFlat = TRUE;

	m_pOpenGLView->SetFlatChk(TRUE);
	m_pOpenGLView->SetModel(FALSE);
	m_pOpenGLView->RedrawWindow();
}

void COBJViewerDlg::OnBnClickedRadioRgb()
{
	// TODO: Add your control notification handler code here

	m_blnBGR = FALSE;

	m_pOpenGLView->SetBGRChk(FALSE);
	if(g_nFileType != _MQO) 
		m_pOpenGLView->LoadTexture(m_pObjParser->MtlParser(g_sTextureImagePath));
	else
		m_pOpenGLView->LoadTexture(g_sTextureImagePath);

	m_pOpenGLView->RedrawWindow();
}

void COBJViewerDlg::OnBnClickedRadioBgr()
{
	// TODO: Add your control notification handler code here

	m_blnBGR = TRUE;

	m_pOpenGLView->SetBGRChk(TRUE);
	if(g_nFileType != _MQO) 
		m_pOpenGLView->LoadTexture(m_pObjParser->MtlParser(g_sTextureImagePath));
	else
		m_pOpenGLView->LoadTexture(g_sTextureImagePath);

	m_pOpenGLView->RedrawWindow();
}

void COBJViewerDlg::OnBnClickedCheckDepth()
{
	// TODO: Add your control notification handler code here

	if(m_blnDepth == TRUE)
	{
		m_blnDepth = FALSE;
		m_pOpenGLView->SetDepthChk(FALSE);
	}
	else if(m_blnDepth == FALSE)
	{
		m_blnDepth = TRUE;
		m_pOpenGLView->SetDepthChk(TRUE);
	}

	m_pOpenGLView->SetModel(FALSE);
	m_pOpenGLView->RedrawWindow();
}

void COBJViewerDlg::OnBnClickedCheckBlend()
{
	// TODO: Add your control notification handler code here
	
	if(m_blnBlend == TRUE)
	{
		m_blnBlend = FALSE;
		m_pOpenGLView->SetBlendChk(FALSE);
	}
	else if(m_blnBlend == FALSE)
	{
		m_blnBlend = TRUE;
		m_pOpenGLView->SetBlendChk(TRUE);
	}

	m_pOpenGLView->SetModel(FALSE);
	m_pOpenGLView->RedrawWindow();
}

void COBJViewerDlg::OnBnClickedCheckCullface()
{
	// TODO: Add your control notification handler code here
	
	if(m_blnCullface == TRUE)
	{
		m_blnCullface = FALSE;
		m_pOpenGLView->SetCullfaceChk(FALSE);
	}
	else if(m_blnCullface == FALSE)
	{
		m_blnCullface = TRUE;
		m_pOpenGLView->SetCullfaceChk(TRUE);
	}

	m_pOpenGLView->SetModel(FALSE);
	m_pOpenGLView->RedrawWindow();
}

void COBJViewerDlg::OnBnClickedCheckMipmap()
{
	// TODO: Add your control notification handler code here

	if(m_blnMipmap == TRUE)
	{
		m_blnMipmap = FALSE;
		m_pOpenGLView->SetMipmapChk(FALSE);
	}
	else if(m_blnMipmap == FALSE)
	{
		m_blnMipmap = TRUE;
		m_pOpenGLView->SetMipmapChk(TRUE);
	}

	if(g_nFileType != _MQO) 
		m_pOpenGLView->LoadTexture(m_pObjParser->MtlParser(g_sTextureImagePath));
	else
		m_pOpenGLView->LoadTexture(g_sTextureImagePath);

	m_pOpenGLView->RedrawWindow();
}

void COBJViewerDlg::OnCbnSelchangeComboShape()
{
	// TODO: Add your control notification handler code here
	m_pOpenGLView->SetBasicShape(m_BasicShape.GetCurSel());
	m_pOpenGLView->RedrawWindow();
}

BOOL COBJViewerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	switch(pMsg->message)
	{
	case WM_KEYDOWN :
	case VK_ESCAPE :
		return TRUE;
		break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void COBJViewerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	m_sFPS.Format(_T("OBJViewer [%.2d Frames / second]"), g_dwFPS);
	SetWindowText(m_sFPS);

	m_pOpenGLView->RedrawWindow();

	CDialog::OnTimer(nIDEvent);
}
