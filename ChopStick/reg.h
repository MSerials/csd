#pragma once


// reg �Ի���

class reg : public CDialogEx
{
	DECLARE_DYNAMIC(reg)

public:
	reg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~reg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonReg1();
};
