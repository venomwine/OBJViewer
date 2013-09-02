/* OBJViewer Copyright (C) 2013 Hong, Wooram */
#include <StdAfx.h>
#include <iostream>
#include <vector>
#include "ObjParser.h"

using namespace std;

// Obj File Data
extern vector	< Vert3D >		vertexArray;
extern vector	< TexUV >		textureArray;
extern vector	< Norm3D >		normal3DArray;
extern vector	< IndexVTN >	indexArray;
extern vector	< TexFile >		textureFileNameArray;

// FirstVal -> Min = Max
extern BOOL g_blnFirstX, g_blnFirstY, g_blnFirstZ;

// Object 범위
VectCoordType g_MinX, g_MinY, g_MinZ;
VectCoordType g_MaxX, g_MaxY, g_MaxZ;

// Vertex, Texture Real Array
extern VectCoordType *vertices_Tri;
extern TexCoordType *textures;

// Array Size
extern int g_nIndexSize, g_nVertexSize, g_nTextureSize;

// File Type
extern int g_nFileType;

// Vertex File Name Num
extern int g_nVertexFileNum;

// parser 용 Flag
BOOL g_blnVertexFlag;
BOOL g_blnFaceFlag;
BOOL g_blnTextureFlag;

CString g_sMtlFileName;


CObjParser::CObjParser()
{
}
CObjParser::~CObjParser()
{
}

void CObjParser::ChkFileType(CString sLineBuf)
{
	if(sLineBuf == _T("# 3ds Max Wavefront OBJ Exporter v0.94b - (c)2007 guruware"))	// 임시. format 을 확실하게 확인할 방법이 없음.
		g_nFileType = _NAVTEQOBJ;
	else if(sLineBuf == _T("Metasequoia Document"))
		g_nFileType = _MQO;
	else 
		g_nFileType = _NORMALOBJ;
}

void CObjParser::ObjDataParser(CString sObjLineData)
{
	int pos = 0;
	CString sMQOTokenData, sChkTokenData;

	sObjLineData.Replace(_T("\r"), _T(""));
	sObjLineData.Replace(_T("\t"), _T(""));

	switch(g_nFileType)
	{
	case _MQO:
		sChkTokenData = sObjLineData;

		sMQOTokenData = sChkTokenData.Tokenize(_T(" "), pos);

		if(sMQOTokenData == _T("vertex"))	// vertex
		{
			g_blnVertexFlag = TRUE;
		}
		else if(sMQOTokenData == _T("face"))	// face
		{
			g_blnFaceFlag = TRUE;
		}
		else if(sMQOTokenData == _T("Material"))
		{
			g_blnTextureFlag = TRUE;
		}
		else if(sMQOTokenData == _T("}"))
		{
			g_blnVertexFlag = FALSE;
			g_blnFaceFlag = FALSE;
			g_blnTextureFlag = FALSE;
		}
		else if(g_blnVertexFlag == TRUE)	// vertex
		{
			Msg_OBJ_v_Parser(sObjLineData);
		}
		else if(g_blnFaceFlag == TRUE)		// face
		{
			Msg_MQO_face_Parser(sObjLineData);
		}
		else if(g_blnTextureFlag == TRUE)	// Texture
		{
			Msg_MQO_Load_Texture(sObjLineData);
		}
		break;
	
	case _NAVTEQOBJ:
	case _NORMALOBJ:
		if(sObjLineData.GetAt(0) == 'v')	
		{
			if(sObjLineData.GetAt(1) == 'n')		// vn : normal vertex
			{
				Msg_OBJ_vn_Parser(sObjLineData);
			}
			else if(sObjLineData.GetAt(1) == 't')	// vt : texture
			{
				Msg_OBJ_vt_Parser(sObjLineData);
			}
			else if(sObjLineData.GetAt(1) == ' ')	// v : vertex
			{
				Msg_OBJ_v_Parser(sObjLineData);
			}
		}
		else if(sObjLineData.GetAt(0) == 'f')		// f : Index
		{
			Msg_OBJ_f_Parser(sObjLineData);
		}
		else if(sObjLineData.GetAt(0) == 'm')		// .mtl File Name
		{
			if(sObjLineData.GetAt(1) == 't' && sObjLineData.GetAt(2) == 'l' && sObjLineData.GetAt(3) == 'l' && 
				sObjLineData.GetAt(4) == 'i' && sObjLineData.GetAt(5) == 'b' )
			{
				ChkMtlFileName(sObjLineData);
			}
		}
		break;
	}
}

void CObjParser::Msg_MQO_Load_Texture(CString sObjLineData)
{
	int pos = 0, index = 0;
	CString sMQOTextureTokenData;
	TexFile textureFileNameArrayTmp;

	while((sMQOTextureTokenData = sObjLineData.Tokenize(_T(" "), pos)) != _T(""))
	{
		if(sMQOTextureTokenData.GetAt(0) == 't')
		{
			sMQOTextureTokenData.Replace(_T("tex("), _T(""));
			sMQOTextureTokenData.Replace(_T(")"), _T(""));
			sMQOTextureTokenData.Replace(_T("\""), _T(""));
		}
		else 
			sMQOTextureTokenData = _T("");

		textureFileNameArrayTmp.sTextureFileName = sMQOTextureTokenData;
	}
	textureFileNameArrayTmp.nTextureFileNum = g_nVertexFileNum++;
	textureFileNameArray.push_back(textureFileNameArrayTmp);
}

void CObjParser::Msg_MQO_face_Parser(CString fLineData)
{
	int pos = 0;
	CString sMQOFaceTokenData;

	fLineData.Replace(_T("\r"), _T(""));
	fLineData.Replace(_T("\t"), _T(""));

	sMQOFaceTokenData = fLineData.Tokenize(_T(" "), pos);

	switch(_ttoi(sMQOFaceTokenData))
	{
	case 3:	// 기본도형 삼각형
		Msg_MQO_3Index_Parser(fLineData);
		Msg_MQO_3UV_Parser(fLineData);
		break;
	case 4:	// 기본도형 사각형
		Msg_MQO_4Index_Parser(fLineData);
		Msg_MQO_4UV_Parser(fLineData);
		break;
	}
}

void CObjParser::Msg_MQO_3UV_Parser(CString fLineData)
{
	int pos = 0, index = 0;
	CString sMQO3FaceTokenData;
	TexUV TexUVTmp;

	while((sMQO3FaceTokenData = fLineData.Tokenize(_T(" "), pos)) != _T(""))
	{
		switch(index)
		{
		case 5:
			sMQO3FaceTokenData.Replace(_T("UV("), _T(""));
			TexUVTmp.x = ((TexCoordType)_wtof(sMQO3FaceTokenData));
			break;
		case 6:
			TexUVTmp.y = ((TexCoordType)_wtof(sMQO3FaceTokenData));
			textureArray.push_back(TexUVTmp);
			break;
		case 7:
			TexUVTmp.x = ((TexCoordType)_wtof(sMQO3FaceTokenData));
			break;
		case 8:
			TexUVTmp.y = ((TexCoordType)_wtof(sMQO3FaceTokenData));
			textureArray.push_back(TexUVTmp);
			break;
		case 9:
			TexUVTmp.x = ((TexCoordType)_wtof(sMQO3FaceTokenData));
			break;
		case 10:
			sMQO3FaceTokenData.Replace(_T(")"), _T(""));
			TexUVTmp.y = ((TexCoordType)_wtof(sMQO3FaceTokenData));
			textureArray.push_back(TexUVTmp);
			break;
		}
		index++;
	}
}

void CObjParser::Msg_MQO_4UV_Parser(CString fLineData)
{
	int pos = 0, index = 0;
	CString sMQO3FaceTokenData;
	TexUV TexUVTmp1, TexUVTmp2, TexUVTmp3, TexUVTmp4;

	while((sMQO3FaceTokenData = fLineData.Tokenize(_T(" "), pos)) != _T(""))
	{
		switch(index)
		{
		case 6:
			sMQO3FaceTokenData.Replace(_T("UV("), _T(""));
			TexUVTmp1.x = ((TexCoordType)_wtof(sMQO3FaceTokenData));
			break;
		case 7:
			TexUVTmp1.y = ((TexCoordType)_wtof(sMQO3FaceTokenData));
			textureArray.push_back(TexUVTmp1);
			break;
		case 8:
			TexUVTmp2.x = ((TexCoordType)_wtof(sMQO3FaceTokenData));
			break;
		case 9:
			TexUVTmp2.y = ((TexCoordType)_wtof(sMQO3FaceTokenData));
			textureArray.push_back(TexUVTmp2);
			break;
		case 10:
			TexUVTmp3.x = ((TexCoordType)_wtof(sMQO3FaceTokenData));
			break;
		case 11:
			TexUVTmp3.y = ((TexCoordType)_wtof(sMQO3FaceTokenData));
			textureArray.push_back(TexUVTmp3);
			break;
		case 12:
			TexUVTmp4.x = ((TexCoordType)_wtof(sMQO3FaceTokenData));
			break;
		case 13:
			sMQO3FaceTokenData.Replace(_T(")"), _T(""));
			TexUVTmp4.y = ((TexCoordType)_wtof(sMQO3FaceTokenData));
			textureArray.push_back(TexUVTmp4);
			textureArray.push_back(TexUVTmp1);
			textureArray.push_back(TexUVTmp3);
			break;
		}
		index++;
	}
}

void CObjParser::Msg_MQO_3Index_Parser(CString fLineData)
{
	int pos = 0, index = 0;
	CString sMQO3FaceTokenData, sMposition;
	IndexVTN indexTemp;

	int nMposition = fLineData.Find(_T("M("));
	sMposition = fLineData.Mid(nMposition, 5);

	sMposition.Replace(_T("M("), _T(""));
	sMposition.Replace(_T(")"), _T(""));

	while((sMQO3FaceTokenData = fLineData.Tokenize(_T(" "), pos)) != _T(""))
	{
		switch(index)
		{
		case 1:
			sMQO3FaceTokenData.Replace(_T("V("), _T(""));
			indexTemp.vect = ((IndexCoordType)_wtoi(sMQO3FaceTokenData));
			indexTemp.texFileIndex = _ttoi(sMposition);
			indexArray.push_back(indexTemp);
			g_nIndexSize++;
			break;
		case 2:
			indexTemp.vect = ((IndexCoordType)_wtoi(sMQO3FaceTokenData));
			indexTemp.texFileIndex = _ttoi(sMposition);
			indexArray.push_back(indexTemp);
			g_nIndexSize++;
			break;
		case 3:
			sMQO3FaceTokenData.Replace(_T(")"), _T(""));
			indexTemp.vect = ((IndexCoordType)_wtoi(sMQO3FaceTokenData));
			indexTemp.texFileIndex = _ttoi(sMposition);
			indexArray.push_back(indexTemp);
			g_nIndexSize++;
			break;
		}
		index++;
	}
}

void CObjParser::Msg_MQO_4Index_Parser(CString fLineData)
{
	int pos = 0, index = 0;
	CString sMQO4FaceTokenData, sMposition;
	IndexVTN indexTemp1, indexTemp2, indexTemp3, indexTemp4;

	int nMposition = fLineData.Find(_T("M("));
	sMposition = fLineData.Mid(nMposition, 5);

	sMposition.Replace(_T("M("), _T(""));
	sMposition.Replace(_T(")"), _T(""));

	while((sMQO4FaceTokenData = fLineData.Tokenize(_T(" "), pos)) != _T(""))
	{
		switch(index)
		{
			// Vertex Index
		case 1:
			sMQO4FaceTokenData.Replace(_T("V("), _T(""));
			indexTemp1.vect = ((IndexCoordType)_wtoi(sMQO4FaceTokenData));
			indexTemp1.texFileIndex = _ttoi(sMposition);
			indexArray.push_back(indexTemp1);
			g_nIndexSize++;
			break;
		case 2:
			indexTemp2.vect = ((IndexCoordType)_wtoi(sMQO4FaceTokenData));
			indexTemp2.texFileIndex = _ttoi(sMposition);
			indexArray.push_back(indexTemp2);
			g_nIndexSize++;
			break;
		case 3:
			indexTemp3.vect = ((IndexCoordType)_wtoi(sMQO4FaceTokenData));
			indexTemp3.texFileIndex = _ttoi(sMposition);
			indexArray.push_back(indexTemp3);
			g_nIndexSize++;
			break;
		case 4:
			sMQO4FaceTokenData.Replace(_T(")"), _T(""));
			indexTemp4.vect = ((IndexCoordType)_wtoi(sMQO4FaceTokenData));
			indexTemp4.texFileIndex = _ttoi(sMposition);
			indexArray.push_back(indexTemp4);
			g_nIndexSize++;
			indexArray.push_back(indexTemp1);
			g_nIndexSize++;
			indexArray.push_back(indexTemp3);
			g_nIndexSize++;
			break;
		}
		index++;
	}
}

void CObjParser::ChkMtlFileName(CString sObjLineData)
{
	int pos = 0;
	sObjLineData.Tokenize(_T(" "), pos);
	g_sMtlFileName = sObjLineData.Tokenize(_T(" "), pos);
}

void CObjParser::Msg_OBJ_vn_Parser(CString vnLineData)	// vn : normal vertex
{
	CString ObjDataToken;
	int pos = 0, index = 0;
	Norm3D normal3DTemp;

	while((ObjDataToken = vnLineData.Tokenize(_T(" "), pos )) !=  _T(""))
	{
		switch(index)
		{
		case 1:
			normal3DTemp.x = (NormCoordType)_wtof(ObjDataToken);
			break;
		case 2:
			normal3DTemp.y = (NormCoordType)_wtof(ObjDataToken);
			break;
		case 3:
			normal3DTemp.z = (NormCoordType)_wtof(ObjDataToken);
			break;
		default:
			break;
		}
		index++;
	}
	normal3DArray.push_back(normal3DTemp);
}

void CObjParser::Msg_OBJ_vt_Parser(CString vtLineData)	// vt : texture
{
	CString ObjDataToken;
	int pos = 0, index = 0;
	TexUV textureTemp;

	while((ObjDataToken = vtLineData.Tokenize(_T(" "), pos )) !=  _T(""))
	{
		switch(index)
		{
		case 1:
			textureTemp.x = (TexCoordType)_wtof(ObjDataToken);
			break;
		case 2:
			textureTemp.y = (TexCoordType)_wtof(ObjDataToken);
			break;
		default:
			break;
		}
		index++;
	}
	g_nTextureSize++;
	textureArray.push_back(textureTemp);
}

void CObjParser::Msg_OBJ_v_Parser(CString vLineData)	// v : vertex
{
	CString ObjDataToken;
	int pos = 0, index = 0;
	Vert3D vectorTemp;

	if(g_nFileType == _MQO) index = 1;

	while((ObjDataToken = vLineData.Tokenize(_T(" "), pos )) !=  _T(""))
	{
		switch(index)
		{
		case 1:
			vectorTemp.x = (VectCoordType)_wtof(ObjDataToken);
			if(g_blnFirstX == TRUE)
			{
				g_blnFirstX = FALSE;
				g_MinX = g_MaxX = vectorTemp.x;
			}
			else if(g_MinX > vectorTemp.x) g_MinX = vectorTemp.x;
			else if(g_MaxX < vectorTemp.x) g_MaxX = vectorTemp.x;
			break;
		case 2:
			vectorTemp.y = (VectCoordType)_wtof(ObjDataToken);
			if(g_blnFirstY == TRUE)
			{
				g_blnFirstY = FALSE;
				g_MinY = g_MaxY = vectorTemp.y;
			}
			else if(g_MinY > vectorTemp.y) g_MinY = vectorTemp.y;
			else if(g_MaxY < vectorTemp.y) g_MaxY = vectorTemp.y;
			break;
		case 3:
			vectorTemp.z = (VectCoordType)_wtof(ObjDataToken);
			if(g_blnFirstZ == TRUE)
			{
				g_blnFirstZ = FALSE;
				g_MinZ = g_MaxZ = vectorTemp.z;
			}
			else if(g_MinZ > vectorTemp.z) g_MinZ = vectorTemp.z;
			else if(g_MaxZ < vectorTemp.z) g_MaxZ = vectorTemp.z;
			break;
		default:
			break;
		}
		index++;
	}
	g_nVertexSize++;
	vertexArray.push_back(vectorTemp);
}

void CObjParser::Msg_OBJ_f_Parser(CString fLineData)	// f : Index
{
	CString ObjDataToken;
	int pos = 0;
	int index = 0;

	while((ObjDataToken = fLineData.Tokenize(_T(" "), pos )) != _T(""))
	{
		switch(index)
		{
		case 1:
		case 2:
		case 3:
			Msg_OBJ_f_Parser_Mini(ObjDataToken);
			break;
		default:
			break;
		}
		index++;
	}
}

void CObjParser::Msg_OBJ_f_Parser_Mini(CString fLineData)	// f 
{
	IndexVTN indexTemp;
	CString ObjMiniToken;
	int pos = 0;
	int index = 0;

	while((ObjMiniToken = fLineData.Tokenize(_T("/"), pos )) != _T(""))
	{
		switch(index)
		{

		case 0:
			switch(g_nFileType)
			{
			case _NAVTEQOBJ:
				indexTemp.vect = ((IndexCoordType)_wtoi(ObjMiniToken)) + vertexArray.size();
				break;
			case _NORMALOBJ:
				indexTemp.vect = ((IndexCoordType)_wtoi(ObjMiniToken)) - 1;
				break;
			}
			break;
		case 1:
			switch(g_nFileType)
			{
			case _NAVTEQOBJ:
				indexTemp.tex = ((IndexCoordType)_wtoi(ObjMiniToken)) + textureArray.size();
				break;
			case _NORMALOBJ:
				indexTemp.tex = ((IndexCoordType)_wtoi(ObjMiniToken)) - 1;
				break;
			}
			break;
		case 2:
			indexTemp.norm = abs((IndexCoordType)_wtoi(ObjMiniToken)) - 1;
			break;
		default	:
			break;
		}
		index++;
	}
	indexArray.push_back(indexTemp);
	g_nIndexSize++;
}

CString CObjParser::MtlParser(CString s_FileName)
{
	CString sMtlFileName, sFilePath;
	CString ImageName;

	int pos = 0, pos2 = 0, index = 0;

	sFilePath = s_FileName.Mid(0, s_FileName.ReverseFind('\\'));
	sMtlFileName = sFilePath + _T('\\') + g_sMtlFileName;

	CStdioFile ObjFile;

	if(!ObjFile.Open(sMtlFileName,CFile::modeRead))
	{
		AfxMessageBox(_T("Mtl File Error"));
		return _T("Mtl File Error");
	}

	CString sLineBuf;
	while(ObjFile.ReadString(sLineBuf))
	{
		sLineBuf.Replace(_T("\r"), _T(""));
		sLineBuf.Replace(_T("\t"), _T(""));

		if(sLineBuf.GetAt(0) == 'm')
		{
			if(sLineBuf.GetAt(1) == 'a' &&sLineBuf.GetAt(2) == 'p' && sLineBuf.GetAt(3) == '_' && sLineBuf.GetAt(4) == 'K' && sLineBuf.GetAt(5) == 'd')	
			{
				while((ImageName = sLineBuf.Tokenize(_T(" "), pos2 )) !=  _T(""))
				{
					switch(index)
					{
					case 1:
						ObjFile.Close();
						return sFilePath + _T('\\') + ImageName;
						break;
					}
					index++;
				}
			}
		}
	}
	ObjFile.Close();
	return _T("Mtl File Error");
}

void CObjParser::MakeDataArray()
{
	if(vertices_Tri) delete[] vertices_Tri;
	if(g_nFileType != _MQO) if(textures) delete[] textures;

	VectCoordType fltMidX, fltMidY, fltMidZ;
	fltMidX = ((g_MaxX + g_MinX)/2);
	fltMidY = ((g_MaxY + g_MinY)/2);
	fltMidZ = ((g_MaxZ + g_MinZ)/2);

	vertices_Tri=new VectCoordType[g_nIndexSize*3];
	for(int i=0;i<g_nIndexSize;i++)
	{
		switch(g_nFileType)
		{
		case _NAVTEQOBJ:
			vertices_Tri[i*3] = (vertexArray.at(indexArray.at(i).vect).x -  fltMidX ) * BASESCALE;	
			vertices_Tri[i*3+1] = (vertexArray.at(indexArray.at(i).vect).y - fltMidY) * BASESCALE * 1.5;
			vertices_Tri[i*3+2] = (vertexArray.at(indexArray.at(i).vect).z - fltMidZ) * BASESCALE;
			break;
		case _NORMALOBJ:
		case _MQO:
			vertices_Tri[i*3] = vertexArray.at(indexArray.at(i).vect).x - fltMidX;	
			vertices_Tri[i*3+1] = vertexArray.at(indexArray.at(i).vect).y - fltMidY;
			vertices_Tri[i*3+2] = vertexArray.at(indexArray.at(i).vect).z - fltMidZ;
			break;
		}
	}

	textures=new TexCoordType[g_nIndexSize*2];
	for(int i=0;i<g_nIndexSize;i++)
	{
		switch(g_nFileType)
		{
		case _NAVTEQOBJ:
		case _NORMALOBJ:
			textures[i*2] = textureArray.at(indexArray.at(i).tex).x;
			textures[i*2+1] = textureArray.at(indexArray.at(i).tex).y;
			break;
		case _MQO:
			textures[i*2] = textureArray.at(i).x;
			textures[i*2+1] = textureArray.at(i).y;
			break;
		}
	}

// Array check
// 	CString sText, sTextTmp;
// 	sText = _T("{");
// 	for(int i = 0; i < g_nIndexSize*2; i++)
// 	{
// 		sTextTmp.Format(_T("%f"), textures[i]);
// 		if(i < g_nIndexSize*3-1)	sText += sTextTmp + _T(",");
// 		if(i%36 == 0)				sText += _T("\n");
// 	}
// 	sText += _T("}");


}