#pragma once


// Reset �Ի���

class Reset : public CDialogEx
{
	DECLARE_DYNAMIC(Reset)

public:
	Reset(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~Reset();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHOPSTICK_DIALOG_RESET };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSysreset();
	void ErrorShow(CString str);
	bool startOriginThread();
	bool isOrigin;
	static UINT OriginThread(LPVOID lp);
	afx_msg void OnClose();
};
