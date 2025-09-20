#pragma once
//�ʼ��˲���
//X(smooth) = (1 - ��) * X(last) + ��*X(new)
//
//���űʼ�ǰ����֡�ƶ�����ʸ���ĳ���, ��̬�����˲�ϵ���ˡ�
//�� = K*|��D|^2 + b
//��|��D^2| <= D1ʱ, �� = 0,���˶�ʸ������<D1����ʼ�λ�ñ��ֲ���
//��|��D^2| >= D2ʱ,,�� = 1,���˶�ʸ������>D2���������˲� �ʼ��Զ������µ�����㡣
//
//Ϊ�˹�ܸ�������, ��ϵ��k,b,���˲�ϵ���˶�����1024���������˲��������ֵ��Ҫ����10λ��
//
//
template <int MAX_STROKE_NUM>
class CStrokeFilter
{
public:
    CStrokeFilter()
    {
        //////////////////////////////////////////////////////////////////////////
        //��������ߴ����Ļ�ֱ��ʣ�������Ӧ���߼�����D1��D2
        int nScreenDiagonalPhysicalLength = int(DEFAULT_SCREEN_DIAGONAL_LENGTH);//ȱʡ����ߴ�80inches
        int nCx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int nCy = GetSystemMetrics(SM_CYVIRTUALSCREEN);

        SIZE screenResolution;
        screenResolution.cx = nCx;
        screenResolution.cy = nCy;

        OnSetTouchScreenDimension(nScreenDiagonalPhysicalLength, screenResolution);
        //////////////////////////////////////////////////////////////////////////



        ////���ݷ�������㷽��
        ////�� = K*|��d^2| + b
        ////��Kϵ����bϵ��
        ////
        //// 0 = K*D1 + b
        //// 1 = K*D2 + b
        ////ע��:
        //m_Coef_K = MAGINIFY/(D2 - D1);
        //m_Coef_B = -m_Coef_K*D1;

        ////��ʼ���˲���״̬
        //for(UINT32 i = 0; i < _countof(m_aryFilters); i++)
        //{
        //   // for(UINT32 j = 0; j < DEBOUCNE_DELAY; j++)
        //   //{
        //       //m_aryFilters[i].m_LastPoints[j].x  = 0;
        //       // m_aryFilters[i].m_LastPoints[j].y  = 0;
        //        m_aryFilters[i].m_LastPoint.x = 0;
        //        m_aryFilters[i].m_LastPoint.y = 0;
        //    //}
        //    m_aryFilters[i].m_eFilterState   = E_FILTER_STATE_UP;
        //    //m_aryFilters[i].m_uFrameCount    = 0;

        //};

    }

    ~CStrokeFilter()
    {

    }

    //@����:���ô����ĳߴ��¼���Ӧ����
    //@����:nPhysicalDiagonalLength, ��Ļ�ߴ�,�Խ��߳���, ��λ:����
    //      szLogicalDimension, ��Ļ�߼��ߴ磬��λ:����
    void OnSetTouchScreenDimension(int  nPhysicalDiagonalLength, SIZE szLogicalDimension)
    {
        int nCx = szLogicalDimension.cx;
        int nCy = szLogicalDimension.cy;

        int nScreenDialgonalLogicalLength = (int)sqrt((float)(nCx*nCx + nCy*nCy));


        m_nD1 = (KD1PhysicalDistance * nScreenDialgonalLogicalLength)/nPhysicalDiagonalLength;
        m_nD1 *= m_nD1;

        //modified by xuke,2016/09/24
        //m_nD1 = 1;


        m_nD2 = (KD2PhysicalDistance * nScreenDialgonalLogicalLength)/nPhysicalDiagonalLength;
        m_nD2 *= m_nD2;

        //m_nD2 = 3;

        InitFilter();

        //int nDefaultScreenDiagonalPhysicalLength = int(DEFAULT_SCREEN_DIAGONAL_LENGTH);//ȱʡ����ߴ�80inches
        //int nDefaultScreenDiagonalLogicalLength = int(DEFAULT_SCREEN_DIAGONAL_LOGICAL_LENGTH);//ȱʡ�߼��ߴ�

        //int nCx = szLogicalDimension.cx;
        //int nCy = szLogicalDimension.cy;
        //int nScreenDialgonalLogicalLength = (int)sqrt((float)(nCx*nCx + nCy*nCy));

        //int nRatio = (int)(((float)(nDefaultScreenDiagonalPhysicalLength * nScreenDialgonalLogicalLength) / (nPhysicalDiagonalLength * nDefaultScreenDiagonalLogicalLength)) + 0.5);

        //if (nRatio > 1)
        //{            
        //    m_nD1 = 4 + nRatio;
        //    m_nD2 = 10 + nRatio;
        //}
        //else if (nRatio < 1)
        //{
        //    m_nD1 = 4 - nRatio;
        //    m_nD2 = 10 - nRatio;
        //}
        //else
        //{
        //    m_nD1 = 4;
        //    m_nD2 = 10;
        //}

        //m_nD1 *= m_nD1;
        //m_nD2 *= m_nD2;

        //InitFilter();

    }


    //@����:������ıʼ������˲�����
    //@����:pContactInfo, ����/�������, ���ص���Ϣ
    //      nCount, ����Ĵ��ص����
    void DoFilter(TContactInfo* pContactInfo, int nCount)
    {
        for(int i=0; i < nCount; i++)
        {
            TContactInfo& contactInfo = pContactInfo[i];
            UINT32 uId = contactInfo.uId;

            if(uId >= MAX_STROKE_NUM)
            {//�����±�Խ�硣
                continue;
            }

            TFilterState& filter = m_aryFilters[uId];

            switch(filter.m_eFilterState)
            {
            case E_FILTER_STATE_UP://����״̬

                if(pContactInfo->ePenState == E_PEN_STATE_DOWN)
                {
                    //filter.m_uFrameCount = 0;

                    //contactInfo.ePenState  = E_PEN_STATE_UP;//�ô��ص���ʱ���ֵ���״̬��ʵ���ӳٰ���Ч��

                    filter.m_eFilterState = E_FILTER_STATE_DN;
                    filter.m_LastPoint    = contactInfo.pt;
                }
                 
                break;


            //case E_FILTER_STATE_DN_1://"����"��̬
            //    if(contactInfo.ePenState ==  E_PEN_STATE_UP)
            //    {
            //        filter.m_uFrameCount  = 0;
            //        filter.m_eFilterState = E_FILTER_STATE_UP;
            //    }
            //    else if(contactInfo.ePenState == E_PEN_STATE_DOWN)
            //    {
            //        filter.m_LastPoints[filter.m_uFrameCount] = contactInfo.pt;

            //        filter.m_uFrameCount ++;
            //        if(DEBOUCNE_DELAY == filter.m_uFrameCount)
            //        {
            //           filter.m_eFilterState = E_FILTER_STATE_DN_2;//
            //        }
            //        else
            //        {
            //            contactInfo.ePenState  = E_PEN_STATE_UP;//���´��ص��״̬����ʱ���ֵ���״̬��ʵ���ӳٰ���Ч��
            //        }
            //    }
            //    break;


            case E_FILTER_STATE_DN://"����"��̬
                if(contactInfo.ePenState ==  E_PEN_STATE_UP)
                {
                    //filter.m_uFrameCount  = 0;
                    filter.m_eFilterState = E_FILTER_STATE_UP;
                }
                else if(contactInfo.ePenState == E_PEN_STATE_DOWN)
                {
                    const POINT& ptLast  = filter.m_LastPoint;
                    const POINT& ptNew   = contactInfo.pt;

                    //int abs_dx = ptNew.x > ptLast.x ? (ptNew.x - ptLast.x):(ptLast.x - ptNew.x);
                    //int abs_dy = ptNew.y > ptLast.y ? (ptNew.y - ptLast.y):(ptLast.y - ptNew.y);

                    int dx = ptNew.x - ptLast.x;
                    int dy = ptNew.y - ptLast.y;

                    int dx2 = dx*dx;
                    int dy2 = dy*dy;

                    int d2   = dx2 + dy2;
                   

                    POINT ptSmooth;

                    //����xƽ��λ��
                    if(d2 < m_nD1)
                    {
                        ptSmooth = ptLast;
                        
                    }
                    else if( m_nD1 <= d2 && d2 <= m_nD2)
                    {
                        //����ƽ��ϵ��                        
                        int Lambda = m_Coef_K*d2 + m_Coef_B;
                        ptSmooth.x = ((MAGINIFY - Lambda) * ptLast.x + Lambda * ptNew.x) >> 10;
                        ptSmooth.y = ((MAGINIFY - Lambda) * ptLast.y + Lambda * ptNew.y) >> 10;
                    }
                    else
                    {
                        ptSmooth = ptNew;
                    }

                    contactInfo.pt = ptSmooth;

                    //for(int j=0; j< E_FILTER_STATE_DN_2 - 1; j++)
                    //{
                    //    filter.m_LastPoints[j] = filter.m_LastPoints[j +1];
                    //}

                   filter.m_LastPoint = ptSmooth;

                }

                break;

            }//switch


        }
        
        
    }

private:
    //<added by Jiqw 201505191136
    void InitFilter()
    {
        //���ݷ�������㷽��
        //�� = K*|��d^2| + b
        //��Kϵ����bϵ��
        //
        // 0 = K*D1 + b
        // 1 = K*D2 + b
        //ע��:
        m_Coef_K = MAGINIFY/(m_nD2 - m_nD1);
        m_Coef_B = -m_Coef_K*m_nD1;

        //��ʼ���˲���״̬
        for(UINT32 i = 0; i < _countof(m_aryFilters); i++)
        {
            // for(UINT32 j = 0; j < DEBOUCNE_DELAY; j++)
            //{
            //m_aryFilters[i].m_LastPoints[j].x  = 0;
            // m_aryFilters[i].m_LastPoints[j].y  = 0;
            m_aryFilters[i].m_LastPoint.x = 0;
            m_aryFilters[i].m_LastPoint.y = 0;
            //}
            m_aryFilters[i].m_eFilterState   = E_FILTER_STATE_UP;
            //m_aryFilters[i].m_uFrameCount    = 0;

        };

    }
    //>

protected:
    enum E_FilterState
    {
        E_FILTER_STATE_UP   ,//��ʵ���״̬
        E_FILTER_STATE_DN   ,//��ʰ���״̬
        //E_FILTER_STATE_DN_1 ,//��ߵ�һ�ΰ���״̬
        //E_FILTER_STATE_DN_2 ,//��߳�������״̬
    };

    static const int DEBOUCNE_DELAY = 2;//�����ӳ�
    static const int MAGINIFY = 1024;

    //�˲�״̬
    struct TFilterState{
    //POINT         m_LastPoints[DEBOUCNE_DELAY] ;//�ʼ���һ�����λ�á�
    POINT          m_LastPoint;//�ʼ���һ�����λ�á�
    E_FilterState m_eFilterState ;//�˲�״̬��״̬
    //UINT          m_uFrameCount  ;//֡����
    };//

    TFilterState m_aryFilters[MAX_STROKE_NUM];//��¼������״̬������

    int m_nD1;//�߼����룬��λ���أ���ֵ������ߴ����Ļ�ֱ�����أ��ƶ�����<D1, ����Ϊ���� 
    int m_nD2;//�߼����룬��λ���أ���ֵ������ߴ����Ļ�ֱ�����أ��ƶ�����>D2, ����Ϊ����Ҫ��ƽ������

    //////////////////////////////////////////////////////////////////////////
    //<added by jiqw 201505191105
    //static const int KD1PhysicalDistance = 5; //D1��Ӧ��������룬��ֻ������ߴ����,��λΪmm�����������κ�ͶӰ����ֻҪ�����ƶ�����<m_nD1PhysicalDistance������Ϊ��δ�ƶ�
#ifdef PICO_PROJECTOR

    static const int KD1PhysicalDistance = 2; //D1��Ӧ��������룬��ֻ������ߴ����,��λΪmm�����������κ�ͶӰ����ֻҪ�����ƶ�����<m_nD1PhysicalDistance������Ϊ��δ�ƶ�

    //<<added by xuke 2016/09/024
    static const int KD2PhysicalDistance = 5; //D2��Ӧ��������룬��ֻ������ߴ����,��λΪmm�����������κ�ͶӰ����ֻҪ�����ƶ�����>m_nD2PhysicalDistance������Ϊ����Ҫ��ƽ������
    //>>

#else
    static const int KD1PhysicalDistance = 2; //D1��Ӧ��������룬��ֻ������ߴ����,��λΪmm�����������κ�ͶӰ����ֻҪ�����ƶ�����<m_nD1PhysicalDistance������Ϊ��δ�ƶ�
    static const int KD2PhysicalDistance = 14; //D2��Ӧ��������룬��ֻ������ߴ����,��λΪmm�����������κ�ͶӰ����ֻҪ�����ƶ�����>m_nD2PhysicalDistance������Ϊ����Ҫ��ƽ������
#endif
    

    static const int KSTANDARDPDIALEN    = int(DEFAULT_SCREEN_DIAGONAL_LENGTH); //����ߴ���80inchesΪ��׼
    static const int KSTANDARDLDIALEN    = 2032;                                //�߼��ߴ���1024*7658Ϊ��׼

    //static const int K
    //>
    //////////////////////////////////////////////////////////////////////////

    //�� = K*|��x| + b
    int m_Coef_K;//Kϵ��, ����1024��
    int m_Coef_B;//Bϵ��, ����1024��

};
