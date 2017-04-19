// Preferences.h: interface for the CPreferences class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PREFERENCES_H__4133CB8C_D2DA_4B24_9A3E_66C33ABA291E__INCLUDED_)
#define AFX_PREFERENCES_H__4133CB8C_D2DA_4B24_9A3E_66C33ABA291E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4996)

#include <afxcoll.h>  // for CStringList
#include <afxtempl.h>
#include <iostream>
#include <vector>
using namespace std;


//string encoding parameters
#define C1            52845
#define C2            22719
#define KEY           1315
#define SHIFT         120


//ini macros
#define ITEM_ROW       8
#define ITEM_COL       10
//ini macros

#define PARA_ALL            0xffffffff
#define PARA_PRJ            (0x1<<0)
#define PARA_IMAGE			(0x1<<1)
#define PARA_IO				(0x1<<2)
#define PARA_SN				(0x1<<3)
class CPreferences  
{
public:
	CString m_strCurrentProjectName;

public:
	CPreferences();
	virtual ~CPreferences();
	
//***********************************************************************************************************
	//all private variables
private:
	//stores pathname of ini file to read/write
	CString path;
	CString dir;
	//all keys are of this time
	struct key
	{
		//list of values in key
		CArray<CString, CString> values; 
		//corresponding list of value names
		CArray<CString, CString> names;
	};
	//list of keys in ini
	CArray<key, key> keys; 
	//corresponding list of key names
	CArray<CString, CString> names; 		
	//all private functions
private:
	//overloaded to take CString
	istream & getline( istream & is, CString & str );
	//returns index of specified value, in the specified key, or -1 if not found
	int FindValue(int keynum, CString valuename);
	//returns index of specified key, or -1 if not found
	int FindKey(CString keyname);
	//public variables
public:
	//will contain error info if one occurs
	//ended up not using much, just in ReadFile and GetValue
	CString error;
	//public functions
public:
	//***************************************
	bool SetIniDir(CString filedir, bool bmodule=true);
	void CreateIniFile(CString fileName);
	void DeleteIniFile(CString fileName);
	void CopyIniFile(CString srcName, CString dstName);
	
	int SetIniFile(CString fileName);
	void LoadParaFile(UINT item);                    //item=IMAGE|LOAD|PRJ.....
	void SaveParaFile(UINT item);	
	
	CString GetAllFileName(CString suffix,int &nCnt,TCHAR separ='|');//such as: suffix=".ini" separ='|'
	CString SplitString(CString srcStr, int nIndex, TCHAR separ ='|');
	
	CString GetModulePath();
	CString GetFilePath();
	CString GetFileDir();
	bool    CreateMultipleDirectory(CString szPath);
	int     SelFolder(CWnd *pWnd, CString &strFolder);
	void    OpenIniFile();
	
	CString Encrypt(CString S, WORD Key=KEY); // ���ܺ���
	CString Decrypt(CString S, WORD Key=KEY); // ���ܺ���
	
	//gets value of [keyname] valuename = 
	//overloaded to return CString, int, and double,
	//returns "", or 0 if key/value not found.  Sets error member to show problem
	CString GetValue(CString keyname, CString valuename); 
	int		GetValueI(CString keyname, CString valuename); 
	double  GetValueF(CString keyname, CString valuename);
	bool	GetValuebool(CString keyname,CString valuename);
	CString GetValueStr(CString keyname, CString valuename);
	//sets value of [keyname] valuename =.
	//specify the optional parameter as false (0) if you do not want it to create
	//the key if it doesn't exist. Returns true if data entered, false otherwise
	//overloaded to accept CString, int, and double
	bool SetValue(CString key, CString valuename, CString value, bool create = 1);
	bool SetValueI(CString key, CString valuename, int value, bool create = 1);
	bool SetValueF(CString key, CString valuename, double value, bool create = 1);
	bool SetValuebool(CString keyname,CString valuename,bool value,bool create=1);
	bool SetValueStr(CString keyname, CString valuename,CString value,bool create=1);
	//returns true if successful, false otherwise
	bool ReadFile();
	//writes data stored in class to ini file
	void WriteFile(); 
	//deletes all stored ini data
	void Reset();
	//returns number of keys currently in the ini
	int GetNumKeys();
	//returns number of values stored for specified key
	int GetNumValues(CString keyname);
	
	//deletes specified value
	//returns true if value existed and deleted, false otherwise
	bool DeleteValue(CString keyname, CString valuename);
	//deletes specified key and all values contained within
	//returns true if key existed and deleted, false otherwise
	bool DeleteKey(CString keyname);

	//For Project Parameter list
	//Image parameter
	//Parameter

	void initData();

	//IMAGE #define PARA_IMAGE			(0x1<<1)
	int m_imageDealMode;		//����
	int m_nESCPixel;
	int m_idThreshNoise;		//����������ʽ
	int m_idOfThreshold;		//��ֵ���㷨ѡ��
	int m_idFixThreshold;
	int m_iThreshNoiseLow;		//������ͨ��������
	int m_iThreshNoiseHi;		//������ͨ��������
	int m_iCanBeRecognisedLenMin; //ʶ�����������
	int m_iCanBeRecognisedLenMax; //ʶ�����������
	int m_iExistMax;			//���������������
	int m_iExistMin;			//���������������
	int m_iExistHeightMax;		//������ڸ߶���������
	int m_iExistHeightMin;		//������ڸ߶���������
	int m_iExistWidthMax;		//������ڿ�����������
	int m_iExistWidthMin;		//������ڿ�����������
	int m_bDisplayImageEffect;  //�����ʾͼ��ԭͼ������ͼ��
	float m_nWHRatio;			//ͼ��ĳ�����
	int m_blocksize;			//����Ӧ��ֵ����С
	int m_threshold2;			//����Ӧ��ֵ����ֵ,ȡ����Ʒ��
	int m_direction;			//�������泯��ѡ��
		
	int	d;						//˫���˲�����������
	float sigmaColor;
	float sigmaSpace;


	//#define PARA_PRJ            (0x1<<0)
	int m_idCamera;
	int m_nTriggerOutForBack, m_nTriggerOutForFront, m_nTriggerOutForFinish;
	int m_nSetTimerIntervals;
	int m_nSnapTimeDelay;
	unsigned int m_markdelay;		//��������ӳ�
	double m_shutter;				//����ع�ʱ��
	CString m_password;			//����

	int m_nTotal, m_nUp, m_nDown, m_nLeft, m_nRight, m_nVoid;	//ͳ���������Ҽ�⵽����Ŀ��û����Ʒ����Ŀ	

	//��Ӧ���õĽ����ROI
	int m_nTemp_X, m_nTemp_Y, m_nTemp_W, m_nTemp_H;
	//��������ͼ���ROI
	int m_nSear_X, m_nSear_Y, m_nSear_W, m_nSear_H;

	//IO�� #define PARA_IO				(0x1<<2)
	int deviceNumber;			//IO�����к�
	int m_nTriggerIdPort1;		//��������
	int m_nTriggerIdPort2;		//��Ӧ����
	int m_nTriggerIdPort3;		//ԭ���Ӧ����
	int m_nTriggerIdPort4;
	int m_MotorSpinWise;
	int m_MotorSpinWaves;
	int m_MotorSpinSpeed;
	int m_MotorSpinWavesPerTime;
	int m_MotorCompenstate;
	int m_mode;
	int m_MotorSpinWise1;
	int m_MotorSpinWaves1;
	int m_MotorSpinSpeed1;
	int m_MotorSpinWavesPerTime1;
	int m_MotorCompenstate1;
	int m_mode1;
	//����ʱ��
	int m_stoptime;
	int m_DelayLaserTrigger;
};
//extern CPreferences *ini;
#endif // !defined(AFX_PREFERENCES_H__4133CB8C_D2DA_4B24_9A3E_66C33ABA291E__INCLUDED_)