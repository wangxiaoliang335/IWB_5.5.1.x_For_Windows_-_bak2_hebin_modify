#pragma once



struct TSingleClickEvent
{
    UINT   uId      ;//�ʵı��
    POINT  ptClkPos ;//����λ��
    DWORD  dwClkTime;//����ʱ��

};

struct TDownEvent
{
    POINT ptDownPos   ;//��һ�ΰ���λ��
    DWORD dwDownTime  ;//��һ�ΰ���ʱ��
    BOOL  bAlreadyDown;//�Ѿ����±�־
};

//template<int MaxNum>
//class CTouchScreenSingleClickChecker
//{
//
//public:
//    CTouchScreenSingleClickChecker()
//        :
//    m_nTimeThreshold(300)//300����
//    {
//        int nInitPhysicalDimension = int(80.0 * 25.4);//��λ:mm
//        int nCx = GetSystemMetrics(SM_CXSCREEN);
//        int nCy = GetSystemMetrics(SM_CYSCREEN);
//
//        SIZE screenSize;
//        screenSize.cx = nCx;
//        screenSize.cy = nCy;
//
//        OnSetTouchScreenDimension(nInitPhysicalDimension, screenSize);
//
//        Reset();
//    }
//
//    ~CTouchScreenSingleClickChecker()
//    {
//
//    }
//
//    void Reset()
//    {
//        memset(&m_aryDownEvent[0], 0, sizeof(m_aryDownEvent));
//        m_nDownEventCount = 0;
//
//        memset(&m_arySingleClickEvent[0], 0, sizeof(m_arySingleClickEvent));
//        m_nSingleClickeEventCount = 0;
//    }
//
//
//    void DoChecker(const TContactInfo* pContactInfos, int nContactCount)
//    {
//        m_nSingleClickeEventCount = 0;
//
//        for(int i=0; i < nContactCount; i++)
//        {
//            const TContactInfo* pPen = pContactInfos + i;
//
//            if(pPen->uId >= MaxNum)
//            {
//                continue;
//            }
//
//            UINT uId = pPen->uId;
//            switch(pPen->ePenState)
//            {
//            case E_PEN_STATE_UP:
//                //��ⵥ��ʱ��
//                {
//                    DWORD dwNow = GetTickCount();
//
//                    const POINT& ptContact = pPen->pt;
//
//                    if(!m_aryDownEvent[uId].bAlreadyDown)
//                    {//������, ���ִ�id��Ӧ�ıʾ�Ȼ��δ����
//                        break;
//                    }
//
//                    const POINT& ptOldDownPos = m_aryDownEvent[uId].ptDownPos;
//
//                    int dx   = ptContact.x - ptOldDownPos.x;
//                    int dy   = ptContact.y - ptOldDownPos.y;
//                    int dist2 = dx*dx + dy*dy;
//
//                    int dT   = dwNow - m_aryDownEvent[uId].dwDownTime;
//
//                    if(dT  < m_nTimeThreshold && dist2  < m_nDistance2Threshold)
//                    {//ʱ��Ϳռ����޶�����,����Ϊ�����¼�������
//                        m_arySingleClickEvent[m_nSingleClickeEventCount].uId       = uId;
//                        m_arySingleClickEvent[m_nSingleClickeEventCount].ptClkPos  = ptOldDownPos;
//                        m_arySingleClickEvent[m_nSingleClickeEventCount].dwClkTime = dwNow;
//                        m_nSingleClickeEventCount ++;
//                    }
//
//                    m_aryDownEvent[uId].bAlreadyDown = FALSE;//��λ���±�־, ��������¼�
//
//
//                }
//
//
//                break;
//
//
//            case E_PEN_STATE_DOWN:
//                if(!m_aryDownEvent[uId].bAlreadyDown)
//                {
//                    m_aryDownEvent[uId].dwDownTime   = GetTickCount();
//                    m_aryDownEvent[uId].ptDownPos    =pPen->pt;
//                    m_aryDownEvent[uId].bAlreadyDown = TRUE;
//                    m_nDownEventCount ++;
//                }
//
//                break;
//            }//switch
//
//        }
//    }
//
//    //@����:���ô����ĳߴ��¼���Ӧ����
//    //@����:nPhysicalDiagonalLength, ��Ļ�ߴ�,�Խ��߳���, ��λ:����
//    //      szLogicalDimension, ��Ļ�߼��ߴ磬��λ:����
//    //
//    void OnSetTouchScreenDimension(int  nPhysicalDiagonalLength, SIZE szLogicalDimension)
//    {
//        int  nCxScreen = szLogicalDimension.cx;
//        int  nCyScreen = szLogicalDimension.cy;
//
//        int  nLogicalDiagonalLength = (int)sqrt(double(nCxScreen *  nCxScreen + nCyScreen * nCyScreen));
//
//        int  nDistanceThreshold = nLogicalDiagonalLength * MAX_SINGLE_CLICK_OFFSET_PHY_DISTANCE /nPhysicalDiagonalLength;
//
//         m_nDistance2Threshold = nDistanceThreshold * nDistanceThreshold;
//    }
//
//    const TSingleClickEvent* GetSingleClickEvent(int* pEventCount)
//    {
//        if(pEventCount) 
//        {
//            *pEventCount = m_nSingleClickeEventCount;
//        }
//        return &m_arySingleClickEvent[0];
//    }
//protected:
//    struct TDownEvent
//    {
//        POINT ptDownPos   ;//��һ�ΰ���λ��
//        DWORD dwDownTime  ;//��һ�ΰ���ʱ��
//        BOOL  bAlreadyDown;//�Ѿ����±�־
//    };
//
//
//    TDownEvent          m_aryDownEvent[MaxNum];//���㰴���¼�����
//    int                 m_nDownEventCount     ;//���㰴���¼�����
//
//    TSingleClickEvent   m_arySingleClickEvent[MaxNum];//�����¼�����
//    int                 m_nSingleClickeEventCount     ;//�����¼�����
//
//
//    int  m_nDistance2Threshold;//����ƽ������,��λ:����
//    int  m_nTimeThreshold     ;//ʱ������,��λ:����
//
//    static const int MAX_SINGLE_CLICK_OFFSET_PHY_DISTANCE = 7;//��λ:����,����ʱ����Ŀռ��ݴ����
//
//};


//����˫���¼��������
//����ڶ��ΰ���λ�����һ�ΰ���λ��С��һ����Χ,��ʱ�������趨������,�򽫵ڶ��ΰ��µ�λ���������������ⴥ������ǰ�޸�Ϊ��һ�ΰ���ʱ��λ������.
//
template<int MaxNum>
class CTouchScreenDblClickChecker
{
public:
    CTouchScreenDblClickChecker()
        :
    m_nDlbClkTimeThreshold(500)//˫��ʱ������, ��λ:����
    {
        int nInitPhysicalDimension = int(80.0 * 25.4);//80inches, ��λ:mm
        int nCx = GetSystemMetrics(SM_CXSCREEN);
        int nCy = GetSystemMetrics(SM_CYSCREEN);

        SIZE screenSize;
        screenSize.cx = nCx;
        screenSize.cy = nCy;

        OnSetTouchScreenDimension(nInitPhysicalDimension, screenSize);

        Reset();
    }

    ~CTouchScreenDblClickChecker()
    {


    }

    void Reset()
    {
        memset(&m_aryDownEvents[0], 0, sizeof(m_aryDownEvents));
        m_nDownEventCount = 0;

        m_nSingleClickeEventCount = 0;
        memset(&m_arySingleClickEvents[0], 0, sizeof(m_arySingleClickEvents));
    }


    //@����:���ô����ĳߴ��¼���Ӧ����
    //@����:nPhysicalDiagonalLength, ��Ļ�ߴ�,�Խ��߳���, ��λ:����
    //      szLogicalDimension, ��Ļ�߼��ߴ磬��λ:����
    //
    void OnSetTouchScreenDimension(int  nPhysicalDiagonalLength, SIZE szLogicalDimension)
    {
        int  nCxScreen = szLogicalDimension.cx;
        int  nCyScreen = szLogicalDimension.cy;

        int  nLogicalDiagonalLength = (int)sqrt(double(nCxScreen *  nCxScreen + nCyScreen * nCyScreen));
        int  nDistanceThreshold = nLogicalDiagonalLength * MAX_DBL_CLICK_OFFSET_PHY_DISTANCE /nPhysicalDiagonalLength;

        m_nDblClkSpan2Threshold = nDistanceThreshold * nDistanceThreshold;

    }


    //@����:pContactInfos, ����/���������������Ϣ
    //      nContactCount, �������
    //
    //@����:�����⵽����˫���������������õڶ��ΰ��µĵ��������ڵ�һ�ΰ��µ�����
    //@˵��:�㷨˵��
    //     1)��ʼ�� �����¼�����SinglClickEventsΪ��
    //     2)����һ�����ص�c����Ϣ����Ϣ�������ص��ţ����ص�����
    //       ���ص�c�ı��Ϊid,
    //       i)������ص�c��״̬Ϊ����, 
    //       ������c����SinglClickEvents�е�ÿһ��Ԫ�ؽ��о���ȽϺ�ʱ��Ƚ�
    //       ���ʱ��;����ֵ������˫���������ޣ���ǿ�ƽ����ص�c��λ���޸�Ϊ��֮��ƥ��ĵ����¼�����ʼλ�ã��Ա�ϵͳ����˫���¼���
    //       ������ص�c�����κε����¼�ƥ�䣬��"�����¼�����"DownEvents��δ��¼���Ϊid�Ĵ������Ϣ�����ڼ���DownEvents������һ�����Ϊid�ļ�¼��
    //
    //       ii)������ص�c��״̬Ϊ����
    //       �����"�����¼�����"DownEvents�в��ұ��Ϊid����Ϣ����,�ж�ʱ��;���֮���Ƿ������ж�����, 
    //      �����������"�����¼�����"SinglClickEvents��������һ�������¼�, ͬʱ��DownEvents�������Ƴ���¼��
    //
    //     3)�Ƴ�"�����¼�����"SinglClickEvents��"�����¼�����"DownEvents�����г���ʱ�����޵ļ�¼��
    //P.S:
    //һ�ε����¼����жϱ�׼
    //1.������ʱ�����ڷ�ΧR�ڡ�
    //2.���µ���ʱ������T�ڡ�
    //3.���µ���Ϊͬһ֧��
    //
    //

    void DoChecker(TContactInfo* pContactInfos, int nContactCount)
    {
        int i,j;
        //1.���Ȳ��Ұ��µĴ������¼��"�����¼�"������пռ��ʱ�����
        //   ����أ����޸Ĵ�������Ϊ��һ�ΰ���ʱ������
        //  ������أ�����Ϊ�µ�"�����¼�"��¼
        DWORD dwNow = GetTickCount();

        //2.�õ���ĵ���"�����¼�"���пռ��ʱ�����, �Ա�����"�����¼�"
       for(i=0; i < nContactCount; i++)
       {
            TContactInfo* pPenInfo = pContactInfos + i;

            UINT uId = pPenInfo->uId;

            if (uId >= MaxNum)
            {
                continue;//����Խ�����
            }
            
            switch(pPenInfo->ePenState)
            {
                  case E_PEN_STATE_DOWN://�ʰ���
                      {
                          OutputDebugString(L"DoChecker E_PEN_STATE_DOWN\n");
                          BOOL bDblClkTriggered = FALSE;//˫��������־

                          for(j = 0; j < m_nSingleClickeEventCount; j++)
                          {
                                const TSingleClickEvent& oEvent = m_arySingleClickEvents[j];
                                
                                int dT    = dwNow - oEvent.dwClkTime;
                                
                                int dx     = pPenInfo->pt.x  - oEvent.ptClkPos.x;
                                int dy     = pPenInfo->pt.y  - oEvent.ptClkPos.y;
                                int dSpan2 = dx*dx + dy*dy;

                                //AtlTrace(_T("dT=%d, dSpan2 =%d\n"), dT, dSpan2);
                                if(dT < m_nDlbClkTimeThreshold && dSpan2 < m_nDblClkSpan2Threshold)
                                {
                                    
                                    //���������ģ�����
                                    //������˫���¼�
                                    //�޸ıʵ�λ�ã�ʹ֮���ڵ�һ�ΰ���ʱ�̵�λ�ã�����ϵͳ����˫���¼���
                                    pPenInfo->pt = oEvent.ptClkPos;

                                    bDblClkTriggered = TRUE;

                                    //��"�����¼�"�������Ƴ��˵����¼�
                                    int k = 0;
                                    for(k = j; k < (m_nSingleClickeEventCount - 1); k++)
                                    {
                                        m_arySingleClickEvents[k] = m_arySingleClickEvents[k+1];
                                    }

                                    m_nSingleClickeEventCount -- ;

                                    //<<debug
                                    //static int s_count = 0;
                                    //AtlTrace(_T("[Touch Double Click Checker][%d]double click triggered.\n"), s_count++);
                                    //debug>>
                                    break;
                                }
                          }//for-each(i)


                        if(!bDblClkTriggered)
                        {//δ����˫���¼�,����"�����¼�"���������һ����¼
                            if(!m_aryDownEvents[uId].bAlreadyDown)
                            {
                                m_aryDownEvents[uId].dwDownTime   = GetTickCount();
                                m_aryDownEvents[uId].ptDownPos    = pPenInfo->pt;
                                m_aryDownEvents[uId].bAlreadyDown = TRUE;
                                m_nDownEventCount ++;
                            }
                        }

                      }//case
                        break;

                  case E_PEN_STATE_UP://�ʵ���
                        //��ⵥ���¼�
                        {
                            OutputDebugString(L"DoChecker E_PEN_STATE_UP\n");
                            DWORD dwNow = GetTickCount();

                            const POINT& ptContact = pPenInfo->pt;

                            if(!m_aryDownEvents[uId].bAlreadyDown)
                            {//������, ���ִ�id��Ӧ�ıʾ�Ȼ��δ����
                                break;
                            }

                            const POINT& ptOldDownPos = m_aryDownEvents[uId].ptDownPos;

                            int dx   = ptContact.x - ptOldDownPos.x;
                            int dy   = ptContact.y - ptOldDownPos.y;
                            int dist2 = dx*dx + dy*dy;

                            int dT   = dwNow - m_aryDownEvents[uId].dwDownTime;

                            if(dT  < m_nDlbClkTimeThreshold && dist2  < m_nDblClkSpan2Threshold)
                            {//ʱ��Ϳռ����޶�����,����Ϊ�����¼�������

                                if(m_nSingleClickeEventCount < _countof(m_arySingleClickEvents))
                                {
                                    m_arySingleClickEvents[m_nSingleClickeEventCount].uId       = uId;
                                    m_arySingleClickEvents[m_nSingleClickeEventCount].ptClkPos  = ptOldDownPos;
                                    //m_arySingleClickEvents[m_nSingleClickeEventCount].ptClkPos  = ptContact;
                                    m_arySingleClickEvents[m_nSingleClickeEventCount].dwClkTime = dwNow;
                                    m_nSingleClickeEventCount ++;
                                }
                            }

                            m_aryDownEvents[uId].bAlreadyDown = FALSE;//��Ȼ�Ѿ�̧�𣬴�"�����¼�"�����������¼
                            m_nDownEventCount -- ;
                            
                        }//case E_PEN_STATE_UP

                        break;

            }//switch

       }//for(i)

        //3.�Ƴ�"�����¼�"�����еķ���"˫����ʱ"�оݵļ�¼��
        i = 0;
        while(i < m_nSingleClickeEventCount)
        {
            DWORD dwElapse = dwNow - m_arySingleClickEvents[i].dwClkTime;

            if((int)dwElapse > m_nDlbClkTimeThreshold)
            {
                //�Ӻ���ǰŲ��һ��Ԫ��
                for(j = i; j < m_nSingleClickeEventCount - 1; j++)
                {
                    m_arySingleClickEvents[j] = m_arySingleClickEvents[j+1];
                    
                }//for(j)

                m_nSingleClickeEventCount --;
            }
            else
            {
                i ++;
            }
        }//while(i)


        //�ڳ���������, �����Ƴ�"�����¼�"�����еĳ�ʱ��¼,�������¼��ᱻ�������¼�¼��
        //
        //�Ƴ�"�����¼�"�����еķ���"��ʱ"�оݵļ�¼
        //for( i = 0; i < _countof(m_aryDownEvents); i++)
        //{
        //    if(!m_aryDownEvents[i].bAlreadyDown)
        //    {
        //        continue;
        //    }

        //    DWORD dwElapse = dwNow - m_aryDownEvents[i].dwDownTime;
        //    if(dwElapse > m_nDlbClkTimeThreshold)
        //    {
        //        m_aryDownEvents[i].bAlreadyDown = FALSE;
        //        m_nDownEventCount --;
        //    }
        //}//for



        
    }

protected:
    //CTouchScreenSingleClickChecker<MaxNum> m_oSingleClickChecker;

    TSingleClickEvent   m_arySingleClickEvents[MaxNum] ;//�����¼�����,
    int                 m_nSingleClickeEventCount      ;//�����¼�������

    TDownEvent          m_aryDownEvents[MaxNum]        ;//���㰴���¼�����
    int                 m_nDownEventCount              ;//���㰴���¼�����



    int  m_nDblClkSpan2Threshold;//˫������ƽ������,��λ:����
    int  m_nDlbClkTimeThreshold ;//˫��ʱ������,��λ:����

    static const int MAX_DBL_CLICK_OFFSET_PHY_DISTANCE = 8;//��λ:����,����ʱ����Ŀռ��ݴ����


};