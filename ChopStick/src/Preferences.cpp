
#include "../stdafx.h"
// Preferences.cpp: implementation of the CPreferences class.
//
//////////////////////////////////////////////////////////////////////

//#include "../stdafx.h"
//#include "djiajiao.h"
#include "../include/Preferences.h"
#include <fstream>

#pragma warning(disable:4996)

using namespace std;
//CPreferences *ini = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
////////////////////////////////////////////////////////////////////// 

CPreferences::CPreferences() 
{
	path = L"";
	dir = L"";
	Reset();
	initData();
}

CPreferences::~CPreferences()
{
	
}

//constructor, can specify pathname here instead of using SetPath later
void CPreferences::CreateIniFile(CString fileName)
{       
	CString strCurDir;	 
	strCurDir  = dir + L"\\" + fileName; 
	HANDLE hf = CreateFile(strCurDir, GENERIC_WRITE, FILE_SHARE_READ, NULL,CREATE_NEW, NULL, NULL);
	CloseHandle(hf);
	path = strCurDir;
}

void CPreferences::DeleteIniFile(CString fileName)
{
	CString strCurDir;	 
	strCurDir  = dir + "\\" + fileName; 
	DeleteFile(strCurDir);
}

void CPreferences::CopyIniFile(CString srcName, CString dstName)
{
	CString strOldPath,strNewPath;	 
	strOldPath  = dir + "\\" + srcName; 
	strNewPath  = dir + "\\" + dstName; 
	CopyFile(strOldPath,strNewPath,FALSE);
}

int CPreferences::SetIniFile(CString fileName)
{
	
	if (dir == "")
	{
		AfxMessageBox(L"���������ļ�Ŀ¼!");
		return -1;
	}
	else
	{
		path = dir + L"\\" + fileName;
	}
	if (!PathFileExists(path))
	{
		HANDLE hf = CreateFile(path, GENERIC_WRITE, FILE_SHARE_READ, NULL,CREATE_NEW, NULL, NULL);
		CloseHandle(hf);
		
		return 0;
	}
	return 1;
}

CString CPreferences::GetAllFileName(CString suffix, int &nCnt, TCHAR separ)
{
	CString sFileName;
	CFileFind finder;	
	CString rStr;
	CString strWildcard;
	rStr.Empty();
	
	nCnt=0;
	strWildcard = dir + L"\\*" + suffix;		
	BOOL bWorking = finder.FindFile(strWildcard);
	
	if (dir == L"")
	{
		AfxMessageBox(L"���������ļ�Ŀ¼!");
		return L"";
	}
	
	while (bWorking)
	{
		sFileName.Empty();		
		bWorking = finder.FindNextFile();
		if (finder.IsDots()) continue;
		sFileName = finder.GetFileName();
		sFileName = sFileName.Left(sFileName.ReverseFind('.')) + separ;
		rStr += sFileName;
		nCnt++;
	}
	rStr.TrimRight(separ);
	
	return rStr;
}

CString CPreferences::SplitString(CString srcStr, int nIndex, TCHAR separ/* = */)
{
	int i, len =0;
	CString strSplit;
	srcStr.TrimRight(separ);
	for(i=0; i<srcStr.GetLength(); i++)
	{
		if(srcStr[i] == separ)  len++;
	}
	
	if ((nIndex>len) || (srcStr==L"")) return L"";
	AfxExtractSubString(strSplit, srcStr, nIndex, separ);
	return strSplit;
}

/////////////////////////////////////////////////////////////////////
// Public Functions
/////////////////////////////////////////////////////////////////////

//sets path of ini file to read and write from
bool CPreferences::SetIniDir(CString filedir, bool bmodule/* =true */)
{
	if (bmodule)
		dir = GetModulePath() + "\\" + filedir;
	else
		dir = filedir;
	
	return CreateMultipleDirectory(dir);
}

//reads ini file specified using CIniFile::SetPath()
//returns true if successful, false otherwise
bool CPreferences::ReadFile()
{
	CFile file;
	CFileStatus status;
	if (!file.GetStatus(path,status))
		return 0;
	ifstream inifile;
	CString readinfo;
	inifile.open(path);
	int curkey = -1, curval = -1;
	if (inifile.fail())
	{
		error = "���ļ�����.";
		return 0;
	}
	CString keyname, valuename, _value;
	CString temp;
	while (getline(inifile,readinfo))
	{
		if (readinfo != "")
		{
			if (readinfo[0] == '[' && readinfo[readinfo.GetLength()-1] == ']') //if a section heading
			{
				//ȥ��[]
				keyname = readinfo;
				keyname.TrimLeft('[');
				keyname.TrimRight(']');
			}
			else //if a value
			{
				valuename = readinfo.Left(readinfo.Find(L"="));
				_value = readinfo.Right(readinfo.GetLength()-valuename.GetLength()-1);
				SetValue(keyname,valuename,_value);
			}
		}
	}
	inifile.close();
	return 1;
}

//writes data stored in class to ini file
void CPreferences::WriteFile()
{
	FILE *fp;
	fp=_tfopen(path,L"wt");
	if(fp==(FILE*)NULL)
	{
		return ;
	}
	for (int keynum = 0; keynum <= names.GetUpperBound(); keynum++)
	{
		if (keys[keynum].names.GetSize() != 0)
		{
			_ftprintf(fp,_T("[%s]\n"),names[keynum]);
			for (int valuenum = 0; valuenum <= keys[keynum].names.GetUpperBound(); valuenum++)
			{
				CString strtemp=keys[keynum].names[valuenum];
				CString strtemp1=keys[keynum].values[valuenum];
				_ftprintf(fp,_T("%s=%s"),strtemp,strtemp1);
				
				if (valuenum != keys[keynum].names.GetUpperBound())
					_ftprintf(fp,_T("\n"));
				else
					if (keynum < names.GetSize())
						_ftprintf(fp,_T("\n"));
			}
			if (keynum < names.GetSize())
				_ftprintf(fp,_T("\n"));
		}
	}
	fclose(fp);
}

//deletes all stored ini data
void CPreferences::Reset()
{
	keys.SetSize(0);
	names.SetSize(0);
}

//returns number of keys currently in the ini
int CPreferences::GetNumKeys()
{
	return keys.GetSize();
}

//returns number of values stored for specified key, or -1 if key found
int CPreferences::GetNumValues(CString keyname)
{
	int keynum = FindKey(keyname);
	if (keynum == -1)
		return -1;
	else
		return keys[keynum].names.GetSize();
}

//gets value of [keyname] valuename = 
//overloaded to return CString, int, and double
CString CPreferences::GetValue(CString keyname, CString valuename)
{
	int keynum = FindKey(keyname), valuenum = FindValue(keynum,valuename);
	
	if (keynum == -1)
	{
		error = L"û�д���"+keyname;
		return L"";
	}
	
	if (valuenum == -1)
	{
		error = L"û�д���"+valuename;
		return L"";
	}
	return keys[keynum].values[valuenum];
}

//gets value of [keyname] valuename = 
//overloaded to return CString, int, and double
int CPreferences::GetValueI(CString keyname, CString valuename)
{
	return _tstoi(GetValue(keyname,valuename));
}

//gets value of [keyname] valuename = 
//overloaded to return CString, int, and double
double CPreferences::GetValueF(CString keyname, CString valuename)
{
	return _tstof(GetValue(keyname, valuename));
}

CString CPreferences::GetValueStr(CString keyname, CString valuename)
{
	return GetValue(keyname, valuename);
}
bool CPreferences::GetValuebool(CString keyname,CString valuename)
{
	return (GetValue(keyname,valuename)==L"0")?false:true;
}


//sets value of [keyname] valuename =.
//specify the optional paramter as false (0) if you do not want it to create
//the key if it doesn't exist. Returns true if data entered, false otherwise
//overloaded to accept CString, int, and double
bool CPreferences::SetValue(CString keyname, CString valuename, CString value, bool create)
{
	int keynum = FindKey(keyname), valuenum = 0;
	//find key
	if (keynum == -1) //if key doesn't exist
	{
		if (!create) //and user does not want to create it,
			return 0; //stop entering this key
		names.SetSize(names.GetSize()+1);
		keys.SetSize(keys.GetSize()+1);
		keynum = names.GetSize()-1;
		names[keynum] = keyname;
	}
	
	//find value
	valuenum = FindValue(keynum,valuename);
	if (valuenum == -1)
	{
		if (!create)
			return 0;
		keys[keynum].names.SetSize(keys[keynum].names.GetSize()+1);
		keys[keynum].values.SetSize(keys[keynum].names.GetSize()+1);
		valuenum = keys[keynum].names.GetSize()-1;
		keys[keynum].names[valuenum] = valuename;
	}
	keys[keynum].values[valuenum] = value;
	return 1;
}

//sets value of [keyname] valuename =.
//specify the optional paramter as false (0) if you do not want it to create
//the key if it doesn't exist. Returns true if data entered, false otherwise
//overloaded to accept CString, int, and double
bool CPreferences::SetValueI(CString keyname, CString valuename, int value, bool create)
{
	CString temp;
	temp.Format(L"%d",value);
	return SetValue(keyname, valuename, temp, create);
}

//sets value of [keyname] valuename =.
//specify the optional paramter as false (0) if you do not want it to create
//the key if it doesn't exist. Returns true if data entered, false otherwise
//overloaded to accept CString, int, and double
bool CPreferences::SetValueF(CString keyname, CString valuename, double value, bool create)
{
	CString temp;
	temp.Format(L"%e",value);
	return SetValue(keyname, valuename, temp, create);
}

bool CPreferences::SetValueStr(CString keyname, CString valuename,CString value,bool create)
{
	return SetValue(keyname, valuename, value, create);
}
bool CPreferences::SetValuebool(CString keyname,CString valuename,bool value,bool create)
{
	CString temp;
	if(value==false)
		temp=L"0";
	else
		temp=L"1";
	return SetValue(keyname, valuename, temp, create);
}

/////////////////////////////////////////////////////////////////////
// Private Functions
/////////////////////////////////////////////////////////////////////

//returns index of specified key, or -1 if not found
int CPreferences::FindKey(CString keyname)
{
	int keynum = 0;
	while ( keynum < keys.GetSize() && names[keynum] != keyname)
		keynum++;
	if (keynum == keys.GetSize())
		return -1;
	return keynum;
}

//returns index of specified value, in the specified key, or -1 if not found
int CPreferences::FindValue(int keynum, CString valuename)
{
	if (keynum == -1)
		return -1;
	int valuenum = 0;
	while (valuenum < keys[keynum].names.GetSize() && keys[keynum].names[valuenum] != valuename)
		valuenum++;
	if (valuenum == keys[keynum].names.GetSize())
		return -1;
	return valuenum;
}

//overloaded from original getline to take CString
istream & CPreferences:: getline(istream & is, CString & str)
{
    char buf[2048];
    is.getline(buf,2048);
    str = buf;
    return is;
}


void CPreferences::OpenIniFile()
{
	::ShellExecute(NULL,L"open",path,NULL,NULL,SW_SHOW);
}

CString CPreferences::GetFilePath()
{
	return path;
}

CString CPreferences::GetFileDir()
{
	return dir;
}

CString CPreferences::GetModulePath()
{
	TCHAR szModulePath[MAX_PATH] = {0};        
	CString strCurDir;
	
	::GetModuleFileName(NULL,szModulePath,_MAX_PATH);  	
	strCurDir  = szModulePath;  
	strCurDir  = strCurDir.Left(strCurDir.ReverseFind(TEXT('\\')));
	return strCurDir;
}

bool CPreferences::CreateMultipleDirectory(CString szPath)
{
	if (szPath == _T("")) return false;
	
    CString strDir(szPath);
	
    if (strDir.GetAt(strDir.GetLength()-1)!=_T('\\'))
    {
        strDir += _T('\\');
    }
    std::vector<CString> vPath;
    CString strTemp;
    bool bSuccess = false;
	
    for (int i=0;i<strDir.GetLength();++i)
    {
        if (strDir.GetAt(i) != _T('\\')) 
        {
            strTemp += strDir.GetAt(i);
        }
        else 
        {
            vPath.push_back(strTemp);
            strTemp += _T('\\');
			
        }
    }
	
    std::vector<CString>::const_iterator vIter;
    for (vIter = vPath.begin(); vIter != vPath.end(); vIter++) 
    {
        bSuccess = CreateDirectory(*vIter, NULL) ? true : false;    
    }	
    return bSuccess;
}


int CPreferences::SelFolder(CWnd *pWnd, CString &strFolder)
{
	HWND hParent = pWnd->m_hWnd;
    LPMALLOC lpMalloc;
    strFolder.Empty();	
	
    if (::SHGetMalloc(&lpMalloc) != NOERROR) return 0;
	
    TCHAR szDisplayName[_MAX_PATH];
    TCHAR szBuffer[_MAX_PATH];
    BROWSEINFO browseInfo;
    browseInfo.hwndOwner = hParent;
    browseInfo.pidlRoot = NULL; // set root at Desktop
    browseInfo.pszDisplayName = szDisplayName;
    browseInfo.lpszTitle = L"Select a folder";
    browseInfo.ulFlags = BIF_RETURNFSANCESTORS|BIF_RETURNONLYFSDIRS;
    browseInfo.lpfn = NULL;
    browseInfo.lParam = 0;
	
    LPITEMIDLIST lpItemIDList;
    if ((lpItemIDList = ::SHBrowseForFolder(&browseInfo)) != NULL)
    {
        // Get the path of the selected folder from the    item ID list.
        if (::SHGetPathFromIDList(lpItemIDList, szBuffer))
        {
            // At this point, szBuffer contains the path the user chose.
            if (szBuffer[0] == '\0') return 0;
			
            // We have a path in szBuffer! Return it.
            strFolder = szBuffer;
            return 1;
        }
        else return 1; // strResult is empty
		
        lpMalloc->Free(lpItemIDList);
        lpMalloc->Release();
    }
    
	return 1;
}
CString CPreferences::Encrypt(CString Source, WORD Key) // ���ܺ���
{
	CString Result,str;
	int i,j;
	
	Result=Source; // ��ʼ������ַ���
	for(i=0; i<Source.GetLength(); i++) // ���ζ��ַ����и��ַ����в���
	{
		Result.SetAt(i, Source.GetAt(i)^(Key>>8)); // ����Կ��λ�����ַ����
		Key = ((BYTE)Result.GetAt(i)+Key)*C1+C2; // ������һ����Կ
	}
	Source=Result; // ������
	Result.Empty(); // ������
	for(i=0; i<Source.GetLength(); i++) // �Լ��ܽ������ת��
	{
		j=(BYTE)Source.GetAt(i); // ��ȡ�ַ�
		// ���ַ�ת��Ϊ������ĸ����
		str=L"12"; // ����str����Ϊ2
		str.SetAt(0, SHIFT+j/26);//���ｫ65�Ĵ���������256�����ľͻ�����룬Ч�����ã���Ӧ�ģ����ܴ�Ҫ��Ϊ��ͬ����
		str.SetAt(1, SHIFT+j%26);
		Result += str;
	}
	return Result;
}


CString CPreferences::Decrypt(CString Source, WORD Key) // ���ܺ���
{
	CString Result,str;
	int i,j;
	
	Result.Empty(); // ������
	for(i=0; i < Source.GetLength()/2; i++) // ���ַ���������ĸһ����д���
	{
		j = ((BYTE)Source.GetAt(2*i)-SHIFT)*26;//��Ӧ�ģ����ܴ�Ҫ��Ϊ��ͬ����		
		j += (BYTE)Source.GetAt(2*i+1)-SHIFT;
		str="1"; // ����str����Ϊ1
		str.SetAt(0, j);
		Result+=str; // ׷���ַ�����ԭ�ַ���
	}
	Source=Result; // �����м���
	for(i=0; i<Source.GetLength(); i++) // ���ζ��ַ����и��ַ����в���
	{
		Result.SetAt(i, (BYTE)Source.GetAt(i)^(Key>>8)); // ����Կ��λ�����ַ����
		Key = ((BYTE)Source.GetAt(i)+Key)*C1+C2; // ������һ����Կ
	}
	return Result;
}

void CPreferences::LoadParaFile(UINT item)
{
	UINT mask = item;

	if (ReadFile())
	{
		////////////////////////////////////
		if (PARA_PRJ == (mask&PARA_PRJ))
		{
			//m_strCurrentProjectName = GetValue(L"projectSetting",L"CurrentProject");
			m_idCamera				= GetValueI(L"projectSetting", L"CAMERAID");
			m_nTriggerOutForBack	= GetValueI(L"projectSetting", L"TRGOUTBACK");
			m_nTriggerOutForFront	= GetValueI(L"projectSetting", L"TRGOUTFRONT");
			m_nTriggerOutForFinish	= GetValueI(L"projectSetting", L"TRGOUTFINISH");
			m_nSetTimerIntervals	= GetValueI(L"projectSetting", L"TIMERINTERVALS");
			m_nSnapTimeDelay		= GetValueI(L"projectSetting", L"SANPDELAY");
			m_shutter				= GetValueF(L"projectSetting", L"SHUTTER");
			m_markdelay				= GetValueI(L"projectSetting", L"MARKDELAY");
			m_direction				= GetValueI(L"projectSetting", L"DIR");

			m_password				= GetValue(L"projectSetting", L"PWD");
			m_nTotal				= GetValueI(L"projectSetting", L"TOTAL_QTY");
			m_nUp					= GetValueI(L"projectSetting", L"UP_QTY");
			m_nDown					= GetValueI(L"projectSetting", L"DOWN_QTY");
			m_nLeft					= GetValueI(L"projectSetting", L"LEFT_QTY");
			m_nRight				= GetValueI(L"projectSetting", L"RIGHT_QTY");
			m_nVoid					= GetValueI(L"projectSetting", L"VOID_QTY");
		}
		if (PARA_IMAGE == (mask&PARA_IMAGE))
		{
			m_nESCPixel = GetValueI(L"IMAGE_SETTING", L"EscPixel");
			m_idThreshNoise = GetValueI(L"IMAGE_SETTING", L"idThreshNoise");
			m_idOfThreshold = GetValueI(L"IMAGE_SETTING", L"idOfThreshold");
			m_idFixThreshold = GetValueI(L"IMAGE_SETTING", L"idFixThreshold");
			m_iThreshNoiseLow = GetValueI(L"IMAGE_SETTING", L"iThreshNoiseLow");
			m_iThreshNoiseHi = GetValueI(L"IMAGE_SETTING", L"iThreshNoiseHi");
			m_iCanBeRecognisedLenMin = GetValueI(L"IMAGE_SETTING", L"RECOGNISED_LEN_MIN");
			m_iCanBeRecognisedLenMax = GetValueI(L"IMAGE_SETTING", L"RECOGNISED_LEN_MAX");
			m_iExistMax = GetValueI(L"IMAGE_SETTING", L"EXIST_PX_MAX");
			m_iExistMin = GetValueI(L"IMAGE_SETTING", L"EXIST_PX_MIN");
			m_iExistHeightMax = GetValueI(L"IMAGE_SETTING", L"HEIGHT_MAX");
			m_iExistHeightMin = GetValueI(L"IMAGE_SETTING", L"HEIGHT_MIN");
			m_iExistWidthMax = GetValueI(L"IMAGE_SETTING", L"WIDTH_MAX");
			m_iExistWidthMin = GetValueI(L"IMAGE_SETTING", L"WIDTH_MIN");
			m_bDisplayImageEffect = GetValueI(L"IMAGE_SETTING", L"EFFECT_KIND");
			m_nWHRatio = (float)GetValueF(L"IMAGE_SETTING", L"WHRATIO");
			m_imageDealMode = GetValueI(L"IMAGE_SETTING", L"m_imagedealmode");
			m_blocksize = GetValueI(L"IMAGE_SETTING", L"BLOCKSIZE");
			m_threshold2 = GetValueI(L"IMAGE_SETTING", L"THRESHOLD2");

			m_nTemp_X = GetValueI(L"IMAGE_SETTING", L"m_nTemp_X");
			m_nTemp_Y = GetValueI(L"IMAGE_SETTING", L"m_nTemp_Y");
			m_nTemp_W = GetValueI(L"IMAGE_SETTING", L"m_nTemp_W");
			m_nTemp_H = GetValueI(L"IMAGE_SETTING", L"m_nTemp_H");
			m_nSear_X = GetValueI(L"IMAGE_SETTING", L"m_nSear_X");
			m_nSear_Y = GetValueI(L"IMAGE_SETTING", L"m_nSear_Y");
			m_nSear_W = GetValueI(L"IMAGE_SETTING", L"m_nSear_W");
			m_nSear_H = GetValueI(L"IMAGE_SETTING", L"m_nSear_H");

			//˫���˲�����������

			d = GetValueI(L"IMAGE_SETTING", L"biFilterd");
			sigmaColor = GetValueF(L"IMAGE_SETTING", L"biFilterc");
			sigmaSpace = GetValueF(L"IMAGE_SETTING", L"biFilters");
		}



		if (PARA_IO == (mask&PARA_IO))
		{
			deviceNumber = GetValueI(L"IOCARD_SETTING", L"DEVICE_NUM" );
			m_nTriggerIdPort1 = GetValueI(L"IOCARD_SETTING", L"TriggerIdPort1" );
			m_nTriggerIdPort2 = GetValueI(L"IOCARD_SETTING", L"TriggerIdPort2" );
			m_nTriggerIdPort3 = GetValueI(L"IOCARD_SETTING", L"TriggerIdPort3" );
			m_nTriggerIdPort4 = GetValueI(L"IOCARD_SETTING", L"TriggerIdPort4");
			m_MotorSpinWise = GetValueI(L"IOCARD_SETTING", L"MotorSpinWise" );
			m_MotorSpinWaves = GetValueI(L"IOCARD_SETTING", L"MotorSpinWaves");
			m_MotorSpinSpeed = GetValueI(L"IOCARD_SETTING", L"MotorSpinSpeed");
			m_MotorSpinWavesPerTime = GetValueI(L"IOCARD_SETTING", L"MotorSpinWavesPerTime");
			m_MotorCompenstate = GetValueI(L"IOCARD_SETTING", L"OringinCompenstate");
			m_mode = GetValueI(L"IOCARD_SETTING", L"mode");
			m_MotorSpinWise1 = GetValueI(L"IOCARD_SETTING", L"MotorSpinWise1");
			m_MotorSpinWaves1 = GetValueI(L"IOCARD_SETTING", L"MotorSpinWaves1");
			m_MotorSpinSpeed1 = GetValueI(L"IOCARD_SETTING", L"MotorSpinSpeed1");
			m_MotorSpinWavesPerTime1 = GetValueI(L"IOCARD_SETTING", L"MotorSpinWavesPerTime1");
			m_MotorCompenstate1 = GetValueI(L"IOCARD_SETTING", L"OringinCompenstate1");
			m_mode1 = GetValueI(L"IOCARD_SETTING", L"mode1");
			m_stoptime  = GetValueI(L"IOCARD_SETTING", L"STOPTIME");
			m_DelayLaserTrigger = GetValueI(L"IOCARD_SETTING", L"LSRDELAY");
		}
	
	}

}

void CPreferences::SaveParaFile(UINT item)
{
	UINT mask = item;
	////////////////////////////////////
	if (PARA_PRJ==(mask&PARA_PRJ))
	{
		//m_strCurrentProjectName = GetValue(L"projectSetting",L"CurrentProject");

		SetValueI(L"projectSetting", L"CAMERAID", m_idCamera);
		SetValueI(L"projectSetting", L"TRGOUTBACK", m_nTriggerOutForBack);
		SetValueI(L"projectSetting", L"TRGOUTFRONT", m_nTriggerOutForFront);
		SetValueI(L"projectSetting", L"TRGOUTFINISH", m_nTriggerOutForFinish);
		SetValueI(L"projectSetting", L"TIMERINTERVALS", m_nSetTimerIntervals);
		SetValueI(L"projectSetting", L"SANPDELAY", m_nSnapTimeDelay);
		SetValueF(L"projectSetting", L"SHUTTER", m_shutter);
		SetValueI(L"projectSetting", L"MARKDELAY", m_markdelay);
		SetValueI(L"projectSetting", L"DIR", m_direction);
		
		SetValue(L"projectSetting", L"PWD", m_password);
		SetValueI(L"projectSetting", L"TOTAL_QTY", m_nTotal);
		SetValueI(L"projectSetting", L"UP_QTY", m_nUp);
		SetValueI(L"projectSetting", L"DOWN_QTY", m_nDown);
		SetValueI(L"projectSetting", L"LEFT_QTY", m_nLeft);
		SetValueI(L"projectSetting", L"RIGHT_QTY", m_nRight);
		SetValueI(L"projectSetting", L"VOID_QTY", m_nVoid);
	}

	if(PARA_IMAGE==(mask&PARA_IMAGE))
	{
		SetValueI(L"IMAGE_SETTING",L"EscPixel",m_nESCPixel);
		SetValueI(L"IMAGE_SETTING",L"idThreshNoise",m_idThreshNoise);	//��������ʽ
		SetValueI(L"IMAGE_SETTING",L"idOfThreshold",m_idOfThreshold);	//��ֵ���㷨ѡ��
		SetValueI(L"IMAGE_SETTING",L"idFixThreshold",m_idFixThreshold );	//��ֵ��
		SetValueI(L"IMAGE_SETTING", L"iThreshNoiseLow", m_iThreshNoiseLow);	//������������
		SetValueI(L"IMAGE_SETTING", L"iThreshNoiseHi", m_iThreshNoiseHi);	//������������
		SetValueI(L"IMAGE_SETTING", L"RECOGNISED_LEN_MIN", m_iCanBeRecognisedLenMin); //������������������
		SetValueI(L"IMAGE_SETTING", L"RECOGNISED_LEN_MAX", m_iCanBeRecognisedLenMax);		//�������������
		SetValueI(L"IMAGE_SETTING", L"EXIST_PX_MAX", m_iExistMax); //���������
		SetValueI(L"IMAGE_SETTING", L"EXIST_PX_MIN", m_iExistMin);  //�����ʾͼ��ԭͼ�����ͼ��
		SetValueI(L"IMAGE_SETTING", L"HEIGHT_MAX", m_iExistHeightMax);
		SetValueI(L"IMAGE_SETTING", L"HEIGHT_MIN", m_iExistHeightMin);
		SetValueI(L"IMAGE_SETTING", L"WIDTH_MAX", m_iExistWidthMax);
		SetValueI(L"IMAGE_SETTING", L"WIDTH_MIN", m_iExistWidthMin);
		SetValueI(L"IMAGE_SETTING", L"EFFECT_KIND", m_bDisplayImageEffect);
		SetValueF(L"IMAGE_SETTING", L"WHRATIO", m_nWHRatio);
		SetValueI(L"IMAGE_SETTING", L"m_imagedealmode", m_imageDealMode);
		SetValueI(L"IMAGE_SETTING", L"BLOCKSIZE", m_blocksize);
		SetValueI(L"IMAGE_SETTING", L"THRESHOLD2", m_threshold2);




		SetValueI(L"IMAGE_SETTING",L"m_nTemp_X",m_nTemp_X);
		SetValueI(L"IMAGE_SETTING",L"m_nTemp_Y",m_nTemp_Y);
		SetValueI(L"IMAGE_SETTING",L"m_nTemp_W",m_nTemp_W);
		SetValueI(L"IMAGE_SETTING",L"m_nTemp_H",m_nTemp_H);
		SetValueI(L"IMAGE_SETTING",L"m_nSear_X",m_nSear_X);
		SetValueI(L"IMAGE_SETTING",L"m_nSear_Y",m_nSear_Y);
		SetValueI(L"IMAGE_SETTING",L"m_nSear_W",m_nSear_W);
		SetValueI(L"IMAGE_SETTING",L"m_nSear_H",m_nSear_H);

							//˫���˲�����������
	
		SetValueI(L"IMAGE_SETTING", L"biFilterd", d);
		SetValueF(L"IMAGE_SETTING", L"biFilterc", sigmaColor);
		SetValueF(L"IMAGE_SETTING", L"biFilters", sigmaSpace);
	}
	if(PARA_IO==(mask&PARA_IO))
	{
		SetValueI(L"IOCARD_SETTING", L"DEVICE_NUM", deviceNumber);
		SetValueI(L"IOCARD_SETTING", L"TriggerIdPort1", m_nTriggerIdPort1);
		SetValueI(L"IOCARD_SETTING", L"TriggerIdPort2", m_nTriggerIdPort2);
		SetValueI(L"IOCARD_SETTING", L"TriggerIdPort3", m_nTriggerIdPort3);
		SetValueI(L"IOCARD_SETTING", L"TriggerIdPort4", m_nTriggerIdPort4);
		SetValueI(L"IOCARD_SETTING", L"MotorSpinWise", m_MotorSpinWise);
		SetValueI(L"IOCARD_SETTING", L"MotorSpinWaves", m_MotorSpinWaves);
		SetValueI(L"IOCARD_SETTING", L"MotorSpinSpeed", m_MotorSpinSpeed);
		SetValueI(L"IOCARD_SETTING", L"MotorSpinWavesPerTime",  m_MotorSpinWavesPerTime);
		SetValueI(L"IOCARD_SETTING", L"OringinCompenstate", m_MotorCompenstate);
		SetValueI(L"IOCARD_SETTING", L"mode", m_mode);
		SetValueI(L"IOCARD_SETTING", L"MotorSpinWise1", m_MotorSpinWise1);
		SetValueI(L"IOCARD_SETTING", L"MotorSpinWaves1", m_MotorSpinWaves1);
		SetValueI(L"IOCARD_SETTING", L"MotorSpinSpeed1", m_MotorSpinSpeed1);
		SetValueI(L"IOCARD_SETTING", L"MotorSpinWavesPerTime1", m_MotorSpinWavesPerTime1);
		SetValueI(L"IOCARD_SETTING", L"OringinCompenstate1", m_MotorCompenstate1);
		SetValueI(L"IOCARD_SETTING", L"mode1", m_mode1);
		SetValueI(L"IOCARD_SETTING", L"STOPTIME", m_stoptime);
		SetValueI(L"IOCARD_SETTING", L"LSRDELAY", m_DelayLaserTrigger);
	}
	
	//if(PARA_SN==(mask&PARA_SN))
	//{
	//	SetValue(L"PRJ_SN",L"SN",m_password); 
	//}
	WriteFile();
}





void CPreferences::initData()
{
	//#define PARA_PRJ            (0x1<<0)

	m_shutter = 2500.0;
	m_idCamera = 0;
	m_nTriggerOutForBack = 0;
	m_nTriggerOutForFront = 0;
	m_nTriggerOutForFinish = 0;
	m_nSetTimerIntervals = 10;
	m_nSnapTimeDelay = 1; //����ֵ�������������ӳ���35ms
	m_markdelay = 700;
	m_password = "";			//����

	m_nTotal = 0;
	m_nUp = 0;
	m_nDown = 0;
	m_nLeft = 0;
	m_nRight = 0;
	m_nVoid = 0;	//ͳ���������Ҽ�⵽����Ŀ��û����Ʒ����Ŀ	

					//IMAGE #define PARA_IMAGE			(0x1<<1)
	m_imageDealMode = 0; //OTSU
	m_nESCPixel = 0;
	m_idThreshNoise = 0;		//��������ʽ
	m_idOfThreshold = 0;		//��ֵ���㷨ѡ��
	m_idFixThreshold = 0;
	m_iThreshNoiseLow = 50;		//������ͨ��������
	m_iThreshNoiseHi = 600000;		//������ͨ��������
	m_iCanBeRecognisedLenMin = 2; //ʶ�����������
	m_iCanBeRecognisedLenMax = 800; //ʶ�����������
	m_iExistMax = 200000;			//���������������
	m_iExistMin = 600;			//���������������
	m_iExistHeightMax = 2000;		//������ڸ߶���������
	m_iExistHeightMin = 20;		//������ڸ߶���������
	m_iExistWidthMax = 2000;		//������ڿ����������
	m_iExistWidthMin = 20;		//������ڿ����������
	m_bDisplayImageEffect = 0;  //�����ʾͼ��ԭͼ�����ͼ��
	m_nWHRatio = 1.5;			//ͼ��ĳ����
	m_blocksize = 75;			//����Ӧ��ֵ����С
								//��Ӧ���õĽ����ROI
	m_threshold2 = 30;

	//˫���˲�����������
	d = 9;
	sigmaColor = 20;
	sigmaSpace = 5;

	m_nTemp_X = 0;
	m_nTemp_Y = 0;
	m_nTemp_W = 0;
	m_nTemp_H = 0;
	//��������ͼ���ROI
	m_nSear_X = 0;
	m_nSear_Y = 0;
	m_nSear_W = 0;
	m_nSear_H = 0;

	//IO�� #define PARA_IO				(0x1<<2)
	deviceNumber;			//IO�����к�
	m_nTriggerIdPort1 = 0x1;		//��������
	m_nTriggerIdPort2 = 0x2;		//��Ӧ����
	m_nTriggerIdPort3 = 0x3;		//ԭ���Ӧ����
	m_nTriggerIdPort4 = 0x4;
	m_MotorSpinWise = 1;
	m_MotorSpinWaves = 1600;
	m_MotorSpinSpeed = 80000;
	m_MotorSpinWavesPerTime = 1000;
	m_MotorCompenstate = 1600;
	m_mode = 0;
	m_MotorSpinWise1 = 1;
	m_MotorSpinWaves1 = 1600;
	m_MotorSpinSpeed1 = 230000;
	m_MotorSpinWavesPerTime1 = 1600;
	m_MotorCompenstate1 = 1600;
	m_mode1 = 0;
	m_stoptime = 0;
	m_DelayLaserTrigger = 250;
}