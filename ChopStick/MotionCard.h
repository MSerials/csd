// MotionCard.h: interface for the CMotionCard class.
// for googol motion card GE-400-PG-PCI-G
//	1,Profile Pos:the pulse is launch over
//	2,Target pos:the pos that driver wants the motor to be
//	3,Current pos:motor's current pos wicth has lag with the driver's pulse
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOTIONCARD_H__FAF08485_1C98_4A40_AA13_7EAAA74DD9B5__INCLUDED_)
#define AFX_MOTIONCARD_H__FAF08485_1C98_4A40_AA13_7EAAA74DD9B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#include "GoogolMotion.h"
//#include "AdvanIO.h"
#include "..\include\Dmc1380.h"
#include <math.h>
#include <afxmt.h>

#define TOTAL_CARD_NUM           1
#define MAX_AXIS_LEN             5
#define MAX_IO_NUM               16
#define INPUT_NUM				 16
#define OUTPUT_NUM				 16
#define dwPortCount              2
#define dwPortBit                16
#define EXCUTE_OK				 0

#define STATUS_BIT_NUMS          32
#define RUN_STATUS_BIT           10
#define MIN_COMPAREA_VALUE       1E-6
#define MIN_REL_MOVE_MM          0.001

#define RETURN_OK                0
#define RETURN_CYL_ERROR         252
#define RETURN_ESTOP             254

#define POSDIR                   1
#define NEGDIR                   -1

#define MSG_MOTION_CARD          (WM_USER+100)
#define MSG_IO_CARD				 (WM_USER+101)

//#include "HomeDlg.h"

//��һ�����ԭ��
#define IN_ORIGIN_POSITION0										1
//�ڶ������ԭ��
#define IN_ORIGIN_POSITION1										2
//�Զ�����
#define IN_START												3
//��ͣ��ť
#define IN_PAUSE												4
//��ͣ��ť
#define IN_EMERGENCY											5
//�����������ɿ���
#define IN_TRAP_IS_OPEN											6
//�ŷ���������
#define IN_SEVRO_ALM											7

#define IN_TRAP_IS_CLOSE										8
//ƽ������
//#define IN_PUSH_CHOPSTICK_LINE_BACK								9
//��������

//��ӡ���������ס���ӵ�����
#define IN_HOLD_CHOPSTICK										9
//��Ӧ��ӡ��ֽ֮����ͣ����
#define  IN_PRINTED_SENSOR										10


#define IN_HOLD_CYL

#define IN_PRINTED_CYL				 


//����ָʾ��
#define OUT_START_INDICATOR										1
//��ָͣʾ��
#define OUT_PAUSE_INDICATOR										2
//�ŷ�����
#define OUT_ALM													3
//�ŷ���￪��
#define OUT_SERVO_MOTOR											4
//��������
#define OUT_TRAP_CYL											5

//ѹסӡ��ֽ������
#define OUT_��������											6											
 
//��ס���ӵ�����
#define OUT_��ס����											7
//ӡ�������
#define OUT_ӡ�������											8


#ifdef PRINTED_VERSION
//ӡ�����//��Ƶ�����
#define OUT_THIRD_MOTOR											6
	#endif
#ifdef LASER_VERSION
//������
	#define OUT_LASER_ACTION									6
#endif

#define OUT_HOLD_CHOPSTIC										7


#define PUSH_BACK												1
#define PUSH_OUT												0

#define OFF														1
#define ON														0

#define CLOSE													0
#define OPEN													1

#define FIRST_MOTOR												0
#define SECOND_MOTOR											1
#define THIRD_MOTOR												2

#define FIRST_ORIGIN_SENSOR										1
#define SECOND_ORIGIN_SENSOR									2
#define THIRD_ORIGIN_SENSOR										11

#define is_out_on(x)											!d1000_get_outbit(x)
#define is_out_close(x)											!d1000_get_outbit(x)
#define read_input_state(x)										!d1000_in_bit(x)
#define check_motor_origin(serial)								d1000_check_done(serial)
#define write_output(BitNo,BitData)								d1000_out_bit(BitNo,BitData)
#define estop_move(axis)										d1000_immediate_stop(axis)
#define get_position(axis)										d1000_get_command_pos(axis)		
#define start_TV_move(axis,strVel,MaxVel,Tacc)					d1000_start_tv_move(axis, strVel, MaxVel, Tacc)
#define start_t_move(axis, Dist, Vel, MaxVel, Tacc)				d1000_start_t_move(axis, Dist, Vel, MaxVel, Tacc)

#define MAX_KNODE 256


class CMotionCard : public CObject  
{
public:
	LRESULT		   m_ErrCode; 
	LONG		   m_DriverHandle;
	unsigned char  m_szErrorMsg[40];

	BOOL m_iInput[dwPortCount][dwPortBit];
	BOOL m_iOutput[dwPortCount][dwPortBit];

	USHORT m_mcInput[TOTAL_CARD_NUM];
	USHORT m_mcOutput[TOTAL_CARD_NUM];

public:
	virtual bool WaitMotorTimeout(short No, DWORD time);

	virtual bool wait_input_norm(short Bits, DWORD Time);

	virtual bool wait_input_inverse(short Bits, DWORD Time);

	int Clock180();

	int CounterClock180();

	int CounterClock90();

	int Clock90();

	bool StartSecondThread();
	bool isSndThread;
	static UINT SecondBackThread(LPVOID lParam);

	bool StartFirstBackThread();
	bool isFstThread;
	static UINT FirstBackThread(LPVOID lParam);

	virtual int FisrtMotorBackToOrigin();

	virtual int SeconMotorBackToOrigin();

	int ConveyorStepRun();

	int PrintStepRun();

	int FirstMotorStepRunCW();

	int FirstMotorStepRunCCW();

	int SecondMotorStepRunCW();

	int SecondMotorStepRunCCW();





	long lTargetPos;
	BOOL WriteOutPutBit(short BitNo,short BitData);
	BOOL WriteLOUJ;
	void EStopMove(short axis);
	void StartTMove(short axis,long Dist,long Vel,long MaxVel,double Tacc);
	void StartTVMove(short axis,long strVel,long MaxVel,double Tacc);
	long ReadPosition(short axis);
	BOOL m_bHomeStop;

	//IO card
	virtual BOOL ReadInputBit(short iBit);
	virtual BOOL ReadOutputBit(short iBit);
	virtual BOOL Stop(short Bit);

	//motion card
	UINT WaitSensorTimeOutPos(short ibit,DWORD dwSensorTime,DWORD dwOverTime=1000);
	UINT WaitSensorTimeOutNeg(short iPort,UINT iChanIN,DWORD dwSensorTime,DWORD dwOverTime=1000);

	CMotionCard();
	virtual ~CMotionCard();
	virtual bool Init();
	int caculate_position(int counter, int position);
	void resetData();

	BOOL InitMotionCard(short iCardNum);
	BOOL ScanMotionCard(HWND hWnd,UINT iMsg);

	BOOL ResumeScanThread();
	BOOL SuspendScanThread();

	BOOL WaitMoveDone(short iAxis);

	BOOL  CheckEStop(DWORD dwOverTime=0,short iCardID=0,short iAxis=0);

	long   UnitToPulse(short iAxis,const double dValue);
	double PulseToUnit(short iAxis,const long  dValue);




    


	CString GetErrorString();
	short   GetErrorID(){return m_nCardError;}
	struct ActionInfo
	{
		int RoationInfo;
	};
	ActionInfo *actioninfo;
private:	
	short m_nCardError;
	short m_nCardID;
	short m_nAxisID;
	long m_lTargetPos[TOTAL_CARD_NUM][MAX_AXIS_LEN];
	long m_lCurrentPos[TOTAL_CARD_NUM][MAX_AXIS_LEN];
	long m_lProfilePos[TOTAL_CARD_NUM][MAX_AXIS_LEN];
	double m_dDefaultVel[TOTAL_CARD_NUM][MAX_AXIS_LEN];
	double m_dDefaultAcc[TOTAL_CARD_NUM][MAX_AXIS_LEN];
	double m_dDefaultJOGVel[TOTAL_CARD_NUM][MAX_AXIS_LEN];
	double m_dDefaultJOGAcc[TOTAL_CARD_NUM][MAX_AXIS_LEN];
	double m_dDefaultHomeVel[TOTAL_CARD_NUM][MAX_AXIS_LEN];
	double m_dDefaultHomeAcc[TOTAL_CARD_NUM][MAX_AXIS_LEN];
	double m_dScaleNumerator[MAX_AXIS_LEN];
	double m_dScaleDenominator[MAX_AXIS_LEN];
	bool m_bPosLimitLevel[TOTAL_CARD_NUM][MAX_AXIS_LEN];
	bool m_bNegLimitLevel[TOTAL_CARD_NUM][MAX_AXIS_LEN];
	bool m_bDrvAlarmLevel[TOTAL_CARD_NUM][MAX_AXIS_LEN];
	bool m_bMotorHomeLevel[TOTAL_CARD_NUM][MAX_AXIS_LEN];
	bool m_bMotorIndexLevel[TOTAL_CARD_NUM][MAX_AXIS_LEN];
	bool m_bEStop[TOTAL_CARD_NUM][MAX_AXIS_LEN];
	bool m_bStop[TOTAL_CARD_NUM][MAX_AXIS_LEN];
	bool m_bMoveDone[TOTAL_CARD_NUM][MAX_AXIS_LEN];
	CString m_strError;
	CEvent m_EventSkipScan;
	CEvent m_EventSkipScan1;
	CEvent m_EventEstop[TOTAL_CARD_NUM][MAX_AXIS_LEN];;
	CEvent m_EventMoveDone[TOTAL_CARD_NUM][MAX_AXIS_LEN];
	CCriticalSection m_CriticalCard;
	CCriticalSection m_CriticalIOCard;






};

extern CMotionCard *mc;

#endif // !defined(AFX_MOTIONCARD_H__FAF08485_1C98_4A40_AA13_7EAAA74DD9B5__INCLUDED_)
