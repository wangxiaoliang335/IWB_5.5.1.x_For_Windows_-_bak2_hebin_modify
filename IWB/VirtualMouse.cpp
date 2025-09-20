#include "stdafx.h"
//#include "headers.h"

CVirtualMouse::CVirtualMouse()
:
m_hDriverDevice(INVALID_HANDLE_VALUE),
m_oDoubleClickChecker(this),
m_oRightClickTrigger(this),
m_bForceUseWinAPI(FALSE)
{
    Reset();
}

//@����:���������豸���
//@����:hDriverDevice�� �����豸���
//@˵��:���豸���ΪINVALID_HANDLE_VALUEʱ��ʹ��Window APIģ�������Ϣ��
void CVirtualMouse::SetDeviceHandle(HANDLE hDriverDevice)
{
    m_hDriverDevice = hDriverDevice;
}


//����:�������������������
//����:bPenOn, ��������־
//     pPos, ���λ��,��Ļ����
//     hDriverDevice, �����豸���
void CVirtualMouse::Input(BOOL bPenOn, const POINT* pPos, BOOL bForceUseWinAPI)
{
    POINT ptPenPos;
    ptPenPos.x = ptPenPos.y =0;
    if(pPos)
    {
        ptPenPos.x = pPos->x;
        ptPenPos.y = pPos->y;
    }

	m_bForceUseWinAPI = bForceUseWinAPI;
    //״̬������
    switch(m_eVirutalMouseState)
    {
    case VIRTUAL_MOUSE_STATE_IDLE:

        if(bPenOn)
        {
            GenEvent(ptPenPos,E_MOUSE_MOVE);

            m_ptLastPenAbsolutePos  = ptPenPos;

            GenEvent(ptPenPos,E_LBUTTON_DOWN);

            //˫���¼����
            m_oDoubleClickChecker.Check(ptPenPos, CDoubleClickChecker::E_MOUSE_STATE_LEFT_DOWN);
            m_eVirutalMouseState = VIRTUAL_MOUSE_STATE_LEFT_DOWN;

            //��λ�һ������,���¿�ʼ�һ����
            m_oRightClickTrigger.Reset();
        }

        break;

    case VIRTUAL_MOUSE_STATE_LEFT_DOWN:
        if(bPenOn)
        {
            if(m_oRightClickTrigger.Process(ptPenPos))
            {
                //m_eVirutalMouseState = VIRTUAL_MOUSE_STATE_RIGHT_CONTROL;
                m_eVirutalMouseState = VIRTUAL_MOUSE_STATE_MOVE;
                
            }
            else
            {
                GenEvent(ptPenPos,E_LBUTTON_DOWN_MOVE);//

                m_oDoubleClickChecker.Check(ptPenPos, CDoubleClickChecker::E_MOUSE_STATE_LEFT_DOWN_MOVE);

                m_ptLastPenAbsolutePos = ptPenPos;
            }
        }
        else
        {
            GenEvent(ptPenPos,E_LBUTTON_UP);//
            m_oDoubleClickChecker.Check(ptPenPos, CDoubleClickChecker::E_MOUSE_STATE_LEFT_UP);
            m_eVirutalMouseState = VIRTUAL_MOUSE_STATE_IDLE;
        }

        break;

    //case VIRTUAL_MOUSE_STATE_RIGHT_CONTROL:
    //    if(bPenOn)
    //    {
    //        if(m_ptLastPenAbsolutePos != ptPenPos)
    //        {
    //            GenEvent(ptPenPos,E_MOUSE_MOVE);
    //            m_ptLastPenAbsolutePos = ptPenPos;
    //        }

    //    }
    //    else
    //    {
    //        //mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, NULL);
    //        m_eVirutalMouseState = VIRTUAL_MOUSE_STATE_IDLE;
    //    }
    //    break;

    case VIRTUAL_MOUSE_STATE_MOVE://ģ�������ƶ�����
        if (bPenOn)
        {
             if(m_ptLastPenAbsolutePos != ptPenPos)
            {
                GenEvent(ptPenPos,E_MOUSE_MOVE);
                m_ptLastPenAbsolutePos = ptPenPos;
            }
        }
        else
        {
            m_eVirutalMouseState = VIRTUAL_MOUSE_STATE_IDLE;
        }
        break;
    }//switch

}


//����:�������������������
//����:bPenOn, ��������־
//     pPos, ���λ��,��Ļ����
//     hDriverDevice, �����豸���
void CVirtualMouse::Input_AirOperate(BOOL bPenOn, const POINT* pPos, EAIROPERATE_CLICKMODE eClickMode, BOOL bForceUseWinAPI)
{
	POINT ptPenPos;
	ptPenPos.x = ptPenPos.y = 0;
	if (pPos)
	{
		ptPenPos.x = pPos->x;
		ptPenPos.y = pPos->y;
	}

	m_bForceUseWinAPI = bForceUseWinAPI;
	//״̬������

    char szBUf[128] = { 0 };
    sprintf(szBUf, " Input_AirOperate m_eVirutalMouseState:%d, bPenOn:%d\n", m_eVirutalMouseState, bPenOn);
    OutputDebugStringA(szBUf);

	switch (m_eVirutalMouseState)
	{
	case VIRTUAL_MOUSE_STATE_IDLE:

		if (bPenOn)
		{
			GenEvent(ptPenPos, E_MOUSE_MOVE);
			m_ptLastPenAbsolutePos = ptPenPos;
			m_eVirutalMouseState = VIRTUAL_MOUSE_STATE_MOVE;
		}
		break;

	case VIRTUAL_MOUSE_STATE_MOVE://ģ�������ƶ�����

		if (bPenOn)
		{
			if (m_ptLastPenAbsolutePos != ptPenPos)
			{
				GenEvent(ptPenPos, E_MOUSE_MOVE);
				m_ptLastPenAbsolutePos = ptPenPos;
			}
		}
		else
		{
			switch(eClickMode)
			{
			   case E_MODE_CLICK:

				    GenEvent(ptPenPos, E_LBUTTON_DOWN);
					GenEvent(ptPenPos, E_LBUTTON_UP);
				  break;

			   case E_MODE_DOUBLE_CLICK:

				   GenEvent(ptPenPos, E_LBUTTON_DOWN);
				   GenEvent(ptPenPos, E_LBUTTON_UP);

				   GenEvent(ptPenPos, E_LBUTTON_DOWN);
				   GenEvent(ptPenPos, E_LBUTTON_UP);

				  break;
			   default:
				   break;

			}
			m_eVirutalMouseState = VIRTUAL_MOUSE_STATE_IDLE;
		}
		break;

	default:
		break;
	}//switch

}

//����:״̬��λ
void CVirtualMouse::Reset()
{
     if(m_hDriverDevice != INVALID_HANDLE_VALUE)
     {   //ʹ������ģ������¼�
         if(m_eVirutalMouseState == VIRTUAL_MOUSE_STATE_LEFT_DOWN)
         {
            int nCxScreen = GetSystemMetrics(SM_CXSCREEN);
            int nCyScreen = GetSystemMetrics(SM_CYSCREEN);

            EASI_MouseInput mouseInput;
            ZeroMemory(&mouseInput, sizeof(mouseInput));

            mouseInput.Buttons = ALL_BTN_UP;
            mouseInput.X = USHORT(EASI_MOUSE_MAXIMUM_X * m_ptLastPenAbsolutePos.x / nCxScreen);
            mouseInput.Y = USHORT(EASI_MOUSE_MAXIMUM_Y * m_ptLastPenAbsolutePos.y / nCyScreen);
            
            EASI_WriteVirtualMouse(
                m_hDriverDevice,
                &mouseInput
                );
         }
     }
     else
     {//ʹ��APIģ������¼�
        
         if(m_eVirutalMouseState == VIRTUAL_MOUSE_STATE_LEFT_DOWN)
         {
             //!ע��!:ʹ��APIģ�����ʱ, ��һ������Ҫʹ��������Ļ����
            int nCxVirtualScreen = GetSystemMetrics(SM_CXVIRTUALSCREEN);
            int nCyVirtualScreen = GetSystemMetrics(SM_CYVIRTUALSCREEN);

            INPUT input;
            memset(&input, 0 ,sizeof(input));
            input.type    = INPUT_MOUSE;

            //��һ����0~65535��Χ��
            input.mi.dx   = m_ptLastPenAbsolutePos.x * 65535/nCxVirtualScreen;
            input.mi.dy   = m_ptLastPenAbsolutePos.y * 65535/nCyVirtualScreen;

            input.mi.dwFlags  = MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_VIRTUALDESK;


            SendInput(1, &input, sizeof(input));
         }
     }


    //��λ״̬
    m_eVirutalMouseState = VIRTUAL_MOUSE_STATE_IDLE;

    m_ptLastPenAbsolutePos.x = 0;
    m_ptLastPenAbsolutePos.y = 0;
}

//@����:����ָ��������¼���
//@����:  pos, ���λ��
//     eEvent, ����¼�ö��
//     nMouseData,  E_VERT_WHEEL, Ϊ�������ƶ���, ��WHEEL_DELTA����������������ʾ��ǰ������������ʾ��������
//                  E_HORZ_WHEEL, Ϊ�������ƶ���, ��WHEEL_DELTA����������������ʾ�ҹ�����������ʾ���������
//
void CVirtualMouse::GenEvent(const POINT& ptMouse, EVirtualMouseEvent eEvent, int nMouseData)
{
    if(m_hDriverDevice != INVALID_HANDLE_VALUE && !m_bForceUseWinAPI)
    {
        int nCxScreen = GetSystemMetrics(SM_CXSCREEN);
        int nCyScreen = GetSystemMetrics(SM_CYSCREEN);

        int nCxVirtualScreen = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int nCyVirtualScreen = GetSystemMetrics(SM_CYVIRTUALSCREEN);
        int nVSLeft          = GetSystemMetrics(SM_XVIRTUALSCREEN);
        int nVSTop           = GetSystemMetrics(SM_YVIRTUALSCREEN);

        //ʹ������ģ������¼�
        BOOL bSuccess = FALSE;
        EASI_MouseInput mouseInput;
        ZeroMemory(&mouseInput, sizeof(mouseInput));

        //mouseInput.X = (USHORT)(EASI_MOUSE_MAXIMUM_X * ptMouse.x / nCxScreen);
        //mouseInput.Y = (USHORT)(EASI_MOUSE_MAXIMUM_Y * ptMouse.y / nCyScreen);

        //win10 1709 (OS�ڲ��汾 16299.125)
        if(IsWin10OrGreater())
        {//win10�µ������������Ĺ�һ��
            mouseInput.X = (USHORT)(EASI_MOUSE_MAXIMUM_X * (ptMouse.x - nVSLeft) / nCxVirtualScreen);
            mouseInput.Y = (USHORT)(EASI_MOUSE_MAXIMUM_Y * (ptMouse.y - nVSTop) / nCyVirtualScreen);
        }
        else 
        {//win7�µ������������Ĺ�һ��
            mouseInput.X = (USHORT)(EASI_MOUSE_MAXIMUM_X * (ptMouse.x - 0) / nCxScreen);
            mouseInput.Y = (USHORT)(EASI_MOUSE_MAXIMUM_Y * (ptMouse.y - 0) / nCyScreen);
        }

        char szBUf[128] = { 0 };
        sprintf(szBUf, " GenEvent mouseInput.X:%d, mouseInput.Y:%d\n", mouseInput.X, mouseInput.Y);
        OutputDebugStringA(szBUf);

        //mouseInput.X += g_tSysCfgData.globalSettings.nXCoordOffset * 5;
        //mouseInput.Y += g_tSysCfgData.globalSettings.nYCoordOffset * 5;

        switch(eEvent)
        {

        case E_LBUTTON_DOWN:
            mouseInput.Buttons = LEFT_BTN_DOWN;
            //AtlTrace(_T("E_LBUTTON_DOWN \n"));
            break;

        case E_LBUTTON_UP:
            mouseInput.Buttons = ALL_BTN_UP;
            //AtlTrace(_T("E_LBUTTON_UP \n"));
            break;

        case E_RBUTTON_DOWN:
            mouseInput.Buttons = RIGHT_BTN_DOWN;
            AtlTrace(_T("E_RBUTTON_DOWN \n"));
            break;

        case E_RBUTTON_UP:
            mouseInput.Buttons = ALL_BTN_UP;
            AtlTrace(_T("E_RBUTTON_UP \n"));
            break;


        case E_MOUSE_MOVE:
            mouseInput.Buttons = ALL_BTN_UP;
            AtlTrace(_T("E_MOUSE_MOVE \n"));
            break;

        case E_LBUTTON_DOWN_MOVE:
            mouseInput.Buttons = LEFT_BTN_DOWN;
            //AtlTrace(_T("E_LBUTTON_DOWN_MOVE \n"));
            break;

        case E_RBUTTON_DOWN_MOVE:
            mouseInput.Buttons = RIGHT_BTN_DOWN;
            AtlTrace(_T("E_RBUTTON_DOWN_MOVE \n"));
            break;

        case E_VERT_WHEEL://��ֱ����
            mouseInput.Wheel = (UCHAR)(nMouseData/WHEEL_DELTA);
            AtlTrace(_T("E_VERT_WHEEL \n"));
            break;

        case E_HORZ_WHEEL://ˮƽ����
            mouseInput.ACPan = (UCHAR)(nMouseData/WHEEL_DELTA);
            AtlTrace(_T("E_HORZ_WHEEL \n"));
            break;
        default:
            AtlTrace(_T("Unkown Mouse Event Type.\n"));

        }//switch

        bSuccess =
            EASI_WriteVirtualMouse(
            m_hDriverDevice,
            &mouseInput
            );

    }
    else
    {   //ʹ��Windows APIģ�����
        //!ע��!:ʹ��APIģ�����ʱ, ��һ������Ҫʹ��������Ļ����
        int nCxVirtualScreen = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int nCyVirtualScreen = GetSystemMetrics(SM_CYVIRTUALSCREEN);
        int nVSLeft          = GetSystemMetrics(SM_XVIRTUALSCREEN);
        int nVSTop           = GetSystemMetrics(SM_YVIRTUALSCREEN);

        INPUT input;
        memset(&input, 0 ,sizeof(input));
        input.type    = INPUT_MOUSE;

        //��һ����0~65535��Χ��
        input.mi.dx   = (ptMouse.x - nVSLeft ) * 65535/nCxVirtualScreen;
        input.mi.dy   = (ptMouse.y - nVSTop) * 65535/nCyVirtualScreen;

        switch(eEvent)
        {

        case E_LBUTTON_DOWN:
            input.mi.dwFlags  = MOUSEEVENTF_MOVE|MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_VIRTUALDESK;

            break;

        case E_LBUTTON_UP:
            input.mi.dwFlags  = MOUSEEVENTF_MOVE|MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_VIRTUALDESK;

            break;

        case E_RBUTTON_DOWN:
            input.mi.dwFlags  = MOUSEEVENTF_MOVE|MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_VIRTUALDESK;

            break;

        case E_RBUTTON_UP:
            input.mi.dwFlags  = MOUSEEVENTF_MOVE|MOUSEEVENTF_RIGHTUP | MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_VIRTUALDESK;
            break;


        case E_MOUSE_MOVE:
        case E_LBUTTON_DOWN_MOVE:
        case E_RBUTTON_DOWN_MOVE:
            input.mi.dwFlags  = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_VIRTUALDESK;
            break;


        case E_VERT_WHEEL://��ֱ����
            input.mi.mouseData = (DWORD)nMouseData;
            input.mi.dwFlags   = MOUSEEVENTF_WHEEL;

            break;

        case E_HORZ_WHEEL://ˮƽ����
            input.mi.mouseData = (DWORD)nMouseData;
            input.mi.dwFlags   = MOUSEEVENTF_HWHEEL;
            break;

        }//switch

        SendInput(1, &input, sizeof(input));
    }
    
}    

