#pragma once


// Reset 对话框

class Reset : public CDialogEx
{
	DECLARE_DYNAMIC(Reset)

public:
	Reset(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~Reset();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHOPSTICK_DIALOG_RESET };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSysreset();
	void ErrorShow(CString str);
	bool startOriginThread();
	bool isOrigin;
	static UINT OriginThread(LPVOID lp);
	afx_msg void OnClose();
};
