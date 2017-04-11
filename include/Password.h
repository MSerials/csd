// Password.h: interface for the CPassword class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_PASSWORD_H__)
#define _PASSWORD_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPassword  
{
public:
	BOOL GetHDSerial(CStringA *HDSerialNo);
	BOOL GetMD5(CStringA strHDSerial, CStringA *res);
	int PwdCompare();
	BOOL GetSerialNo(CStringA *password);
	BOOL RegSerial(LPCTSTR Reg);
	long GetThisYearDay(int year, int month, int day);
	BOOL RegDate();
	int GetDate();
	vector<CString> Split(CString string);
	CString GetRegSerial();
	wchar_t strReg[1024];
	CPassword();
	virtual ~CPassword();

};

#endif // !defined(_PASSWORD_H__
