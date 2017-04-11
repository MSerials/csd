#pragma once
#include "GetHDSerial.h"
#include "AES.h"
#include "md5.h"
#include <iostream>

#pragma warning(disable:4996)

enum
{
	NOT_OFFICIAL_INSTALL = 1, OUT_DATE = 2
};

class PassWord
{
public:
	PassWord();
	~PassWord();
	
	bool CheckTrial() { return CheckBeforeUse(); }

	int isSNOK()
	{
			CString HDSerial = GetHDserial();
			CString Serial = GetSerial();
			MD5 md5((std::string)(CStringA)HDSerial);
			const char* md5string = md5.toString().c_str();
			CString res = (CString)(CStringA)md5string;
			return res.Compare(Serial);
	};

	CString GetHDserial()
	{
	//	return getHDSerial.GetHDSerialNo();
	//	CString str;
		if (getHDSerial.GetPhyDriveSerial(szModelNo, szSerialNo))
		{
			return (CString)szSerialNo;
		}
		return L"";
	}

	int GetDate()
	{
		ATL::CRegKey key;
		DWORD dw = 512;
		char sValue[512] = { 0 };// = L"";
		if (key.Open(HKEY_CURRENT_USER, L"Software\\ImageVision"))
		{
			key.Close();
			if (!RegDate())
				return 0; //����û�й���ԱȨ�����У�����һ��0
			else
				return 1;//��ʾ�Ѿ�����ԱȨ�������ˣ������Ū
		}
		else {
			key.QueryValue((LPTSTR)sValue, L"T", &dw);
			key.Close();
			CString s = (CString)sValue;
			if (s.IsEmpty())
			{
				if (!RegDate())
					return 0; //����û�й���ԱȨ�����У�����һ��0
				else
					return 1;//��ʾ�Ѿ�����ԱȨ�������ˣ������Ū
			}
		}

		vector<CString> strValue = Split((LPTSTR)sValue);
		if (3 != strValue.size()) return -2;//��ʽ����

		int nYear = StrToInt(strValue[0]);
		int nMonth = StrToInt(strValue[1]);
		int nDay = StrToInt(strValue[2]);

		CTime time = CTime::GetCurrentTime();
		int tYear = time.GetYear();
		int tMonth = time.GetMonth();
		int tDay = time.GetDay();

		long newyearday = GetThisYearDay(tYear, tMonth, tDay);
		long oldyearday = GetThisYearDay(nYear, nMonth, nDay);

		int _data = newyearday - oldyearday;
		return 1;
	//	if ((_data < 90))
		if ((_data < 90) && (_data >= 0))
			return 1; //ok
		else
			return -1;//outdate
	}


	BOOL RegSerial(LPCTSTR Reg)
	{
		//if (ERROR_SUCCESS == ::RegCreateKeyEx(hCKey, _T("testreg"), 0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_READ, NULL, &hAKey, &dw))
		CRegKey key;
		if (key.Open(HKEY_LOCAL_MACHINE, L"HARDWARE"))
		{
			if (key.Create(HKEY_LOCAL_MACHINE, L"HARDWARE"))
				return 0;
			key.SetValue(Reg, L"Serial");
		}
		else
		{
			key.SetValue(Reg, L"Serial");
		}
		key.Close();
		return 1;

	}


private:
	GetHDSerial getHDSerial;
	wchar_t strReg[1024];
	TCHAR szModelNo[48], szSerialNo[24];
	bool CheckBeforeUse() {return (0 == CheckVersion().Compare(L"1.0.0"));}

	CString CheckVersion()
	{
		CRegKey key;
		DWORD dw = 1024;
		char sValue[512] = { 0 };// = L"";
		if (key.Open(HKEY_LOCAL_MACHINE, L"HARDWARE"))
		{
			key.Close();
			return L"";
		}
		else
		{
			memset(strReg, 0, 1024);
			key.QueryValue(strReg, L"Verion", &dw);
			key.Close();
			return (CString)strReg;
		}
	}

	CString GetSerial()
	{
			CRegKey key;
		DWORD dw = 1024;
		char sValue[512] = { 0 };// = L"";
		if (key.Open(HKEY_LOCAL_MACHINE, L"HARDWARE"))
		{
			key.Close();
			return L"";
		}
		else
		{
			memset(strReg, 0, 1024);
			key.QueryValue(strReg, L"Serial", &dw);
			key.Close();
			return (CString)strReg;
		}





	}





















	CString GetAES(CString str = L"")
	{
		

		AES aes;
#define keylen 7
		const unsigned char *key = new unsigned char[16];
		memcpy((void*)key, "lsdkfj\0", keylen);


		const unsigned char* plainText = new unsigned char[160];
		/*memcpy*/strcpy((char*)plainText, "mybirthdaymybirthdaymybirthdaymybirthdaymybirthdaymybirthday\0");


		cout << plainText << endl << endl << endl;
		const unsigned char* cipherText;
		cipherText = aes.Cipher(plainText, key, keylen);
		cout << cipherText << endl << endl << endl;


		const unsigned char* plainText1;
		plainText1 = aes.InVCipher(cipherText, key, keylen);
		cout << plainText1 << endl << endl << endl;

		delete[] key;
		delete[] plainText;
		return L"";

	}
		










	BOOL RegDate()
	{
		CTime time = CTime::GetCurrentTime();
		int Year = time.GetYear();
		int Month = time.GetMonth();
		int Day = time.GetDay();
		CString str;
		str.Format(_T("%d-%d-%d"), Year, Month, Day);
		LPCTSTR l_time = (LPCTSTR)str;
		ATL::CRegKey key;
		if (key.Open(HKEY_CURRENT_USER, L"Software\\ImageVision"))
		{
			if (key.Create(HKEY_CURRENT_USER, L"Software\\ImageVision"))
				return 0;
			key.SetValue(l_time, L"T");
		}
		else
		{
			key.SetValue(l_time, L"T");
		}
		key.Close();
		return 1;
	}





	long GetThisYearDay(int year, int month, int day)
	{
		int counterOfOddYear = (year / 4);
		if (!(year % 4)) {
			counterOfOddYear--;
			long allday = (year - 1) * 365 + counterOfOddYear;
			switch (month)
			{
			case 1: return (day + allday);
			case 2: return (day + allday + 31);
			case 3: return (day + allday + 60);
			case 4: return (day + allday + 91);
			case 5: return (day + allday + 121);
			case 6: return (day + allday + 152);
			case 7: return (day + allday + 182);
			case 8: return (day + allday + 213);
			case 9: return (day + allday + 244);
			case 10: return (day + allday + 274);
			case 11: return (day + allday + 305);
			case 12: return (day + allday + 335);
			default:return 0;
			}
		}
		else
		{
			long allday = (year) * 365 + counterOfOddYear;
			switch (month)
			{
			case 1: return (day + allday);
			case 2: return (day + allday + 31);
			case 3: return (day + allday + 59);
			case 4: return (day + allday + 90);
			case 5: return (day + allday + 120);
			case 6: return (day + allday + 151);
			case 7: return (day + allday + 181);
			case 8: return (day + allday + 212);
			case 9: return (day + allday + 243);
			case 10: return (day + allday + 273);
			case 11: return (day + allday + 304);
			case 12: return (day + allday + 334);
			default:return 0;
			}

		}
	}


	vector<CString> Split(CString string)
	{
		CString oriStr = string;
		vector<CString> strVec;
		while (true)
		{
			CString n = oriStr.SpanExcluding(L"-");
			strVec.push_back(n);
			oriStr = oriStr.Right(oriStr.GetLength() - n.GetLength() - 1);
			if (oriStr.IsEmpty())
			{
				break;
			}
		}
		return strVec;
	}

};

