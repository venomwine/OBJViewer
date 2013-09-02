#ifndef __OBJ_PARSER__
#define __OBJ_PARSER__

#include <vector>
using namespace std;

// TESTTODO: Implement Obj File Parser Here
class CObjParser{
public:
	CObjParser();
	~CObjParser();
	void ObjDataParser(CString sObjLineData);

private:
	

public:

	void Msg_OBJ_vn_Parser(CString vnLineData);
	void Msg_OBJ_vt_Parser(CString vtLineData);
	void Msg_OBJ_v_Parser(CString vLineData);
	void Msg_OBJ_f_Parser(CString fLineData);
	void Msg_OBJ_f_Parser_Mini(CString fLineData);

	void Msg_MQO_face_Parser(CString fLineData);
	void Msg_MQO_3Index_Parser(CString fLineData);
	void Msg_MQO_4Index_Parser(CString fLineData);
	void Msg_MQO_3UV_Parser(CString fLineData);
	void Msg_MQO_4UV_Parser(CString fLineData);
	void Msg_MQO_Load_Texture(CString sObjLineData);

	CString MtlParser(CString s_FileName);

	void MakeDataArray();

	void ChkFileType(CString sLineBuf);
	void ChkMtlFileName(CString sObjLineData);

};

#endif