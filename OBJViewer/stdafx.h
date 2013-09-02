
// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������ 
// ��� �ִ� ���� �����Դϴ�.

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // �Ϻ� CString �����ڴ� ��������� ����˴ϴ�.

// MFC�� ���� �κа� ���� ������ ��� �޽����� ���� ����⸦ �����մϴ�.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC �ٽ� �� ǥ�� ���� ����Դϴ�.
#include <afxext.h>         // MFC Ȯ���Դϴ�.





#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC�� ���� �� ��Ʈ�� ���� ����



// OpenGL Header Files
#include <gl\gl.h>
#include <gl\glu.h>

// CxImage Header File
#include "./CxImage/CxImage/ximage.h"

// Vector Header
#include <vector>



#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

typedef double VectCoordType;
typedef float TexCoordType ;
typedef float NormCoordType ;
typedef int IndexCoordType ;

typedef enum _texType{
	TEX_TYPE_RGB,
	TEX_TYPE_RGBA
}TexType;

typedef struct _Vert3D{
	VectCoordType x;
	VectCoordType y;
	VectCoordType z;
}Vert3D;

typedef struct _TexUV{
	TexCoordType x;
	TexCoordType y;
}TexUV;

typedef struct _Norm3D{
	NormCoordType x;
	NormCoordType y;
	NormCoordType z;
}Norm3D;

typedef struct _IndexVTN{
	IndexCoordType shape;
	IndexCoordType vect;
	IndexCoordType texFileIndex;
	IndexCoordType tex;
	IndexCoordType norm;
}IndexVTN;

typedef struct _TexFile{
	int nTextureFileNum;
	CString sTextureFileName;
}TexFile;

#define _PI 3.14159265358979323846264338327950288
#define _RADIAN (_PI / 180)
#define BASESCALE 100000

#define _NAVTEQOBJ  0
#define _NORMALOBJ  1
#define _MQO		2