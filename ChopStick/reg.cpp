// reg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ChopStick.h"
#include "reg.h"
#include "afxdialogex.h"



// reg �Ի���

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


// reg ��Ϣ�������


void reg::OnBnClickedButtonReg1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}
