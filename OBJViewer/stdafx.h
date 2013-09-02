
// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 일부 CString 생성자는 명시적으로 선언됩니다.

// MFC의 공통 부분과 무시 가능한 경고 메시지에 대한 숨기기를 해제합니다.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>         // MFC 확장입니다.





#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 공용 컨트롤에 대한 MFC 지원입니다.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows 공용 컨트롤에 대한 MFC 지원입니다.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC의 리본 및 컨트롤 막대 지원



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