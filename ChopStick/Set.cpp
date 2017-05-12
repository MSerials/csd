// Set.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ChopStick.h"
#include "Set.h"
#include "afxdialogex.h"
#include "DahengCam.h"
#include "ChopStickDlg.h"

// Set �Ի���
Set *SetDlg = NULL;
IMPLEMENT_DYNAMIC(Set, CDialogEx)

Set::Set(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CHOPSTICK_DIALOG_SET, pParent)
{

}

Set::~Set()
{
}

void Set::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Set, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_MTR1_BACK, &Set::OnBnClickedButtonMtr1Back)
	ON_BN_CLICKED(IDC_BUTTON_CCW1, &Set::OnBnClickedButtonCcw1)
	ON_BN_CLICKED(IDC_BUTTON_CW1, &Set::OnBnClickedButtonCw1)
	ON_BN_CLICKED(IDC_BUTTON_MTR1_BACK2, &Set::OnBnClickedButtonMtr1Back2)
	ON_BN_CLICKED(IDC_BUTTON_CCW2, &Set::OnBnClickedButtonCcw2)
	ON_BN_CLICKED(IDC_BUTTON_CW2, &Set::OnBnClickedButtonCw2)
	ON_BN_CLICKED(IDC_BUTTON_CYL, &Set::OnBnClickedButtonCyl)
	ON_BN_CLICKED(IDC_BUTTON_SAVEPARA, &Set::OnBnClickedButtonSavepara)
	ON_BN_CLICKED(IDC_BUTTON_SYSRESET, &Set::OnBnClickedButtonSysreset)
	ON_BN_CLICKED(IDC_BUTTON_COMF_THRESHOLD, &Set::OnBnClickedButtonComfThreshold)
	ON_BN_CLICKED(IDC_BUTTON_COMF_THRESHOLD2, &Set::OnBnClickedButtonComfThreshold2)
	ON_BN_CLICKED(IDC_BUTTON_READ, &Set::OnBnClickedButtonRead)
	ON_BN_CLICKED(IDC_BUTTON_SHOW, &Set::OnBnClickedButtonShow)
	ON_CBN_SELCHANGE(IDC_COMBO_SELTHEMETHOD, &Set::OnSelchangeComboSelthemethod)
	ON_BN_CLICKED(IDC_BUTTON_ACTIVATE, &Set::OnBnClickedButtonActivate)
	ON_BN_CLICKED(IDC_BUTTON_MOTOR3, &Set::OnBnClickedButtonMotor3)
	ON_BN_CLICKED(IDC_BUTTON_ROTATION_CYL, &Set::OnBnClickedButtonRotationCyl)
	ON_BN_CLICKED(IDC_BUTTON_HOLD_CYL, &Set::OnBnClickedButtonHoldCyl)
END_MESSAGE_MAP()







BOOL Set::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	UpdateUI();

	CComboBox* pbox = (CComboBox*)GetDlgItem(IDC_COMBO_SELTHEMETHOD);
	pbox->AddString(L"OTSU");
	pbox->AddString(L"�̶�ֵ");

	if (g.ini.m_imageDealMode == 0)
		pbox->SetCurSel(0);
	else if (1 == g.ini.m_imageDealMode)
		pbox->SetCurSel(1);


	if (1 == g.ini.m_imageDealMode)
	{
		GetDlgItem(IDC_STATIC_BLOCKSIZE)->ShowWindow(FALSE);
		GetDlgItem(IDC_EDIT_BLOCK)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_THRESCOMP)->SetWindowText(L"��ֵ����ֵ");
	}
	else if (0 == g.ini.m_imageDealMode)
	{
		GetDlgItem(IDC_STATIC_BLOCKSIZE)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT_BLOCK)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_THRESCOMP)->SetWindowText(L"��ֵ������");
	}

#ifdef PRINTED_VERSION2
	GetDlgItem(IDC_BUTTON_ROTATION_CYL)->SetWindowText(L"���������Ƴ�");
	GetDlgItem(IDC_BUTTON_HOLD_CYL)->SetWindowText(L"�п�����������");
	GetDlgItem(IDC_BUTTON_MOTOR3)->SetWindowText(L"���������ת");
#else
	GetDlgItem(IDC_BUTTON_ROTATION_CYL)->ShowWindow(FALSE);
	GetDlgItem(IDC_BUTTON_HOLD_CYL)->ShowWindow(FALSE);
	GetDlgItem(IDC_BUTTON_MOTOR3)->ShowWindow(FALSE);
#endif
	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


// Set ��Ϣ�������
void Set::UpdateUI()
{
	CString str;
	str.Format(_T("%d"), g.ini.m_MotorSpinSpeed);
	GetDlgItem(IDC_EDIT_SPD)->SetWindowText(str);
	str.Format(_T("%d"), g.ini.m_MotorSpinSpeed1);
	GetDlgItem(IDC_EDIT_SPD2)->SetWindowText(str);


	str.Format(_T("%d"), g.ini.m_MotorCompenstate);
	GetDlgItem(IDC_EDIT_COMP)->SetWindowText(str);
	str.Format(_T("%d"), g.ini.m_MotorCompenstate1);
	GetDlgItem(IDC_EDIT_COMP2)->SetWindowText(str);

	str.Format(_T("%d"), g.ini.m_MotorSpinWavesPerTime);
	GetDlgItem(IDC_EDIT_STEP)->SetWindowText(str);
	//str.Format(_T("%d"), g.ini.m_MotorSpinSpeed);


	str.Format(_T("%d"), g.ini.m_MotorSpinWaves);
	GetDlgItem(IDC_EDIT_BYHAND)->SetWindowText(str);
	str.Format(_T("%d"), g.ini.m_MotorSpinWaves1);
	GetDlgItem(IDC_EDIT_BYHAND2)->SetWindowText(str);

	str.Format(_T("%f"), g.ini.m_shutter);
	GetDlgItem(IDC_EDIT_EXPOSURE)->SetWindowText(str);
//
	str.Format(_T("%d"), g.ini.m_idFixThreshold);
	GetDlgItem(IDC_EDIT_THRESHOLDCOPS)->SetWindowText(str);

	str.Format(_T("%d"), g.ini.m_blocksize);
	GetDlgItem(IDC_EDIT_BLOCK)->SetWindowText(str);

	str.Format(_T("%d"), g.ini.m_threshold2);
	GetDlgItem(IDC_EDIT_THRESHOLD)->SetWindowText(str);


	if (g.mc.ReadOutputBit(OUT_TRAP_CYL))
	{
		GetDlgItem(IDC_BUTTON_CYL)->SetWindowText(L"�ſ���ָ����");
	}
	else
	{ 
		GetDlgItem(IDC_BUTTON_CYL)->SetWindowText(L"�ر���ָ����");
	}

	str.Format(_T("%d"), g.ini.d);
	GetDlgItem(IDC_EDIT_d)->SetWindowText(str);

	str.Format(_T("%f"), g.ini.sigmaColor);
	GetDlgItem(IDC_EDIT_sigmaColor)->SetWindowText(str);

	str.Format(_T("%f"), g.ini.sigmaSpace);
	GetDlgItem(IDC_EDIT_sigmaSpace)->SetWindowText(str);

	str.Format(_T("%d"), g.ini.m_stoptime);
	GetDlgItem(IDC_EDIT_STOPTIME)->SetWindowText(str);

	str.Format(_T("%d"), g.ini.m_DelayLaserTrigger);
	GetDlgItem(IDC_EDIT_DELAYLASER)->SetWindowText(str);

#ifdef PRINTED_VERSION2
	if(!g.mc.ReadOutputBit(OUT_��������))
		GetDlgItem(IDC_BUTTON_ROTATION_CYL)->SetWindowText(L"���������Ƴ�");
	else
		GetDlgItem(IDC_BUTTON_ROTATION_CYL)->SetWindowText(L"����������");


	if(!g.mc.ReadOutputBit(OUT_��ס����))
		GetDlgItem(IDC_BUTTON_HOLD_CYL)->SetWindowText(L"�п�����������");
	else
		GetDlgItem(IDC_BUTTON_HOLD_CYL)->SetWindowText(L"�п��������½�");

	if(!g.mc.ReadOutputBit(OUT_ӡ�������))
		GetDlgItem(IDC_BUTTON_MOTOR3)->SetWindowText(L"���������ת");
	else
		GetDlgItem(IDC_BUTTON_MOTOR3)->SetWindowText(L"�������ֹͣ");

	str.Format(_T("%d"), g.ini.m_MotorSpinWavesPerTime1);
	GetDlgItem(IDC_EDIT_STEP2)->SetWindowText(str);

	str.Format(_T("%d"), g.ini.m_Hold_Cyl_delay);
	GetDlgItem(IDC_EDIT_HODL_DELAY)->SetWindowText(str);

	str.Format(_T("%d"), g.ini.m_Hold_Cyl_delay1);
	GetDlgItem(IDC_EDIT_HODL_DELAY1)->SetWindowText(str);


	str.Format(_T("%d"), g.ini.m_Push_Cyl_delay);
	GetDlgItem(IDC_EDIT_CYL_PUSH_DELAY)->SetWindowText(str);


	str.Format(_T("%d"), g.ini.m_Push_Cyl_delay1);
	GetDlgItem(IDC_EDIT_CYL_PUSH_DELAY1)->SetWindowText(str);

	
#else
	GetDlgItem(IDC_EDIT_HODL_DELAY)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_HODL_DELAY1)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_CYL_PUSH_DELAY)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_CYL_PUSH_DELAY1)->ShowWindow(FALSE);
#endif
}

void Set::OnBnClickedButtonMtr1Back()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BOOL emState = !g.mc.ReadInputBit(IN_EMERGENCY);  //�߼���
	//BOOL alamState = g.mc.ReadInputBit(IN_SEVRO_ALM);
	if (emState) { AfxMessageBox(L"��ͣ��ť���£�"); return; }
	g.mc.StartFirstBackThread();
}


void Set::OnBnClickedButtonCcw1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	g.mc.FirstMotorStepRunCCW();
}


void Set::OnBnClickedButtonCw1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	g.mc.FirstMotorStepRunCW();

}


void Set::OnBnClickedButtonMtr1Back2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BOOL emState = !g.mc.ReadInputBit(IN_EMERGENCY);  //�߼���
	BOOL alamState = !g.mc.ReadInputBit(IN_SEVRO_ALM);
	if (emState || alamState) { AfxMessageBox(L"��ͣ��ť���»����ŷ����������"); return; }
	g.mc.StartSecondThread();
}


void Set::OnBnClickedButtonCcw2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	g.mc.SecondMotorStepRunCCW();
}


void Set::OnBnClickedButtonCw2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	g.mc.SecondMotorStepRunCW();
}



void Set::OnBnClickedButtonCyl()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (g.mc.ReadOutputBit(OUT_TRAP_CYL))
	{
		GetDlgItem(IDC_BUTTON_CYL)->SetWindowText(L"��ס����");
		g.mc.WriteOutPutBit(OUT_TRAP_CYL, OFF);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_CYL)->SetWindowText(L"�ſ�����");
		g.mc.WriteOutPutBit(OUT_TRAP_CYL, ON);
	}
}


void Set::OnBnClickedButtonSavepara()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	GetDlgItem(IDC_EDIT_SPD)->GetWindowText(str);
	g.ini.m_MotorSpinSpeed = _ttoi(str);
	GetDlgItem(IDC_EDIT_SPD2)->GetWindowText(str);
	g.ini.m_MotorSpinSpeed1 = _ttoi(str);

	
	GetDlgItem(IDC_EDIT_COMP)->GetWindowText(str);
	g.ini.m_MotorCompenstate = _ttoi(str);
	GetDlgItem(IDC_EDIT_COMP2)->GetWindowText(str);
	g.ini.m_MotorCompenstate1 = _ttoi(str);

	GetDlgItem(IDC_EDIT_STEP)->GetWindowText(str);
	g.ini.m_MotorSpinWavesPerTime = _ttoi(str);
#ifdef PRINTED_VERSION2
	GetDlgItem(IDC_EDIT_STEP2)->GetWindowText(str);
	g.ini.m_MotorSpinWavesPerTime1 = _ttoi(str);

	GetDlgItem(IDC_EDIT_HODL_DELAY)->GetWindowText(str);
	g.ini.m_Hold_Cyl_delay = _ttoi(str);

	GetDlgItem(IDC_EDIT_HODL_DELAY1)->GetWindowText(str);
	g.ini.m_Hold_Cyl_delay1 = _ttoi(str);

	GetDlgItem(IDC_EDIT_CYL_PUSH_DELAY)->GetWindowText(str);
	g.ini.m_Push_Cyl_delay = _ttoi(str);

	GetDlgItem(IDC_EDIT_CYL_PUSH_DELAY1)->GetWindowText(str);
	g.ini.m_Push_Cyl_delay1 = _ttoi(str);
#endif
	//str.Format(_T("%d"), g.ini.m_MotorSpinSpeed);

	GetDlgItem(IDC_EDIT_BYHAND)->GetWindowText(str);
	g.ini.m_MotorSpinWaves = _ttoi(str);
	GetDlgItem(IDC_EDIT_BYHAND2)->GetWindowText(str);
	g.ini.m_MotorSpinWaves1 = _ttoi(str);

	GetDlgItem(IDC_EDIT_STOPTIME)->GetWindowText(str);
	g.ini.m_stoptime = _ttoi(str);
	g.ini.SaveParaFile(PARA_ALL);
}


void Set::OnBnClickedButtonSysreset()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	GetDlgItem(IDC_EDIT_EXPOSURE)->GetWindowText(str);
	float exp = _ttof(str);
	g.ini.m_shutter = exp;
	GX_STATUS status = GX_STATUS_SUCCESS;
	status = GXSetFloat(dahengcam->m_hDevice, GX_FLOAT_EXPOSURE_TIME, exp);
	if (GX_STATUS_SUCCESS != status) AfxMessageBox(L"�ع�ʱ������ʧ��");

	GetDlgItem(IDC_EDIT_DELAYLASER)->GetWindowText(str);
	g.ini.m_DelayLaserTrigger = _ttoi(str);

	g.ini.SaveParaFile(PARA_PRJ);

}


void Set::OnBnClickedButtonComfThreshold()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	GetDlgItem(IDC_EDIT_THRESHOLDCOPS)->GetWindowText(str);
	g.ini.m_idFixThreshold = _ttoi(str);
	{
		if (1 == g.ini.m_imageDealMode)
		{
			if (1 < g.ini.m_idFixThreshold || g.ini.m_idFixThreshold > 250)
			g.ini.m_idFixThreshold = 128;
			str.Format(_T("%d"), g.ini.m_idFixThreshold); GetDlgItem(IDC_EDIT_THRESHOLDCOPS)->SetWindowText(str);
		}
	}

	GetDlgItem(IDC_EDIT_BLOCK)->GetWindowText(str);
	g.ini.m_blocksize = _ttoi(str);
	if (!(g.ini.m_blocksize & 0x1))
	{
		g.ini.m_blocksize++;
		str.Format(_T("%d"), g.ini.m_blocksize); GetDlgItem(IDC_EDIT_BLOCK)->SetWindowText(str);
	}

	GetDlgItem(IDC_EDIT_THRESHOLD)->GetWindowText(str);
	g.ini.m_threshold2 = _ttoi(str);
	if (!((1 < g.ini.m_threshold2) && (g.ini.m_threshold2 < 250)))	g.ini.m_threshold2 = 30;
	str.Format(_T("%d"), g.ini.m_threshold2); GetDlgItem(IDC_EDIT_THRESHOLD)->SetWindowText(str);

	GetDlgItem(IDC_EDIT_d)->GetWindowText(str);
	g.ini.d = _ttoi(str);

	GetDlgItem(IDC_EDIT_sigmaColor)->GetWindowText(str);
	g.ini.sigmaColor = _ttof(str);

	GetDlgItem(IDC_EDIT_sigmaSpace)->GetWindowText(str);
	g.ini.sigmaSpace = _ttof(str);
	
	g.ini.SaveParaFile(PARA_ALL);
}


void Set::OnBnClickedButtonComfThreshold2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (g.opencv.m_RawMatImg.empty()) return;
	cv::Mat res;
	cv::threshold(g.opencv.m_RawMatImg, res, g.ini.m_idFixThreshold, 255, THRESH_BINARY);	//��ȡ���ӵĶ�ֵ��
	pMainFrm->DisplayMatImg(res);
}


void Set::OnBnClickedButtonRead()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (WAIT_OBJECT_0 == ::WaitForSingleObject(g.g_evtActionProc.evt, 0)) return;

	try {
		CFileDialog fileDlg(TRUE, (LPCTSTR)"BMP", L"", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"All Files (*.*)|*.*|λͼ�ļ� (*.bmp)|*.bmp||");

		if (fileDlg.DoModal() != IDOK) return;

		//��ȡ�����ļ�
		g.opencv.LoadMatImg((char*)(LPCSTR)(CStringA)fileDlg.GetPathName());

		pMainFrm->SetCapState(STOP);// = STOP;

		g.opencv.ColorInvert(g.opencv.m_RawMatImg);


		pMainFrm->DisplayMatImg(g.opencv.m_RawMatImg);
	}
	catch (...)
	{
		
		return;
	}

}


void Set::OnBnClickedButtonShow()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	cv::Mat res;
	g.opencv.MatImgProcess(g.opencv.m_RawMatImg,res);
	pMainFrm->DisplayMatImg(res);
}


void Set::OnSelchangeComboSelthemethod()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	int nSel;
	// ��ȡ��Ͽ�ؼ����б����ѡ���������
	CComboBox* pbox = (CComboBox*)GetDlgItem(IDC_COMBO_SELTHEMETHOD);
	nSel = pbox->GetCurSel();
	// ����ѡ����������ȡ�����ַ���   
	pbox->GetLBText(nSel, str);

	if (L"OTSU" == str)
		g.ini.m_imageDealMode = 0;
	else if (L"�̶�ֵ" == str)
		g.ini.m_imageDealMode = 1;


	//IDC_STATIC_THRESCOMP
	//IDC_STATIC_BLOCKSIZE
	if(1 == g.ini.m_imageDealMode)
	{
		GetDlgItem(IDC_STATIC_BLOCKSIZE)->ShowWindow(FALSE);
		GetDlgItem(IDC_EDIT_BLOCK)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_THRESCOMP)->SetWindowText(L"��ֵ����ֵ");
	}
	else if (0 == g.ini.m_imageDealMode)
	{
		GetDlgItem(IDC_STATIC_BLOCKSIZE)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT_BLOCK)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_THRESCOMP)->SetWindowText(L"��ֵ������");
	}

	g.ini.SaveParaFile(PARA_ALL);
}


void Set::OnBnClickedButtonActivate()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void Set::OnBnClickedButtonMotor3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (g.mc.ReadOutputBit(OUT_ӡ�������))
	{ 
		for (int i = 0; i < MAX_COUNTER; i++) g.mc.WriteOutPutBit(OUT_ӡ�������, OFF);
		GetDlgItem(IDC_BUTTON_MOTOR3)->SetWindowText(L"���������ת");
	}
	else
	{
		for (int i = 0; i < MAX_COUNTER; i++) g.mc.WriteOutPutBit(OUT_ӡ�������, ON);
		GetDlgItem(IDC_BUTTON_MOTOR3)->SetWindowText(L"�������ֹͣ");
	}
}


void Set::OnBnClickedButtonRotationCyl()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (g.mc.ReadOutputBit(OUT_��������))
	{
		for (int i = 0; i < MAX_COUNTER; i++) g.mc.WriteOutPutBit(OUT_��������,OFF);
		GetDlgItem(IDC_BUTTON_ROTATION_CYL)->SetWindowText(L"���������Ƴ�");
	}
	else
	{ 
		for (int i = 0; i < MAX_COUNTER; i++) g.mc.WriteOutPutBit(OUT_��������, ON);
		GetDlgItem(IDC_BUTTON_ROTATION_CYL)->SetWindowText(L"����������");
	}
}


void Set::OnBnClickedButtonHoldCyl()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (g.mc.ReadOutputBit(OUT_��ס����))
	{
		for (int i = 0; i < MAX_COUNTER; i++) g.mc.WriteOutPutBit(OUT_��ס����, OFF);
		GetDlgItem(IDC_BUTTON_HOLD_CYL)->SetWindowText(L"�п�����������");
	}
	else
	{ 
		for (int i = 0; i < MAX_COUNTER; i++) g.mc.WriteOutPutBit(OUT_��ס����, ON);
		GetDlgItem(IDC_BUTTON_HOLD_CYL)->SetWindowText(L"�п��������½�");
	}
}
