// reg.cpp : 实现文件
//

#include "stdafx.h"
#include "ChopStick.h"
#include "reg.h"
#include "afxdialogex.h"



// reg 对话框

IMPLEMENT_DYNAMIC(reg, CDialogEx)

reg::reg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_REG, pParent)
{

}

reg::~reg()
{
}

void reg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(reg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_REG1, &reg::OnBnClickedButtonReg1)
END_MESSAGE_MAP()


// reg 消息处理程序


void reg::OnBnClickedButtonReg1()
{
	// TODO: 在此添加控件通知处理程序代码
}
