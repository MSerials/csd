
// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��









#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

//#define _TEST

#define LASER_VERSION
#define MAX_COUNTER 1024

#define g_CameraPos				0
#define g_PushCylPos			-1

#ifdef LASER_VERSION
#define g_RotationCylPos		2
#define g_LaserPos				3
#endif



#include "source.h"

#include "Opencvprocess.h"
#include "include/Preferences.h"
#include "MotionCard.h"


enum {
	NoError, NOALREADY,MTR1ALM,MTR2ALM,EMERGENCY
};

enum {
	STOP,START,PAUSE,EFFECT
};

enum {
	VOID_STATE, TIMEOUT,TIMEOUTIMG,SEVRO,MOTOR,NOCHOPSTICK, THIRDMOTORTIMEOUT, EMERGNCY
};

class exEvent
{
public:
	exEvent() :evt(FALSE, TRUE), isSetEvent(false) {};
	~exEvent() {};
	bool SetEvent() { evt.SetEvent();	isSetEvent = true;	 return true; }
	bool ResetEvent() { evt.ResetEvent(); isSetEvent = false;	 return false; }
	bool EventState() { return isSetEvent; }
	CEvent evt;
private:
	bool isSetEvent;
};

class _global
{
public:
	_global() {
		Controller.SysState = 0;//chushihua
		Controller.SysState |= NOALREADY;
	//	Controller.SysState |= MTR1ALM;
		Controller.SysState |= MTR2ALM;
		Controller.MotorState = STOP;
	};
	~_global() {};

	COpencvprocess opencv;
	CPreferences ini;
	CMotionCard mc;


	//
	bool isActivated;
	struct _SysController
	{
		int SysState;
		int MotorState;
	};
	_SysController Controller;

	exEvent	g_evtImageProc;
	exEvent	g_evtActionProc;//
	exEvent	g_evtGetVideoCapture;
	exEvent	g_evtEStop;
	exEvent	g_evtXLS;
	exEvent	g_evtOrginBack;
	exEvent	g_bCanVideoCapture;
	exEvent	g_evtLoadChopstick;
	exEvent g_evtSensorAlm;

};

extern _global g;

//camera
//#include ""

//opencv
//#include//






//gloabal