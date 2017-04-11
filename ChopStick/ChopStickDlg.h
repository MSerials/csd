
// ChopStickDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// CChopStickDlg 对话框
class CChopStickDlg : public CDialogEx
{
// 构造
public:
	CChopStickDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHOPSTICK_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg
		void InitHardWare();
	void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonReset();
	afx_msg void OnBnClickedButtonEffect();
	afx_msg void OnBnClickedButtoncCap();
	afx_msg void OnBnClickedButtonSaveDelay();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedButtoncSetdlg();

	void ErrorShow(CString str);
	void ErrorTip(int ErrorCode);

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	
	void ShowDealedImg(const cv::Mat& mat);

	bool startVideoCaputreThread();
	bool isVideo;
	static UINT VideoCaptureThread(LPVOID lParam);

	bool startImageDealThread();
	bool isDeal;
	static UINT ImageDealThread(LPVOID lParam);


	bool StartProcThread();
	bool isProcedure;
	static UINT ProcThread(LPVOID lParam);

	bool startOriginThread();
	bool isOrigin;
	static UINT OriginlThread(LPVOID lParam);
	
	UINT Procedure();
	UINT CheckBeforeProcedure();


	void InitData() { isVideo = isDeal = isProcedure = false; CapState = STOP; 
	m_ChopstickCounter = 0;
	m_stopCounter = 0;
	m_ImageResult = 0;
	isOrigin = false;
	}



private:
	void Veritfy();
	void InitUI();
	void UpdateUI();
	
	int m_stopCounter;
	int m_ChopstickCounter;
	int m_ImageResult;
	int m_total;
	int m_up;
	int m_left;
	int m_right;
	int m_down;
	int m_none;

	//第一个框应该是显示还是处理？
	int CapState;

	CFont Font;
	CComboBox m_SelDir;


	void DelayForLaser(DWORD tick, int Wait = 270);
	void StartRotation();
	void StartLaser();
	void StartCamera();
	UINT RotationBack();
	bool WaitThreadOver(bool Sign, DWORD TimeOut);
	cv::Mat res;
public:

	void SetCapState(int s) { int CapState = s; }
	bool isVideoing()const{ return isVideo; }
	bool isDealing()const { return isDeal; }
	bool isProcedureing() const { isProcedure; }
	void DisplayMatImg(cv::Mat mat);
	void DisplayImg(BITMAPINFO* bmpinfo, BYTE *imgData);
	afx_msg void OnClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedButtoncSaveimg();
	afx_msg void OnBnClickedButtonActive();
	afx_msg void OnSelchangeComboSeldir();
};

extern CChopStickDlg * pMainFrm;