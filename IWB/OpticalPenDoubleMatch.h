#pragma once
#include <assert.h>

//{begin}, 2014/07/08
//@����:�ʵı�ʶ��Ϣ
struct TPenIdentity
{
    int    nPenNo;//�ʱ��, ��0��ʼ�ı��
    UINT   wMsg  ;//��������Ϣ���
    BOOL   bAvail;//���ñ�־
};


//MaxPenCount, �ʵ����֧��
//FirstPenMessage, ��һ֧�ʹ�������Ϣ
//MsgCountEachPen, ÿ֧�ʹ�������Ϣ�ĸ���
template<unsigned int uMaxPenCount, unsigned int uFirstPenMessage, unsigned int uMsgCountEachPen>
class CPenResource
{
public:

    CPenResource()
    {
        unsigned int uMsg =  uFirstPenMessage;
        for(unsigned int i=0; i < _countof(m_aryIdentity); i++)
        {
            m_aryIdentity[i].nPenNo = i;
            m_aryIdentity[i].wMsg   = uMsg;
            m_aryIdentity[i].bAvail = TRUE;
            uMsg += uMsgCountEachPen;
        }
    }

    //@��  ��:����ʵ������Ϣ
    //@����ֵ:ʧ�ܷ���UINT(-1)
    //        ���򷵻��ҵ��ĵ�һ֧���ñʵı��
    //@����:�÷����̰߳�ȫ
    UINT RequestPenIdentity()
    {
        CComCritSecLock<CComAutoCriticalSection> lock(m_csForResource);
        for(unsigned int i=0; i < _countof(m_aryIdentity); i++)
        {
            if(m_aryIdentity[i].bAvail)
            {
                m_aryIdentity[i].bAvail = FALSE;
                return i;
            }


        }
        return UINT(-1);
    }

    //@��  ��:���ձʵı����Ϣ
    //@����ֵ:ʧ�ܷ���FALSE
    //        ���򷵻�TRUE
    //@����:�÷����̰߳�ȫ
    BOOL RecyclePenIdentity(unsigned int nIndex)
    {
        CComCritSecLock<CComAutoCriticalSection> lock(m_csForResource);
        if(nIndex >= _countof(m_aryIdentity)) return FALSE;
        m_aryIdentity[nIndex].bAvail = TRUE;
		return TRUE;
    }


    const TPenIdentity& operator[](unsigned int nIndex)
    {
        return m_aryIdentity[nIndex];
    }

protected:
    TPenIdentity m_aryIdentity[uMaxPenCount];
    CComAutoCriticalSection m_csForResource ;
};
//{end}, 2014/07/08

class CVirtualPenInfo
{
public:
    CVirtualPenInfo()
        :
        m_bPenActive(FALSE),    //���Ƿ��ǻ��
        m_bPenMatched(FALSE),   //���Ƿ���ƥ���
        m_nPenMissCounter(0),   //�������ݴ�Ĵ���
        //m_bFakeMouse(FALSE)  ,   //���Ƿ���ģ������
        m_eSimulMousePossageState(SIMUL_MOUSE_POSSAGE_STATE_IDLE),
        m_PenMsg(0),
        m_uPenNo(UINT(-1))
    {

    }
    ///////////////////////////////////////////////////////////////////////////////////////
    //@����:ʹ�ܹ�ʣ�������ʵ������Ϣ
    //@����:�����������Ϣʧ���򷵻�FALSE
    BOOL Enable()
    {
       m_uPenNo = m_PenResource.RequestPenIdentity();
       if(UINT(-1) == m_uPenNo)
       {
            return FALSE;
       }

       m_PenMsg = m_PenResource[m_uPenNo].wMsg;

        m_bPenActive = TRUE;
        m_nPenMissCounter  = 0;

		return TRUE;
    }
    /////////////////////////////////////////////////////////////////////////////////////
    void Disable()
    {
        m_PenResource.RecyclePenIdentity(m_uPenNo);
        m_uPenNo = UINT(-1);

        m_bPenActive = FALSE;
    }
    ////////////////////////////////////////////////////////////////////////////////////
    BOOL IsPenActive()
    {
        return m_bPenActive;
    }
    ////////////////////////////////////////////////////////////////////////////////////
    BOOL IsFakeMouse()
    {
        //���Ϊ0�Ĺ��ģ��������
        //������ŵ���Ҫͨ��Windows��Ϣ��������
        return m_uPenNo != 0 ;
    }
    //////////////////////////////////////////////////////////////////////////////////
    //void SetSimMouse(BOOL bFakeMouse)
    //{
    //    m_bFakeMouse = bFakeMouse;
    //}


    /////////////////////////////////////////////////////////////////////////////////

    void SetHistoryPos(const POINT& Pos)
    {
        m_ptHistoryPos = Pos;
        m_bPenMatched = TRUE;
    }
    /////////////////////////////////////////////////////////////////////////////////
    const POINT& GetHistoryPos()const
    {
        return m_ptHistoryPos;
    }
    ////////////////////////////////////////////////////////////////////////////////

    void ClearMatchedFlag()
    {
        m_bPenMatched = FALSE;
    }
    ///////////////////////////////////////////////////////////////////////////////

    BOOL IsPenMatch()const
    {
        return m_bPenMatched;
    }
    //////////////////////////////////////////////////////////////////////////////
    void incMissCounter()
    {
        m_nPenMissCounter++;
    }
    ////////////////////////////////////////////////////////////////////////////

    int GetMissCounter()const
    {
        return m_nPenMissCounter;
    }
    void ReSetHistoryPos()
    {
        m_ptHistoryPos.x = 0;
        m_ptHistoryPos.y = 0;
        m_nPenMissCounter = 0;
    }
    void ReSetMissCount()
    {
        m_nPenMissCounter  = 0;
    }
    ////////////////////////////////////////////////////////////////////////////////�ʵĲ���

    void SimulMouseMotion(HWND hwnd , const POINT *ptPencilPos)
    {
        BOOL bFindPenCil = TRUE;
        POINT SimulMousePoint;
        SimulMousePoint.x = 0;
        SimulMousePoint.y = 0;
        if (ptPencilPos == NULL)
        {
            bFindPenCil = FALSE ;
            if (!m_objStrokeSmoother.IsReset())
            {
                SimulMousePoint = m_objStrokeSmoother.Reset();
                //RunSimulStateMachine(TRUE, SimulMousePoint/*��Ч����*/,hwnd);   //��֤�˲�������һ�㲻��©
            }
            RunSimulStateMachine(FALSE, SimulMousePoint, hwnd );    //���
        }
        else
        {
            SimulMousePoint = m_objStrokeSmoother.Smooth( *ptPencilPos);
            RunSimulStateMachine(bFindPenCil, SimulMousePoint,hwnd );
        }
    }



    void RunSimulStateMachine(BOOL bFindPen, const POINT &ptPencilPos, HWND hwnd )
    {

        POINT ptPencilPosTemp;
        ptPencilPosTemp.x = ptPencilPos.x;
        ptPencilPosTemp.y = ptPencilPos.y;

        switch(m_eSimulMousePossageState)
        {
        case SIMUL_MOUSE_POSSAGE_STATE_IDLE:
            {
                if (bFindPen)
                {
                    PostMessage(hwnd , m_PenMsg , ptPencilPosTemp.x , ptPencilPosTemp.y );
                    m_eSimulMousePossageState = SIMUL_MOUSE_POSSAGE_STATE_DOWN;
                }
                break;
            }
        case  SIMUL_MOUSE_POSSAGE_STATE_DOWN:
            {
                if (bFindPen)
                {
                    PostMessage(hwnd, m_PenMsg+1 , ptPencilPosTemp.x , ptPencilPosTemp.y );
                    m_eSimulMousePossageState = SIMUL_MOUSE_POSSAGE_STATE_MOVE;
                }
                else
                {
                    PostMessage(hwnd, m_PenMsg+2 , ptPencilPosTemp.x , ptPencilPosTemp.y );
                    m_eSimulMousePossageState = SIMUL_MOUSE_POSSAGE_STATE_IDLE;
                }

                break;
            }
        case SIMUL_MOUSE_POSSAGE_STATE_MOVE:
            {
                if (bFindPen)
                {
                    PostMessage(hwnd, m_PenMsg+1 , ptPencilPosTemp.x , ptPencilPosTemp.y );
                }
                else
                {
                    PostMessage(hwnd, m_PenMsg+2 , ptPencilPosTemp.x , ptPencilPosTemp.y );
                    m_eSimulMousePossageState = SIMUL_MOUSE_POSSAGE_STATE_IDLE;
                }
                break;
            }

        }  //switch

    }
    void SetPenMsge(UINT msg)
    {
        m_PenMsg = msg;	
    }
protected:
    POINT   m_ptHistoryPos      ;
    BOOL    m_bPenActive        ;
    BOOL    m_bPenMatched       ;
    int     m_nPenMissCounter   ;
    //BOOL   m_bFakeMouse         ;
    UINT   m_PenMsg             ;

    UINT   m_uPenNo             ;//�ʵı��
    CStrokeSmoother<2> m_objStrokeSmoother;//�ʻ�ƽ����

    enum E_SIMUL_MOUSE_POSSAGE_STATE
    {
        SIMUL_MOUSE_POSSAGE_STATE_IDLE    ,
        SIMUL_MOUSE_POSSAGE_STATE_DOWN    ,
        SIMUL_MOUSE_POSSAGE_STATE_MOVE    ,
    }m_eSimulMousePossageState;

    //��̬��Դ,�ʵ������Դ
    static CPenResource<10u, 1027u, 3u> m_PenResource;
};
_declspec(selectany)  CPenResource<10, 1027, 3> CVirtualPenInfo::m_PenResource;

template <int MaxPenCount>
class CMulPenMatch
{
public:
    CMulPenMatch(CMouseEventGenerator* pMouseEventGenerator)
        :
    m_bMulPenMatched(FALSE),
        m_pMouseEventGenerator(pMouseEventGenerator)
    {
        /*
        m_AryPen[0].SetPenMsge(1027);   //1
        m_AryPen[1].SetPenMsge(1030);   //2
        m_AryPen[2].SetPenMsge(1033);   //3
        m_AryPen[3].SetPenMsge(1036);   //4
        m_AryPen[4].SetPenMsge(1039);   //5
        m_AryPen[5].SetPenMsge(1042);   //6
        m_AryPen[6].SetPenMsge(1045);   //7
        m_AryPen[7].SetPenMsge(1048);   //8
        m_AryPen[8].SetPenMsge(1051);   //9
        m_AryPen[9].SetPenMsge(1054);   //10
        */
    }

    /////////////////////////////////////////////////////////////////////////////////
    ~CMulPenMatch()
    {
    }
    /////////////////////////////////////////////////////////////////////////////////
    CVirtualPenInfo& Getpen(int iIndex)
    {
        ASSERT(0 <= iIndex && iIndex < MaxPenCount);
        return m_AryPen[iIndex];
    }

    ////////////////////////////////////////////////////////////////////////////////
    ///////*********************************************************////////////////
    ////////////////////////////////////////////////////////////////////////////////@���ܣ�ƥ�俪ʼ
    void BeforeMulPenMatch()
    {
        for(int i =0 ; i< MaxPenCount ; i++)
        {
            CVirtualPenInfo & MulPen = m_AryPen[i];
            if (MulPen.IsPenActive())                 //���������ǻ�Ļ����ڿ�ʼ֮ǰ���������ƥ���Ϊ��ƥ��ġ�������������εĲ��ҡ�
            {
                MulPen.ClearMatchedFlag();
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    ///////*********************************************************////////////////
    ////////////////////////////////////////////////////////////////////////////////@���ܣ�����ƥ��
    void  DoMulMatch(HWND hwnd ,const POINT& ptNow)
    {
        int nMinDisTance = 3000;
        int iPenIndex = -1 ;

        for( int i = 0; i < MaxPenCount ; i++)
        {
            CVirtualPenInfo& MulPen = m_AryPen[i];
            ////////////////////���Ȳ�����û��ƥ��Ļ�ʣ�����л�ıʡ�˵���б������С�
            if (!MulPen.IsPenActive())
            {
                continue;
            }
            const POINT& ptHistory = MulPen.GetHistoryPos();
            int nx =(ptNow.x - ptHistory.x) ;
            int ny =(ptNow.y - ptHistory.y) ;

            int nSquareDistance = nx* nx + ny * ny;

            if (nSquareDistance < nMinDisTance)
            {
                iPenIndex = i;
                nMinDisTance = nSquareDistance;
            }
        } //for

        if (iPenIndex != -1)              //˵���ҵ��˻�ʵ���Ϣ
        {
            CVirtualPenInfo& MulPen = m_AryPen[iPenIndex];
            if (!MulPen.IsPenMatch())
            {
                MulPen.SetHistoryPos(ptNow);   //m_bMatch ��Ϊ TURE  .�ҵ�ƥ�����Ϣ�ˣ�����Ҫ������Ϣ�ˡ�
                MulPen.ReSetMissCount();

                //�ҵ���ʵĻ�����Ҫ�ж���֧����ģ��ʲô��Ӧ��
                if(!MulPen.IsFakeMouse())
                {
                    ////////˵������Ӧ���ġ����������������Ϣ
                    this->m_pMouseEventGenerator->GenMouseEvent(
                        &ptNow
#ifdef _DEBUG
                        ,true
#endif 					
                        );
                }
                else
                {
                    MulPen.SimulMouseMotion(hwnd, &ptNow);   //�ҵ�ƥ��ķ�����Ϣ�Ϳɡ�
                }
            }
        } //if

        else//û�л�ıʻ�����û����֮ƥ��ıʵ���Ϣ
        {
            iPenIndex = -1;
            int iMouseIndex = -1;

            for (int i = 0; i < MaxPenCount ; i++)
            {
                CVirtualPenInfo &MulPen = m_AryPen[i];
                if (!MulPen.IsPenActive())
                {
                    iPenIndex = i;
                    break;
                }
            }

            if (iPenIndex != -1)    //�ҵ�û�л�ı�
            {
                CVirtualPenInfo &pen = Getpen(iPenIndex);

                if(!pen.Enable()) return;//ʹ��ʧ����������

                pen.SetHistoryPos(ptNow);
                pen.ReSetMissCount();

                /*
                int iMouseIndex = -1;
                for(int j = 0 ; j < MaxPenCount ; j++)
                {
                    CVirtualPenInfo & pen =  m_AryPen[j];

                    if (pen.IsFakeMouse())    //˵���Ѿ������������Ӧ��
                    {
                        iMouseIndex = j;
                        break;
                    }
                }*/

            

//                if (iMouseIndex != -1)    //˵�������Ӧ�Ѿ���ռ���ˡ�
//                {
//                    pen.SimulMouseMotion(hwnd, &ptNow );	
//                }
//                else                       //˵�������Ӧû�б��õ�
//                {
//                    pen.SetFakeMouse(TRUE);
//                    //g_oMouseEventGen.GenMouseEvent(
//                    this->m_pMouseEventGenerator->GenMouseEvent(
//                        &ptNow
//#ifdef _DEBUG
//                        ,true
//#endif 					
//                        );
//                }


                if (pen.IsFakeMouse())    //˵�������Ӧ�Ѿ���ռ����,�����һ�����
                {
                    pen.SimulMouseMotion(hwnd, &ptNow );
                }
                else//˵�������Ӧû�б��õ�
                {
                    this->m_pMouseEventGenerator->GenMouseEvent(
                        &ptNow
#ifdef _DEBUG
                        ,true
#endif 
                        );
                }



            }

        } //else
    }
    ////////////////////////////////////////////////////////////////////////////////
    ///////*********************************************************////////////////
    ////////////////////////////////////////////////////////////////////////////////@���ܣ�ƥ�����
    void AfterMulPenMatch(HWND hwnd )
    {
        for ( int i = 0; i < MaxPenCount ; i++)
        {
            CVirtualPenInfo &pen = m_AryPen[i];
            if (pen.IsPenActive() && !pen.IsPenMatch())                   //������ǻ�ĵ���û���ҵ�ƥ��ıʵ���Ϣ�������Ѿ�ֹͣ�ʵĲ����ˡ�
            {
                pen.incMissCounter();                                        //����û��ƥ���ϵı�ʧ����Ϣ
                if(pen.GetMissCounter() >=  MAX_MISS_TIME)                  //����ʵ�û��ƥ���ϵ���Ϣ��������5����ʱ��˵������ʾͳ��׵�ֹͣ����������¼��֧�ʵ���Ϣ��Ӧ�ó�ʼ�������������������ɾ��������ʵ���Ϣ��
                {
                    
                    pen.Disable();
                    pen.ReSetHistoryPos();
                    if(!pen.IsFakeMouse())
                    {
                        //pen.SetSimMouse(FALSE);
                        //g_oMouseEventGen.GenMouseEvent(NULL);
                        this->m_pMouseEventGenerator->GenMouseEvent(NULL);
                    }
                    else
                    {
                        pen.SimulMouseMotion(hwnd,NULL);
                    }

                }
            }
        }
    }
private:
    CVirtualPenInfo m_AryPen[MaxPenCount]  ;
    BOOL m_bMulPenMatched                  ;
    static const  int MAX_MISS_TIME = 3    ;
    CMouseEventGenerator* m_pMouseEventGenerator;
};


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////�������Ӧ��Ϣ
class CVirtualEraserInfo
{
public:
    CVirtualEraserInfo()
        :   
    m_eSimulEarserPossageState(SIMUL_EARSER_POSSAGE_STATE_IDLE)
    {

    }
    void EarserEnable()
    {
        m_bEarserActive =TRUE     ;
        m_nEarserMissCounter = 0  ;
    }
    void EarserDisable()
    {
        m_bEarserActive = FALSE  ;
    }

    BOOL IsEarserActive()
    {
        return m_bEarserActive   ;
    }

    void SetEarserHistoryPos(const POINT& Pos)
    {
        m_ptEarserHistoryPos = Pos ;
        m_bEraserMatched  = TRUE   ;
    }

    const POINT& GetEarserHistoryPos()const
    {
        return m_ptEarserHistoryPos  ;
    }

    void ClearEarserMatchFlag()
    {
        m_bEraserMatched = FALSE;
    }

    BOOL isEarserMatch()const
    {
        return m_bEraserMatched;
    }
    void incEarserMissCounter()
    {
        m_nEarserMissCounter++;
    }

    int GetMissCounter()const
    {
        return m_nEarserMissCounter;
    }

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    void SimulEraserMotion(HWND hwnd , const POINT *ptEarserPos)    //ģ�����Ĳ���
    {
        BOOL bFindEarser = TRUE;
        POINT SimulEarserPoint;
        SimulEarserPoint.x = 0;
        SimulEarserPoint.y = 0;
        if (ptEarserPos == NULL)
        {
            bFindEarser = FALSE ;
        }
        else
        {
            SimulEarserPoint = *ptEarserPos ;
        }

        RunSimulEarserStateMachine(bFindEarser,SimulEarserPoint,hwnd);
    }
    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////
    void RunSimulEarserStateMachine(BOOL bFindEarser, const POINT &ptPencilPos, HWND hwnd)
    {
        switch(m_eSimulEarserPossageState)
        {
        case SIMUL_EARSER_POSSAGE_STATE_IDLE:
            {
                if (bFindEarser)
                {
                    PostMessage(hwnd , m_EarserMsg , ptPencilPos.x , ptPencilPos.y )   ;
                    m_eSimulEarserPossageState = SIMUL_EARSER_POSSAGE_STATE_DOWN ;
                }
                break;
            }
        case SIMUL_EARSER_POSSAGE_STATE_DOWN:
            {
                if (bFindEarser)
                {
                    PostMessage(hwnd , m_EarserMsg+1 , ptPencilPos.x , ptPencilPos.y )   ;
                    m_eSimulEarserPossageState = SIMUL_EARSER_POSSAGE_STATE_MOVE ;
                }
                else
                {
                    PostMessage(hwnd , m_EarserMsg+2 , ptPencilPos.x , ptPencilPos.y )   ;
                    m_eSimulEarserPossageState = SIMUL_EARSER_POSSAGE_STATE_IDLE ;
                }
                break;
            }
        case SIMUL_EARSER_POSSAGE_STATE_MOVE:
            {
                if (bFindEarser)
                {
                    PostMessage(hwnd , m_EarserMsg+1 , ptPencilPos.x , ptPencilPos.y )   ;

                    AtlTrace(_T("x= %d , y = %d "), ptPencilPos.x ,ptPencilPos.y );
                }
                else
                {
                    PostMessage(hwnd , m_EarserMsg+2 , ptPencilPos.x , ptPencilPos.y )   ;
                    m_eSimulEarserPossageState = SIMUL_EARSER_POSSAGE_STATE_IDLE ;
                }

                break;
            }
        }
    }

    void SetEarserMsge(UINT msg)
    {
        m_EarserMsg = msg;	
    }

protected:
    BOOL     m_bEarserActive               ;
    POINT    m_ptEarserHistoryPos          ;
    BOOL   m_bEraserMatched                ;
    int   m_nEarserMissCounter             ;
    UINT   m_EarserMsg                     ;

    enum E_SIMUL_EARSER_POSSAGE_STATE
    {
        SIMUL_EARSER_POSSAGE_STATE_IDLE    ,
        SIMUL_EARSER_POSSAGE_STATE_DOWN    ,
        SIMUL_EARSER_POSSAGE_STATE_MOVE    ,
    }m_eSimulEarserPossageState;
};

////////////�������Ӧ��
template <int MaxEarserCount>
class CEraserMatch
{
public:
    CEraserMatch(CMouseEventGenerator* pMouseEventGenerator)
        :m_bEraserMatched(FALSE),
        m_pMouseEventGenerator(pMouseEventGenerator)
    {
        m_AryEarser[0].SetEarserMsge(1057) ;   ////1
        m_AryEarser[1].SetEarserMsge(1060) ;   ////2
    }

    ~CEraserMatch()
    {

    }

    CVirtualEraserInfo& GetEarser(int iIndex)
    {
        ASSERT(0 <= iIndex && iIndex < MaxEarserCount);
        return m_AryEarser[iIndex];
    }

    /////////////////////////////////////////////////////////////
    void BeforeEarserMatch()
    {
        for (int i = 0 ; i < MaxEarserCount ; i++)
        {
            CVirtualEraserInfo & MulEarser = m_AryEarser[i] ;
            if(MulEarser.IsEarserActive())
            {
                MulEarser.ClearEarserMatchFlag();
            }
        }

    }
    void DoEarserMatch(HWND hwnd ,const POINT& ptNow)
    {
        int nMinDisTance = 3000;
        int iEraserIndex = -1 ;
        for(int i = 0 ; i <MaxEarserCount ; i++ )
        {
            CVirtualEraserInfo & MulEarser = m_AryEarser[i];

            if (!MulEarser.IsEarserActive())
            {
                continue;
            }
            const POINT& ptHistory = MulEarser.GetEarserHistoryPos();

            int nx =(ptNow.x - ptHistory.x) ;
            int ny =(ptNow.y - ptHistory.y) ;

            int nSquareDistance = nx* nx + ny * ny;

            if (nSquareDistance < nMinDisTance)
            {
                iEraserIndex = i;
                nMinDisTance = nSquareDistance;
            }
        }

        if (iEraserIndex != -1)
        {
            CVirtualEraserInfo & MulEarser = m_AryEarser[iEraserIndex];

            if (!m_AryEarser->isEarserMatch())
            {
                MulEarser.SetEarserHistoryPos(ptNow);
                MulEarser.SimulEraserMotion(hwnd,&ptNow);
            }
        }
        else
        {
            iEraserIndex = -1;
            for(int i = 0 ;i < MaxEarserCount ; i++)
            {
                CVirtualEraserInfo& MulEarser = m_AryEarser[i];
                if (!MulEarser.IsEarserActive())
                {
                    iEraserIndex = i ;
                    break;
                }
            }

            if (iEraserIndex != -1)
            {
                CVirtualEraserInfo &MulEarser = GetEarser(iEraserIndex) ;
                MulEarser.EarserEnable();
                MulEarser.SetEarserHistoryPos(ptNow);
                MulEarser.SimulEraserMotion(hwnd,&ptNow);
            }
        }
    }
    void AfterEarserMatch(HWND hwnd )
    {
        for(int i = 0 ; i <MaxEarserCount ; i++ )
        {
            CVirtualEraserInfo &MulEarser = m_AryEarser[i] ;
            if (MulEarser.IsEarserActive() && !MulEarser.isEarserMatch())
            {
                MulEarser.incEarserMissCounter();
                if (MulEarser.GetMissCounter() > MAX_MISS_TIME)
                {
                    MulEarser.EarserDisable();
                    MulEarser.SimulEraserMotion(hwnd,NULL);
                }
            }
        }
    }

private:
    CVirtualEraserInfo m_AryEarser[MaxEarserCount] ;
    BOOL m_bEraserMatched                          ;
    static const  int MAX_MISS_TIME = 3            ;
    CMouseEventGenerator* m_pMouseEventGenerator;

};







