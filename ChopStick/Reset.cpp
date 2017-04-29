// Reset.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ChopStick.h"
#include "Reset.h"
#include "afxdialogex.h"


// Reset �Ի���

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


// Reset ��Ϣ�������


void Reset::OnBnClickedButtonSysreset()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	Reset* pDlg = (Reset*)lp;
	pDlg->isOrigin = true;
	pDlg->ErrorShow(L"��λ��");
	int res = g.mc.FisrtMotorBackToOrigin();
	if (res == TIMEOUT)
	{
		pDlg->ErrorShow(L"��λʧ�ܣ������Ƿ񱨾�������ԭ���Ӧ��û�Ӻ�");
	}
	else if (res == EMERGNCY)
	{
		pDlg->ErrorShow(L"��λʧ�ܣ���ͣ��ť����");
	}
	res = g.mc.SeconMotorBackToOrigin();
	if (res == TIMEOUT)
	{
		pDlg->ErrorShow(L"��λʧ�ܣ������Ƿ񱨾�������ԭ���Ӧ��û�Ӻ�");
	}
	else if (res == EMERGNCY)
	{
		pDlg->ErrorShow(L"��λʧ�ܣ���ͣ��ť����");
	}
#ifdef PRINTED_VERSION2
	res = g.mc.PrintStepRun();
	if (res == TIMEOUT)
	{
		pDlg->ErrorShow(L"�������Ƿ�Ӻã�ӡ��ֽ��Ӧ���Ƿ�Ӻ�");
	}
	else if (res == EMERGNCY)
	{
		pDlg->ErrorShow(L"��λʧ�ܣ���ͣ��ť����");
	}
	g.mc.WriteOutPutBit(OUT_��������,OFF);
	g.mc.WriteOutPutBit(OUT_��ס����, OFF);
	g.mc.WriteOutPutBit(OUT_��������, OFF);
	g.mc.WriteOutPutBit(OUT_��ס����, OFF);
	g.mc.WriteOutPutBit(OUT_��������, OFF);
	g.mc.WriteOutPutBit(OUT_��������, OFF);
	g.mc.WriteOutPutBit(OUT_��������, OFF);
	g.mc.WriteOutPutBit(OUT_��ס����, OFF);
	g.mc.WriteOutPutBit(OUT_��ס����, OFF);
	g.mc.WriteOutPutBit(OUT_��ס����, OFF);
#endif
	if (!res) { g.Controller.SysState = NoError; pDlg->ErrorShow(L"��λ");}
	pDlg->isOrigin = false;
	return 0;
}


void Reset::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (isOrigin) return;
	CDialogEx::OnClose();
}
