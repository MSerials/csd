// Reset.cpp : 实现文件
//

#include "stdafx.h"
#include "ChopStick.h"
#include "Reset.h"
#include "afxdialogex.h"


// Reset 对话框

IMPLEMENT_DYNAMIC(Reset, CDialogEx)

Reset::Reset(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CHOPSTICK_DIALOG_RESET, pParent)
{
	isOrigin = false;
}

Reset::~Reset()
{
}

void Reset::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Reset, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SYSRESET, &Reset::OnBnClickedButtonSysreset)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// Reset 消息处理程序


void Reset::OnBnClickedButtonSysreset()
{
	// TODO: 在此添加控件通知处理程序代码
	g.g_evtEStop.ResetEvent();
	startOriginThread();
}

void Reset::ErrorShow(CString str)
{
	GetDlgItem(IDC_BUTTON_SYSRESET)->SetWindowText(str);
}

bool Reset::startOriginThread()
{
	if (isOrigin) return false;
	CWinThread* T = AfxBeginThread(OriginThread, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
	if (!T) return false;
	return true;
}


UINT Reset::OriginThread(LPVOID lp)
{
	// TODO: 在此添加控件通知处理程序代码
	Reset* pDlg = (Reset*)lp;
	pDlg->isOrigin = true;
	pDlg->ErrorShow(L"复位中");
	int res = g.mc.FisrtMotorBackToOrigin();
	if (res == TIMEOUT)
	{
		pDlg->ErrorShow(L"复位失败，请检查是否报警，或者原点感应器没接好");
	}
	else if (res == EMERGNCY)
	{
		pDlg->ErrorShow(L"复位失败，急停按钮按下");
	}
	res = g.mc.SeconMotorBackToOrigin();
	if (res == TIMEOUT)
	{
		pDlg->ErrorShow(L"复位失败，请检查是否报警，或者原点感应器没接好");
	}
	else if (res == EMERGNCY)
	{
		pDlg->ErrorShow(L"复位失败，急停按钮按下");
	}
#ifdef PRINTED_VERSION2
	res = g.mc.PrintStepRun();
	if (res == TIMEOUT)
	{
		pDlg->ErrorShow(L"请检查电机是否接好，印花纸感应器是否接好");
	}
	else if (res == EMERGNCY)
	{
		pDlg->ErrorShow(L"复位失败，急停按钮按下");
	}
	g.mc.WriteOutPutBit(OUT_滚花气缸,OFF);
	g.mc.WriteOutPutBit(OUT_拖住气缸, OFF);
	g.mc.WriteOutPutBit(OUT_滚花气缸, OFF);
	g.mc.WriteOutPutBit(OUT_拖住气缸, OFF);
	g.mc.WriteOutPutBit(OUT_滚花气缸, OFF);
	g.mc.WriteOutPutBit(OUT_滚花气缸, OFF);
	g.mc.WriteOutPutBit(OUT_滚花气缸, OFF);
	g.mc.WriteOutPutBit(OUT_拖住气缸, OFF);
	g.mc.WriteOutPutBit(OUT_拖住气缸, OFF);
	g.mc.WriteOutPutBit(OUT_拖住气缸, OFF);
#endif
	if (!res) { g.Controller.SysState = NoError; pDlg->ErrorShow(L"复位");}
	pDlg->isOrigin = false;
	return 0;
}


void Reset::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (isOrigin) return;
	CDialogEx::OnClose();
}
