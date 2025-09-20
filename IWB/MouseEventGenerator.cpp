#include "stdafx.h"
#include "headers.h"

//CMouseEventGenerator g_oMouseEventGen;

typedef  BOOL  (WINAPI* ChangeWindowMessageFiltetProc)( UINT, DWORD );
//class CMouseEventGenerator
CMouseEventGenerator::CMouseEventGenerator(CVideToScreenMap* pVideo2ScreenMap)
:
m_eSimulMouseState(SIMUL_MOUSE_STATE_IDLE),
m_bIsCalibrating(FALSE),
m_hCalibrateWnd(NULL),
m_bPenControl(FALSE),
m_eCtrlPenSrc(E_CTRL_PEN_LIGHT_SOURCE_INFRARED),
m_bEnableInterpolate(FALSE),
m_bIsCollectingSpot(FALSE),//zhaown
m_hCollectSpotWnd(NULL),
m_eSimulMousePossageState(SIMUL_MOUSE_POSSAGE_STATE_IDLE),
m_pVideo2ScreenMap(pVideo2ScreenMap)
{

    m_ptLastPenAbsolutePos.x = 0;
    m_ptLastPenAbsolutePos.x = 0;

    OSVERSIONINFOEX osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    BOOL bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*) &osvi);

    if(bOsVersionInfoEx)
    {
#ifdef _LOG
        LOG_INF("OS Version %d.%d", osvi.dwMajorVersion, osvi.dwMinorVersion);
#endif
        if(osvi.dwMajorVersion == 6)
        {

            HINSTANCE hUser32; 

            hUser32 = LoadLibrary(_T("user32.dll"));

            ChangeWindowMessageFiltetProc fpChangeWindowMessageFilter = (ChangeWindowMessageFiltetProc) GetProcAddress(hUser32, "ChangeWindowMessageFilter"); 

            if(fpChangeWindowMessageFilter)
            {
                BOOL bRet = fpChangeWindowMessageFilter(WM_LBUTTONDOWN, MSGFLT_ADD);
                if(!bRet)
                {
#ifdef _LOG
					LOG_ERR("ChangeWindowMessageFilte remove WM_LBUTTONDOWN failed! error=0x%x",GetLastError());
#endif
                }

                bRet = fpChangeWindowMessageFilter(WM_LBUTTONUP, MSGFLT_ADD);
                if(!bRet)
                {
#ifdef _LOG
					LOG_ERR("ChangeWindowMessageFilte remove WM_LBUTTONUP failed! error=0x%x",GetLastError());
#endif
                }

                bRet = fpChangeWindowMessageFilter(WM_MOUSEMOVE, MSGFLT_ADD);
                if(!bRet)
                {
#ifdef _LOG
                    LOG_ERR("ChangeWindowMessageFilte remove WM_MOUSEMOVE failed! error=0x%x",GetLastError());
#endif
                }

                bRet = fpChangeWindowMessageFilter(WM_RBUTTONDOWN, MSGFLT_ADD);
                if(!bRet)
                {
#ifdef _LOG
					LOG_ERR("ChangeWindowMessageFilte remove WM_RBUTTONDOWN failed! error=0x%x",GetLastError());
#endif
                }

                bRet = fpChangeWindowMessageFilter(WM_RBUTTONUP, MSGFLT_ADD);
                if(!bRet)
                {
#ifdef _LOG
					LOG_ERR("ChangeWindowMessageFilte remove WM_RBUTTOUP failed! error=0x%x",GetLastError());
#endif
                }

            }

            FreeLibrary(hUser32); 

        }


    }

    //m_nMouseQueueDataCount = 0;
    //m_hForceExitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    //m_hNewMouseEvent  = CreateEvent(NULL, FALSE, FALSE, NULL);



    ////��������¼������߳�
    //DWORD dwThreadId = 0;

    //m_hGenSysMouseEventThread = 
    //CreateThread(
    //	NULL,
    //	0,
    //	GenSysMouseEventThreadProc,
    //	(LPVOID)this,
    //	0,
    //	&dwThreadId);


    //SetThreadPriority(m_hGenSysMouseEventThread, THREAD_PRIORITY_TIME_CRITICAL);

#ifdef _DEBUG
    m_filePosData.open(_T("PenPos.dat"));
#endif

}

CMouseEventGenerator::~CMouseEventGenerator()
{
    //SetEvent(m_hForceExitEvent);
    //WaitForSingleObject(m_hGenSysMouseEventThread, INFINITE);
    //
    //CloseHandle(m_hForceExitEvent);
    //CloseHandle(m_hNewMouseEvent);
    //CloseHandle(m_hGenSysMouseEventThread);

    //<<����, 2012/10/10
#ifdef _DEBUG
    this->m_filePosData.close();
#endif
    //>>
}




//@����:��������¼�
//@����:pPtVideo, ָ��Ŀ����Ļ�����ָ��,ΪNULL����ζ��û�м�⵽Ŀ��
//      bDebug, ���Ա�־��
void CMouseEventGenerator::GenMouseEvent(const POINT* pPtScreen, BOOL bDebug)
{
	//<<debug
	//{
		static DWORD s_dwLastTime;
		static BOOL s_bFirstTime = TRUE;
		if(s_bFirstTime)
		{
			s_dwLastTime = GetTickCount();
			s_bFirstTime = FALSE;
		}
		else
		{
			DWORD dwNow = GetTickCount();
			if(dwNow - s_dwLastTime == 0) 
			{
				//AtlTrace(_T("CMouseEventGenerator::GenMouseEvent Interval = 0 ms, pPtVideo=0x%x\r\n"), pPtVideo);

			}
			s_dwLastTime = dwNow;
		}
	//}
	//debug>>

    if(!m_bIsCalibrating &&  !m_bPenControl)
    {
        return;
    }

    BOOL bFindPen = TRUE;
    POINT ptPenPos;
    ptPenPos.x  = 0;
    ptPenPos.y  = 0;
    //SIZE ScreenSize = g_oVideo2ScreenMap.GetScreenSize();
    //SIZE ScreenSize = m_pVideo2ScreenMap->GetScreenSize();


    if(pPtScreen == NULL)
    {
        bFindPen = FALSE;

        if(!m_objStrokeSmoother.IsReset())//��λƽ����
        {
            ptPenPos = m_objStrokeSmoother.Reset();        

            if( m_bEnableInterpolate)//���ʹ�ܲ�ֵ, �����һ��ƽ��ֵ����������ֵ��
            {
                int nPointCount = m_oAkimaInterpolator.Process(ptPenPos);
                const POINT* ptInterpolated = m_oAkimaInterpolator.GetInterpolatePts();

                for(int i=0;i < nPointCount; i++)
                {
                    RunStateMachine(TRUE, ptInterpolated[i]);
                    Sleep(1);//��ʱ1ms
                }
            }
            else
            {
                //��֤�˲�������һ�㲻��©
                RunStateMachine(TRUE, ptPenPos);
            }
        }


        //��ʲ���
        if( m_bEnableInterpolate)////ʹ�ܲ�ֵ
        {
            int nPointCount = m_oAkimaInterpolator.EndInterpolate();//������ֵ
            const POINT* ptInterpolated = m_oAkimaInterpolator.GetInterpolatePts();

            for(int i=0;i < nPointCount; i++)
            {
                RunStateMachine(TRUE, ptInterpolated[i]);
                Sleep(1);
            }

             m_oAkimaInterpolator.Reset();//��ֵ����λ

             RunStateMachine(FALSE, ptPenPos);
        }
        else
        {
            RunStateMachine(FALSE, ptPenPos);
        }


		//��λ�ٶȲ�����
		m_oVelocityCompensator.Reset();
    }
    else//�µ����λ����Ϣ
    {
        //if(m_bIsCalibrating)
        //{
        //    return DoManualCalibrate(*pPtVideo);
        //}

       // ptPenPos = m_oVideo2ScreenMap.GetPt(pPtVideo->x, pPtVideo->y);
         ptPenPos = *pPtScreen;
//		AtlTrace(_T(" CMouseEventGenerator::GenMouseEvent(), pPtVideo->x = %d ,pPtVideo->y = %d  ptPenPos.x = %d , ptPenPos.y = %d \r\n") ,pPtVideo->x,pPtVideo->y ,ptPenPos.x ,ptPenPos.y );

		//�ٶȲ���,����û���ʵʱ����Ч��
		//ptPenPos = m_oVelocityCompensator.Compensate(ptPenPos);

		//


        //ptPenPos.x = (ptPenPos.x * nCxScreen + 65535/2/*��������*/)/ 65535;
        //ptPenPos.y = (ptPenPos.y * nCyScreen + 65535/2/*��������*/)/65535;

        //<<Mouseλ��ƽ������ ��������΢С�Ķ���
        ptPenPos = m_objStrokeSmoother.Smooth(ptPenPos);
        //>>
        
        //ptPenPos.x = (ptPenPos.x * 65535 + nCxScreen/2/*��������*/) /nCxScreen;
        //ptPenPos.y = (ptPenPos.y * 65535 + nCyScreen/2/*��������*/) /nCyScreen;


        if(m_bEnableInterpolate)//ʹ�ܲ�ֵ
        {
          
            int nPointCount = m_oAkimaInterpolator.Process(ptPenPos);
            const POINT* ptInterpolated = m_oAkimaInterpolator.GetInterpolatePts();

            for(int i=0;i < nPointCount; i++)
            {
                RunStateMachine(TRUE, ptInterpolated[i]);
                Sleep(1);
            }

        }
        else
        {
            RunStateMachine(bFindPen, ptPenPos);
        }

    }


#ifdef _DEBUG
//    if(bDebug)
	if(pPtScreen)
    {
        //POINT ptDbg;

        //AtlTrace(_T("<u,v>=<%d,%d>:<x,y>=<%d,%d>\r\n"), pPtVideo->x, pPtVideo->y, ptDbg.x, ptDbg.y);
//         int nCxScreen = ::GetSystemMetrics(SM_CXSCREEN);
//         int nCyScreen = ::GetSystemMetrics(SM_CYSCREEN);

       // ptDbg.x = (ptPenPos.x * nCxScreen + 65535/2/*��������*/) / 65535;
       // ptDbg.y = (ptPenPos.y * nCyScreen + 65535/2/*��������*/) / 65535;

        //<<����
        CAtlStringA strInfo;
        //strInfo.Format("%d,%d\n",ptDbg.x, ptDbg.y);
        strInfo.Format("%d,%d\n",ptPenPos.x, ptPenPos.y);
        this->m_filePosData.write(strInfo, strInfo.GetLength());
		this->m_filePosData.flush();
        //>>
    }
#endif





}

//@����:����״̬��
void CMouseEventGenerator::RunStateMachine(BOOL bFindPen, const POINT& ptPenPos)
{
	//SIZE  screenSize = g_oVideo2ScreenMap.GetScreenSize();
    //SIZE  screenSize = m_pVideo2ScreenMap->GetScreenSize();

    //������������Ͻ�����
    POINT vdLeftTop;
    vdLeftTop.x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    vdLeftTop.y = GetSystemMetrics(SM_YVIRTUALSCREEN);


    int nCxVirtualScreen = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int nCyVirtualScreen = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	
	//�������������0~65535��Χ��
	POINT pt;
	/*pt.x =  (ptPenPos.x - vdLeftTop.x) * 65535/screenSize.cx;
	pt.y =  (ptPenPos.y - vdLeftTop.y) * 65535/screenSize.cy;*/

    pt.x =  (ptPenPos.x - vdLeftTop.x) * 65535/nCxVirtualScreen;
	pt.y =  (ptPenPos.y - vdLeftTop.y) * 65535/nCyVirtualScreen;

    switch(m_eCtrlPenSrc)
    {
    case E_CTRL_PEN_LIGHT_SOURCE_RED_LASER:
        RunLaserPenStateMachine(bFindPen, pt);
        break;

    case E_CTRL_PEN_LIGHT_SOURCE_INFRARED:
        RunInfraredPenStateMachine(bFindPen, pt);
        break;

    }
}

//@����:���м����ʱ��״̬��
void CMouseEventGenerator::RunLaserPenStateMachine(BOOL bFindPen, const POINT& ptPenPos)
{
    INPUT input;
    memset(&input, 0 ,sizeof(input));
    input.type = INPUT_MOUSE;
    int nCxVirtualScreen = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int nCyVirtualScreen = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if(bFindPen)
    {
        input.mi.dx       = ptPenPos.x;
        input.mi.dy       = ptPenPos.y;
        input.mi.dwFlags  =  MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK ;
        SendInput(1, &input, sizeof(INPUT));
        //QueueMouseData(input);//modified by toxuke@gmail.com, 2012/05/21
        m_ptLastPenAbsolutePos  = ptPenPos;
    }
}

//@����:���к�����ʱ��״̬��
void CMouseEventGenerator::RunInfraredPenStateMachine(BOOL bFindPen, const POINT& ptPenPos)
{
    INPUT input;
    memset(&input, 0 ,sizeof(input));
    input.type = INPUT_MOUSE;

    int nCxVirtualScreen = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int nCyVirtualScreen = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    //״̬������
    switch(m_eSimulMouseState)
    {
    case SIMUL_MOUSE_STATE_IDLE:

//			LOG("CALL SIMUL_MOUSE_STATE_IDLE ");
			//�����дʱ�����ʻ����Ĺ�����
			//����ʻ�A,B���ĺܽ����ʻ�Aд��󣬹����ʣ����״̬��ΪSIMUL_MOUSE_STATE_IDLE�� Ȼ��������д
			//�ʻ�B�����ڱʻ�B��ʻ�A����ܽ��� ����IsMouseLeftStatinonary����TRUE, ��
			//if(IsMouseLeftStatinonary(ptPenPos, m_ptLastPenAbsolutePos))
			//{
			//	mouse_event(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, m_ptLastPenAbsolutePos.x, m_ptLastPenAbsolutePos.y, 0, NULL);
			//	AtlTrace("Left Down Stationary!\r\n");
			//}
			//else
			//{
			//mouse_event(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE , ptPenPos.x, ptPenPos.y, 0, NULL);

        if(bFindPen)
        {
//            LOG("CALL SIMUL_MOUSE_STATE_IDLE ");
            //�����дʱ�����ʻ����Ĺ�����
            //����ʻ�A,B���ĺܽ����ʻ�Aд��󣬹����ʣ����״̬��ΪSIMUL_MOUSE_STATE_IDLE�� Ȼ��������д
            //�ʻ�B�����ڱʻ�B��ʻ�A����ܽ��� ����IsMouseLeftStatinonary����TRUE, 
            input.mi.dx       = ptPenPos.x;
            input.mi.dy       = ptPenPos.y;
            input.mi.dwFlags  =  MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_VIRTUALDESK;
            SendInput(1, &input, sizeof(INPUT));


            m_ptLastPenAbsolutePos  = ptPenPos;

            input.mi.dx       = 0;
            input.mi.dy       = 0;
            input.mi.dwFlags  = MOUSEEVENTF_LEFTDOWN ;
            SendInput(1, &input, sizeof(INPUT));

            AtlTrace(_T("[Thread(0x%08x)]D.Left Down,x=%d,y=%d \n"), GetCurrentThreadId(), ptPenPos.x*nCxVirtualScreen/65535, ptPenPos.y*nCyVirtualScreen/65535);



			//<<debug
			m_dwLastMouseEventTime = GetTickCount();
			//>>

            //˫���¼����
            m_oDoubleClickChecker.Check(ptPenPos);
            m_eSimulMouseState = SIMUL_MOUSE_STATE_LEFT_DOWN;

			//��λ�һ������,���¿�ʼ�һ����
			m_oRightClickTrigger.Reset();
        }

        break;

    case SIMUL_MOUSE_STATE_LEFT_DOWN:
        if(bFindPen)
        {
            if(m_oRightClickTrigger.Process(ptPenPos))
            {
                m_eSimulMouseState = SIMUL_MOUSE_STATE_RIGHT_CONTROL;
            }
            else
            {
				//<<debug
				DWORD dwNow = GetTickCount();

				DWORD dwDelta = dwNow - m_dwLastMouseEventTime;
				if((dwDelta) == 0)
				{
					//AtlTrace(_T("Mouse LDOWN MOUSE MOVE Event Elapse=%d ms \r\n "),dwDelta);
					//Sleep(1);
				}
				else
				{
					//AtlTrace(_T("Mouse LDOWN MOUSE MOVE Event Elapse=%dms"),dwDelta);
				}
				m_dwLastMouseEventTime = dwNow;
				//debug>>

                input.mi.dx       = ptPenPos.x;
                input.mi.dy       = ptPenPos.y;
                input.mi.dwFlags  = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_VIRTUALDESK;
                SendInput(1, &input, sizeof(INPUT));
                m_ptLastPenAbsolutePos = ptPenPos;

                AtlTrace(_T("[Thread(0x%08x)]E.Left Down Move,x=%d,y=%d \n"), GetCurrentThreadId(), ptPenPos.x*nCxVirtualScreen/65535, ptPenPos.y*nCyVirtualScreen/65535);
            }
        }
        else
        {

            input.mi.dx       = 0;
            input.mi.dy       = 0;
            input.mi.dwFlags  = MOUSEEVENTF_LEFTUP;
            SendInput(1, &input, sizeof(INPUT));
            m_eSimulMouseState = SIMUL_MOUSE_STATE_IDLE;
            
            AtlTrace(_T("[Thread(0x%08x)]F.Left Up,x=%d,y=%d \n"), GetCurrentThreadId(), ptPenPos.x*nCxVirtualScreen/65535, ptPenPos.y*nCyVirtualScreen/65535);

			

        }

        break;

    case SIMUL_MOUSE_STATE_RIGHT_CONTROL:
        if(bFindPen)
        {
//            LOG("CALL SIMUL_MOUSE_STATE_RIGHT_CONTROL    ");
            if(m_ptLastPenAbsolutePos != ptPenPos)
            {
                //mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, ptPenPos.x, ptPenPos.y, 0, NULL);
                input.mi.dx       = ptPenPos.x;
                input.mi.dy       = ptPenPos.y;
                input.mi.dwFlags  = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_VIRTUALDESK;
                SendInput(1, &input, sizeof(INPUT));
                m_ptLastPenAbsolutePos = ptPenPos;
            }

        }
        else
        {
            //mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, NULL);
            m_eSimulMouseState = SIMUL_MOUSE_STATE_IDLE;
        }
        break;

	case SIMUL_MOUSE_STATE_MOVE:               //ģ�������ƶ�����
		if (bFindPen)
		{
//			AtlTrace("CMouseEventGenerator::RunInfraredPenStateMachine_move1() ,ptPenPos->x = %d ,ptPenPos->y = %d \r\n",ptPenPos.x , ptPenPos.y );
// 			input.mi.dx       = ptPenPos.x;
// 			input.mi.dy       = ptPenPos.y;
// 			input.mi.dwFlags  =  MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
// 		//	SendInput(1, &input, sizeof(INPUT));
// 			m_ptLastPenAbsolutePos  = ptPenPos;
		}
		else
		{
			m_eSimulMouseState = SIMUL_MOUSE_STATE_IDLE;
		}
		break;
	}//switch
}

//BOOL CMouseEventGenerator::IsMouseRightStatinonary(const POINT& ptNow, const POINT& ptLast)
//{
//	int deltaX = abs(ptNow.x - ptLast.x);
//	int deltaY = abs(ptNow.y - ptLast.y);
//
//	if(deltaX <= MOUSE_RIGHT_STATIC_PERMIT_MAX_SHIFT && deltaY <= MOUSE_RIGHT_STATIC_PERMIT_MAX_SHIFT)
//	{
//		//AtlTrace(_T("Mouse is stationary!dx=%d,dy=%d\r\n"), deltaX,deltaY);
//		return TRUE;
//	}
//	//AtlTrace(_T("Mouse is moving!dx=%d,dy=%d\r\n"), deltaX,deltaY);
//	return FALSE;
//}


//BOOL CMouseEventGenerator::IsMouseLeftStatinonary(const POINT& ptNow, const POINT& ptLast)
//{
//	int deltaX = abs(ptNow.x - ptLast.x);
//	int deltaY = abs(ptNow.y - ptLast.y);
//
//	if(deltaX <= MOUSE_LEFT_STATIC_PERMIT_MAX_SHIFT && deltaY <= MOUSE_LEFT_STATIC_PERMIT_MAX_SHIFT)
//	{
//		AtlTrace(_T("Mouse is Left Down stationary!dx=%d,dy=%d\r\n"), deltaX,deltaY);
//		return TRUE;
//	}
//	AtlTrace(_T("Mouse is Left Down moving!dx=%d,dy=%d\r\n"), deltaX,deltaY);
//	return FALSE;
//}


//@����:����У��ģʽ
//@����:hAdjustWnd, У������
//      eCalibrateMode, У��ģʽ
//      
//void CMouseEventGenerator::EnterCalibrateMode(HWND hAdjustWnd, E_CALIBRATE_MODE eCalibrateMode)
//{
//    AtlTrace(_T("EnterCalibrateMode\r\n"));
//    m_hCalibrateWnd  = hAdjustWnd;
//    m_bIsCalibrating = TRUE;
//    m_eCalibrateMode = eCalibrateMode;
//}
//
////@����:�뿪У��ģʽ
//void CMouseEventGenerator::LeaveCalibrateMode()
//{
//    AtlTrace(_T("LeaveCalibrateMode\r\n"));
//    m_hCalibrateWnd   = NULL;
//    m_bIsCalibrating = FALSE;
//}
//
////@����:�˹�У��
////@����:pt, У�������Ƶ����
//void CMouseEventGenerator::DoManualCalibrate(POINT pt)
//{
//
//    if(m_hCalibrateWnd)
//    {
//        LPARAM lParam = MAKELONG(pt.x, pt.y);
//        //SendMessage(m_hCalibrateWnd, WM_MANUAL_CALIBRATE_DATA, 0, lParam);
//        //���ʹ��SendMessage, 
//        //��DoManualCalibrate������ͼ�����̵߳���, ��У�����ڴ���WM_MANUAL_CALIBRATE_DATA������, �ṹ����ͼ�����̵߳ȴ���
//        //ͼ��ʵʱ������жϡ�
//        PostMessage(m_hCalibrateWnd, WM_MANUAL_CALIBRATE_DATA, 0, lParam);
//
//    }
//
//}


//@����:�Զ�У��
//@����:У��ͼ��
//void CMouseEventGenerator::DoAutoCalibrate(const CYUY2Frame* pImage)
//{
//    if(m_hCalibrateWnd)
//    {
//        LPARAM lParam = (LPARAM)pImage;
//        SendMessage(m_hCalibrateWnd, WM_AUTO_CALIBRATE_DATA, 0, lParam);
//    }
//}


//@����:�л���ʿع�Դ
//@����:eCtrlPenSrc, ��ʿ���Դ
void CMouseEventGenerator::SetCtrlPenSrc(ECtrlPenLightSource eCtrlPenSrc)
{
    if(m_eSimulMouseState == SIMUL_MOUSE_STATE_LEFT_DOWN)
    {
        INPUT input;
        memset(&input, 0 ,sizeof(input));
        input.type = INPUT_MOUSE;

        input.mi.dx       = 0;
        input.mi.dy       = 0;
        input.mi.dwFlags  = MOUSEEVENTF_LEFTUP;
        SendInput(1, &input, sizeof(INPUT));
        //QueueMouseData(input);//modified by toxuke@gmail.com, 2012/05/21
    }
    m_eSimulMouseState = SIMUL_MOUSE_STATE_IDLE;


    this->m_eCtrlPenSrc = eCtrlPenSrc;

    if(this->m_eCtrlPenSrc == E_CTRL_PEN_LIGHT_SOURCE_RED_LASER)
    {
        AtlTrace(_T("Laser Pen Is Controlling\r\n"));	
    }
    else if(this->m_eCtrlPenSrc == E_CTRL_PEN_LIGHT_SOURCE_INFRARED)
    {

        AtlTrace(_T("Infrared Pen Is Controlling\r\n"));	
    }
}
//@����
ECtrlPenLightSource CMouseEventGenerator::GetCtrlPenSrc()
{
    return this->m_eCtrlPenSrc;
}

//@����:ģ������ֵ����
BOOL CMouseEventGenerator::SimulateMouseInterpolate(LPCTSTR lpszDataFileName)
{
    ///std::ifstream file;
    //file.open(lpszDataFileName);

    FILE* stream = NULL;

    _tfopen_s(&stream, lpszDataFileName, _T("r"));

    if(stream == NULL)
    {

        return FALSE;
    }

    std::vector<POINT> sample;

    BOOL bError = FALSE;
    while(1)
    {
        POINT pt;
        int nRet = fscanf_s(stream, "%d,%d\r\n", &pt.x, &pt.y);
        if(nRet == 2)
        {
            int nCxScreen = GetSystemMetrics(SM_CXSCREEN);
            int nCyScreen = GetSystemMetrics(SM_CYSCREEN);

            pt.x = (65535*pt.x + nCxScreen/2)/nCxScreen;
            pt.y = (65535*pt.y + nCyScreen/2)/nCyScreen;

            sample.push_back(pt);
        }
        else if(nRet == EOF)
        {

            break;
        }
        else
        {
            bError = TRUE;
            break;
        }

    }

    if(bError)
    {
        fclose(stream);
        return FALSE;

    }


    if(sample.size() > 0)
    {
        INPUT input;
        memset(&input, 0 ,sizeof(input));
        input.type = INPUT_MOUSE;


        POINT ptPenPos = sample[0];

        input.mi.dx       = ptPenPos.x;
        input.mi.dy       = ptPenPos.y;
        input.mi.dwFlags  = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_VIRTUALDESK;
        SendInput(1, &input, sizeof(INPUT));


        input.mi.dx       = 0;
        input.mi.dy       = 0;
        input.mi.dwFlags  = MOUSEEVENTF_LEFTDOWN ;
        SendInput(1, &input, sizeof(INPUT));

        //m_oInterpolater.Reset();
        //m_oInterpolater.Process(ptPenPos);
        m_oAkimaInterpolator.Reset();
        m_oAkimaInterpolator.Process(ptPenPos);

        for(size_t i=1; i< sample.size();i++)
        {
            POINT ptPenPos = sample[i];

            //int nPointCount = m_oInterpolater.Process(ptPenPos);
            //const int* pX = m_oInterpolater.GetInterpolateX();
            //const int* pY = m_oInterpolater.GetInterpolateY();
            int nPointCount = m_oAkimaInterpolator.Process(ptPenPos);
            const POINT* ptInterpolated = m_oAkimaInterpolator.GetInterpolatePts();

            for(int j=0; j<nPointCount; j++)
            {		
                input.mi.dx       = ptInterpolated[j].x;//ptPenPos.x;
                input.mi.dy       = ptInterpolated[j].y;//ptPenPos.y;
                input.mi.dwFlags  =  MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_VIRTUALDESK;
                Sleep(10);
                SendInput(1, &input, sizeof(INPUT));

            }


        }//for

        input.mi.dx       = 0;
        input.mi.dy       = 0;
        input.mi.dwFlags  = MOUSEEVENTF_LEFTUP;
        SendInput(1, &input, sizeof(INPUT));

    }


    fclose(stream);

    return TRUE;


}



//void CMouseEventGenerator::QueueMouseData(const INPUT& data)
//{
//	CComCritSecLock<CComAutoCriticalSection> lock(m_csForMouseData);
//	if(m_nMouseQueueDataCount == MAX_MOUSE_NUMBER) return;
//	m_aryMouseData[m_nMouseQueueDataCount ++ ] = data;
//	SetEvent(this->m_hNewMouseEvent);
//
//}


//@����:����¼������߳�
//ULONG  WINAPI CMouseEventGenerator::GenSysMouseEventThreadProc(LPVOID lpCtx)
//{
//	CMouseEventGenerator* pGen = reinterpret_cast<CMouseEventGenerator*>(lpCtx);
//
//	HANDLE hEvents[2];
//	hEvents[0] = pGen->m_hForceExitEvent;
//	hEvents[1] = pGen->m_hNewMouseEvent;
//	
//	DWORD dwRet;
//
//	INPUT lastData;
//	DWORD dwLastMoveTime;
//	memset(&lastData, 0, sizeof(lastData));
//	dwLastMoveTime = GetTickCount();
//
//	const DWORD dwInterval = 2;
//	
//	for(;;)
//	{
//		dwRet = WaitForMultipleObjects(_countof(hEvents), hEvents, FALSE, INFINITE);
//
//		if(dwRet == WAIT_OBJECT_0)//�˳��¼�
//		{
//			break;
//		}
//		else if(dwRet == WAIT_OBJECT_0 + 1)//���ģ���¼�
//		{
//			
//				INPUT  data[MAX_MOUSE_NUMBER];
//				int    count = 0;
//
//				{
//					CComCritSecLock<CComAutoCriticalSection>  lock(pGen->m_csForMouseData);
//					count = pGen->m_nMouseQueueDataCount;
//					memcpy(data, pGen->m_aryMouseData, count * sizeof(INPUT));
//					pGen->m_nMouseQueueDataCount = 0;
//
//				}
//
//				for(int i=0; i<count; i++)
//				{
//					BOOL bIsMouseMove = data[i].mi.dwFlags & MOUSEEVENTF_MOVE;
//					
//					DWORD dwNow;
//					dwNow = GetTickCount();
//					if((lastData.mi.dwFlags & MOUSEEVENTF_MOVE) && bIsMouseMove)
//					{
//
//						if((dwNow - dwLastMoveTime) < dwInterval)//ʱ����<1ms
//						{
//							Sleep(dwInterval);
//						}
//
//					}
//
//					//ģ���������
//					SendInput(1, &data[i], sizeof(INPUT));
//
//					if(bIsMouseMove)
//					{
//						dwLastMoveTime = dwNow;
//					}
//
//					lastData = data[i];
//
//				}//for-each
//
//
//
//		}
//		else//��������
//		{
//			break;
//		}
//
//
//
//	}
//
//
//	
//
//
//	return 0U;
//}

///////////////////////////////////////zhaown  start 2012-07-11
//@���ܣ�����������״̬
//@���������ô��壬����ģʽ  
//void CMouseEventGenerator::EnterSpotSamplingMode(HWND hSettingWnd,E_COLLECTSPOT_MODE eSpotSettingMode)
//{
//    AtlTrace(_T("EnterSpotSamplingMode\r\n"));
//    m_hCollectSpotWnd = hSettingWnd;
//    m_bIsCollectingSpot =TRUE;
//    m_eSpotSamplingMode = eSpotSettingMode;
//}
//
//void CMouseEventGenerator::LeaveSpotSettingMode()
//{
//    AtlTrace(_T("LeaveSpotSettingMode\r\n"));
//    m_hCollectSpotWnd = NULL;
//    m_bIsCollectingSpot = FALSE;
//}

//void CMouseEventGenerator::DoSpotSampling(POINT pt,LONG nlArea)
//{
//    if (m_hCollectSpotWnd)
//    {
//		//<<modified by toxuke@gmail.com, 2013/04/11
//	//	pt.x >>=INT_SCALE_SHIFT_SIZE;
//	//  	pt.y >>=INT_SCALE_SHIFT_SIZE;
//		//>>
//
//        LPARAM lParam = MAKELONG(pt.x, pt.y);
//        PostMessage(m_hCollectSpotWnd,WM_COLLECT_SPOT_DATA,(WPARAM)(nlArea), lParam);
//    }
//}
//
//void CMouseEventGenerator::DoSimulateCollectSpotSetting(POINT pt,LONG m_nlArea)
//{
//    if (m_hCollectSpotWnd)
//    {
//		//<<modified by toxuke@gmail.com, 2013/04/11
//		pt.x >>=INT_SCALE_SHIFT_SIZE;
//		pt.y >>=INT_SCALE_SHIFT_SIZE;
//		//>>
//
//        LPARAM lParam = MAKELONG(pt.x, pt.y);
//        PostMessage(m_hCollectSpotWnd,WM_COLLECT_SPOT_DATA,(WPARAM)(m_nlArea), lParam);
//    }
//}

// void CMouseEventGenerator::GetImageFrame(const CYUY2Frame* pImage)
// {
//     if (m_hCollectSpotWnd)
//     {
//         LPARAM lParam = (LPARAM)pImage;
//         SendMessage(m_hCollectSpotWnd,WM_CALIBRATE_FRAME_DATA,0,lParam);
//     }
// }



//@����:���������¼�
//@����:pPtScreen, ָ��Ŀ����Ļ�����ָ��
//      MouseWheelLength,��������
void CMouseEventGenerator::GenMouseWheelEvent(const POINT* pPtScreen , int MouseWheelLength)
{
	if(!m_bIsCalibrating && !m_bPenControl)
	{
		return;
	}

    if(pPtScreen == NULL) return;

	INPUT MouseInput;
	MouseInput.type = INPUT_MOUSE;
	MouseInput.mi.dx  = 0 ;
	MouseInput.mi.dy  = 0 ;
                                            
	MouseInput.mi.mouseData = 0;
	MouseInput.mi.time = 0;
	MouseInput.mi.dwExtraInfo = 0;

	if (m_eSimulMouseState == SIMUL_MOUSE_STATE_LEFT_DOWN)                  //�������������µ�ʱ�򣬾���Ҫȡ����������Ĺ���
	{
	    //SIZE  screenSize = g_oVideo2ScreenMap.GetScreenSize();
        //SIZE  screenSize = m_pVideo2ScreenMap->GetScreenSize();
        POINT vdLeftTop;
        vdLeftTop.x = GetSystemMetrics(SM_XVIRTUALSCREEN);
        vdLeftTop.y = GetSystemMetrics(SM_YVIRTUALSCREEN);


        int nCxVirtualScreen = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int nCyVirtualScreen = GetSystemMetrics(SM_CYVIRTUALSCREEN);

		POINT pPtPos;
		pPtPos.x = pPtPos.y = 0;
        pPtPos   = *pPtScreen;
		pPtPos.x =  (pPtPos.x  - vdLeftTop.x)* 65535/nCxVirtualScreen;
		pPtPos.y =  (pPtPos.y  - vdLeftTop.y)* 65535/nCyVirtualScreen;

		MouseInput.mi.dx = pPtPos.x;
		MouseInput.mi.dy = pPtPos.y +1000;
		MouseInput.mi.dwFlags = MOUSEEVENTF_MOVE|MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_VIRTUALDESK ;
		SendInput(1,&MouseInput,sizeof(INPUT));                            //�ƿ������λ��

		MouseInput.mi.dx = 0;
		MouseInput.mi.dy = 0;

		MouseInput.mi.dwFlags = MOUSEEVENTF_LEFTUP;
		SendInput(1,&MouseInput,sizeof(INPUT));

		m_eSimulMouseState = SIMUL_MOUSE_STATE_MOVE;

	}

	if(0 == MouseWheelLength)	
	{
		return;
	}

	MouseInput.mi.dwFlags = MOUSEEVENTF_WHEEL;
	MouseInput.mi.mouseData = MouseWheelLength;
	MouseInput.mi.time = 0;
	MouseInput.mi.dwExtraInfo = 0;

    SendInput(1,&MouseInput,sizeof(INPUT));
}


void CMouseEventGenerator::SetSimulMouseResponseState(E_SIMUL_MOUSE_STATE  eSimulmouseState)
{
    m_eSimulMouseState = eSimulmouseState;
}