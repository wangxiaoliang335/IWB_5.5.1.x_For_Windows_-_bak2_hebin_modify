#include "stdafx.h"
//#include "headers.h"
CSpotMerger::CSpotMerger()
{
#if 0
    int nCxScreen = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int nCyScreen = GetSystemMetrics(SM_CYVIRTUALSCREEN);
#else 
    int nCxScreen = GetSystemMetrics(SM_CXSCREEN);
    int nCyScreen = GetSystemMetrics(SM_CYSCREEN);
#endif
    int nRange = nCxScreen > nCyScreen ? nCyScreen : nCxScreen;

    m_nMergeDistThreshold = nRange * 2 / 100;
}

CSpotMerger::~CSpotMerger()
{

}

////@����:ִ����Ļ�ں�����ߺϲ�����
////@����:pLightSpots������/�������, �������
////      pLightSpotCount, ����/���������ָ���߸�����ָ��
//void CSpotMerger::DoMerge(TLightSpot* pLightSpots, int* pLightSpotCount)
//{
//    int nSpotCount = *pLightSpotCount;
//
//    if(nSpotCount < 2) return;
//
// 
//
//    int MERGE_THRESHOLD = m_nMergeDistThreshold * m_nMergeDistThreshold;//�ں����ޡ�
//
//    enum ESpotLocation
//    {   
//        E_NOT_IN_MERGE_AREA,
//        E_IN_RIGHT_MERGE_AREA,
//        E_IN_LEFT_MERGE_AREA
//    };
//
//    
//    //AtlTrace(_T("Before Merge %d"), nSpotCount);
//    BOOL bDoMerge = FALSE;
//
//    int i = 0;
//
//    while(i < nSpotCount)
//    {
//#ifdef _DEBUG
//        if(nSpotCount >=1)
//        {
//            //AtlTrace(_T("Before Merge nSpotCount = %d\n"), nSpotCount);
//            int nDebug = 0;
//        }
//#endif
//         TLightSpot& s1 = pLightSpots[i];
//
//         ESpotLocation eS1Location = E_NOT_IN_MERGE_AREA;//S1���������־
//
//         //if(s1.ptPosInScreen.x < m_nSeperateX && (m_nSeperateX - s1.ptPosInScreen.x) < nHalfMergeAreaWidth)
//         if(m_nMergeAreaLeftBorder <= s1.ptPosInScreen.x  && s1.ptPosInScreen.x<= m_nSeperateX )
//         {
//            eS1Location = E_IN_LEFT_MERGE_AREA;
//         }
//         //else if(s1.ptPosInScreen.x > m_nSeperateX && (s1.ptPosInScreen.x - m_nSeperateX ) < nHalfMergeAreaWidth)
//         else if(m_nSeperateX < s1.ptPosInScreen.x && s1.ptPosInScreen.x <= m_nMergeAreaRightBorder)
//         {
//            eS1Location = E_IN_RIGHT_MERGE_AREA;
//         }
//
//
//
//        if( E_IN_LEFT_MERGE_AREA == eS1Location || E_IN_RIGHT_MERGE_AREA == eS1Location)
//        {
//            
//            for(int j = i+1; j < nSpotCount; j++)
//            {
//                
//                TLightSpot& s2 = pLightSpots[j];
//
//                ESpotLocation eS2Location = E_NOT_IN_MERGE_AREA;//S2���������־
//
//
//                if(m_nMergeAreaLeftBorder <= s2.ptPosInScreen.x  && s2.ptPosInScreen.x < m_nSeperateX)
//                {
//                    eS2Location = E_IN_LEFT_MERGE_AREA;
//                }
//                else if( m_nSeperateX <= s2.ptPosInScreen.x  && s2.ptPosInScreen.x <= m_nMergeAreaRightBorder)
//                {
//                    eS2Location = E_IN_RIGHT_MERGE_AREA;
//                }
//
//
//                if(eS2Location != E_NOT_IN_MERGE_AREA) 
//                {
//                    int dx = s2.ptPosInScreen.x - s1.ptPosInScreen.x;
//                    int dy = s2.ptPosInScreen.y - s1.ptPosInScreen.y;
//
//                    int R2 = dx*dx + dy*dy;//�������֮��ľ���
//
//                    if(R2 < MERGE_THRESHOLD)
//                    {
//                        //��߾��빻��, ��Ҫ���ϲ�
//                        
//                        
//                        int total_mass = s1.mass + s2.mass;
//
//                        //���չ�������Ȩ����ȡ�ϲ������Ļ����
//                        s1.ptPosInScreen.x = (s1.mass * s1.ptPosInScreen.x)/total_mass + (s2.mass * s2.ptPosInScreen.x)/total_mass;
//                        s1.ptPosInScreen.y = (s1.mass * s1.ptPosInScreen.y)/total_mass + (s2.mass * s2.ptPosInScreen.y)/total_mass;
//
//                        s1.mass += s2.mass;
//
//
//                        ////ȡ������Ĺ��ֵ, 
//                        ////ע��:���ܹ��򵥵ؽ�����������,�����������ӱ����������ơ�
//                        //if(s1.mass < s2.mass)
//                        //{
//                        //    s1 = s2;
//                        //}
//                        s1.lStdSpotAreaInVideo += s2.lStdSpotAreaInVideo;
//
//                        //��Ӿ��������ӣ�������,
//                        //���ӵļ�����:
//                        //bound.left   = min(s1.bound.left   , s2.bound.left  )
//                        //bound.right  = max(s1.bound.right  , s2.bound.right )
//                        //bound.top    = min(s1.bound.top    , s2.bound.top   )
//                        //bound.bottom = max(s1.bound.bottom , s2.bound.bottom)
//                        s1.lAreaInVideo += s2.lAreaInVideo;
//
//                        //�����Ԫ����ǰŲ��һ��λ��
//                        for(int k =j+1; k < nSpotCount; k++)
//                        {
//                            pLightSpots[k-1] = pLightSpots[k];
//                        }
//                        //AtlTrace(_T("Merge a spot\n"));
//                        
//                        nSpotCount --;
//
//                        bDoMerge = TRUE;
//                    }//if
//
//                }//if
//
//            }//for
//        }//if
//        
//        i++;
//
//    }//while
//
//#ifdef _DEBUG
//    if(nSpotCount)
//    {
//        //AtlTrace(_T("After Merge nSpotCount = %d, bDoMerge=%d\n\n"), nSpotCount, bDoMerge);
//    }
//#endif
//
//    *pLightSpotCount = nSpotCount;
//
//}


//@����:ִ����Ļ�ں�����ߺϲ�����
//@����:pLightSpots������/�������, �������
//      pLightSpotCount, ����/���������ָ���߸�����ָ��
void CSpotMerger::DoMerge(TLightSpot* pLightSpots, int* pLightSpotCount)
{
    int nSpotCount = *pLightSpotCount;

    if (nSpotCount < 2) return;

    //�ںϵľ������޵�ƽ��
    int MERGE_THRESHOLD = m_nMergeDistThreshold * m_nMergeDistThreshold;


    //�ںϷ�����־
    BOOL bMergeHappened = FALSE;


    int nSpotIndex = 0;

    while (nSpotIndex < nSpotCount)
    {
        TLightSpot&  spot = pLightSpots[nSpotIndex];

        if (spot.aux.uMergeAreaIndex != UINT(-1))
        {
            for (int j = nSpotIndex + 1; j < nSpotCount; j++)
            {
                TLightSpot&  cmp_spot = pLightSpots[j];

                if (cmp_spot.aux.uMergeAreaIndex == spot.aux.uMergeAreaIndex)
                {//��ǰ����Ĺ�ߺ���֮�ȽϵĹ�ߴ���ͬһ�ں���
                 //ͨ���ж��Ƿ�С���ںϵľ�������, �������Ƿ��ں�

                    int dx = cmp_spot.spotParam.ptPosInScreen.x - spot.spotParam.ptPosInScreen.x;
                    int dy = cmp_spot.spotParam.ptPosInScreen.y - spot.spotParam.ptPosInScreen.y;

                    int R2 = dx*dx + dy*dy;//�������֮��ľ���

                    if (R2 < MERGE_THRESHOLD)
                    {
                        //��߾��빻��, ��Ҫ���ϲ�
                        int total_mass = spot.mass + cmp_spot.mass;

                        //���չ�������Ȩ����ȡ�ϲ������Ļ����
                        spot.spotParam.ptPosInScreen.x = (spot.mass * spot.spotParam.ptPosInScreen.x) / total_mass + (cmp_spot.mass * cmp_spot.spotParam.ptPosInScreen.x) / total_mass;
                        spot.spotParam.ptPosInScreen.y = (spot.mass * spot.spotParam.ptPosInScreen.y) / total_mass + (cmp_spot.mass * cmp_spot.spotParam.ptPosInScreen.y) / total_mass;

                        spot.mass += cmp_spot.mass;


                        spot.lStdSpotAreaInVideo += cmp_spot.lStdSpotAreaInVideo;

                        spot.lAreaInVideo += cmp_spot.lAreaInVideo;

                        //�����Ԫ����ǰŲ��һ��λ��
                        for (int k = j + 1; k < nSpotCount; k++)
                        {
                            pLightSpots[k - 1] = pLightSpots[k];
                        }//for(k)

                        nSpotCount--;
                        bMergeHappened = TRUE;


                        AtlTrace(
                            _T("spot(%d,%d) of camera_%d is merged\n"),
                            cmp_spot.spotParam.ptPosInScreen.x,
                            cmp_spot.spotParam.ptPosInScreen.y,
                            cmp_spot.spotParam.dwCameraId);
                    }//if(R2 < MERGE_THRESHOLD)

                }//if(cmp_spot.aux.mergeAreaIndex == spot.aux.mergeAreaIndex)

            }//for(j)

        }

        nSpotIndex++;
    }//while

#ifdef _DEBUG
    if (nSpotCount)
    {
        AtlTrace(_T("After Merge nSpotCount = %d, bDoMerge=%d\n\n"), nSpotCount, bMergeHappened);
    }
#endif

    *pLightSpotCount = nSpotCount;
}





//@����:��Ļ�ֱ��ʱ仯�¼���Ӧ����
//@����:nScreenWidth, �µ���Ļ���
//      nScreenHeight,�µ���Ļ�߶�
void CSpotMerger::OnDisplayChange(int nScreenWidth, int nScreenHeight)
{
    this->m_nSeperateX =nScreenWidth   >> 1;//����2
    int nMergeAreaWidth = nScreenWidth >> 6;//����64
    //this->m_nMergeAreaLeftBorder  = m_nSeperateX - nMergeAreaWidth/2;
    //this->m_nMergeAreaRightBorder = m_nSeperateX + nMergeAreaWidth/2;

    int nRange = nScreenWidth > nScreenHeight ? nScreenHeight : nScreenWidth;
    //m_nMergeDistThreshold = nRange * 4/ 100;
    //m_nMergeDistThreshold = this->m_nMergeAreaRightBorder - this->m_nMergeAreaLeftBorder;
    m_nMergeDistThreshold = nMergeAreaWidth;
}


//@����:�����ں�����
//@����:mergeAreas, �ں�������
//      nCount, �ں�����Ŀ
void CSpotMerger::SetMergeAreas(const RECT  mergeAreas[], int nAreaCount)
{
    m_vecMergeAreas.resize(nAreaCount);

    for (size_t i = 0; i < m_vecMergeAreas.size(); i++)
    {
        m_vecMergeAreas[i] = mergeAreas[i];
    }

}


//@����:����ָ�����ں���
const RECT* CSpotMerger::GetMergeArea(UINT nIndex) const
{
    if (nIndex >= m_vecMergeAreas.size()) return NULL;

    return &m_vecMergeAreas[nIndex];
}