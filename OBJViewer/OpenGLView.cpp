/* OBJViewer Copyright (C) 2013 Hong, Wooram */
// OpenGLView.cpp : implementation file
//

#include "stdafx.h"
#include "OBJViewer.h"
#include "OpenGLView.h"

#include <mmsystem.h>

using namespace std;

// Obj File Data
extern vector	< Vert3D >		vertexArray;
extern vector	< TexUV >		textureArray;
extern vector	< Norm3D >		normal3DArray;
extern vector	< IndexVTN >	indexArray;
extern vector	< TexFile >		textureFileNameArray;


// Vertex, Texture Real Array
extern VectCoordType *vertices_Tri;
extern TexCoordType *textures;

// Object 범위
extern VectCoordType g_MinX, g_MinY, g_MinZ;
extern VectCoordType g_MaxX, g_MaxY, g_MaxZ;

// Array Size
extern int g_nIndexSize, g_nVertexSize, g_nTextureSize;

// File Type
extern int g_nFileType;

// Vertex File Name Num
extern int g_nVertexFileNum;

// FPS
extern DWORD g_dwFPS; 

// textureID
extern GLuint *g_textureID;

// COpenGLView

IMPLEMENT_DYNCREATE(COpenGLView, CView)

COpenGLView::COpenGLView()
{
	m_width=0;
	m_height=0;
	m_hglrc=NULL;
	m_hdc=NULL;
	m_blnMousingLeftBTN = FALSE;
	m_blnMousingRightBTN = FALSE;
}

COpenGLView::~COpenGLView()
{
}

BEGIN_MESSAGE_MAP(COpenGLView, CView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
ON_WM_RBUTTONDOWN()
ON_WM_RBUTTONUP()
ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// COpenGLView drawing

void COpenGLView::ResetOBJPosition()
{
	m_nClear_color_R=200;
	m_nClear_color_G=242;
	m_nClear_color_B=121;

	m_nObj_color_R=255;
	m_nObj_color_G=255;
	m_nObj_color_B=255;
	
	// View
	m_fltDistance = 150.0f;
	m_nAltitude = 0;
	m_nAzimuth = 0;

	m_fltObjX_P = m_fltObjZ_P = 0;

	m_fltObjScale = 1.0f;
	m_nObjR = 0;

	m_fltEyeX = 0;
	m_fltEyeY = 0;
	m_fltEyeZ = m_fltDistance;

	m_blnMousingLeftBTN = FALSE;
	m_blnMousingRightBTN = FALSE;
}

void COpenGLView::SetModel(bool reset)
{
	if(reset) ResetOBJPosition();
	
	glClearDepth(1 << 16);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(60.0f, 1, 1.0f, 1000.0f);
	//gluOrtho(-100.0f, 100.0f , -100.0f, 100.0f, -1000.0, 1000.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(m_fltEyeX, m_fltEyeY, m_fltEyeZ, 0, 0, 0, 0, 1, 0);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	ChkFlat_Mode();
	ChkDepth_Mode();
	ChkBlend_Mode();
	ChkCw_Mode();
	ChkCullface_Mode();
}

void COpenGLView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here

	HDC hdc = ::GetDC(m_hWnd);

	glClearColor(((GLclampf)m_nClear_color_R)/255, ((GLclampf)m_nClear_color_G)/255, ((GLclampf)m_nClear_color_B)/255, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(((GLclampf)m_nObj_color_R)/255, ((GLclampf)m_nObj_color_G)/255, ((GLclampf)m_nObj_color_B)/255);


	glPushMatrix();
	
	glTranslatef(m_fltObjX_P, 0, m_fltObjZ_P);	
	glScalef(m_fltObjScale, m_fltObjScale, m_fltObjScale);
	//if(g_nFileType != _MQO)		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);	// 기본
	glRotatef((float)m_nObjR, 0.0f, 0.0f, 1.0f);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glVertexPointer(3, GL_DOUBLE, 0, vertices_Tri);
	glTexCoordPointer(2, GL_FLOAT, 0, textures);

	ChkTexture_N_Draw();	

	switch(g_nFileType)
	{
	case _NAVTEQOBJ:
	case _NORMALOBJ:
		glDrawArrays(m_nBasicShape, 0, g_nIndexSize);
		break;
	case _MQO:
		for(int i = 0; i < g_nIndexSize/3; i++)
		{
			glBindTexture(GL_TEXTURE_2D, g_textureID[indexArray[i*3].texFileIndex]);
			glDrawArrays(m_nBasicShape, i*3, 3);
		}
	}
	
	ChkVertex_N_Draw();

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
	

	glFlush();

	SwapBuffers(hdc);

	CheckFPS();
}

void COpenGLView::CheckFPS()
{
	static DWORD FPS_Frames=0;	
	static float FPS_Num, FPS_LastTime=0;

	if(FPS_Frames == 0)	FPS_LastTime = (float)timeGetTime()/1000.0f;	

	float	FPS_Time = (float)timeGetTime()/1000.0f;

	if(FPS_Time - FPS_LastTime < 1.0f)
	{ 
		FPS_Frames++;
	}
	else
	{	
		FPS_Num = FPS_Frames/((FPS_Time-FPS_LastTime));	
		FPS_Frames = 0;
		FPS_LastTime = FPS_Time;
		g_dwFPS = (DWORD)FPS_Num;
	}
}

// COpenGLView diagnostics

#ifdef _DEBUG
void COpenGLView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void COpenGLView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG

int COpenGLView::SetupPixelFormat(HDC hdc)
{
	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR), //size of this pfd
		1, //version number
		PFD_DRAW_TO_WINDOW | //support window
		PFD_SUPPORT_OPENGL| //supprt opengl
		PFD_DOUBLEBUFFER, //double buffering
		PFD_TYPE_RGBA, //RGBA type
		24, //24bit color depth
		0,0,0,0,0,0, //color bit ignored
		0, //no alpha buffer
		0, //shift bit ignored
		0, //no accumulation buffer
		0,0,0,0, //accumm bits ignored
		32, //32bit z buffer
		0,
		0,
		PFD_MAIN_PLANE, //main layer
		0,
		0,0,0
	};
	int iPixelFormat;
	//get the device context's best, abailable pixel format match
	if((iPixelFormat = ChoosePixelFormat(hdc,&pfd))==0)
	{
		MessageBox(_T("ChoosePixelFormat Failed!"),_T("Error"),MB_OK);
		return 0;
	}
	//make that match the device context's current pixel format
	if(SetPixelFormat(hdc,iPixelFormat,&pfd)==FALSE)
	{
		MessageBox(_T("SetPixelFormat Failed!"),_T("Error"),MB_OK);
		return 0;
	}
	return 1;
}

// COpenGLView message handlers

int COpenGLView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	m_hdc = ::GetDC(m_hWnd);
	SetupPixelFormat(m_hdc);

	m_hglrc = wglCreateContext(m_hdc);
	wglMakeCurrent(m_hdc,m_hglrc);

	RECT rect;
	this->GetWindowRect(&rect);
	m_width=rect.bottom-rect.top;
	m_height=rect.right-rect.left;

	glViewport(0, 0, m_width, m_height);
	glDepthRange(0, 1);

	return 0;
}

void COpenGLView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: Add your message handler code here
	wglMakeCurrent(NULL,NULL);
	::ReleaseDC(m_hWnd,m_hdc);
	wglDeleteContext(m_hglrc);

	if(vertices_Tri) delete[] vertices_Tri;
	if(textures) delete[] textures;
}

void COpenGLView::LoadTexture(CString sFileName)
{
	CxImage image;
	CString sTextureChipPath, sTextureChipName;
	int pos = 0, m_texType = 0;
	m_texWidth = 0;
	m_texHeight = 0;

	switch(g_nFileType)
	{
	case _NORMALOBJ:
	case _NAVTEQOBJ:
		if(image.Load(sFileName,CXIMAGE_FORMAT_TGA))
		{
			if(image.GetBpp() == 24)
				m_texType=TEX_TYPE_RGB;
			else
				m_texType=TEX_TYPE_RGBA;
			m_texWidth=(int)image.GetWidth();
			m_texHeight=(int)image.GetHeight();
			int imageSize;
			if(m_texType==TEX_TYPE_RGB)
				imageSize=m_texWidth*m_texHeight*3;
			else
				imageSize=m_texWidth*m_texHeight*4;
			m_texRawData=(BYTE*)malloc(imageSize);
			memcpy(m_texRawData,image.GetBits(),imageSize);

			SetTexture();
			free(m_texRawData);
		}

		break;

	case _MQO:
		if(g_textureID) delete[] g_textureID;
		g_textureID=new GLuint[g_nVertexFileNum];

		glGenTextures(g_nVertexFileNum, &g_textureID[0]);

		sTextureChipPath = sFileName.Mid(0, sFileName.ReverseFind('\\'));

		for(int i = 0; i < g_nVertexFileNum; i++)	// texture 수 만큼 반복
		{
			sTextureChipName = 	sTextureChipPath + _T("\\") + textureFileNameArray.at(i).sTextureFileName;
			if(image.Load(sTextureChipName,CXIMAGE_FORMAT_BMP))
			{
				if(image.GetBpp() == 8)	// 8bits bitmap 은 24bit로 변환
					image.IncreaseBpp(24);

				if(image.GetBpp() == 24)
					m_texType=TEX_TYPE_RGB;
				else
					m_texType=TEX_TYPE_RGBA;
				m_texWidth=(int)image.GetWidth();
				m_texHeight=(int)image.GetHeight();
				int imageSize;
				if(m_texType==TEX_TYPE_RGB)
					imageSize=m_texWidth*m_texHeight*3;
				else
					imageSize=m_texWidth*m_texHeight*4;
				m_texRawData=(BYTE*)malloc(imageSize);
				image.Flip(TRUE, FALSE);	// BMP 는 뒤집혀서 들어옴
				memcpy(m_texRawData,image.GetBits(),imageSize);

				glBindTexture(GL_TEXTURE_2D, g_textureID[i]);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

				SetTexture();
				free(m_texRawData);
			}
		}
		break;
	}
}

void COpenGLView::SetTexture()
{
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if(m_blnBGRChk == TRUE)			//BGR
	{
		if(m_blnMipmapChk == TRUE)
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
			gluBuild2DMipmaps( GL_TEXTURE_2D, 4, m_texWidth,m_texHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, m_texRawData);
		}
		else if(m_blnMipmapChk == FALSE)
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_texWidth, m_texHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, m_texRawData);
		}
	}
	else if(m_blnBGRChk == FALSE)	//RGB
	{
		if(m_blnMipmapChk == TRUE)
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
			gluBuild2DMipmaps( GL_TEXTURE_2D, 4, m_texWidth, m_texHeight, GL_RGB, GL_UNSIGNED_BYTE, m_texRawData);
		}
		else if(m_blnMipmapChk == FALSE)
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_texWidth, m_texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, m_texRawData);
		}
	}
}

void COpenGLView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CView::OnLButtonDown(nFlags, point);

	m_blnMousingLeftBTN = TRUE;
	m_blnFirstLeftClick = TRUE;
	
}

void COpenGLView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CView::OnLButtonUp(nFlags, point);

	m_blnMousingLeftBTN = FALSE;
}

void COpenGLView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CView::OnRButtonDown(nFlags, point);

	m_blnMousingRightBTN = TRUE;
	m_blnFirstRightClick = TRUE;
}

void COpenGLView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CView::OnRButtonUp(nFlags, point);

	m_blnMousingRightBTN = FALSE;
}

void COpenGLView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CView::OnMouseMove(nFlags, point);

	switch(m_blnMouseChk)
	{
	case 0:		// Obj
		if( m_blnMousingLeftBTN )	// Mouse Left Button
		{
			GetScreenPoint(point);

			m_blnFirstLeftClick ? m_nMousingstep = 0 : m_nMousingstep = abs(m_lngScreenX - m_lngScreenX_Old);
			if(m_lngScreenX > m_lngScreenX_Old)		// Obj 위치 X	
				m_fltObjX_P += m_nMousingstep;	
			else if(m_lngScreenX < m_lngScreenX_Old)	
				m_fltObjX_P -= m_nMousingstep;

			m_blnFirstLeftClick ? m_nMousingstep = 0 : m_nMousingstep = abs(m_lngScreenY - m_lngScreenY_Old);
			if(m_lngScreenY > m_lngScreenY_Old)		// Obj 위치 Z	
				m_fltObjZ_P += m_nMousingstep;	
			else if(m_lngScreenY < m_lngScreenY_Old)	
				m_fltObjZ_P -= m_nMousingstep;

			SetOldScreenPoint();

			RedrawWindow();

			m_blnFirstLeftClick = FALSE;
		}
		else if(m_blnMousingRightBTN)	// Mouse Right Button
		{
			GetScreenPoint(point);

			m_blnFirstRightClick ? m_nMousingstep = 0 : m_nMousingstep = abs(m_lngScreenX - m_lngScreenX_Old);
			if(m_lngScreenX > m_lngScreenX_Old)		// Obj 회전	
				m_nObjR = (m_nObjR + m_nMousingstep) % 360;	
			else if(m_lngScreenX < m_lngScreenX_Old)	
				m_nObjR = (m_nObjR - m_nMousingstep) % 360;

			m_blnFirstRightClick ? m_nMousingstep = 0 : m_nMousingstep = abs(m_lngScreenY - m_lngScreenY_Old);
			if(m_lngScreenY > m_lngScreenY_Old)		// Obj 크기	
				m_fltObjScale -= ((float)m_nMousingstep / 100);	
			else if(m_lngScreenY < m_lngScreenY_Old)	
				m_fltObjScale += ((float)m_nMousingstep / 100);

			SetOldScreenPoint();

			RedrawWindow();
			
			m_blnFirstRightClick = FALSE;
		}
		break;

	case 1:	// View
		if( m_blnMousingLeftBTN )	// Mouse Left Button
		{
			GetScreenPoint(point);

			m_blnFirstLeftClick ? m_nMousingstep = 0 : m_nMousingstep = abs(m_lngScreenX - m_lngScreenX_Old);
			if(m_lngScreenX > m_lngScreenX_Old)		// View 방위		
				m_nAzimuth = (m_nAzimuth - m_nMousingstep) % 360; 
			else if(m_lngScreenX < m_lngScreenX_Old)	
				m_nAzimuth = (m_nAzimuth + m_nMousingstep) % 360; 

			m_blnFirstLeftClick ? m_nMousingstep = 0 : m_nMousingstep = abs(m_lngScreenY - m_lngScreenY_Old); 
			if(m_lngScreenY > m_lngScreenY_Old)		// View 고도		
			{
				if(m_nAltitude > 0)		
					m_nAltitude -= m_nMousingstep; 
			}
			else if(m_lngScreenY < m_lngScreenY_Old)	
			{
				if(m_nAltitude < 87)	
					m_nAltitude += m_nMousingstep; 
			}
			
			SetEyePoint();
			SetOldScreenPoint();

			SetModel(FALSE);
			RedrawWindow();

			m_blnFirstLeftClick = FALSE;
		}
		else if(m_blnMousingRightBTN)	// Mouse Right Button
		{
			GetScreenPoint(point);

			m_blnFirstRightClick ? m_nMousingstep = 0 : m_nMousingstep = abs(m_lngScreenY - m_lngScreenY_Old);
			if(m_lngScreenY > m_lngScreenY_Old)		// View 거리
			{
				m_fltDistance += m_nMousingstep; 
			}
			else if(m_lngScreenY < m_lngScreenY_Old)	
			{
				if(m_fltDistance > 0)		
					m_fltDistance -= m_nMousingstep; 
			}

			SetEyePoint();
			SetOldScreenPoint();

			SetModel(FALSE);
			RedrawWindow();

			m_blnFirstRightClick = FALSE;
		}
		break;
	}
}

BOOL COpenGLView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	//return CView::OnEraseBkgnd(pDC);
	return FALSE;
}

void COpenGLView::SetEyePoint()
{
	m_fltEyeX = -m_fltDistance * (float)( sin(_RADIAN * (90-m_nAltitude)) * sin(_RADIAN * m_nAzimuth) );
	m_fltEyeY = m_fltDistance * (float)( cos(_RADIAN * (90-m_nAltitude)));
	m_fltEyeZ = m_fltDistance * (float)( sin(_RADIAN * (90-m_nAltitude)) * cos(_RADIAN * m_nAzimuth) );
}

void COpenGLView::SetOldScreenPoint()
{
	m_lngScreenX_Old = m_lngScreenX;
	m_lngScreenY_Old = m_lngScreenY;
}

void COpenGLView::GetScreenPoint(CPoint point)
{
	m_lngScreenX = point.x;
	m_lngScreenY = point.y;
}

void COpenGLView::InitOptionChk()
{
	m_blnVertexChk	= FALSE;
	m_blnBGRChk		= TRUE;
	m_blnTextureChk = TRUE;
	m_blnCwChk		= FALSE;
	m_blnFlatChk	= FALSE;
	m_blnDepthChk	= TRUE;
	m_blnBlendChk	= TRUE;
	m_blnCullfaceChk= TRUE;
	m_blnMipmapChk	= TRUE;
	m_blnMouseChk	= TRUE;

	ResetOBJPosition();
}

void COpenGLView::ChkVertex_N_Draw()
{
	if(m_blnVertexChk)
	{
		glColor3f(0,0,0);
		glPointSize(4.0f);
		glDrawArrays(GL_POINTS, 0, g_nIndexSize);
	}
}

void COpenGLView::ChkTexture_N_Draw()
{
	if(m_blnTextureChk) 
	{
		glEnable(GL_TEXTURE_2D);
	}
}

void COpenGLView::ChkCw_Mode()
{
	if(m_blnCwChk == FALSE)
	{
		glFrontFace(GL_CCW);
	}
	else if(m_blnCwChk == TRUE)
	{
		glFrontFace(GL_CW);
	}
}

void COpenGLView::ChkFlat_Mode()
{
	if(m_blnFlatChk == FALSE)
	{
		glShadeModel(GL_SMOOTH);
	}
	else if(m_blnFlatChk == TRUE)
	{
		glShadeModel(GL_FLAT);
	}
}

void COpenGLView::ChkDepth_Mode()
{
	if(m_blnDepthChk == TRUE)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
	}
	else if(m_blnDepthChk == FALSE)
	{
		glDisable(GL_DEPTH_TEST);
	}
}

void COpenGLView::ChkBlend_Mode()
{
	if(m_blnBlendChk == TRUE)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}
	else if(m_blnBlendChk == FALSE)
	{
		glDisable(GL_BLEND);
	}
}

void COpenGLView::ChkCullface_Mode()
{
	if(m_blnCullfaceChk == TRUE)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
	else if(m_blnCullfaceChk == FALSE)
	{
		glDisable(GL_CULL_FACE);
	}
}

void COpenGLView::SetMouseChk(bool setting)
{
	m_blnMouseChk = setting;
}

void COpenGLView::SetVertexChk(bool setting)
{
	m_blnVertexChk = setting;
}

void COpenGLView::SetTextureChk(bool setting)
{
	m_blnTextureChk = setting;
}

void COpenGLView::SetCwChk(bool setting)
{
	m_blnCwChk = setting;
}

void COpenGLView::SetFlatChk(bool setting)
{
	m_blnFlatChk = setting;
}

void COpenGLView::SetBGRChk(bool setting)
{
	m_blnBGRChk = setting;
}

void COpenGLView::SetDepthChk(bool setting)
{
	m_blnDepthChk = setting;
}

void COpenGLView::SetBlendChk(bool setting)
{
	m_blnBlendChk = setting;
}

void COpenGLView::SetCullfaceChk(bool setting)
{
	m_blnCullfaceChk = setting;
}

void COpenGLView::SetMipmapChk(bool setting)
{
	m_blnMipmapChk = setting;
}

void COpenGLView::SetClearColor(int R, int G, int B)
{
	m_nClear_color_R = R;
	m_nClear_color_G = G;
	m_nClear_color_B = B;
}

void COpenGLView::SetObjColor(int R, int G, int B)
{
	m_nObj_color_R = R;
	m_nObj_color_G = G;
	m_nObj_color_B = B;
}

int COpenGLView::GetClear_R_Color()
{
	return m_nClear_color_R;
}

int COpenGLView::GetClear_G_Color()
{
	return m_nClear_color_G;
}

int COpenGLView::GetClear_B_Color()
{
	return m_nClear_color_B;
}

int COpenGLView::GetObj_R_Color()
{
	return m_nObj_color_R;
}

int COpenGLView::GetObj_G_Color()
{
	return m_nObj_color_G;
}

int COpenGLView::GetObj_B_Color()
{
	return m_nObj_color_B;
}

void COpenGLView::SetBasicShape(int type)
{
	m_nBasicShape = type + 1;	// 0 번 째 GL_POINTS 는 따로 구현함.
}