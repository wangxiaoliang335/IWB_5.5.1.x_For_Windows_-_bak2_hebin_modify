#pragma once

//˵��:��Ƶ�м�⵽�Ĺ�����굽��Ļ�����ת����
enum EVirtualMouseEvent
{
    E_LBUTTON_DOWN,
    E_LBUTTON_UP,
    E_RBUTTON_DOWN,
    E_RBUTTON_UP,
    E_MOUSE_MOVE,
    E_LBUTTON_DOWN_MOVE,
    E_RBUTTON_DOWN_MOVE,
    E_VERT_WHEEL,//��ֱ����
    E_HORZ_WHEEL //ˮƽ����
};

class IVirtualMouse
{
public:
    //@����:����ָ��������¼���
    //@����:  pos, ���λ��
    //     eEvent, ����¼�ö��
    //     nMouseData,  E_VERT_WHEEL, Ϊ�������ƶ���, ��WHEEL_DELTA����������������ʾ��ǰ������������ʾ��������
    //                  E_HORZ_WHEEL, Ϊ�������ƶ���, ��WHEEL_DELTA����������������ʾ�ҹ�����������ʾ���������
    //    
    virtual void GenEvent(const POINT& ptMouse, EVirtualMouseEvent eEvent, int nMouseData=0) = 0;
};
//˫���¼������
class CDoubleClickChecker
{
public:
    CDoubleClickChecker(IVirtualMouse* pVirtualMouse)
        :
    m_pVirtualMouse(pVirtualMouse),
    m_eCheckerState(E_DOUBLE_CLICK_CHECKER_STATE_IDLE)
    {
        m_ptFirstLeftDownPos.x = 0;
        m_ptFirstLeftDownPos.y = 0;
        m_pFristLeftDownTime   = 0;

        int nInitPhysicalDimension = int(DEFAULT_SCREEN_DIAGONAL_LENGTH);//80 inches, ��λ:mm
        int nCx = GetSystemMetrics(SM_CXSCREEN);
        int nCy = GetSystemMetrics(SM_CYSCREEN);

        SIZE screenSize;
        screenSize.cx = nCx;
        screenSize.cy = nCy;
        OnSetTouchScreenDimension(nInitPhysicalDimension, screenSize);

        m_nDblClickTime = 500;//����

        //��ȡϵͳ����
        m_nSysDblClickTime = GetDoubleClickTime();
        
        m_nSysCxDblClick = ::GetSystemMetrics(SM_CXDOUBLECLK);
        m_nSysCyDblClick = ::GetSystemMetrics(SM_CYDOUBLECLK);

    }

    ~CDoubleClickChecker()
    {
        //SetEvent(m_hExitEvent);
        //WaitForSingleObject(m_hProcessThread, INFINITE);
    }

    enum EMouseState
    {
        E_MOUSE_STATE_LEFT_DOWN       = 0,//����������
        E_MOUSE_STATE_LEFT_UP         = 1,//����������
        E_MOUSE_STATE_LEFT_DOWN_MOVE  = 2,//�����������ƶ�
    };
    //@����:�������״̬�������У�����˫���¼�
    //@����:ptLeftDownPos������������ʱ��λ��
    //      eMouseState�� ���״̬
    //      
    //      
    void Check(const POINT& ptLeftDownPos, EMouseState eMouseState)
    {
        DWORD dwNow = GetTickCount();//����ʱ��
        LONG     dx = 0,dy = 0      ;//
        DWORD    dT = 0             ;//

        //<<debug
        //static int s_nDebug = 0;
        //AtlTrace(_T("Check[%03d]input x=%d,y=%d, mouse state=%d\n"), s_nDebug, ptLeftDownPos.x, ptLeftDownPos.y,eMouseState);
        //>>


        switch(m_eCheckerState)
        {
        case E_DOUBLE_CLICK_CHECKER_STATE_IDLE:
            if(E_MOUSE_STATE_LEFT_DOWN == eMouseState)
            {
                m_ptFirstLeftDownPos = ptLeftDownPos;
                m_pFristLeftDownTime = dwNow;
                m_eCheckerState     = E_DOUBLE_CLICK_CHECKER_STATE_FIRST_DOWN;

                //AtlTrace(_T("Check[%03d]idle --> first down\n"), s_nDebug);
            }
            
            break;

        case E_DOUBLE_CLICK_CHECKER_STATE_FIRST_DOWN://��һ�ΰ���״̬
            dx =  abs(ptLeftDownPos.x - m_ptFirstLeftDownPos.x);
            dy  = abs(ptLeftDownPos.y - m_ptFirstLeftDownPos.y);
            
            //���һ�ΰ���ʱ��ʱ���
            dT = dwNow - m_pFristLeftDownTime;

            if((int)dT > m_nDblClickTime)
            {//��ʱ
                if(E_MOUSE_STATE_LEFT_DOWN == eMouseState)
                {
                    m_ptFirstLeftDownPos = ptLeftDownPos;
                    m_pFristLeftDownTime = dwNow;

                }
                
            }
            else
            {//δ��ʱ
                if(dx > m_nDblClickLogicalWidth
                    ||
                   dy > m_nDblClickLogicalHeight)
                {//�ƶ���Χ�����ռ��ݴ�Χ, ������ص���ʼ״̬
                    m_eCheckerState = E_DOUBLE_CLICK_CHECKER_STATE_IDLE;
                   
                    //AtlTrace(_T("Check[%03d]first down --> idle for out of range\n"), s_nDebug);
                }
                else
                {
                    if(E_MOUSE_STATE_LEFT_UP == eMouseState)
                    {
                        m_eCheckerState = E_DOUBLE_CLICK_CHECKER_STATE_FIRST_UP;

                        //AtlTrace(_T("Check[%03d]first down --> first up\n"), s_nDebug);
                    }
                    else if(E_MOUSE_STATE_LEFT_DOWN_MOVE != eMouseState && E_MOUSE_STATE_LEFT_DOWN != eMouseState)
                    {
                        m_eCheckerState = E_DOUBLE_CLICK_CHECKER_STATE_IDLE;

                        //AtlTrace(_T("Check[%03d]first down --> idlen for unexpected events\n"), s_nDebug);
                    }
                }
            }

            break;


        case E_DOUBLE_CLICK_CHECKER_STATE_FIRST_UP://��һ�ε���״̬

            dx =  abs(ptLeftDownPos.x - m_ptFirstLeftDownPos.x);
            dy  = abs(ptLeftDownPos.y - m_ptFirstLeftDownPos.y);

            dT = dwNow - m_pFristLeftDownTime;

            if((int)dT > m_nDblClickTime)
            {//��ʱ
                if(E_MOUSE_STATE_LEFT_DOWN == eMouseState)
                {
                    m_ptFirstLeftDownPos = ptLeftDownPos;
                    m_pFristLeftDownTime = dwNow;
                    m_eCheckerState = E_DOUBLE_CLICK_CHECKER_STATE_FIRST_DOWN;//ת���һ�ΰ���״̬

                     //AtlTrace(_T("Check[%03d]first up -->firt down for timeout\n"), s_nDebug);
                }
                
            }
            else
            {//δ��ʱ
                if(dx > m_nDblClickLogicalWidth
                    ||
                   dy > m_nDblClickLogicalHeight)
                {//�ƶ���Χ�����ռ��ݴ�Χ, ������ص���ʼ״̬
                    m_eCheckerState = E_DOUBLE_CLICK_CHECKER_STATE_IDLE;

                    //AtlTrace(_T("Check[%03d]first up --> idle for out of range\n"), s_nDebug);
                   
                }
                else
                {
                    if(E_MOUSE_STATE_LEFT_DOWN ==  eMouseState)
                    {
                        //���������ε�����ģ��˫���¼�
                        m_pVirtualMouse->GenEvent(ptLeftDownPos, E_LBUTTON_UP);
                        m_pVirtualMouse->GenEvent(ptLeftDownPos, E_LBUTTON_DOWN);

                        // AtlTrace(_T("Check[%03d]Trigger Double Click\n"), s_nDebug);
                      }
    
                    m_eCheckerState = E_DOUBLE_CLICK_CHECKER_STATE_IDLE;

                   //AtlTrace(_T("Check[%03d]first up --> idle\n"), s_nDebug); 


                }//else

            }//else

            break;

        }//switch 


        //<<debug
        //s_nDebug ++;
        //debug>>

    };



    //@����:���ô����ĳߴ��¼���Ӧ����
    //@����:nPhysicalDiagonalLength, ��Ļ�ߴ�,�Խ��߳���, ��λ:����
    //      szLogicalDimension, ��Ļ�߼��ߴ磬��λ:����
    void OnSetTouchScreenDimension(int  nPhysicalDiagonalLength, SIZE szLogicalDimension)
    {
        int  nCxScreen = szLogicalDimension.cx;
        int  nCyScreen = szLogicalDimension.cy;

        int  nLogicalDiagonalLength = (int)sqrt(double(nCxScreen *  nCxScreen + nCyScreen * nCyScreen));

        m_nDblClickLogicalWidth  = nLogicalDiagonalLength * m_nDlbClickPhysicalWidth / nPhysicalDiagonalLength;
        m_nDblClickLogicalHeight = nLogicalDiagonalLength * m_nDlbClickPhysicalHeight / nPhysicalDiagonalLength;

    }

protected:

    //˫�������״̬
    enum E_DoubleClickCheckerState
    {
        E_DOUBLE_CLICK_CHECKER_STATE_IDLE       ,//˫�����������״̬
        E_DOUBLE_CLICK_CHECKER_STATE_FIRST_DOWN ,//��һ�ΰ���״̬
        E_DOUBLE_CLICK_CHECKER_STATE_FIRST_UP   ,//�ڶ��ε���״̬
    }m_eCheckerState;

    POINT m_ptFirstLeftDownPos ;//���ε����еĵ�һ���������λ��(��Ļ����)��
    DWORD m_pFristLeftDownTime ;//���ε����еĵ�һ���������ʱ�̡�


    //˫���¼���������
    //ʱ������    
    int  m_nDblClickTime;

    //�ռ�����    
    int  m_nDblClickLogicalWidth ;//����˫���¼����ڶ��ε����������ڵľ�������Χ��ˮƽ���,��λ:����
    int  m_nDblClickLogicalHeight;//����˫���¼����ڶ��ε����������ڵľ�������Χ�Ĵ�ֱ�߶�,��λ:����

    static const int m_nDlbClickPhysicalWidth  = 10 ;//����˫���¼����ڶ��ε����������ڵľ�������Χ��ˮƽ���,��λ:����
    static const int m_nDlbClickPhysicalHeight = 10 ;//����˫���¼����ڶ��ε����������ڵľ�������Χ�Ĵ�ֱ�߶�,��λ:����

    //ϵͳ����˫���Ŀռ�����
    int  m_nSysCxDblClick;//��λ:����
    int  m_nSysCyDblClick;//��λ:����

    //ϵͳ���õ�˫����ʱ������,��λ:����
    int  m_nSysDblClickTime;


    IVirtualMouse* m_pVirtualMouse;

};


//�Ҽ��¼�������
class CRightClickTrigger
{
public:
    CRightClickTrigger(IVirtualMouse* pVirtualMouse)
        :
    m_pVirtualMouse(pVirtualMouse)
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
                m_pVirtualMouse->GenEvent(ptNow, E_RBUTTON_DOWN);
                m_pVirtualMouse->GenEvent(ptNow, E_RBUTTON_UP);
                return TRUE;
            }


        }

        return FALSE;
    }

protected:
    POINT m_ptRangeCenter;
    static const DWORD MOUSE_RIGHT_CLICK_RANGE = 3;//�ڼ���Ҽ������¼�ʱҪ�����ͣ��������İ뾶��
    //������ڸ�����Χ�ڳ���һ��ʱ�伴��Ϊ�ɴ����Ҽ�
    //�����¼�
    DWORD m_dwRangeUpdateTime;
    static const DWORD MOUSE_RIGHTDOWN_EVENT_TRIGGER_TIMER = 1500;//����Ҽ������¼�������ʱ��
    IVirtualMouse* m_pVirtualMouse;
};



class CVirtualMouse:public IVirtualMouse
{
public:

    CVirtualMouse();

    //����:�������������������
    //����:bPenOn, ��������־
    //     ptMouse, ���λ��,��Ļ����

    void Input(BOOL bPenOn, const POINT* ptMouse, BOOL bUseWinAPI = FALSE);
	void Input_AirOperate(BOOL bPenOn, const POINT* ptMouse, EAIROPERATE_CLICKMODE eClickMode, BOOL bUseWinAPI = FALSE);

    //����:״̬��λ
    void Reset();

    //@����:����ָ��������¼���
    //@����:  pos, ���λ��
    //     ptMouse, ����¼�ö��
    //     nMouseData,  E_VERT_WHEEL, Ϊ�������ƶ���, ��WHEEL_DELTA����������������ʾ��ǰ������������ʾ��������
    //                  E_HORZ_WHEEL, Ϊ�������ƶ���, ��WHEEL_DELTA����������������ʾ�ҹ�����������ʾ���������
	//     bUseWinAPI,�Ƿ�ʹ��Windows API��ģ��������
    virtual void GenEvent(const POINT& ptMouse, EVirtualMouseEvent eEvent, int nMouseData = 0);


    //@����:���������豸���
    //@����:hDriverDevice�� �����豸���
    //@˵��:���豸���ΪINVALID_HANDLE_VALUEʱ��ʹ��Window APIģ�������Ϣ��
    void SetDeviceHandle(HANDLE hDriverDevice);



    //@����;����˫�������
    CDoubleClickChecker& GetDoubleClickChecker()
    {
        return this->m_oDoubleClickChecker;
    }
protected:


    //void RunStateMachine()(BOOL bFindPen, const POINT& ptPenPos);
    enum E_VIRTUAL_MOUSE_STATE
    {
        VIRTUAL_MOUSE_STATE_IDLE             ,//ģ��������״̬
        VIRTUAL_MOUSE_STATE_LEFT_DOWN        ,//ģ������������״̬
        //VIRTUAL_MOUSE_STATE_RIGHT_CONTROL  ,//ģ������Ҽ�����״̬
        VIRTUAL_MOUSE_STATE_MOVE             ,//ģ�������ƶ�״̬  
    }m_eVirutalMouseState;

    CDoubleClickChecker m_oDoubleClickChecker ;
    CRightClickTrigger  m_oRightClickTrigger  ;
    POINT               m_ptLastPenAbsolutePos;
    HANDLE              m_hDriverDevice       ;
	BOOL                m_bForceUseWinAPI     ;//ǿ��ʹ��Windows APIģ��������


};



__inline BOOL operator==(const POINT& right, const POINT& left)
{
    return (right.x == left.x && right.y == left.y);
}

__inline BOOL operator!=(const POINT& right, const POINT& left)
{
    return (right.x != left.x || right.y != left.y);
}






