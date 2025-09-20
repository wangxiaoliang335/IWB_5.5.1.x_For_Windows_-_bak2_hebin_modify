#include "stdafx.h"
#include "headers.h"
//#include "ScreenAreaDetectorImpl.h"
//#include "../../inc/Mf2StdGf.h"
//#include "../../inc/MsgDefine.h"
//#include "../../inc/ImageFrameAPI.h"
//#include "../../inc/ImageDib.h"
//#include "ImageDebug.h"
//#include "GlobalVar.h"

CScreenAreaDetectorImpl::CScreenAreaDetectorImpl()
:
m_eStage(E_STAGE_INITIAL),
m_hBoardWnd(NULL),
m_szClassName(_T("ScreenDetectorBoardWindow")),
m_nStageCount(0)
{

	WNDCLASSEX wndclass;
	wndclass.cbSize = sizeof wndclass;
	wndclass.style = 0;
	wndclass.lpfnWndProc = MF2GF<WNDPROC>(&CScreenAreaDetectorImpl::WndProc, this);
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	m_hInst = wndclass.hInstance = _AtlBaseModule.m_hInst;
	wndclass.hIcon = 0;
	wndclass.hCursor = 0;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName = 0;
	wndclass.lpszClassName = m_szClassName;
	wndclass.hIconSm = 0;
	ATOM  atom = RegisterClassEx(&wndclass);
}


CScreenAreaDetectorImpl::~CScreenAreaDetectorImpl()
{
	if(m_hBoardWnd != NULL)
	{
		DestroyWindow(m_hBoardWnd);
		m_hBoardWnd  = NULL;
	}
}

//@�� ��:�������,ͬʱ��ʾȫ������
//@�� ��:hNotifyWnd, ����������֪ͨ�Ĵ�����
//@����ֵ;	
BOOL CScreenAreaDetectorImpl::StartDetect(HWND hNotifyWnd)
{
	m_hNotifyWnd = hNotifyWnd;
	if(m_hBoardWnd != NULL)
	{
		DestroyWindow(m_hBoardWnd);
		m_hBoardWnd  = NULL;
	}

    //����ͼƬ���Ŀ¼
    CString strImageDubugDir;
        SYSTEMTIME now;
	    GetLocalTime(&now);
	    strImageDubugDir.Format(
	    _T("%s\\AutoMask\\%04d_%02d_%02d_%02d_%02d_%02d"),
        g_strIntermediatRootDirName,
	    now.wYear,
	    now.wMonth,
	    now.wDay,
	    now.wHour,
	    now.wMinute,
	    now.wSecond);
      
     SetImageDebugDir(strImageDubugDir);

	(CreateWindow)();
	m_eStage = E_STAGE_INITIAL;
	return TRUE;
}



//@��  ��:������⹦��,
//@��  ��:
//@����ָ:
BOOL CScreenAreaDetectorImpl::End()
{
	if(m_hBoardWnd != NULL)
	{
		DestroyWindow(m_hBoardWnd);
		m_hBoardWnd  = NULL;
	}

	m_eStage = E_STAGE_END;
	return TRUE;
}

//@����:������Ļ��������λͼ
const CImageFrame& CScreenAreaDetectorImpl::GetMaskFrame()const
{
	return m_oMaskFrame;
}

//@����:�µ�ͼƬ֡�¼���Ӧ
void CScreenAreaDetectorImpl::OnNewFrame(CImageFrame* pFrame)
{

	//״̬��ģ��
	switch(m_eStage)
	{

	case E_STAGE_INITIAL:
		m_nStageCount = 0;
		m_cBrightness = 0;
		m_eStage = E_STAGE_WHITE_FRAME;//ת����Ұ����׶�
		break;


	case E_STAGE_WHITE_FRAME:
        m_nStageCount ++;
		if(m_nStageCount == 5)//ÿ�εȴ�10֡
		{
            if(m_cBrightness  != 0xFF)
            {
                m_cBrightness += 0x0F;//ע��255�Ĺ�������17, 3, 5, 15
    			
			    HDC hDC = GetDC(m_hBoardWnd);
			    HBRUSH brush = ::CreateSolidBrush(RGB(m_cBrightness,m_cBrightness,m_cBrightness));
    			
			    RECT rect;
			    GetClientRect(m_hBoardWnd, &rect);
			    FillRect(hDC, &rect, brush);
			    ReleaseDC(m_hBoardWnd, hDC);
			    DeleteObject(brush);
            }
            else
            {
                m_oWiteFrame = *pFrame;
			    m_eStage = E_STAGE_BLACK_FRAME;

            }

			m_nStageCount = 0;
		}
	
		break;


	case E_STAGE_BLACK_FRAME:
        m_nStageCount ++;
		if(m_nStageCount == 5)
		{
               if(m_cBrightness != 0)
               {
                    m_cBrightness -= 0x0F;//ע��255�Ĺ�������17, 3, 5, 15
        			
			        HDC hDC = GetDC(m_hBoardWnd);
			        HBRUSH brush = ::CreateSolidBrush(RGB(m_cBrightness,m_cBrightness,m_cBrightness));
        			
			        RECT rect;
			        GetClientRect(m_hBoardWnd, &rect);
			        FillRect(hDC, &rect, brush);
			        ReleaseDC(m_hBoardWnd, hDC);
			        DeleteObject(brush);
               }
               else
               {
                   //�洢����
			       m_oBlackFrame = *pFrame;
                   m_eStage = E_STAGE_CALC;
               }

               m_nStageCount = 0;
		
		}

		break;

	case E_STAGE_CALC:

		//if(m_nStageCount == S_WAITCOUNT)
		{
			//��������ͼ
			//CImageFrame blackFrame;
			//CImageFrame whiteFrame;
			//Covert2MonochromeFrame_Y(m_oBlackFrame, blackFrame);
			//Covert2MonochromeFrame_Y(m_oWiteFrame, whiteFrame);
			
			CalcMaskFrame(m_oWiteFrame, m_oBlackFrame, this->m_oMaskFrame);
			Debug_SaveImageFrame(m_oWiteFrame  , _T("ScreenAutoDetectorWhite.jpg"));
			Debug_SaveImageFrame(m_oBlackFrame  , _T("ScreenAutoDetectorBlack.jpg"));
            Debug_SaveImageFrame(m_oMaskFrame, _T("ScreenAutoDetectorMask.jpg" ));


			BOOL bSuccess = ExtractTheLargestObject(this->m_oMaskFrame);

			if(bSuccess)
			{
				FillHole(m_oMaskFrame);
			}
			//�������ͼ
			Debug_SaveImageFrame(m_oMaskFrame, _T("ScreenAutoDetectorMaskFrame.jpg"));
		
			ShowWindow(m_hBoardWnd, SW_HIDE);

			//֪ͨע������Ļ�����������
			PostMessage(m_hNotifyWnd, WM_SEARCH_SCREEN_AREA_DONE, 0, (LPARAM)bSuccess);
			m_eStage = E_STAGE_END;
		}
		//else
		//{
		//	m_nStageCount++;
		//}

		break;

	case E_STAGE_END:
		
		break;

	}//switch
}

//@����:�жϰװ崰���Ƿ�ɼ�
BOOL CScreenAreaDetectorImpl::IsBoardWindowVisible()
{
	return ::IsWindowVisible(m_hBoardWnd);
}


HRESULT CScreenAreaDetectorImpl::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{

	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		break;



	case WM_KEYUP:
		{
			switch(wParam)
			{
			case VK_ESCAPE:
				if(IsBoardWindowVisible())
				{
					ShowWindow(hWnd, SW_HIDE);

					m_eStage = E_STAGE_END;

					PostMessage(hWnd, WM_SEARCH_SCREEN_AREA_DONE, 0, LPARAM(0));

				}
				break;
			}

			//AtlTrace(_T("virtual key code=0x%x\r\n"), wParam); 

		}

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};


	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


//�װ崰����غ���
BOOL (CScreenAreaDetectorImpl::CreateWindow)()
{
	BOOL  bRet = TRUE;
	DWORD dwErr = 0;

	m_hBoardWnd = ::CreateWindowEx(
		WS_EX_TOOLWINDOW,//dwExStyle,
		m_szClassName,
		m_szClassName,//windows name
		WS_POPUP,
		0,0,0,0,
		NULL,
		0,//Menu ID
		m_hInst,
		0);

	if(m_hBoardWnd == NULL)
	{
		dwErr = GetLastError();
	}

	int nCxScreen = GetSystemMetrics(SM_CXSCREEN);
	int nCyScreen = GetSystemMetrics(SM_CYSCREEN);
	RECT rcPosition;
	rcPosition.left   = 0;
	rcPosition.top    = 0;
	rcPosition.right  = nCxScreen;
	rcPosition.bottom = nCyScreen;	

	SetWindowPos(
		m_hBoardWnd,
		HWND_TOPMOST,//HWND_TOP, 
		rcPosition.left,
		rcPosition.top,
		rcPosition.right,
		rcPosition.bottom,
		SWP_SHOWWINDOW);

	::SetFocus(m_hBoardWnd);//����������뽹��

	return bRet;
}

