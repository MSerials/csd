
// ChopStickDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ChopStick.h"
#include "ChopStickDlg.h"
#include "Reset.h"
#include "Set.h"
#include "afxdialogex.h"
#include "DahengCam.h"
#include "PassWord.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CChopStickDlg * pMainFrm = NULL;

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonReg();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_REG, &CAboutDlg::OnBnClickedButtonReg)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CChopStickDlg �Ի���



CChopStickDlg::CChopStickDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CHOPSTICK_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	 m_total = 0;
	 m_up = 0;
	 m_left = 0;
	 m_right = 0;
	 m_down= 0;
	 m_none = 0;
	 InitData();

	 pMainFrm = this;
	 CString AppPath;
	 ::GetModuleFileName(GetModuleHandle(NULL), AppPath.GetBuffer(300), 300);
	 AppPath.ReleaseBuffer();
	 AppPath = AppPath.Left(AppPath.ReverseFind('\\'));
	 AppPath = AppPath + L"\\" + L"Configuration";
	 g.ini.SetIniDir(AppPath, false);
	 if (0 == g.ini.SetIniFile(L"PrgParameter.ini"))
	 {
		 g.ini.SaveParaFile(PARA_ALL);
	 }
	 g.ini.LoadParaFile(PARA_ALL);
}

void CChopStickDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SELDIR, m_SelDir);
}

BEGIN_MESSAGE_MAP(CChopStickDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CChopStickDlg::OnBnClickedButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_EFFECT, &CChopStickDlg::OnBnClickedButtonEffect)
	ON_BN_CLICKED(IDC_BUTTONC_CAP, &CChopStickDlg::OnBnClickedButtoncCap)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_DELAY, &CChopStickDlg::OnBnClickedButtonSaveDelay)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CChopStickDlg::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTONC_SETDLG, &CChopStickDlg::OnBnClickedButtoncSetdlg)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTONC_SAVEIMG, &CChopStickDlg::OnBnClickedButtoncSaveimg)
	ON_BN_CLICKED(IDC_BUTTON_ACTIVE, &CChopStickDlg::OnBnClickedButtonActive)
	ON_CBN_SELCHANGE(IDC_COMBO_SELDIR, &CChopStickDlg::OnSelchangeComboSeldir)
END_MESSAGE_MAP()


// CChopStickDlg ��Ϣ�������

BOOL CChopStickDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
#ifdef _TEST
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
#endif

	//�½��������
	dahengcam = new CDahengCam();
	if (NULL != dahengcam->m_hDevice) GXSetFloat(dahengcam->m_hDevice, GX_FLOAT_EXPOSURE_TIME, g.ini.m_shutter);

	Veritfy();

	InitUI();
#ifdef PRINTED_VERSION2
	startPrintThread();
#endif

	startVideoCaputreThread();

	startImageDealThread();

	StartProcThread();

	InitHardWare();

	int cx, cy;
	cx = GetSystemMetrics(SM_CXSCREEN);
	cy = GetSystemMetrics(SM_CYSCREEN);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CChopStickDlg::Veritfy()
{
	PassWord * pw = new PassWord();
	//if (!pw->CheckTrial()) { AfxMessageBox(L"error in authorise"); exit(0); }
	if (0 == pw->isSNOK()) {  GetDlgItem(IDC_BUTTON_ACTIVE)->ShowWindow(FALSE); goto end1;}
	
	int res = pw->GetDate();
	if (res == 1)
	{
		goto end1;
	//	EndDialog(0);
	}
	else if (-1 == res)
	{
		AfxMessageBox(L"90�������Ѿ���ȥ��û�н���ע��,�޷�ʹ�ã�");
		CAboutDlg adlg;
		adlg.DoModal();
	}
	else if (0 == res)
	{
		AfxMessageBox(L"û�ù���ԱȨ�����У��޷�����ʹ�ã�");
		exit(0);
	}
	else
	{
		AfxMessageBox(L"ϵͳ�ļ���Ϣ���ƻ���");
		exit(0);
	}

	end1:
	delete pw;
}


void CChopStickDlg::InitHardWare()
{
	if (g.mc.Init())
		//��Ӧ��ɨ��
		SetTimer(1, 100, NULL); //
}

void CChopStickDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CChopStickDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//if(1 == m_CaptionPlayMode) DisplayImg();
		CRect   rect;
		CPaintDC   dc(this);
		GetClientRect(rect);
		dc.FillSolidRect(rect, RGB(32, 32, 35));
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CChopStickDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


HBRUSH CChopStickDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
//	switch (pWnd->GetDlgCtrlID())
//	{
//	case IDC_INFO:
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(32, 32, 64));
//	}

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}

void CChopStickDlg::UpdateUI()
{
	CString str;
	str.Format(_T(" �������:%d"), m_total);
	GetDlgItem(IDC_STATIC_TOTAL)->SetWindowText(str);
	str.Format(_T(" ��������:%d"), m_up);
	GetDlgItem(IDC_STATIC_UP)->SetWindowText(str);
	str.Format(_T(" ��������:%d"), m_down);
	GetDlgItem(IDC_STATIC_DOWN)->SetWindowText(str);
	str.Format(_T(" ��������:%d"), m_left);
	GetDlgItem(IDC_STATIC_LEFT)->SetWindowText(str);
	str.Format(_T(" ��������:%d"), m_right);
	GetDlgItem(IDC_STATIC_RIGHT)->SetWindowText(str);
	str.Format(_T(" ��������:%d"), m_none);
	GetDlgItem(IDC_STATIC_NONE)->SetWindowText(str);
	
}

void CChopStickDlg::InitUI()
{
	UpdateUI();
	SetWindowText(_T("�㽭���������ܼ������޹�˾-�����������-2017.3.27"));
	CString str;
	str.Format(_T("������ʱ: %d ms"), g.ini.m_markdelay);
	GetDlgItem(IDC_EDIT_DELAY)->SetWindowText(str);

	//m_markdelay = ini->m_markdelay;
	m_SelDir.AddString(L"���泯��");
	m_SelDir.AddString(L"���泯��");
	m_SelDir.AddString(L"���泯��");
	m_SelDir.AddString(L"���泯��");
	CComboBox *pbox = (CComboBox*)GetDlgItem(IDC_COMBO_SELDIR);

	switch (g.ini.m_direction)
	{
	case UP:pbox->SetCurSel(0); break;
	case DOWN:pbox->SetCurSel(1); break;
	case LEFT:pbox->SetCurSel(2); break;
	case RIGHT:pbox->SetCurSel(3); break;
	default:g.ini.m_direction = UP; pbox->SetCurSel(0); g.ini.SaveParaFile(PARA_ALL); break;
	}

	if (g.isActivated) { GetDlgItem(IDC_BUTTON_ACTIVE)->ShowWindow(FALSE); return; }
/*
	CFont font;
	font.CreateFont(10, 10, 0, 0, FW_BLACK, FALSE, FALSE,
		FALSE, GB2312_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		FIXED_PITCH | FF_MODERN, _T("����"));
		*/
		//	GetDlgItem(IDC_STATIC_TOTAL)->SetFont(&font);
		//	GetDlgItem(IDC_STATIC_TOTAL)->

//	font.DeleteObject();

}

void CChopStickDlg::OnBnClickedButtonReset()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (WAIT_OBJECT_0 == ::WaitForSingleObject(g.g_evtActionProc.evt, 0)) return;
	Reset ResetDlg;
	ResetDlg.DoModal();
}


void CChopStickDlg::OnBnClickedButtonEffect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(STOP == g.Controller.MotorState)
		CapState = EFFECT;
}


void CChopStickDlg::OnBnClickedButtoncCap()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (STOP == g.Controller.MotorState)
		CapState = START;
}

void CChopStickDlg::OnBnClickedButtonClear()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_total = 0;
	m_up = 0;
	m_left = 0;
	m_right = 0;
	m_down = 0;
	m_none = 0;
	InitUI();
}

void CChopStickDlg::OnBnClickedButtonSaveDelay()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	GetDlgItem(IDC_EDIT_DELAY)->GetWindowText(str);
	g.ini.m_markdelay = _ttoi(str);
	if (g.ini.m_markdelay < 20)
	{
		g.ini.m_markdelay = 20;
	}
	str.Format(_T("������ʱ: %d ms"), g.ini.m_markdelay);
	GetDlgItem(IDC_EDIT_DELAY)->SetWindowText(str);
	g.ini.SaveParaFile(PARA_ALL);
}





void CChopStickDlg::OnBnClickedButtoncSetdlg()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	static Set* SedDialog = NULL;
	if (!SedDialog)
	{
		SedDialog = new Set();
		SedDialog->Create(IDD_CHOPSTICK_DIALOG_SET, this);
	}
	SedDialog->ShowWindow(SW_SHOW);
	SedDialog->UpdateUI();
}

//��ʾͼƬ 1�Ǵ�� 2��С��


//��һ��������ʾ���ͼƬ
void CChopStickDlg::DisplayMatImg(cv::Mat mat)
{
	if (mat.empty())  return;                        //  ˢ�� DIBλͼ��Ϣ

	CStatic *pWnd = (CStatic*)GetDlgItem(IDC_STATIC_SHOW);
	CRect objRect;
	pWnd->GetClientRect(objRect);
	CDC *pDC = pWnd->GetDC();
	HDC  hDC = pDC->GetSafeHdc();
	int nWndWidth = objRect.right - objRect.left;
	int nWndHeight = objRect.bottom - objRect.top;

	if (g.opencv.m_lpBmi)
		free(g.opencv.m_lpBmi);
	g.opencv.m_lpBmi = g.opencv.CreateMapInfo(mat);

	g.opencv.CopyMatDataToMem(mat);

	::SetStretchBltMode(hDC, COLORONCOLOR);
	::StretchDIBits(hDC,
		0, 0, nWndWidth, nWndHeight,
		0, 0, mat.cols, mat.rows,
		g.opencv.ImageData, g.opencv.m_lpBmi, DIB_RGB_COLORS, SRCCOPY);
	pWnd->ReleaseDC(pDC);
}

//�ڶ���������ʾͼƬ
void CChopStickDlg::DisplayImg(BITMAPINFO* bmpinfo, BYTE *imgData) {

	int  nWndWidth = 0;
	int  nWndHeight = 0;

	CStatic *pWnd = (CStatic*)GetDlgItem(IDC_STATIC_SHOW2);
	CRect objRect;
	pWnd->GetClientRect(objRect);
	nWndWidth = objRect.right - objRect.left;
	nWndHeight = objRect.bottom - objRect.top;
	CDC *pDC = pWnd->GetDC();
	HDC  hDC = pDC->GetSafeHdc();

	// ������ø���䣬����ͼ�����ˮ��
	::SetStretchBltMode(hDC, COLORONCOLOR);
	::StretchDIBits(hDC,
		0,
		0,
		nWndWidth,
		nWndHeight,
		0,
		0,
		bmpinfo->bmiHeader.biWidth,
		bmpinfo->bmiHeader.biHeight,
		imgData,
		bmpinfo,
		DIB_RGB_COLORS,
		SRCCOPY
	);
	pWnd->ReleaseDC(pDC);
};



void CChopStickDlg::ErrorShow(CString str)
{
	GetDlgItem(IDC_STATIC_ERROR_INFO)->SetWindowText(str);
}


void CChopStickDlg::ErrorTip(int ErrorCode)
{
	CString str;
	if (NOALREADY == (NOALREADY & ErrorCode)) {str += L"��Ҫ��λ ";}
	if (MTR1ALM == (MTR1ALM & ErrorCode)) { str += L"����������� "; }
	if (MTR2ALM == (MTR2ALM & ErrorCode)) { str += L"�ŷ�������� "; }
	AfxMessageBox(str);
}

void CChopStickDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	if (1 == nIDEvent)
	{
		CString str;
		static BOOL embutton = TRUE;
		static int counter = 0;
		BOOL emState = !g.mc.ReadInputBit(IN_EMERGENCY);  //�߼���
		BOOL startState = g.mc.ReadInputBit(IN_START);
		BOOL pauseState = g.mc.ReadInputBit(IN_PAUSE);
		BOOL alamState = !g.mc.ReadInputBit(IN_SEVRO_ALM);
		static BOOL oldalamState = alamState;				//��֤�˵�һ�ζ�ȡ״̬��һ����

		if (emState)
		{
			startState = FALSE;						//��ͣ��ť�ð���ʧЧ
			g.g_evtEStop.SetEvent();
			g.mc.Stop(FIRST_MOTOR);
			g.mc.Stop(SECOND_MOTOR);
			g.mc.WriteOutPutBit(OUT_ALM, ON);
			g.mc.WriteOutPutBit(OUT_SERVO_MOTOR, OFF);
			g.Controller.SysState |= EMERGENCY;
			str += L"��ͣ��ť���� ";
		}
		else
		{
		//	g.mc.WriteOutPutBit(OUT_ALM, OFF);
			g.mc.WriteOutPutBit(OUT_SERVO_MOTOR, ON);
			g.Controller.SysState &= ~EMERGENCY;
		}


		if (oldalamState != alamState && !alamState)
		{
			g.mc.WriteOutPutBit(OUT_ALM, OFF);
			g.Controller.SysState &= ~MTR2ALM;
		}


		if (alamState)
		{
			g.g_evtEStop.SetEvent();
			g.mc.WriteOutPutBit(OUT_ALM, ON);
			g.Controller.SysState |= MTR2ALM;
			str += L"�ŷ�������� ";

		}


		if (startState && pauseState)
		{
			counter++;
			g.mc.WriteOutPutBit(OUT_ALM, OFF);
			if (counter > 10)
			{
				g.g_evtEStop.ResetEvent();
				startOriginThread();
				counter = -100;
			}
		}
		else if (startState && !pauseState)
		{
			m_stopCounter = 0;
			g.mc.WriteOutPutBit(OUT_ALM, OFF);
			counter = 0;
			if (STOP == (STOP & g.Controller.MotorState) && !g.Controller.SysState)
			{
				//yunxing
				CapState = STOP;
				g.Controller.MotorState = START;
				write_output(OUT_START_INDICATOR, ON);
				write_output(OUT_PAUSE_INDICATOR, OFF);
				g.mc.WriteOutPutBit(OUT_ALM, OFF); //
				g.g_evtActionProc.SetEvent();
			}
		}
		else if (!startState && pauseState)
		{
			counter = 0;
			g.mc.WriteOutPutBit(OUT_ALM, OFF);
			g.Controller.MotorState = STOP;
			write_output(OUT_START_INDICATOR, OFF);
			write_output(OUT_PAUSE_INDICATOR, ON);
			g.g_evtActionProc.ResetEvent();
		}
		else
		{
			//write_output(OUT_START_INDICATOR, OFF);
			//write_output(OUT_PAUSE_INDICATOR, OFF);
			counter = 0;
		}

		if (emState != embutton && emState)
		{
			embutton = emState;
			str += L"��ͣ��ť���£��븴λ��";
		}
		else if (emState != embutton && !emState)
		{
			g.mc.WriteOutPutBit(OUT_ALM, OFF);
		}
		embutton = emState;

		if (!str.IsEmpty())
		{
			ErrorShow(str);
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CChopStickDlg::ShowDealedImg(const cv::Mat& mat)
{
//	cv::Mat res;
	g.opencv.MatImgProcess(mat, res);
	DisplayMatImg(res);
	/**
	int bRet = INVALID;
	//���ͼƬ������
	bRet = cvProcess.MatImgProcess(cvProcess.m_RawMatImg);
	if (bRet)
	{
		DisplayMatImg(cvProcess.m_ResultMatImg);
		if (setdlgEnable)
		{
			setdlg->RefreshData(); //��ʾͼƬֱ��ͼ
		}

	}
	return bRet;
	*/
}


//��ͨ���ȼ��߳̾Ϳ�����
bool CChopStickDlg::startVideoCaputreThread()
{
	CWinThread* T = AfxBeginThread(VideoCaptureThread, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
	if (!T) return false;
	return true;
}

UINT CChopStickDlg::VideoCaptureThread(LPVOID lParam)
{
	CChopStickDlg* pDlg = (CChopStickDlg*)lParam;
	pDlg->isVideo = true;
	for (;;)
	{
		if (WAIT_OBJECT_0 == WaitForSingleObject(g.g_evtGetVideoCapture.evt, 2)) //������д���
		{
			pDlg->DisplayImg((BITMAPINFO*)dahengcam->m_pBmpInfo, dahengcam->m_pImgBuffer);
			if (EFFECT == pDlg->CapState)
			{
				pDlg->ShowDealedImg(g.opencv.BmpToMat((BITMAPINFO*)dahengcam->m_pBmpInfo, dahengcam->m_pImgBuffer));
			}
			else if (START == pDlg->CapState)
			{		
				pDlg->DisplayMatImg(g.opencv.BmpToMat((BITMAPINFO*)dahengcam->m_pBmpInfo, dahengcam->m_pImgBuffer));
			}
			g.g_evtGetVideoCapture.ResetEvent();
		}
		Sleep(10);
	}
	pDlg->isVideo = false;
	return 0;
}


bool  CChopStickDlg::startPrintThread()
{
	CWinThread* T = AfxBeginThread(PrintThread, this, THREAD_PRIORITY_ABOVE_NORMAL, 0, 0, NULL);
	if (!T) return false;
	return true;
}

UINT CChopStickDlg::PrintThread(LPVOID lParam)
{
	CChopStickDlg* pDlg = (CChopStickDlg*)lParam;
	for (;;)
	{
		if (WAIT_OBJECT_0 == ::WaitForSingleObject(g.g_evtPrint.evt, 1))
		{
			switch (g.mc.actioninfo[g.mc.caculate_position(pDlg->m_ChopstickCounter, g_PrintPos)].RoationInfo)
			{
				case DOWN:
				case LEFT:
				case RIGHT:
				case UP:
				{

					for (int i = 0; i < MAX_COUNTER; i++) g.mc.WriteOutPutBit(OUT_��ס����, ON);
					Sleep(180);
					for (int i = 0; i < MAX_COUNTER; i++) g.mc.WriteOutPutBit(OUT_��������, ON);
					Sleep(1000);
					for (int i = 0; i < MAX_COUNTER; i++) g.mc.WriteOutPutBit(OUT_��ס����, OFF);
					for (int i = 0; i < MAX_COUNTER; i++) g.mc.WriteOutPutBit(OUT_��������, OFF);
					Sleep(1100);
					g.mc.PrintStepRun();
				}
			break;
			default:break;
			}
		}
			g.g_evtPrint.ResetEvent();
	}

	return 0;
}


//�߼���
bool CChopStickDlg::startImageDealThread()
{
	CWinThread* T = AfxBeginThread(ImageDealThread, this, THREAD_PRIORITY_ABOVE_NORMAL, 0, 0, NULL);
	if (!T) return false;
	return true;
}

UINT CChopStickDlg::ImageDealThread(LPVOID lParam)
{
	CChopStickDlg* pDlg = (CChopStickDlg*)lParam;
	pDlg->isDeal = true;
	for(;;)
	{
		if (WAIT_OBJECT_0 == ::WaitForSingleObject(g.g_evtImageProc.evt,1))
		{
			//cv::Mat res;
			pDlg->m_ImageResult = g.opencv.MatImgProcess(g.opencv.BmpToMat((BITMAPINFO*)dahengcam->m_pBmpInfo, dahengcam->m_pImgBuffer), pDlg->res);
			pDlg->DisplayMatImg(pDlg->res);
			g.g_evtImageProc.ResetEvent();
		}
	}
	pDlg->isDeal = false;
	return 0;
}

//�м伶��
bool CChopStickDlg::StartProcThread()
{
	CWinThread* T = AfxBeginThread(ProcThread, this, THREAD_PRIORITY_ABOVE_NORMAL, 0, 0, NULL);
	if (!T) return false;
	return true;
}





UINT CChopStickDlg::ProcThread(LPVOID lParam)
{
	CChopStickDlg* pDlg = (CChopStickDlg*)lParam;
	pDlg->isProcedure = true;

	for (;;)
	{
		if (WAIT_OBJECT_0 == WaitForSingleObject(g.g_evtActionProc.evt, 1))
		{
			UINT res = pDlg->Procedure();
			if (res)
			{
				g.g_evtActionProc.ResetEvent();
				switch (res)
				{
				case NoError:			pDlg->ErrorShow(L"��������");								break;
#ifdef PRINTED_VERSION
			//	case SenorUp:

				//	pMainDlg->showError(L"������ָ���ӻ���ӡ��������û�򿪣�"); break;
#endif
#ifdef LASER_VERSION
					//		pMainDlg->showError(L"���޸�Ӧ��������ûͨ�����߸�Ӧ���ɶ�"); break;
#endif
				case TIMEOUT:			pDlg->ErrorShow(L"�����ʱ");								break;
				case TIMEOUTIMG:		pDlg->ErrorShow(L"ͼ����ʱ");							break;
				case SEVRO:				pDlg->ErrorShow(L"�ŷ��ٶ�̫���򱨾�");						break;
				case MOTOR:				pDlg->ErrorShow(L"��Ӧ���޷���Ӧ��λ�û��ߵ������ԭ��");	break;
				case NOCHOPSTICK:		pDlg->ErrorShow(L"û��⵽����");							break;
				case THIRDMOTORTIMEOUT:	pDlg->ErrorShow(L"ӡ�������ʱ");							break;
				case EMERGNCY:			pDlg->ErrorShow(L"��ͣ��ť������");							break;
				case PRINT_TIMEOUT:		pDlg->ErrorShow(L"ӡ����ӡˢ��ʱ"); break;
				default:break;
				}
			}
		}
		Sleep(20);
	}

	pDlg->isProcedure = false;
	return 0;
}



bool CChopStickDlg::startOriginThread()
{
	if (WAIT_OBJECT_0 == ::WaitForSingleObject(g.g_evtActionProc.evt, 0)) return false;//���е�ʱ���ܸ�λ
	if (isOrigin) return false;
	CWinThread* T = AfxBeginThread(OriginlThread, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
	if (!T) return false;
	return true;
}

UINT CChopStickDlg::OriginlThread(LPVOID lParam)
{
	CChopStickDlg* pDlg = (CChopStickDlg*)lParam;
	g.Controller.SysState |= NOALREADY;
	pDlg->isOrigin = true;
	write_output(OUT_START_INDICATOR, ON);
	write_output(OUT_PAUSE_INDICATOR, ON);
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

	if (res) goto end;
	res = g.mc.SeconMotorBackToOrigin();
	if (res == TIMEOUT)
	{
		pDlg->ErrorShow(L"��λʧ�ܣ������Ƿ񱨾�������ԭ���Ӧ��û�Ӻ�");
	}
	else if (res == EMERGNCY)
	{
		pDlg->ErrorShow(L"��λʧ�ܣ���ͣ��ť����");
	}

	end:
	g.Controller.SysState &= ~NOALREADY;
	if(!res) g.Controller.SysState = NoError;
	write_output(OUT_START_INDICATOR, OFF);
	write_output(OUT_PAUSE_INDICATOR, OFF);
	pDlg->isOrigin = false;
	return 0;
}


UINT CChopStickDlg::CheckBeforeProcedure()
{
	if (!read_input_state(IN_EMERGENCY))	//��鼱ͣ����״̬
		return EMERGNCY;
	if (!read_input_state(IN_SEVRO_ALM))
		return SEVRO;
	if (0 != get_position(FIRST_MOTOR))
		return MOTOR;
	return NoError;
}



void CChopStickDlg::StartCamera()
{

	g.g_evtImageProc.SetEvent();
	return;
}



#ifdef PRINTED_VERSION2

UINT CChopStickDlg::Procedure()
{
	UINT flag = CheckBeforeProcedure();				if (NoError != flag) return flag; 
	flag = g.mc.ConveyorStepRun();					if (NoError != flag) return flag;
	DWORD PrintTick = GetTickCount();
	g.g_evtPrint.SetEvent();
	StartRotation(); 
	Sleep(200);
	StartCamera();
	RotationBack();
	for (; g.g_evtImageProc.EventState();)
	{
		//	DWORD StartTick = GetTickCount();
		if ((GetTickCount() - PrintTick) > 20000)
		{
			return TIMEOUTIMG;
			//	break;
		}
	}
	g.mc.actioninfo[g.mc.caculate_position(m_ChopstickCounter, g_CameraPos)].RoationInfo = m_ImageResult;
	DWORD EndTickTime = GetTickCount();
	if ((EndTickTime - PrintTick) < g.ini.m_markdelay)
	{
		Sleep(g.ini.m_markdelay - (EndTickTime - PrintTick));
	}

	//����Ƿ�λ
	for (; g.g_evtPrint.EventState();)
	{
		//	DWORD StartTick = GetTickCount();
		if ((GetTickCount() - PrintTick) > 20000)
		{
			return PRINT_TIMEOUT;
			//	break;
		}
	}

	m_ChopstickCounter++;
	m_stopCounter++;
	if (m_stopCounter > 10) { flag = NOCHOPSTICK; g.mc.WriteOutPutBit(OUT_ALM, ON); };
	UpdateUI();
	//	UpdateData(FALSE);
	return flag;
} 






#endif


#ifdef LAYSER_VERSION

UINT CChopStickDlg::Procedure()
{
	UINT flag = CheckBeforeProcedure();				if (NoError != flag) return flag;
	//��һ�����ȼ��ߵ��̣߳���֤��Ӧ��׼ȷ
	flag = g.mc.ConveyorStepRun();					if (NoError != flag) return flag;
	StartLaser();
	DWORD LaserTick = GetTickCount();
	StartRotation();
	int DelayTime = g.ini.m_DelayLaserTrigger < 270 ? 270 : g.ini.m_DelayLaserTrigger;
#ifdef LASER_VERSION
	DelayForLaser(LaserTick,(DWORD)DelayTime);
#else
	Sleep(DelayTime);
#endif
	StartCamera();
	flag = RotationBack();							if (NoError != flag) return flag;
	for (; g.g_evtImageProc.EventState();)
	{
		//	DWORD StartTick = GetTickCount();
		if ((GetTickCount() - LaserTick) > 20000)
		{
			return TIMEOUTIMG;
		//	break;
		}
	}
	g.mc.actioninfo[g.mc.caculate_position(m_ChopstickCounter,g_CameraPos)].RoationInfo = m_ImageResult;
	DWORD EndTickTime = GetTickCount();
	if ((EndTickTime - LaserTick) < g.ini.m_markdelay)
	{
		Sleep(g.ini.m_markdelay - (EndTickTime - LaserTick));
	}

	//����Ƿ�λ

	m_ChopstickCounter++;
	m_stopCounter++;
	if (m_stopCounter > 10) { flag = NOCHOPSTICK; g.mc.WriteOutPutBit(OUT_ALM, ON); };
	UpdateUI();
//	UpdateData(FALSE);
	return flag;
}

#endif

#ifdef LASER_VERSION
void CChopStickDlg::DelayForLaser(DWORD tick, int Wait)
{
	DWORD TimeCosumed = GetTickCount() - tick;
	if (TimeCosumed < Wait)
	{
		Sleep(Wait - TimeCosumed);
	}
	for (int i = 0; i < MAX_COUNTER; i++) { write_output(OUT_LASER_ACTION, OFF); }
}
#endif

void CChopStickDlg::StartRotation()
{
	switch (g.mc.actioninfo[g.mc.caculate_position(m_ChopstickCounter,g_RotationCylPos)].RoationInfo)
	{
	case DOWN:	for (int i = 0; i < MAX_COUNTER; i++) { g.mc.WriteOutPutBit(OUT_TRAP_CYL, ON); }
				//if (!act->wait_input_inverse(IN_TRAP_IS_OPEN, 2000)) flag = SenorUp;/*�ȴ���Ӧ�����޸�Ӧ����*/
				Sleep(110);/* if (!wait_sensor_timeout(IN_TRAP_IS_CLOSE, 2000)) flag = SenorDown;*/  g.mc.Clock180(); 	m_down++;	m_total++; break;
	case LEFT:	for (int i = 0; i < MAX_COUNTER; i++) { g.mc.WriteOutPutBit(OUT_TRAP_CYL, ON); }
				//if (!act->wait_input_inverse(IN_TRAP_IS_OPEN, 2000)) flag = SenorUp;/*�ȴ���Ӧ�����޸�Ӧ����*/
				Sleep(110);/* if (!wait_sensor_timeout(IN_TRAP_IS_CLOSE, 2000)) flag = SenorDown; */g.mc.CounterClock90();	m_left++;	m_total++; break;
	case RIGHT:	for (int i = 0; i < MAX_COUNTER; i++) { g.mc.WriteOutPutBit(OUT_TRAP_CYL, ON); }
				//if (!act->wait_input_inverse(IN_TRAP_IS_OPEN, 2000)) flag = SenorUp;/*�ȴ���Ӧ�����޸�Ӧ����*/
				Sleep(110); /*if (!wait_sensor_timeout(IN_TRAP_IS_CLOSE, 2000)) flag = SenorDown; */g.mc.Clock90();			m_right++;	m_total++; break;
	case UP:		m_up++;	m_total++; break;
	case NOOBJECT:	m_none++; m_total++; break;
	default:break;
	}
}

void CChopStickDlg::StartLaser()
{
#ifdef PRINTED_VERSION
	return;

#endif

#ifdef LAYSER_VERSION
	switch (g.mc.actioninfo[g.mc.caculate_position(m_ChopstickCounter, g_LaserPos)].RoationInfo)
	{
	case UP:
	case DOWN:
	case LEFT:
	case RIGHT:
//		m_stopCounter = 0;
		Sleep(100);
		for (int i = 0; i < MAX_COUNTER; i++) { write_output(OUT_LASER_ACTION, ON); }
		break;
	case NOOBJECT:break;
	default:break;
	}
#endif
}

UINT CChopStickDlg::RotationBack()
{
	UINT flag = NoError;
	switch (g.mc.actioninfo[g.mc.caculate_position(m_ChopstickCounter, g_RotationCylPos)].RoationInfo)
	{
	case DOWN:m_stopCounter = 0; if (!g.mc.WaitMotorTimeout(FIRST_MOTOR, 2000)) flag = TIMEOUT; Sleep(50); for (int i = 0; i < MAX_COUNTER; i++) { g.mc.WriteOutPutBit(OUT_TRAP_CYL, OFF); } /*if (!wait_sensor_timeout(IN_TRAP_IS_OPEN, 2000))flag = SenorUp; */Sleep(190);   g.mc.CounterClock180();	if (!g.mc.WaitMotorTimeout(FIRST_MOTOR, 2000)) flag = MOTOR; Sleep(150); break;
	case LEFT:m_stopCounter = 0; if (!g.mc.WaitMotorTimeout(FIRST_MOTOR, 2000)) flag = TIMEOUT; Sleep(50); for (int i = 0; i < MAX_COUNTER; i++) { g.mc.WriteOutPutBit(OUT_TRAP_CYL, OFF); } /* if (!wait_sensor_timeout(IN_TRAP_IS_OPEN, 2000))flag = SenorUp;*/ Sleep(190); g.mc.Clock90();				if (!g.mc.WaitMotorTimeout(FIRST_MOTOR, 2000)) flag = MOTOR; Sleep(150);  break;
	case RIGHT:m_stopCounter = 0; if (!g.mc.WaitMotorTimeout(FIRST_MOTOR, 2000)) flag = TIMEOUT; Sleep(50); for (int i = 0; i < MAX_COUNTER; i++) { g.mc.WriteOutPutBit(OUT_TRAP_CYL, OFF); } /*if (!wait_sensor_timeout(IN_TRAP_IS_OPEN, 2000))flag = SenorUp;*/ Sleep(190); g.mc.CounterClock90();	if (!g.mc.WaitMotorTimeout(FIRST_MOTOR, 2000)) flag = MOTOR; Sleep(150);  break;
	case UP:m_stopCounter = 0;  for (int i = 0; i < MAX_COUNTER; i++) { g.mc.WriteOutPutBit(OUT_TRAP_CYL, OFF); }  break;
	default:break;
	}
	return flag;
}

bool CChopStickDlg::WaitThreadOver(bool Sign, DWORD TimeOut)
{
	DWORD Tick = GetTickCount();
	for (; Sign;)
	{
		if ((GetTickCount() - Tick) > TimeOut) return false;
		Sleep(1); //��ʡcpu��
	}
	return true;
}



void CChopStickDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (MessageBox(L"ȷ���˳�", L"", MB_YESNO | MB_ICONQUESTION) == IDYES)
	{

		for (int i = 0; i < MAX_COUNTER; i++)
		{
			write_output(OUT_START_INDICATOR, OFF);
			write_output(OUT_PAUSE_INDICATOR, OFF);
			write_output(OUT_SERVO_MOTOR, OFF);

#ifdef LASER_VERSION
			write_output(OUT_LASER_ACTION, OFF);

#endif
#ifdef PRINTED_VERSION
			write_output(OUT_THIRD_MOTOR, OFF);
			write_output(OUT_HOLD_CHOPSTIC, OFF);

#endif
			write_output(OUT_ALM, OFF);

		}
		d1000_board_close();
		Sleep(100);
	}
	else
		return;
	CDialogEx::OnClose();
}


void CAboutDlg::OnBnClickedButtonReg()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	PassWord * pw = new PassWord();

	CString str;
	GetDlgItem(IDC_EDIT_CODE)->GetWindowText(str);

	LPCTSTR tmp = (LPCTSTR)str;
	pw->RegSerial(tmp);
	if (1 == pw->GetDate())
	{
		EndDialog(0);
	}

	if (0 != pw->isSNOK())
	{
		AfxMessageBox(L"ע�������");
	}
	else
	{
		EndDialog(0);
	}
	delete pw;

}


BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	PassWord * pw = new PassWord();
	GetDlgItem(IDC_EDIT_SERIAL)->SetWindowText(pw->GetHDserial());
	delete pw;
	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void CAboutDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	exit(0);
	CDialogEx::OnClose();
}


int CChopStickDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	//ModifyStyle(WS_CAPTION, 0);
	return 0;
}


void CChopStickDlg::OnBnClickedButtoncSaveimg()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	g.opencv.SaveFile(res);
}


void CChopStickDlg::OnBnClickedButtonActive()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CAboutDlg adlg;
	adlg.DoModal();
}


void CChopStickDlg::OnSelchangeComboSeldir()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CComboBox *pbox = (CComboBox*)GetDlgItem(IDC_COMBO_SELDIR);

	CString str;
	int nSel;
	// ��ȡ��Ͽ�ؼ����б����ѡ���������   
	nSel =pbox->GetCurSel();
	// ����ѡ����������ȡ�����ַ���   
	pbox->GetLBText(nSel, str);

	if (L"���泯��" == str)
		g.ini.m_direction = UP;
	else if (L"���泯��" == str)
		g.ini.m_direction = DOWN;
	else if (L"���泯��" == str)
		g.ini.m_direction = LEFT;
	else if (L"���泯��" == str)
		g.ini.m_direction = RIGHT;
	else
	{
		g.ini.m_direction = UP;
		CComboBox *pbox = (CComboBox*)GetDlgItem(IDC_COMBO_SELDIR);
		g.ini.m_direction = UP; pbox->SetCurSel(0);
	}
	g.ini.SaveParaFile(PARA_PRJ);
}


BOOL CChopStickDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (VK_RETURN == pMsg->wParam)
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}
