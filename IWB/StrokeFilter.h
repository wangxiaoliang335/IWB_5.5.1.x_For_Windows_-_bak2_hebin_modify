#pragma once
#include "ToleranceDistribute.h"
//�ʼ��˲���
//X(smooth_new) = �� * X(smooth_last) + (1 - ��)*X(new)
//�˲�ϵ�� �ˡ�[0,1], ��Խ���˲�Ч��Խ����
//�˲�ϵ���˵�ȷ������
//���űʼ�ǰ����֡�ƶ�����ʸ���ĳ���, ��̬�����˲�ϵ���ˡ�
//��ƽ��ϵ�����������Է���
//�� = ��max - (��max - ��min) * D/D_max 
//����DΪ��ǰ�������ǰһ�����ľ��롣
//D=0ʱ��     �� = ��max
//D=D_maxʱ�� �� = ��min

//2019/11/29
//ʵ������:
//������дʱ������ƽ����ֻ���ֵ���ɡ�
//Ϊ����Ҫһ�����ٶ���ص�ϵ��������ƽ��ϵ��
//�ٶ�Խ�󣬲���ԽС���˲�ϵ�����ٶ�ԽС������Խ����˲�ϵ��
//��μ����ٶ���?
//��ΪCPU�Ĵ���ʱ��ı仯����������֡��ʱ��ʱ��, ��˲���
//�ƶ�������ƶ�ƽ����������ʾ�ٶȡ�
//����ϵ��
//Beta(x) =  a/(a+x)
template <int MAX_STROKE_NUM>
class CStrokeFilter
{
public:
    CStrokeFilter()
    {
        //��������ߴ����Ļ�ֱ��ʣ�������Ӧ���߼�����D1��D2
        int nScreenDiagonalPhysicalLength = int(DEFAULT_SCREEN_DIAGONAL_LENGTH);//ȱʡ����ߴ�80inches
#if 0       
        int nCx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int nCy = GetSystemMetrics(SM_CYVIRTUALSCREEN);
#else
        int nCx = GetSystemMetrics(SM_CXSCREEN);
        int nCy = GetSystemMetrics(SM_CYSCREEN);
#endif
        SIZE screenResolution;
        screenResolution.cx = nCx;
        screenResolution.cy = nCy;

        OnSetTouchScreenDimension(nScreenDiagonalPhysicalLength, screenResolution);
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

        m_nMaxDistance = (KD2PhysicalDistance * nScreenDialgonalLogicalLength)/nPhysicalDiagonalLength;
        //m_nMaxDistance *= (HISTORY_DISTANCE_COUNT >>1);
        //m_nMaxDistance *= HISTORY_DISTANCE_COUNT;

        InitFilter();

        m_oToleranceDistribute.SetScreenSize(szLogicalDimension);
    }


    CToleranceDistribute& GetToleranceDistribute()
    {
        return m_oToleranceDistribute;
    }

    //@����:������ıʼ������˲�����
    //@����:pContactInfo, ����/�������, ���ص���Ϣ
    //      nCount, ����Ĵ��ص����
    //      dbSmoothCoefficient,Խ��Խƽ��
    void DoFilter(TContactInfo* pContactInfo, int& nCount, double dbSmoothCoefficient = 0.1)
    {
        //for(int i=0; i < nCount; i++)
        int i = 0;
        while(i < nCount)
        {
            TContactInfo& contactInfo = pContactInfo[i];
            UINT32 uId = contactInfo.uId;

            if(uId >= MAX_STROKE_NUM)
            {//�����±�Խ�硣
                i++;
                continue;
            }

            TFilterState& filter = m_aryFilters[uId];

            switch(filter.m_eFilterState)
            {
            case E_FILTER_STATE_UP://����״̬

                if(pContactInfo->ePenState == E_PEN_STATE_DOWN)
                {
                    //���:"������ǰ����֡����һСһ��������ߣ��Ҽ����ܺ��Ѵ���"������
                    //pContactInfo->bIgnored = TRUE;//��һ�ΰ���,������ǰ֡

                    //IthԪ�غ����Ԫ����ǰŲ��һ��λ��
                    for (int j = i + 1; j < nCount; j++)
                    {
                        pContactInfo[j - 1] = pContactInfo[j];
                    }
                    nCount -= 1;//����Ĵ�����Ŀ-1;
#ifdef _DEBUG
                    AtlTrace(_T("=======Skip First Frame of Pen %d.=====\n"), pContactInfo->uId);

#endif
                    filter.m_eFilterState = E_FILTER_STATE_DN_1;

                    continue;//������i��Ԫ��,����ִ��i++
                }

                 
                break;

            case E_FILTER_STATE_DN_1://��ߵ�һ����״̬
                if (contactInfo.ePenState == E_PEN_STATE_UP)
                {
                    //filter.m_uFrameCount  = 0;
                   //contactInfo.pt.x = long(filter.m_LastSmoothPoint.d[0] + .5);
                   //contactInfo.pt.y = long(filter.m_LastSmoothPoint.d[1] + .5);

					//<<2020/07/27, by xuke
					//���ȶ�״̬�µ�E_PEN_STATE_UPҲ����, ��ΪE_PEN_STATE_DN�Ѿ���������
					//IthԪ�غ����Ԫ����ǰŲ��һ��λ��
					for (int j = i + 1; j < nCount; j++)
					{
						pContactInfo[j - 1] = pContactInfo[j];
					}
					nCount -= 1;//����Ĵ�����Ŀ-1;

					filter.m_eFilterState = E_FILTER_STATE_UP;

					continue;//������i��Ԫ��,����ִ��i++
					//2020/07/27>>
                }
                else
                {
                    filter.m_LastSmoothPoint.d[0] = double(contactInfo.pt.x);
                    filter.m_LastSmoothPoint.d[1] = double(contactInfo.pt.y);

                    filter.m_LastInputPoint = filter.m_LastSmoothPoint;
                    filter.m_uRepeatSampleCount = 0;

                    filter.m_nHistoryDistanceLength = 0;
                    filter.m_dbTotalDistance = 0.0;
                    filter.m_eFilterState = E_FILTER_STATE_DN_2;
                }
                break;


            case E_FILTER_STATE_DN_2://"����"��̬
                if(contactInfo.ePenState ==  E_PEN_STATE_UP)
                {
                    //filter.m_uFrameCount  = 0;
                    filter.m_eFilterState = E_FILTER_STATE_UP;
                    wchar_t szTmp[1024] = { 0 };
                    wsprintf(szTmp, L"m_LastSmoothPoint d[0]:%d, d[1]:%d\n", long(filter.m_LastSmoothPoint.d[0] + .5), long(filter.m_LastSmoothPoint.d[1] + .5));
                    OutputDebugString(szTmp);

                    wsprintf(szTmp, L" 11 contactInfo pt x:%d, pt y:%d\n", contactInfo.pt.x, contactInfo.pt.y);
                    OutputDebugString(szTmp);
                    contactInfo.pt.x = long(filter.m_LastSmoothPoint.d[0] + .5);
                    contactInfo.pt.y = long(filter.m_LastSmoothPoint.d[1] + .5);

                    wsprintf(szTmp, L" 22 contactInfo pt x:%d, pt y:%d\n", contactInfo.pt.x, contactInfo.pt.y);
                    OutputDebugString(szTmp);

                }
                else if(contactInfo.ePenState == E_PEN_STATE_DOWN)
                {
                    OutputDebugString(L" E_FILTER_STATE_DN_2 E_PEN_STATE_DOWN\n");
                    const TPoint2D& ptLastInput   = filter.m_LastInputPoint;
                    const TPoint2D& ptLastSmooth  = filter.m_LastSmoothPoint;
                    TPoint2D  ptNewInput;
                    ptNewInput.d[0] = contactInfo.pt.x;
                    ptNewInput.d[1] = contactInfo.pt.y;


                    double dx = ptNewInput.d[0] - ptLastInput.d[0];
                    double dy = ptNewInput.d[1] - ptLastInput.d[1];
                    double dbNewDistance = sqrt(dx*dx + dy*dy);

                    //���ƶ��������ƶ�ƽ��
                    //��¼��ʷ����
                    if (filter.m_nHistoryDistanceLength < _countof(filter.m_HistoryDistance))
                    {//��������δ��
                        double dx = ptNewInput.d[0] - ptLastInput.d[0];
                        double dy = ptNewInput.d[1] - ptLastInput.d[1];
                        double dbNewDistance = sqrt(dx*dx + dy*dy);

                        filter.m_HistoryDistance[filter.m_nHistoryDistanceLength] = dbNewDistance;

                        filter.m_dbTotalDistance += dbNewDistance;

                        filter.m_nHistoryDistanceLength++;
                    }
                    else
                    {//������������
                        filter.m_dbTotalDistance -= filter.m_HistoryDistance[0];
                        //�Ƴ�һ���ϵ����ݣ�����һ���µ�����
                        for (int k = 1; k < _countof(filter.m_HistoryDistance); k++)
                        {
                            filter.m_HistoryDistance[k - 1] = filter.m_HistoryDistance[k];
                        }
                        filter.m_HistoryDistance[_countof(filter.m_HistoryDistance) - 1] = dbNewDistance;
                        filter.m_dbTotalDistance += dbNewDistance;
                    }

#ifdef _DEBUG
                    //AtlTrace(_T("newDistance=%f,TotalDistance=%f\n"), dbNewDistance, filter.m_dbTotalDistance);
#endif
                    double avg_distance = filter.m_dbTotalDistance / filter.m_nHistoryDistanceLength;


                    //double dx = ptNewInput.d[0] - ptLastSmooth.d[0];
                    //double dy = ptNewInput.d[1] - ptLastSmooth.d[1];
                    //double dx2 = dx*dx;
                    //double dy2 = dy*dy;

                    //double d2 = dx2 + dy2;
                    //double distance = sqrt(d2);

                    if(dx == 0 && dy == 0)
                    {
                        filter.m_uRepeatSampleCount ++;
                    }
                    else
                    {
                        filter.m_uRepeatSampleCount = 0;
                    }

                    //Beta(x) = a / (a + x)
                    //double Beta = 1 / (1 + filter.m_dbTotalDistance);
                    //Beta = 1.0;

                    TPoint2D ptSmooth;
                    //commented out by xuke, 2023/06/27
                    //if(filter.m_uRepeatSampleCount >= 3)//���������ͬһ�������,�����������ʵ�������,����ƽ������
                    //{
                    //    ptSmooth = ptNewInput;
                    //    //ptSmooth.d[0] = double(ptNewInput.x);
                    //    //ptSmooth.d[1] = double(ptNewInput.y);
                    //}
                    //else if(avg_distance< m_nMaxDistance
                    if (avg_distance< m_nMaxDistance)
                    {
                        
                        //����ƽ��ϵ��
                        //����ģ��
                        //�� = ��max - (��max - ��min) * D / D_max
                        //double smooth_coef = m_dbMaxSmoothCoef - (m_dbMaxSmoothCoef - m_dbMinSmoothCoef)*distance / m_nMaxDistance;

                        //������ģ��
                        //�� = ��min + (��max - ��min) * sqrt(1 - D/D_max)
                        //double smooth_coef = m_dbMinSmoothCoef + (m_dbMaxSmoothCoef - m_dbMinSmoothCoef)*sqrt(1 - distance / m_nMaxDistance);
                        double smooth_coef = m_dbMinSmoothCoef + (m_dbMaxSmoothCoef - m_dbMinSmoothCoef)*sqrt(1 - avg_distance / m_nMaxDistance);

                        //���ݲ�ͬ����д���Ķ�λ���ͬ������ƽ��ϵ��
                        //double dbModulationFactorX = 1.0;
                        //double dbModulationFactorY = 1.0;
                        //this->m_oToleranceDistribute.GetModulateFactors(contactInfo.pt, dbSmoothCoefficient, &dbModulationFactorX, &dbModulationFactorY);
                        //double smooth_coef_x = smooth_coef*dbModulationFactorX;
                        //double smooth_coef_y = smooth_coef*dbModulationFactorY;
                        double smooth_coef_x = smooth_coef*dbSmoothCoefficient;
                        double smooth_coef_y = smooth_coef*dbSmoothCoefficient;

                        ptSmooth.d[0] = smooth_coef_x * ptLastSmooth.d[0] + (1.0 - smooth_coef_x) * ptNewInput.d[0];
                        ptSmooth.d[1] = smooth_coef_y * ptLastSmooth.d[1] + (1.0 - smooth_coef_y) * ptNewInput.d[1];

                    }
                    else
                    {
                        double smooth_coef =  m_dbMinSmoothCoef;
                        ptSmooth.d[0] = smooth_coef * ptLastSmooth.d[0] + (1.0 - smooth_coef) * ptNewInput.d[0];
                        ptSmooth.d[1] = smooth_coef * ptLastSmooth.d[1] + (1.0 - smooth_coef) * ptNewInput.d[1];
                    }


                    
                    //�������ԣ�ѡȡƽ��ϵ��0.6ʱ���ʼ���Ϊƽ����
                    /*
                    //ע��:����״̬ʱ���ܹ��˲�������ᶪʧ�����¼�
                    static double smooth_coef = 0.6;
                    ptSmooth.x = smooth_coef * double(ptLastSmooth.x) + (1.0 - smooth_coef) * ptNewInput.x;
                    ptSmooth.y = smooth_coef * double(ptLastSmooth.y) + (1.0 - smooth_coef) * ptNewInput.y;
                    */
                    contactInfo.pt.x = long(ptSmooth.d[0] + .5);
                    contactInfo.pt.y = long(ptSmooth.d[1] + .5);

#ifdef _DEBUG
                   /*
					AtlTrace(
                        _T("[smooth]input dx=%.2f,dy=%.2f, smooth dx=%.2f, dy=%.2f\n"), 
                        ptNewInput.d[0] - filter.m_LastInputPoint.d[0],
                        ptNewInput.d[1] - filter.m_LastInputPoint.d[1],
                        ptSmooth.d[0]   - filter.m_LastSmoothPoint.d[0],
                        ptSmooth.d[1]   - filter.m_LastSmoothPoint.d[1]
                        );
						*/
#endif
                    filter.m_LastSmoothPoint = ptSmooth;
                    filter.m_LastInputPoint   = ptNewInput;
                }

                break;

            }//switch

            i++;
        }//while

    }

private:
    //<added by Jiqw 201505191136
    void InitFilter()
    {
        //��ʼ���˲���״̬
        for(UINT32 i = 0; i < _countof(m_aryFilters); i++)
        {
            //m_aryFilters[i].m_LastSmoothPoint.x  = 0;
            //m_aryFilters[i].m_LastSmoothPoint.y  = 0;
            //m_aryFilters[i].m_LastInputPoint.x   = 0;
            //m_aryFilters[i].m_LastInputPoint.y   = 0;
            m_aryFilters[i].m_uRepeatSampleCount = 0;
            m_aryFilters[i].m_eFilterState       = E_FILTER_STATE_UP;
        }

        m_dbMinSmoothCoef = 0.0;
        m_dbMaxSmoothCoef = 1.0;

    }
    //>

protected:
    enum E_FilterState
    {
        E_FILTER_STATE_UP   ,//��ʵ���״̬
        //E_FILTER_STATE_DN   ,//��ʰ���״̬
        E_FILTER_STATE_DN_1 ,//��ߵ�һ�ΰ���״̬
        E_FILTER_STATE_DN_2 ,//��߳�������״̬
    };

    static const int DEBOUCNE_DELAY = 2;//�����ӳ�
    static const int MAGINIFY = 1024;

    static const int HISTORY_DISTANCE_COUNT = 5;

    //�˲�״̬
    struct TFilterState
    {
        //POINT        m_LastPoints[DEBOUCNE_DELAY] ;//�ʼ���һ�����λ�á�
        TPoint2D       m_LastSmoothPoint  ;//�ʼ���һ��ƽ�����λ�á�
        TPoint2D       m_LastInputPoint   ;//�ʼ���һ��ƽ�����λ�á�
        UINT           m_uRepeatSampleCount;//ͬһ���ظ���������
        E_FilterState  m_eFilterState;//�˲�״̬��״̬

       //��¼�ƶ�����ʷ���������
        double        m_HistoryDistance[HISTORY_DISTANCE_COUNT];
        int           m_nHistoryDistanceLength;
        double        m_dbTotalDistance;//�ܵ�ƽ���ƶ�����
    };//

    TFilterState m_aryFilters[MAX_STROKE_NUM];//��¼������״̬������,ÿ֧�ʶ�Ӧһ���˲���


    int   m_nMaxDistance;//���ƽ�����룬��λ����
    double m_dbMaxSmoothCoef;//���ƽ��ϵ��
    double m_dbMinSmoothCoef;//��Сƽ��ϵ��

    //static const int KD1PhysicalDistance = 5; //D1��Ӧ��������룬��ֻ������ߴ����,��λΪmm�����������κ�ͶӰ����ֻҪ�����ƶ�����<m_nD1PhysicalDistance������Ϊ��δ�ƶ�
#ifdef PICO_PROJECTOR

    static const int KD1PhysicalDistance = 2; //D1��Ӧ��������룬��ֻ������ߴ����,��λΪmm�����������κ�ͶӰ����ֻҪ�����ƶ�����<m_nD1PhysicalDistance������Ϊ��δ�ƶ�

    //<<added by xuke 2016/09/024
    static const int KD2PhysicalDistance = 5; //D2��Ӧ��������룬��ֻ������ߴ����,��λΪmm�����������κ�ͶӰ����ֻҪ�����ƶ�����>m_nD2PhysicalDistance������Ϊ����Ҫ��ƽ������
    //>>

#else
    static const int KD1PhysicalDistance = 2; //D1��Ӧ��������룬��ֻ������ߴ����,��λΪmm�����������κ�ͶӰ����ֻҪ�����ƶ�����<m_nD1PhysicalDistance������Ϊ��δ�ƶ�
    static const int KD2PhysicalDistance = 14*2; //D2��Ӧ��������룬��ֻ������ߴ����,��λΪmm�����������κ�ͶӰ����ֻҪ�����ƶ�����>m_nD2PhysicalDistance������Ϊ����Ҫ��ƽ������
    //static const int KD1PhysicalDistance = 1; //D1��Ӧ��������룬��ֻ������ߴ����,��λΪmm�����������κ�ͶӰ����ֻҪ�����ƶ�����<m_nD1PhysicalDistance������Ϊ��δ�ƶ�
    //static const int KD2PhysicalDistance = 14; //D2��Ӧ��������룬��ֻ������ߴ����,��λΪmm�����������κ�ͶӰ����ֻҪ�����ƶ�����>m_nD2PhysicalDistance������Ϊ����Ҫ��ƽ������

#endif

    CToleranceDistribute m_oToleranceDistribute;


};
