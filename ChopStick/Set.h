#pragma once


// Set �Ի���

class Set : public CDialogEx
{
	DECLARE_DYNAMIC(Set)

public:
	Set(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~Set();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHOPSTICK_DIALOG_SET };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonMtr1Back();
	afx_msg void OnBnClickedButtonCcw1();
	afx_msg void OnBnClickedButtonCw1();
	afx_msg void OnBnClickedButtonMtr1Back2();
	afx_msg void OnBnClickedButtonCcw2();
	afx_msg void OnBnClickedButtonCw2();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonCyl();


	void UpdateUI();
	afx_msg void OnBnClickedButtonSavepara();
	afx_msg void OnBnClickedButtonSysreset();
	afx_msg void OnBnClickedButtonComfThreshold();
	afx_msg void OnBnClickedButtonComfThreshold2();
	afx_msg void OnBnClickedButtonRead();
	afx_msg void OnBnClickedButtonShow();
	afx_msg void OnSelchangeComboSelthemethod();
	afx_msg void OnBnClickedButtonActivate();
};
extern Set *SetDlg;