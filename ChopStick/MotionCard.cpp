// MotionCard.cpp: implementation of the CMotionCard class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MotionCard.h"

//#include "IniFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//static PT_DioWriteBit			ptDioWriteBit;				// DioWriteBit table
//static PT_DioGetCurrentDOByte	ptDioGetCurrentDOByte;
CMotionCard *mc = NULL;
CMotionCard::CMotionCard()
{
	m_EventSkipScan.ResetEvent();
	m_EventSkipScan1.ResetEvent();
	isFstThread = false;
	isSndThread = false;


	actioninfo = NULL;
	resetData();
}

CMotionCard::~CMotionCard()
{

}


bool CMotionCard::Init()
{
	
	if (!d1000_board_init())
	{
		AfxMessageBox(L"IO板卡初始化失败");
		d1000_set_pls_outmode(0, 0);
		return false;
	};
	d1000_out_bit(OUT_SERVO_MOTOR, ON);
	d1000_in_enable(g.ini.deviceNumber, 3);	//其他口作为通用输入口
	d1000_set_pls_outmode(0, 0);
	d1000_set_pls_outmode(1, 0);
	return true;
}



int CMotionCard::caculate_position(int counter, int position)
{
	if (counter%MAX_KNODE < position)
		return MAX_KNODE + counter%MAX_KNODE - position;
	else
		return (counter - position) % MAX_KNODE;// - position;
}

void CMotionCard::resetData()
{
	ActionInfo actioninfo_clear;
	actioninfo_clear.RoationInfo = 0;
	if (actioninfo)
		free(actioninfo);
	actioninfo = (ActionInfo*)malloc(MAX_KNODE * sizeof(ActionInfo));

	for (int i = 0; i < MAX_KNODE; i++)
	{
		actioninfo[i] = actioninfo_clear;
	}

}



BOOL CMotionCard::InitMotionCard(short iCardNum)
{
   
    d1000_board_init();
	d1000_set_pls_outmode(0,0);
//	mc->WriteOutPutBit(OUT_SV_ON,0);
	
	return TRUE;  
	
}



inline BOOL CMotionCard::WaitMoveDone(short iAxis)
{
	d1000_check_done(iAxis);
	return TRUE;
}
inline BOOL CMotionCard::ReadInputBit(short iBit)
{
	return (!d1000_in_bit(iBit));
}

inline BOOL CMotionCard::Stop(short Bit)
{
	d1000_decel_stop(Bit);
	return TRUE;
} 


BOOL CMotionCard::ScanMotionCard(HWND hWnd, UINT iMsg)
{
	::PostMessage(hWnd,MSG_MOTION_CARD,0,0);
    return true;
}

BOOL CMotionCard::ResumeScanThread()
{
	m_EventSkipScan.ResetEvent();
	m_CriticalCard.Unlock();
	return TRUE;
}

BOOL CMotionCard::SuspendScanThread()
{
	m_EventSkipScan.SetEvent();
	m_CriticalCard.Lock();
	return TRUE;
}



BOOL CMotionCard::CheckEStop(DWORD dwOverTime,short iCardID,short iAxis)
{
	if(::WaitForSingleObject(m_EventEstop[iCardID][iAxis],dwOverTime)==WAIT_OBJECT_0)
		return RETURN_ESTOP;
	return RETURN_OK;
}

double CMotionCard::PulseToUnit(short iAxis,const long dwValue)
{
	double dReturn;
	dReturn = dwValue*0.001/*m_dScaleDenominator[iCardID][iAxis]/m_dScaleNumerator[iCardID][iAxis]*/;
	return dReturn;
}

long CMotionCard::UnitToPulse(short iAxis,const double dValue)
{
	double dReturn,dCeil,dFloor;
	dReturn = dValue*m_dScaleNumerator[iAxis]/m_dScaleDenominator[iAxis];
	dCeil = ceil(dReturn);
	dFloor = floor(dReturn);
	if((dCeil- dReturn) < (dReturn - dFloor))
		dReturn = dCeil;
	else
		dReturn = dFloor;
	return long(dReturn);
}

CString CMotionCard::GetErrorString()
{
	return L"OK"; 
}	

UINT CMotionCard::WaitSensorTimeOutPos(short ibit,DWORD dwSensorTime,DWORD dwOverTime)
{
	DWORD dwTimeBegin,dwTimeEnd;
	dwTimeBegin = GetTickCount();
	do
	{
		Sleep(20);
		if(CheckEStop(dwSensorTime))
			return RETURN_ESTOP;
	   if (d1000_in_bit(ibit)==0)	
	   {
		   break;
	   }
		dwTimeEnd=GetTickCount();
		
	}while (dwTimeEnd-dwTimeBegin<=dwOverTime);
	return RETURN_CYL_ERROR;
}

UINT CMotionCard::WaitSensorTimeOutNeg(short iPort,UINT iChan,DWORD dwSensorTime,DWORD dwOverTime)
{
	DWORD dwTimeBegin, dwTimeEnd = dwOverTime;
	dwTimeBegin = GetTickCount();
	do
	{
		
	}while (dwTimeEnd-dwTimeBegin<=dwOverTime);
	return RETURN_CYL_ERROR;
}


BOOL CMotionCard::ReadOutputBit(short iBit)
{
	return !d1000_get_outbit(iBit);
}

//定长运动
void CMotionCard::StartTMove(short axis, long Dist, long Vel, long MaxVel, double Tacc)  
{

//	lTargetPos=UnitToPulse(axis,Dist);
	d1000_start_t_move(axis,Dist,Vel,MaxVel,Tacc);
}

//连续运动
void CMotionCard::StartTVMove(short axis,long strVel,long MaxVel,double Tacc)
{
	d1000_start_tv_move(axis,strVel,MaxVel,Tacc);
}

//读取位置
long CMotionCard::ReadPosition(short axis)
{
	long position=d1000_get_command_pos(axis);
	return position;
}

void CMotionCard::EStopMove(short axis)
{
  d1000_immediate_stop(axis);
}

BOOL CMotionCard::WriteOutPutBit(short BitNo,short BitData)
{
  d1000_out_bit(BitNo, BitData);
  return TRUE;
}



bool CMotionCard::WaitMotorTimeout(short No, DWORD time)
{
	DWORD startTick = GetTickCount();
	while (!d1000_check_done(No))
	{
		if ((GetTickCount() - startTick) > time)
		{
			return false;
		}
		Sleep(1);
	}
	return true;
}


bool CMotionCard::wait_input_norm(short Bits, DWORD Time)
{
	DWORD startTick = GetTickCount();

	while (d1000_in_bit(Bits))
	{
		int endTick = GetTickCount();
		if ((endTick - startTick) > Time)
		{
			return false;
		}
		Sleep(1);
	}
	return true;
}

bool CMotionCard::wait_input_inverse(short Bits, DWORD Time)
{
	DWORD startTick = GetTickCount();
	while (!d1000_in_bit(Bits))
	{
		int endTick = GetTickCount();
		if ((endTick - startTick) > Time)
		{
			return false;
		}
		Sleep(1);
	}
	return true;
}

int CMotionCard::PrintStepRun()
{
	DWORD StartTick = GetTickCount();
	//DWORD NewOldPrintSensorState = !d1000_in_bit(IN_PRINTED_SENSOR);
	//static DWORD OldPrintSensorState = NewOldPrintSensorState;
	for (int i = 0; i < MAX_COUNTER; i++) WriteOutPutBit(OUT_印花机电机, ON);


	for (; !d1000_in_bit(IN_PRINTED_SENSOR);) //第二个感应器
	{
		//Sleep(1);
		if (::WaitForSingleObject(g.g_evtEStop.evt, 0) == WAIT_OBJECT_0)
		{
			for (int i = 0; i < MAX_COUNTER; i++) WriteOutPutBit(OUT_印花机电机, OFF);
			return EMERGNCY;
		}
		if ((GetTickCount() - StartTick) > 1000)
		{
			for (int i = 0; i < MAX_COUNTER; i++) WriteOutPutBit(OUT_印花机电机, OFF);
			return TIMEOUT;
		}
	}
	for (; d1000_in_bit(IN_PRINTED_SENSOR);) //第二个感应器
	{
		//Sleep(1);
		if (::WaitForSingleObject(g.g_evtEStop.evt, 0) == WAIT_OBJECT_0)
		{
			for (int i = 0; i < MAX_COUNTER; i++) WriteOutPutBit(OUT_印花机电机, OFF);
			return EMERGNCY;
		}
		if ((GetTickCount() - StartTick) > 1500)
		{
			for (int i = 0; i < MAX_COUNTER; i++) WriteOutPutBit(OUT_印花机电机, OFF);
			return TIMEOUT;
		}
	}
	for (int i = 0; i < MAX_COUNTER; i++) WriteOutPutBit(OUT_印花机电机, OFF);
	return 0;
}


int CMotionCard::Clock180()
{
	int tmp = 2 * g.ini.m_MotorSpinWavesPerTime;
	start_t_move(FIRST_MOTOR, tmp, g.ini.m_MotorSpinSpeed / 10, g.ini.m_MotorSpinSpeed, 0.05);
	return 1;
}


int CMotionCard::CounterClock180()
{
	int tmp = -2 * g.ini.m_MotorSpinWavesPerTime;
	start_t_move(FIRST_MOTOR, tmp, g.ini.m_MotorSpinSpeed / 10, g.ini.m_MotorSpinSpeed, 0.05);
	return 1;
}


int CMotionCard::CounterClock90()
{
	int tmp = -1 * g.ini.m_MotorSpinWavesPerTime;
	start_t_move(FIRST_MOTOR, tmp, g.ini.m_MotorSpinSpeed / 10, g.ini.m_MotorSpinSpeed, 0.05);
	return 1;
}


int CMotionCard::Clock90()
{
	int tmp = g.ini.m_MotorSpinWavesPerTime;
	start_t_move(FIRST_MOTOR, tmp, g.ini.m_MotorSpinSpeed / 10, g.ini.m_MotorSpinSpeed, 0.05);
	return 1;
}


bool CMotionCard::StartSecondThread()
{
	if (isSndThread) return false;
	CWinThread * RunSecondBackThread = AfxBeginThread(SecondBackThread, this);
	if (!RunSecondBackThread) return false;
	return true;
}

UINT CMotionCard::SecondBackThread(LPVOID lParam)
{
	CMotionCard* pAction = (CMotionCard*)lParam;
	pAction->isSndThread = true;
	UINT res = pAction->SeconMotorBackToOrigin();
	pAction->isSndThread = false;
	return res;
}

bool CMotionCard::StartFirstBackThread()
{
	if (isFstThread) return false;
	CWinThread * RunFirstBackThread = AfxBeginThread(FirstBackThread, this);
	if (!RunFirstBackThread) return false;
	return true;
}

UINT CMotionCard::FirstBackThread(LPVOID lParam)
{
	CMotionCard* pAction = (CMotionCard*)lParam;
	pAction->isFstThread = true;
	UINT res = pAction->FisrtMotorBackToOrigin();
	pAction->isFstThread = false;
	return res;
}

//步进电机回原点
int CMotionCard::FisrtMotorBackToOrigin()
{
	for (int i = 0; i < MAX_COUNTER; i++) { WriteOutPutBit(OUT_TRAP_CYL, OFF); }

	int initspeed = g.ini.m_MotorSpinSpeed / 2000;
	int speed = g.ini.m_MotorSpinSpeed / 4;
	double Tacc = 0.05;
	d1000_home_move(0, initspeed, speed, Tacc);
	DWORD StartTick = GetTickCount();
	if (d1000_check_done(FIRST_MOTOR))
	{
		d1000_start_t_move(FIRST_MOTOR, -g.ini.m_MotorSpinWaves, initspeed, speed, Tacc);
		while (!d1000_check_done(FIRST_MOTOR))
		{
			if (::WaitForSingleObject(g.g_evtEStop.evt, 0) == WAIT_OBJECT_0)
			{
				d1000_immediate_stop(FIRST_MOTOR);
				Sleep(200);
				return EMERGNCY;
			}
			if ((GetTickCount() - StartTick) > 500)
			{
				d1000_immediate_stop(SECOND_MOTOR);
				return TIMEOUT;
			}
		}
		d1000_home_move(0, initspeed, speed, Tacc);
		while (!d1000_check_done(FIRST_MOTOR))
		{
			if (::WaitForSingleObject(g.g_evtEStop.evt, 0) == WAIT_OBJECT_0)
			{
				d1000_immediate_stop(FIRST_MOTOR);
				Sleep(200);
				return EMERGNCY;
			}
			if ((GetTickCount() - StartTick) > 500)
			{
				d1000_immediate_stop(SECOND_MOTOR);
				return TIMEOUT;
			}
		}
	}
	else
	{
		while (!d1000_check_done(FIRST_MOTOR))
		{
			if (::WaitForSingleObject(g.g_evtEStop.evt, 0) == WAIT_OBJECT_0)
			{
				d1000_immediate_stop(FIRST_MOTOR);
				Sleep(200);
				return EMERGNCY;;
			}
			if ((GetTickCount() - StartTick) > 500)
			{
				d1000_immediate_stop(SECOND_MOTOR);
				return TIMEOUT;
			}
		}
	}

	Sleep(100);
	d1000_start_t_move(FIRST_MOTOR, -g.ini.m_MotorCompenstate, initspeed, speed, Tacc);

	while (!d1000_check_done(FIRST_MOTOR))
	{
		if (::WaitForSingleObject(g.g_evtEStop.evt, 0) == WAIT_OBJECT_0)
		{
			d1000_immediate_stop(FIRST_MOTOR);
			Sleep(200);
			return EMERGNCY;
		}
		if ((GetTickCount() - StartTick) > 500)
		{
			d1000_immediate_stop(SECOND_MOTOR);
			return TIMEOUT;
		}
	}
	d1000_set_command_pos(0, 0);
	return 0;
}







int CMotionCard::SeconMotorBackToOrigin()
{
	d1000_start_tv_move(SECOND_MOTOR, g.ini.m_MotorSpinSpeed1 / 10, g.ini.m_MotorSpinSpeed1, 0.05);
	DWORD StartTick = GetTickCount();
	for (; !d1000_in_bit(SECOND_ORIGIN_SENSOR);) //第二个感应器
	{
		if (::WaitForSingleObject(g.g_evtEStop.evt, 0) == WAIT_OBJECT_0)
		{
			d1000_immediate_stop(SECOND_MOTOR);
			return EMERGNCY;
		}
		if ((GetTickCount() - StartTick) > 500)
		{
			d1000_immediate_stop(SECOND_MOTOR);
			return TIMEOUT;
		}
	}
	for (; d1000_in_bit(SECOND_ORIGIN_SENSOR);) //第二个感应器
	{
		if (::WaitForSingleObject(g.g_evtEStop.evt, 0) == WAIT_OBJECT_0)
		{
			d1000_immediate_stop(SECOND_MOTOR);
			return EMERGNCY;
		}
		if ((GetTickCount() - StartTick) > 1000)
		{
			d1000_immediate_stop(SECOND_MOTOR);
			return TIMEOUT;
		}
	}
	d1000_decel_stop(SECOND_MOTOR);

	d1000_set_command_pos(SECOND_MOTOR, 0);
	return 0;

}






int CMotionCard::ConveyorStepRun()
{
	for (int i = 0; i < MAX_COUNTER; i++) { g.mc.WriteOutPutBit(OUT_TRAP_CYL, OFF); }
#ifdef PRINTED_VERSION
	int tmp = g.ini.m_MotorSpinWaves1;//g.ini.m_MotorSpinWaves1
	start_t_move(SECOND_MOTOR, tmp, g.ini.m_MotorSpinSpeed1 / 10, g.ini.m_MotorSpinSpeed1, 0.05);
	return 0;
#endif
#ifdef LASER_VERSION

	if(!WaitMotorTimeout(SECOND_MOTOR, 10000)) return TIMEOUT;
	d1000_start_tv_move(SECOND_MOTOR, g.ini.m_MotorSpinSpeed1 / 10, g.ini.m_MotorSpinSpeed1, 0.05);
	DWORD StartTick = GetTickCount();
	for (; !d1000_in_bit(SECOND_ORIGIN_SENSOR);) //第二个感应器
	{
		if (::WaitForSingleObject(g.g_evtEStop.evt, 0) == WAIT_OBJECT_0)
		{
			d1000_immediate_stop(SECOND_MOTOR);
			return EMERGNCY;
		}
		if ((GetTickCount() - StartTick) > 1000)
		{
			d1000_immediate_stop(SECOND_MOTOR);
			return TIMEOUT;
		}
	}
	for (; d1000_in_bit(SECOND_ORIGIN_SENSOR);) //第二个感应器
	{
		if (::WaitForSingleObject(g.g_evtEStop.evt, 0) == WAIT_OBJECT_0)
		{
			d1000_immediate_stop(SECOND_MOTOR);
			return EMERGNCY;
		}
		if ((GetTickCount() - StartTick) > 3000)
		{
			d1000_immediate_stop(SECOND_MOTOR);
			return TIMEOUT;
		}
	}
	if (g.ini.m_stoptime < 10) {
		d1000_decel_stop(SECOND_MOTOR);
		return 0;
	}
	int delay = g.ini.m_stoptime / 10;
	for (int i = 0;i<10;i++)
	{
		long long tmpspeed = 9*d1000_get_speed(SECOND_MOTOR);
		d1000_change_speed(SECOND_MOTOR,tmpspeed/10);
		Sleep(delay);
	}
	d1000_decel_stop(SECOND_MOTOR);
	return 0;
#endif

#ifdef PRINTED_VERSION2

	int tmp = g.ini.m_MotorSpinWavesPerTime1;//g.ini.m_MotorSpinWaves1
	start_t_move(SECOND_MOTOR, tmp, g.ini.m_MotorSpinSpeed1 / 10, g.ini.m_MotorSpinSpeed1, 0.05);
	DWORD TickCount = GetTickCount();
	for (; !d1000_check_done(SECOND_MOTOR);)
	{
		Sleep(1);
		if ((GetTickCount() - TickCount)>3000)
		return TIMEOUT;
	}
	return 0;

	if (!WaitMotorTimeout(FIRST_MOTOR, 10000)) return TIMEOUT;
	d1000_start_tv_move(SECOND_MOTOR, g.ini.m_MotorSpinSpeed1 / 10, g.ini.m_MotorSpinSpeed1, 0.05);
	DWORD StartTick = GetTickCount();
	for (; !d1000_in_bit(SECOND_ORIGIN_SENSOR);) //第二个感应器
	{
		if (::WaitForSingleObject(g.g_evtEStop.evt, 0) == WAIT_OBJECT_0)
		{
			d1000_immediate_stop(SECOND_MOTOR);
			return EMERGNCY;
		}
		if ((GetTickCount() - StartTick) > 500)
		{
			d1000_immediate_stop(SECOND_MOTOR);
			return TIMEOUT;
		}
	}
	for (; d1000_in_bit(SECOND_ORIGIN_SENSOR);) //第二个感应器
	{
		if (::WaitForSingleObject(g.g_evtEStop.evt, 0) == WAIT_OBJECT_0)
		{
			d1000_immediate_stop(SECOND_MOTOR);
			return EMERGNCY;
		}
		if ((GetTickCount() - StartTick) > 1000)
		{
			d1000_immediate_stop(SECOND_MOTOR);
			return TIMEOUT;
		}
	}
	if (g.ini.m_stoptime < 10) {
		d1000_decel_stop(SECOND_MOTOR);
		return 0;
	}
	int delay = g.ini.m_stoptime / 10;
	for (int i = 0; i<10; i++)
	{
		long long tmpspeed = 9 * d1000_get_speed(SECOND_MOTOR);
		d1000_change_speed(SECOND_MOTOR, tmpspeed / 10);
		Sleep(delay);
	}
	d1000_decel_stop(SECOND_MOTOR);
	return 0;
#endif

}

int CMotionCard::FirstMotorStepRunCW()
{
	int tmp = g.ini.m_MotorSpinWavesPerTime;
	start_t_move(FIRST_MOTOR, tmp, 100, g.ini.m_MotorSpinSpeed, 0.1);
	return 1;
}

int CMotionCard::FirstMotorStepRunCCW()
{
	int tmp = -g.ini.m_MotorSpinWavesPerTime;
	start_t_move(FIRST_MOTOR, tmp, 100, g.ini.m_MotorSpinSpeed, 0.1);
	return 1;
}


int CMotionCard::SecondMotorStepRunCW()
{
	int tmp = g.ini.m_MotorSpinWaves1;
	start_t_move(SECOND_MOTOR, tmp, 5000, g.ini.m_MotorSpinSpeed1, 0.05);
	return 1;
}

int CMotionCard::SecondMotorStepRunCCW()
{
	int tmp = -g.ini.m_MotorSpinWaves1;
	tmp = 0 - tmp;
	start_t_move(SECOND_MOTOR, tmp, 5000, g.ini.m_MotorSpinSpeed1, 0.05);
	return 1;
}

