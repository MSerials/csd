
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持









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