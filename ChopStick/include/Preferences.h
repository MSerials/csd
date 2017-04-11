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
	
	CString Encrypt(CString S, WORD Key=KEY); // 加密函数
	CString Decrypt(CString S, WORD Key=KEY); // 解密函数
	
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
	int m_imageDealMode;		//处理
	int m_nESCPixel;
	int m_idThreshNoise;		//背景处理方式
	int m_idOfThreshold;		//二值化算法选择
	int m_idFixThreshold;
	int m_iThreshNoiseLow;		//背景连通区域下限
	int m_iThreshNoiseHi;		//背景连通区域上限
	int m_iCanBeRecognisedLenMin; //识别出方向下限
	int m_iCanBeRecognisedLenMax; //识别出方向下限
	int m_iExistMax;			//物体存在像素上限
	int m_iExistMin;			//物体存在像素下限
	int m_iExistHeightMax;		//物体存在高度像素上限
	int m_iExistHeightMin;		//物体存在高度像素下限
	int m_iExistWidthMax;		//物体存在宽度像素上限
	int m_iExistWidthMin;		//物体存在宽度像素下限
	int m_bDisplayImageEffect;  //结果显示图像原图或处理后图像
	float m_nWHRatio;			//图像的长宽比
	int m_blocksize;			//自适应二值化大小
	int m_threshold2;			//自适应二值化阈值,取得物品？
	int m_direction;			//筷子青面朝向选择
		
	int	d;						//双边滤波的三个参数
	float sigmaColor;
	float sigmaSpace;


	//#define PARA_PRJ            (0x1<<0)
	int m_idCamera;
	int m_nTriggerOutForBack, m_nTriggerOutForFront, m_nTriggerOutForFinish;
	int m_nSetTimerIntervals;
	int m_nSnapTimeDelay;
	unsigned int m_markdelay;		//软件打标延迟
	double m_shutter;				//相机曝光时间
	CString m_password;			//密码

	int m_nTotal, m_nUp, m_nDown, m_nLeft, m_nRight, m_nVoid;	//统计上下左右检测到的数目和没有物品的数目	

	//对应设置的界面的ROI
	int m_nTemp_X, m_nTemp_Y, m_nTemp_W, m_nTemp_H;
	//经过换算图像的ROI
	int m_nSear_X, m_nSear_Y, m_nSear_W, m_nSear_H;

	//IO卡 #define PARA_IO				(0x1<<2)
	int deviceNumber;			//IO卡序列号
	int m_nTriggerIdPort1;		//触发拍照
	int m_nTriggerIdPort2;		//感应器？
	int m_nTriggerIdPort3;		//原点感应器？
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
	//减速时间
	int m_stoptime;
	int m_DelayLaserTrigger;
};
//extern CPreferences *ini;
#endif // !defined(AFX_PREFERENCES_H__4133CB8C_D2DA_4B24_9A3E_66C33ABA291E__INCLUDED_)
