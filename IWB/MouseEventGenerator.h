#pragma once
//˵��:��Ƶ�м�⵽�Ĺ�����굽��Ļ�����ת����
//˫���¼������
class CDoubleClickChecker
{
public:
		CDoubleClickChecker()
		{
			m_ptLastLeftDownPos.x = 0;
			m_ptLastLeftDownPos.y = 0;
			m_pLastLeftDownTime  = 0;
		}

		void Check(const POINT& ptLeftDownPos)
		{

			//��������
			INPUT input;
			memset(&input, 0 ,sizeof(input));
			input.type = INPUT_MOUSE;

			DWORD dwNow = GetTickCount();

			if((dwNow - m_pLastLeftDownTime) < DOUBLE_CLICK_MAX_INTERVAL)
			{

				LONG dx = abs(ptLeftDownPos.x - m_ptLastLeftDownPos.x);
				LONG dy = abs(ptLeftDownPos.y - m_ptLastLeftDownPos.y);

				if(dx < MOUSE_LEFT_STATIC_PERMIT_MAX_SHIFT && dy < MOUSE_LEFT_STATIC_PERMIT_MAX_SHIFT  && !(dx ==0 && dy == 0)/*�Զ�����*/)
				{

					//���������ε�����ģ��˫���¼�		
					input.mi.dwFlags  =  MOUSEEVENTF_LEFTUP;
					SendInput(1, &input, sizeof(INPUT));

					input.mi.dwFlags  =  MOUSEEVENTF_LEFTDOWN;
					SendInput(1, &input, sizeof(INPUT));
				}
			}

			m_ptLastLeftDownPos  = ptLeftDownPos;
			m_pLastLeftDownTime = dwNow;
		};

protected:
	POINT m_ptLastLeftDownPos;
	DWORD m_pLastLeftDownTime;//��һ���������ʱ�̡�
    static const UINT DOUBLE_CLICK_MAX_INTERVAL = 1000u;
	static const DWORD MOUSE_LEFT_STATIC_PERMIT_MAX_SHIFT  = 20*65535/768;//�����������(��һ����0~65535)С�ڸ�ֵ,����Ϊ���λ��δ�����仯
																		  //��ͬһ�������������������˫���¼���
};



//�Ҽ��¼�������
class CRightClickTrigger
{
public:
	CRightClickTrigger()
	{
		m_ptRangeCenter.x   = (std::numeric_limits<long>::min)();
		m_ptRangeCenter.y   = (std::numeric_limits<long>::min)();
		m_dwRangeUpdateTime = GetTickCount();

	}

	void Reset()
	{
		m_ptRangeCenter.x   = (std::numeric_limits<long>::min)();
		m_ptRangeCenter.y   = (std::numeric_limits<long>::min)();
		m_dwRangeUpdateTime = GetTickCount();
	}

	//@��  ��:�������Ƿ���һ�����ڳ�ʱ��ͣ��
	//@����ֵ:TRUE, �������������Ҽ��¼�
	//        FALSE, δ��������Ҽ��¼�
	BOOL Process(const POINT& ptNow)
	{

		LONG dx = abs(ptNow.x - m_ptRangeCenter.x);
		LONG dy = abs(ptNow.y - m_ptRangeCenter.y);
		if(dx > MOUSE_RIGHT_CLICK_RANGE || dy > MOUSE_RIGHT_CLICK_RANGE)
		{
			m_ptRangeCenter = ptNow;//�����ƶ�
			m_dwRangeUpdateTime = GetTickCount();
		}
		else
		{
			DWORD dwNow = GetTickCount();
			int  x = MOUSE_RIGHTDOWN_EVENT_TRIGGER_TIMER;

			if( (dwNow - m_dwRangeUpdateTime) > MOUSE_RIGHTDOWN_EVENT_TRIGGER_TIMER)
			{
				INPUT input;
				memset(&input, 0 ,sizeof(input));
				input.type = INPUT_MOUSE;

				input.mi.dx       = 0;
				input.mi.dy       = 0;
				input.mi.dwFlags  = MOUSEEVENTF_LEFTUP;
				SendInput(1, &input, sizeof(INPUT));

                AtlTrace(_T("[Thread(0x%08x)]A.Left Up\n"), GetCurrentThreadId());
								

				input.mi.dwFlags  = MOUSEEVENTF_RIGHTDOWN;
				SendInput(1, &input, sizeof(INPUT));

                AtlTrace(_T("[Thread(0x%08x)]B.Right Down\n"), GetCurrentThreadId());
	

				input.mi.dwFlags  = MOUSEEVENTF_RIGHTUP;
				SendInput(1, &input, sizeof(INPUT));

                AtlTrace(_T("[Thread(0x%08x)]C.Right Up\n"), GetCurrentThreadId());

				return TRUE;
			}


		}

		return FALSE;
	}

protected:
	POINT m_ptRangeCenter;
	static const DWORD MOUSE_RIGHT_CLICK_RANGE = 2*65535/768;//�ڼ���Ҽ������¼�ʱҪ�����ͣ��������İ뾶��
	                                                         //������ڸ�����Χ�ڳ���һ��ʱ�伴��Ϊ�ɴ����Ҽ�
	                                                         //�����¼�
	DWORD m_dwRangeUpdateTime;
	static const DWORD MOUSE_RIGHTDOWN_EVENT_TRIGGER_TIMER = 1500;//����Ҽ������¼�������ʱ��
};


//��ʿ���Դ
enum ECtrlPenLightSource
{
	E_CTRL_PEN_LIGHT_SOURCE_RED_LASER,     //��ɫ�����
	E_CTRL_PEN_LIGHT_SOURCE_INFRARED,      //������
};

class CVideToScreenMap;
class CMouseEventGenerator
{
public:

	CMouseEventGenerator(CVideToScreenMap* pVideo2ScreenMap);
	
	~CMouseEventGenerator();

	//@����:��������¼�
	//@����:pPtVideo, ָ��Ŀ����Ļ�����ָ��,ΪNULL����ζ��û�м�⵽Ŀ��
	//      bDebug, ���Ա�־��
	void GenMouseEvent(const POINT* pPtScreen, BOOL bDebug = FALSE);

    //@����:���������¼�
    //@����:pPtScreen, ָ��Ŀ����Ļ�����ָ��
    //      MouseWheelLength,��������
	void GenMouseWheelEvent( const POINT* pPtScreen, int MouseWheelLength);

	//@����:����У��ģʽ
	//@����:
	//void EnterCalibrateMode(HWND hAdjustWnd, E_CALIBRATE_MODE eCalibrateMode);

	//@����:�뿪У��ģʽ
	//void LeaveCalibrateMode();

	/*void EnableOpticalPenControl(BOOL bEnable)
	{
		m_bPenControl = bEnable;
	}

	BOOL IsOpticalPenControlling() const
	{
		return m_bPenControl;
	}*/


	//CVideToScreenMap m_oVideo2ScreenMap;



	/*E_CALIBRATE_MODE GetCalibrateMode()const
	{
		return m_eCalibrateMode;
	}

	HWND GetCalibrateHWnd()const
	{
		return m_hCalibrateWnd;
	}

	BOOL IsCalibrating()const
	{
		return m_bIsCalibrating;
	}*/

	//@����:�ֶ�У��
	//@����:У������λ������
	//void DoManualCalibrate(POINT pt);

	//@����:�ֶ��ɼ���ߵĴ�С
	//@����:�ɼ�����λ�������������� //add by zhaown
    /*
	void DoSpotSampling(POINT pt,LONG nlArea);
	void DoSimulateCollectSpotSetting(POINT pt,LONG m_nlArea);
//	void GetImageFrame(const CYUY2Frame* pImage);//�õ�ͼ�����Ϣ
	void EnterSpotSamplingMode(HWND hSettingWnd,E_COLLECTSPOT_MODE eSpotSettingMode);
	void LeaveSpotSettingMode();
	E_COLLECTSPOT_MODE GetCollectSpotMode()const{return m_eSpotSamplingMode;}
	HWND  GetSpotHWnd()const {return m_hCollectSpotWnd;}
	BOOL  IsCollectingSpot()const {return m_bIsCollectingSpot;}

    */

	//@����:�Զ�У��
	//@����:У��ͼ��
	void DoAutoCalibrate(const CYUY2Frame* pImage);



	//@����:�л���ʿع�Դ
	//@����:eCtrlPenSrc, ��ʿ���Դ
	void SetCtrlPenSrc(ECtrlPenLightSource eCtrlPenSrc);
	ECtrlPenLightSource GetCtrlPenSrc();

    //@����:����״̬��
    void RunStateMachine(BOOL bFindPen, const POINT& ptPenPos);

	//@����:���м����ʱ��״̬��
	void RunLaserPenStateMachine(BOOL bFindPen, const POINT& ptPenPos);

	//@����:���к�����ʱ��״̬��
	void RunInfraredPenStateMachine(BOOL bFindPen, const POINT& ptPenPos);


	//@����:ģ������ֵ����
	BOOL SimulateMouseInterpolate(LPCTSTR lpszDataFileName);
	//


    //����:�������
   void SetMouseDeviationCorrect(int nXDeviation, int nYDeviation);
   //>>

protected:	
	//@����:����Ҽ��¼���꾲ֹ�ж�
	//
	//BOOL IsMouseRightStatinonary(const POINT& ptNow, const POINT& ptLast);

	//@����:�������¼���꾲ֹ�ж�
	//BOOL IsMouseLeftStatinonary(const POINT& ptNow, const POINT& ptLast);


	//@����:����¼������߳�
	static ULONG WINAPI GenSysMouseEventThreadProc(LPVOID lpCtx);


	//void QueueMouseData(const INPUT& data);

	POINT m_ptLastPenAbsolutePos;//��һ�ε�Ŀ����Ļ����
	//DWORD m_dwTimeLButtonDownStationary    ;//���������¾�ֹʱ��
	//static const DWORD MOUSE_RIGHTDOWN_EVENT_TRIGGER_TIMER = 1500;//����Ҽ������¼�������ʱ��

	//static const DWORD MOUSE_RIGHT_STATIC_PERMIT_MAX_SHIFT = 2*65535/768; //��괹ֱ��ˮƽ�ƶ�����(��һ����0~65535)С�ڸ�ֵ,����Ϊ��꾲ֹ
	//static const DWORD MOUSE_LEFT_STATIC_PERMIT_MAX_SHIFT  = 100*65535/768;//�����������(��һ����0~65535)С�ڸ�ֵ,����Ϊ���λ��δ�����仯
	//																	  //��ͬһ�������������������˫���¼���


	BOOL m_bIsCalibrating;//����У����־
	HWND m_hCalibrateWnd  ;//У������
	BOOL m_bPenControl;//���Ŀ���״̬

	//E_CALIBRATE_MODE m_eCalibrateMode;//У��ģʽ

	CDoubleClickChecker m_oDoubleClickChecker;
	CRightClickTrigger  m_oRightClickTrigger;

	//CBezierInterpolator<5> m_oInterpolater;
    CAkima_Interpolator<5>  m_oAkimaInterpolator;
	//��ʿ���Դ
	ECtrlPenLightSource  m_eCtrlPenSrc;

	//��ֵ���Դ���
	//��¼�������������ʱ��εĹ�ʵ�����λ�á�
	std::ofstream m_ofLog;//�ļ�����

	BOOL m_bEnableInterpolate;//ʹ�ܲ�ֵ��־

	//�ڵ���Window API����SendInputʵ�ֶ����������Բ�ֵʱ, 
	//һ����ֵ�����е�������ֵ��֮����Ҫ��ʱ1ms����, �����ֻ�в�ֵ���еĵ�һ�������һ��������,
	//�м�Ĳ�ֵ���ᱻϵͳ���ԡ�	
	//HANDLE m_hGenSysMouseEventThread;//����¼������߳�
	//HANDLE m_hForceExitEvent        ;//ǿ���˳��¼�
	//HANDLE m_hNewMouseEvent         ;//

	//static const int MAX_MOUSE_NUMBER = 256;
	//INPUT                    m_aryMouseData[MAX_MOUSE_NUMBER];//
	//CComAutoCriticalSection  m_csForMouseData                ;//
	//int                       m_nMouseQueueDataCount              ;//������ݸ���


	///////////////////////���ú����еı���zhaown

   E_COLLECTSPOT_MODE m_eSpotSamplingMode;
   BOOL m_bIsCollectingSpot;
   HWND m_hCollectSpotWnd;

   //CAlphaBetaSmoother<1> m_objMouseSmoother;

   //CStrokeSmoother<2> m_objStrokeSmoother;//�ʻ�ƽ����
   CStrokeSmoother<5> m_objStrokeSmoother;//�ʻ�ƽ����
   public:

	   enum E_SIMUL_MOUSE_STATE
	   {
		   SIMUL_MOUSE_STATE_IDLE             ,//ģ��������״̬
		   SIMUL_MOUSE_STATE_LEFT_DOWN        ,//ģ������������״̬
		   SIMUL_MOUSE_STATE_RIGHT_CONTROL    ,//ģ������Ҽ�����״̬
		   SIMUL_MOUSE_STATE_MOVE             ,//ģ�������ƶ�״̬  
	   }m_eSimulMouseState;

	   enum E_SIMUL_MOUSE_POSSAGE_STATE
	   {
		   SIMUL_MOUSE_POSSAGE_STATE_IDLE    ,
		   SIMUL_MOUSE_POSSAGE_STATE_DOWN    ,
		   SIMUL_MOUSE_POSSAGE_STATE_MOVE    ,
	   }m_eSimulMousePossageState;


	   /////���ܣ���������״̬
	   void SetSimulMouseResponseState(E_SIMUL_MOUSE_STATE eSimulmouseState);

       //<<����
       std::ofstream m_filePosData;//������λ����Ϣ�������ļ���
       //>>

	   DWORD m_dwLastMouseEventTime;

	  CVelocityCompensator m_oVelocityCompensator;
      CVideToScreenMap*    m_pVideo2ScreenMap;

};

__inline BOOL operator==(const POINT& right, const POINT& left)
{
   return (right.x == left.x && right.y == left.y);
}

__inline BOOL operator!=(const POINT& right, const POINT& left)
{
   return (right.x != left.x || right.y != left.y);
}

//extern CMouseEventGenerator g_oMouseEventGen;