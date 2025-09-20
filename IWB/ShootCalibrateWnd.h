#pragma once

#define IDC_STATIC_TEXT 10030
#define IDC_MYBUTTON    10010

//@类名, 自动校正窗口
class CShootCalibrateWnd
{
public:
    CShootCalibrateWnd(/*IAutoCalibratorInterface* pCalibrateInterface*/   //光斑检测对象。
        CPenPosDetector  *oPenPosDetector)
        :
    //m_ptrAutoCalibrateInterface(pCalibrateInterface),
        m_szClassName(_T("OpticalPenAutoCalibrateWindow")),
        m_hWnd(NULL),
        m_hInst(NULL),
        m_bTimeout(TRUE),
		m_liLastTickCount(0),
        m_oPenPosDetector(oPenPosDetector),
        m_hButtonBrush(NULL)
    {
        WNDCLASSEX wndclass;
        wndclass.cbSize = sizeof wndclass;
        wndclass.style = 0;
        wndclass.lpfnWndProc = WndProc;
        wndclass.cbClsExtra = 0;
        wndclass.cbWndExtra = 0;
        m_hInst = wndclass.hInstance = _AtlBaseModule.m_hInst;
        wndclass.hIcon = 0;
        wndclass.hCursor = 0;
        wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wndclass.lpszMenuName = 0;
        wndclass.lpszClassName = m_szClassName;
        wndclass.hIconSm = 0;
        ATOM atom = RegisterClassEx(&wndclass);

        // 创建用于按钮背景的画刷
        m_hButtonBrush = CreateSolidBrush(RGB(255, 255, 0)); // 这里设置为红色
    }

    ~CShootCalibrateWnd()
    {
        FreeThunk(&CShootCalibrateWnd::InternalWndProc, this);
        UnregisterClass(m_szClassName, m_hInst);

        // 释放画刷资源
        if (m_hButtonBrush)
        {
            DeleteObject(m_hButtonBrush);
        }
    }

    inline void FillBoard(HWND hWnd, COLORREF clr, const RECT* pRect = NULL)
    {
        HDC hDC = ::GetDC(hWnd);

        HBRUSH hBrush = ::CreateSolidBrush(clr);//


        RECT rcClient;

        if (pRect)
        {
            rcClient = *pRect;
            LPPOINT pPoint = (LPPOINT)&rcClient;
            ScreenToClient(hWnd, pPoint);
            ScreenToClient(hWnd, pPoint + 1);

        }
        else
        {
            ::GetClientRect(hWnd, &rcClient);
        }


        BOOL bRet = FillRect(hDC, &rcClient, hBrush);
        ::DeleteObject(hBrush);
        ReleaseDC(hWnd, hDC);
    }

    void FillCircle(HWND hWnd, COLORREF clr, const RECT* pRect = NULL)
    {
        HDC hDC = ::GetDC(hWnd);
        HBRUSH hBrush = ::CreateSolidBrush(clr);//
        RECT rcClient;
        if (pRect)
        {
            rcClient = *pRect;
            LPPOINT pPoint = (LPPOINT)&rcClient;
            ScreenToClient(hWnd, pPoint);
            ScreenToClient(hWnd, pPoint + 1);

        }
        else
        {
            ::GetClientRect(hWnd, &rcClient);
        }
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);
        BOOL bRet = Ellipse(hDC, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);
        SelectObject(hDC, hOldBrush);
        ::DeleteObject(hBrush);
        ReleaseDC(hWnd, hDC);
    }

    static HRESULT _stdcall WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        return DefWindowProc(hWnd, uMsg, wParam, lParam);   
    }

    HRESULT InternalWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch(uMsg)
        {

        case WM_PAINT:
            {
                PAINTSTRUCT ps;
                BeginPaint(hWnd, &ps);
                RECT rcVirtualScreen;
                rcVirtualScreen.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
                rcVirtualScreen.top = GetSystemMetrics(SM_YVIRTUALSCREEN);

                rcVirtualScreen.right = rcVirtualScreen.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
                rcVirtualScreen.bottom = rcVirtualScreen.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);

                RECT rcCircle, rcSubCircle;
                rcCircle.left = (rcVirtualScreen.right - rcVirtualScreen.left) / 2 - 100;
                rcCircle.top = (rcVirtualScreen.bottom - rcVirtualScreen.top) / 2 - 100;
                rcCircle.right = (rcVirtualScreen.right - rcVirtualScreen.left) / 2 + 100;
                rcCircle.bottom = (rcVirtualScreen.bottom - rcVirtualScreen.top) / 2 + 100;

                rcSubCircle.left = (rcVirtualScreen.right - rcVirtualScreen.left) / 2 - 15;
                rcSubCircle.top = (rcVirtualScreen.bottom - rcVirtualScreen.top) / 2 - 15;
                rcSubCircle.right = (rcVirtualScreen.right - rcVirtualScreen.left) / 2 + 15;
                rcSubCircle.bottom = (rcVirtualScreen.bottom - rcVirtualScreen.top) / 2 + 15;
                FillBoard(hWnd, RGB(0, 0, 0), &rcVirtualScreen);

                FillCircle(hWnd, RGB(255, 0, 0), &rcCircle);

                OutputDebugString(_T(" xxxxx WM_PAINT\n"));

                FillCircle(hWnd, RGB(255, 255, 255), &rcSubCircle);
                EndPaint(hWnd, &ps);
            }
            break;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDC_MYBUTTON)  // ID 为 1 的按钮被点击
            {
                if (IsVisible())
                {
                    if (m_oPenPosDetector)
                    {
                        m_oPenPosDetector->LeaveShootCalibrateMode();
                    }
                    this->ShowWindow(SW_HIDE);
                }
            }
            break;

        case WM_CREATE:
            {

            RECT rcVirtualScreen;
            rcVirtualScreen.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
            rcVirtualScreen.top = GetSystemMetrics(SM_YVIRTUALSCREEN);

            rcVirtualScreen.right = rcVirtualScreen.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
            rcVirtualScreen.bottom = rcVirtualScreen.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);

            RECT rcStatic;
            rcStatic.left = (rcVirtualScreen.right - rcVirtualScreen.left) / 2 - 40;
            rcStatic.top = (rcVirtualScreen.bottom - rcVirtualScreen.top) *5/ 6 - 40;
            rcStatic.right = (rcVirtualScreen.right - rcVirtualScreen.left) / 2 + 40;
            rcStatic.bottom = (rcVirtualScreen.bottom - rcVirtualScreen.top) *5/ 6 + 40;

            hStatic = CreateWindow(
                L"STATIC", L"请输入内容：",
                WS_CHILD | WS_VISIBLE,
                rcStatic.left, rcStatic.top, 80, 40,
                hWnd, (HMENU)IDC_STATIC_TEXT,
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );

            ::ShowWindow(hStatic, SW_SHOW);

            OutputDebugString(_T(" xxxxx WM_CREATE\n"));

            hButton = CreateWindow(
                L"BUTTON", L"显示内容",
                WS_CHILD | WS_VISIBLE,
                rcStatic.left+90, rcStatic.top+100, 40, 20,
                hWnd, (HMENU)IDC_MYBUTTON,
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );
            ::ShowWindow(hButton, SW_SHOW);

            hShiftButton = CreateWindow(
                L"BUTTON", L"射击偏移清零",
                WS_CHILD | WS_VISIBLE,
                rcVirtualScreen.right - 120, rcVirtualScreen.bottom - 100, 40, 20,
                hWnd, (HMENU)IDC_MYBUTTON,
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );
            ::ShowWindow(hShiftButton, SW_SHOW);

            }
            break;

        case WM_CTLCOLORBTN:
        {
            HDC hdcButton = (HDC)wParam;
            HWND hButton = (HWND)lParam;

            // 设置按钮文本颜色
            SetTextColor(hdcButton, RGB(255, 255, 255)); // 白色文本
            SetBkMode(hdcButton, TRANSPARENT);

            // 返回按钮背景画刷
            return (LRESULT)m_hButtonBrush;
        }
        break;

        case WM_TIP_TEXT:
        {
            wchar_t* pText = (wchar_t*)wParam;
            SetWindowText(hStatic, pText);
            delete[] pText;
        }
        break;

        case WM_AUTO_CALIBRATE_DATA:
            {
				m_liLastTickCount = GetTickCount64();
                m_bTimeout = FALSE;
                //CYUY2Frame* pFrame = reinterpret_cast<CYUY2Frame*>(lParam);
                CImageFrame* pFrame = reinterpret_cast<CImageFrame*>(lParam);

                //AtlTrace(_T("WM_AUTO_CALIBRATE_DATA.\r\n"));
                //if(m_ptrAutoCalibrateInterface)
                //{
                //    m_ptrAutoCalibrateInterface->FeedImage(pFrame);
                //}


            }
            break;

        case WM_KEYUP:
            {
                switch(wParam)
                {
                case VK_ESCAPE:
                    if(IsVisible())
                    {
                        if (m_oPenPosDetector)
                        {
                            m_oPenPosDetector->LeaveShootCalibrateMode();
                        }
                        //Sleep(5000);
                        this->ShowWindow(SW_HIDE);
                        //if(m_ptrAutoCalibrateInterface)
                        //{
                        //    m_ptrAutoCalibrateInterface->Finish();
                        //}
                    }
                    break;
                }

                //AtlTrace(_T("virtual key code=0x%x\r\n"), wParam);

            }

        case WM_TIMER:
            if(m_bTimeout)
            {
                //if(m_ptrAutoCalibrateInterface)
                //{
                //    m_ptrAutoCalibrateInterface->Finish();
                //}
            }
            else
            {
				LONGLONG liNow = GetTickCount64();
				LONGLONG liDelta = liNow - m_liLastTickCount;
				if ((int)liDelta > TIMEOUT_CONSTANT)
				{
                	m_bTimeout = TRUE;//进入下一次超时判断
				}
            }

            break;

        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        };

        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    BOOL (CreateWindow)(const RECT& rcPosition,BOOL bShow = TRUE)
    {
        BOOL bRet = TRUE;
        DWORD dwErr = 0;

        m_hWnd = ::CreateWindowEx(
            WS_EX_TOOLWINDOW | WS_EX_TOPMOST,//dwExStyle,
            m_szClassName,
            m_szClassName,//windows name
            WS_POPUP,
            0,0,0,0,
            NULL,
            0,//Menu ID
            m_hInst,
            0);

        if(m_hWnd == NULL)
        {

            dwErr = GetLastError();
        }


        //int nCxScreen = GetSystemMetrics(SM_CXSCREEN);
        //int nCyScreen = GetSystemMetrics(SM_CYSCREEN);
        //RECT rcPosition;
        //rcPosition.left = 0;
        //rcPosition.top = 0;
        //rcPosition.right = nCxScreen;
        //rcPosition.bottom = nCyScreen;

        SetWindowPos(
            m_hWnd,
            HWND_TOPMOST,
            //HWND_TOP,
            rcPosition.left,
            rcPosition.top,
            rcPosition.right  - rcPosition.left,
            rcPosition.bottom - rcPosition.top,
            bShow?SWP_SHOWWINDOW:SWP_HIDEWINDOW);
        

        SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)MF2GF<WNDPROC>(&CShootCalibrateWnd::InternalWndProc, this));

		m_rcPositionWnd = rcPosition;

        /************************************************/
        RECT rcVirtualScreen;
        rcVirtualScreen.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
        rcVirtualScreen.top = GetSystemMetrics(SM_YVIRTUALSCREEN);

        rcVirtualScreen.right = rcVirtualScreen.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
        rcVirtualScreen.bottom = rcVirtualScreen.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);

        RECT rcStatic;
        rcStatic.left = (rcVirtualScreen.right - rcVirtualScreen.left) / 2 - 40;
        rcStatic.top = (rcVirtualScreen.bottom - rcVirtualScreen.top) * 5 / 6 - 40;
        rcStatic.right = (rcVirtualScreen.right - rcVirtualScreen.left) / 2 + 40;
        rcStatic.bottom = (rcVirtualScreen.bottom - rcVirtualScreen.top) * 5 / 6 + 40;

        //hStatic = CreateWindow(
        //    L"STATIC", L"请输入内容：",
        //    WS_CHILD | WS_VISIBLE,
        //    rcStatic.left, rcStatic.top, 80, 40,
        //    m_hWnd, (HMENU)IDC_STATIC_TEXT,
        //    (HINSTANCE)GetWindowLongPtr(m_hWnd, GWLP_HINSTANCE),
        //    NULL
        //);

        //::ShowWindow(hStatic, SW_SHOW);

        hButton = CreateWindow(
            L"BUTTON", L"关闭",
            WS_CHILD | WS_VISIBLE,
            rcVirtualScreen.right - 350, rcVirtualScreen.top + 120, 200, 60,
            m_hWnd, (HMENU)IDC_MYBUTTON,
            (HINSTANCE)GetWindowLongPtr(m_hWnd, GWLP_HINSTANCE),
            NULL
        );
        ::ShowWindow(hButton, SW_SHOW);
        //m_closeRect = RECT(rcVirtualScreen.right - 150, rcVirtualScreen.top + 20, rcVirtualScreen.right - 150 + 100, rcVirtualScreen.top + 20 + 30);
        m_closeRect.left = rcVirtualScreen.right - 350;
        m_closeRect.top = rcVirtualScreen.top + 120;
        m_closeRect.right = rcVirtualScreen.right - 350 + 200;
        m_closeRect.bottom = rcVirtualScreen.top + 120 + 60;
        /************************************************/

        return bRet;
    };

    CRect GetCloseBtnRect()
    {
        return m_closeRect;
    }

    operator HWND() const
    {

        return m_hWnd;
    }

    HWND GetStaticHwnd()
    {
        return hStatic;
    }

    HWND GetButtonHwnd()
    {
        return hButton;
    }

    HWND SetFocus()
    {
        if(m_hWnd)
        {
            return ::SetFocus(m_hWnd);
        }

        return NULL;
    }

    BOOL IsVisible()
    {
        return ::IsWindowVisible(m_hWnd);
    }

    BOOL ShowWindow(int nCmdShow)
    {
        return ::ShowWindow(m_hWnd, nCmdShow);
    }

    BOOL DestroyWindow()
    {
        BOOL bRet = ::DestroyWindow(m_hWnd);
        m_hWnd = NULL;
        return bRet;

    }

    void ResizeToFullScreen()
    {
        int nCxScreen = GetSystemMetrics(SM_CXSCREEN);
        int nCyScreen = GetSystemMetrics(SM_CYSCREEN);
        RECT rcPosition;
        rcPosition.left = 0;
        rcPosition.top = 0;
        rcPosition.right = nCxScreen;
        rcPosition.bottom = nCyScreen;

        SetWindowPos(
            m_hWnd,
            HWND_TOPMOST,
            //HWND_TOP,
            rcPosition.left,
            rcPosition.top,
            rcPosition.right  - rcPosition.left,
            rcPosition.bottom - rcPosition.top,
            SWP_SHOWWINDOW);
    }


    void MoveWindow(const RECT& rcPosition)
    {
        SetWindowPos(
            m_hWnd,
            HWND_TOPMOST,
            //HWND_TOP,
            rcPosition.left,
            rcPosition.top,
            rcPosition.right  - rcPosition.left,
            rcPosition.bottom - rcPosition.top,
            SWP_SHOWWINDOW);
    }

    //@参数:开启超时检测
    void StartTimeoutDetect()
    {
        if(m_uTimerID)
        {
            KillTimer(m_hWnd, 1);
            m_uTimerID = 0;
        }

        m_uTimerID = SetTimer(m_hWnd, 1, TIMEOUT_CONSTANT, NULL);
        m_bTimeout = FALSE;
		m_liLastTickCount = GetTickCount64();
    }

    void CloseTimeoutDetect()
    {
        if(m_uTimerID)
        {
            KillTimer(m_hWnd, 1);
            m_uTimerID = 0;
            m_bTimeout = FALSE;
        }

    }

	BOOL GetWindowsRect(RECT& rcMonitor)
	{
		rcMonitor = m_rcPositionWnd;
		return TRUE;
	}


protected:
    HWND m_hWnd;
    HWND hStatic;
    HWND hButton;
    HWND hShiftButton;
    HINSTANCE m_hInst;
    const TCHAR* m_szClassName;
    //IAutoCalibratorInterface* m_ptrAutoCalibrateInterface;

	static const INT TIMEOUT_CONSTANT = 1000;
    UINT m_uTimerID;//
    BOOL m_bTimeout;//超时标志
	LONGLONG m_liLastTickCount;

    CPenPosDetector* m_oPenPosDetector = NULL;
	RECT m_rcPositionWnd;
    CRect m_closeRect;
    HBRUSH m_hButtonBrush; // 用于按钮背景的画刷
};