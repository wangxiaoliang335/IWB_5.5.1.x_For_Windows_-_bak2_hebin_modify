#include "stdafx.h"
//#include "headers.h"

//////////////////////////////////////////////////////////////////////////

//����ʶ��
CGestureEventGenerator g_oGLBoardGR;
CWindowsGestureRecognition g_oWinGR;
//bool g_oIsFingerTouchMode= false;

//
CVirtualHID CSpotListProcessor::m_oVirtualHID;

CSpotListProcessor::CSpotListProcessor()
:
m_lReferenceCount(0L),
m_hProcessThread(NULL),
m_uCameraCount(1),
m_bLastHIDOwnnerAfterGR(true),
//m_bIsSmartPenReset(true),
m_bSimulateMode(FALSE),
m_bIsTriggeringGuesture(FALSE),
m_dbSmoothCoefficient(0.0),
m_eCalibrateModel(E_CALIBRATE_MODEL_GENERICAL_CAMERA),
m_bAutoMerge(FALSE)
{
    Reset();
    m_hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_hExitEvent  = CreateEvent(NULL, FALSE, FALSE, NULL);

}


CSpotListProcessor::~CSpotListProcessor()
{
    CloseHandle(m_hWriteEvent);
    CloseHandle(m_hExitEvent );

}

//@����:��ʼ��
void CSpotListProcessor::Init(UINT uCameraCount)
{
    if (uCameraCount > MAX_CAMERA_NUMBER)
    {
        uCameraCount = MAX_CAMERA_NUMBER;
    }

    m_uCameraCount = uCameraCount;

}

FILE * g_hDebugSampleFile1   = NULL;
FILE * g_hDebugSampleFile2  = NULL;
FILE * g_hDebugRawInputData = NULL;

//@����:���������߳�
//@˵��:���ü���Ϊ0ʱ���������߳�, ����һ��,���ü���+1,
void CSpotListProcessor::StartProcess()
{
	Reset();

    if(m_lReferenceCount == 0)
    {
		m_oVirtualHID.OpenDevice();
		////add by vera_zhao
		m_oVirtualHID.OpenTUIOServer(true);

        StartProcessThread();

        //<<debug
#ifdef _DEBUG
        SYSTEMTIME sysTime;
        GetLocalTime(&sysTime);
        char szFileName[256];
        sprintf_s(
            szFileName,
            _countof(szFileName),
            "SampleData_%04d%02d%02d_%02d%02d%02d%03d_InputLightSpot.txt",
            sysTime.wYear,
            sysTime.wMonth,
            sysTime.wDay,
            sysTime.wHour,
            sysTime.wMinute,
            sysTime.wSecond,
            sysTime.wMilliseconds);

        //fopen_s(&g_hDebugSampleFile1, szFileName, "w");
        g_hDebugSampleFile1 = _fsopen(szFileName, "w", _SH_DENYWR);

        sprintf_s(
            szFileName,
            _countof(szFileName),
            "SampleData_%04d%02d%02d_%02d%02d%02d%03d_SmoothVertex.txt",
            sysTime.wYear,
            sysTime.wMonth,
            sysTime.wDay,
            sysTime.wHour,
            sysTime.wMinute,
            sysTime.wSecond,
            sysTime.wMilliseconds);

        //fopen_s(&g_hDebugSampleFile2, szFileName, "w");
        g_hDebugSampleFile2 = _fsopen(szFileName, "w", _SH_DENYWR);

        sprintf_s(
            szFileName,
            _countof(szFileName),
            "SampleData_%04d%02d%02d_%02d%02d%02d%03d_RawInputData.txt",
            sysTime.wYear,
            sysTime.wMonth,
            sysTime.wDay,
            sysTime.wHour,
            sysTime.wMinute,
            sysTime.wSecond,
            sysTime.wMilliseconds);

        //fopen_s(&g_hDebugRawInputData, szFileName, "w");
        g_hDebugRawInputData = _fsopen(szFileName, "w", _SH_DENYWR);

#endif
        //debug>>

    }

    InterlockedIncrement(&m_lReferenceCount);
}

//@����:ֹͣ
//@˵��:����һ��,���ü���-1�����ü���Ϊ0ʱ�������߳̽���
void CSpotListProcessor::StopProcess()
{

    if(m_lReferenceCount)
    {
        InterlockedDecrement(&m_lReferenceCount);
    }
    if(0 == m_lReferenceCount && this->m_hProcessThread != NULL)
    {
        StopProcessThread();
        m_oVirtualHID.CloseDevice();

        //<<debug
#ifdef _DEBUG
        if(g_hDebugSampleFile1)
        {
            fclose(g_hDebugSampleFile1);
            g_hDebugSampleFile1 = NULL;
        }

        if(g_hDebugSampleFile2)
        {
            fclose(g_hDebugSampleFile2);
            g_hDebugSampleFile2 = NULL;
        }

        if(g_hDebugRawInputData)
        {
            fclose(g_hDebugRawInputData);
            g_hDebugRawInputData = NULL;
        }
        //debug>>
#endif
    }
}

////@����:�жϹ�����ں������Ƿ���Ժϲ����ڵ�
////@����:ptPos�����λ��
////     pBuddyCameraSpotList, �ֵ�������ɼ����Ĺ���б�
////     nBuddyCameraSpotCount, �ֵ�������ɼ����Ĺ�߸���
//BOOL CSpotListProcessor::HasNeignborInSpotList(const POINT& ptPos, const TLightSpot* pBuddyCameraSpotList, int nBuddyCameraSpotCount)
//{
//    int threshold = this->GetSpotMerger().GetMergeDistThreshold();
//    threshold = threshold * threshold;
//
//    for(int i=0; i < nBuddyCameraSpotCount; i++)
//    {
//        const TLightSpot& spot = pBuddyCameraSpotList[i];
//
//        if(m_oSpotMerger.GetMergeAreaLeftBorder()   <= spot.ptPosInScreen .x  &&    spot.ptPosInScreen .x <= m_oSpotMerger.GetMergeAreaRightBorder())
//        {
//
//            int dx = ptPos.x - spot.ptPosInScreen.x;
//            int dy = ptPos.y - spot.ptPosInScreen.y;
//            int dist = dx*dx + dy*dy;
//
//            if(dist < threshold)
//            {
//                return TRUE;
//            }
//        }
//    }
//
//    return FALSE;
//}

//@�� ��:�жϹ���Ƿ�����ڴ����ں�����
//@�� ��:lightSpot, ������ݡ�
//       CameraIndex, ������ߵľ�ͷ��ID�š�
//       pMergeAreaIndex, ָ�򱣴��ں����������ڴ��ָ�룬�ڸô����ں���������Ҫ�жϵĹ�ߡ�
//       bBeyondMergeArea, ��Խ�ں�������߽��־
//@����ֵ:TRUE, ���ں����ڳ��֡�
//       FALSE, δ���ں����ڳ���
BOOL CSpotListProcessor::AppearInMergeArea(const TLightSpot& lightSpot, UINT CameraIndex, /*bool* pbBeyondMergeArea = NULL,*/ UINT* pMergeAreaIndex)
{
    if (m_uCameraCount == 1) return FALSE;
    if (CameraIndex >= m_uCameraCount) return FALSE;

    RECT mergeAreas[2];
    UINT mergeAreaIndex[2];
    int  areaCount = 0;

    if (CameraIndex == 0)
    {//ֻ���ұߵ�һ���ں���
        const RECT* pMergeArea = m_oSpotMerger.GetMergeArea(CameraIndex);
        mergeAreas[0] = *pMergeArea;
        mergeAreaIndex[0] = CameraIndex;
        areaCount = 1;

        //if (lightSpot.ptPosInScreen.x > pMergeArea->right)
        //{//��߳�����Ψһ�ں����Ҳ������ھ�Camera������,�������Լ��Ĺ�Ͻ��Χ��
        //    if (pbBeyondMergeArea) *pbBeyondMergeArea = TRUE;
        //}

    }
    else if (CameraIndex == m_uCameraCount - 1)
    {//ֻ����ߵ�һ���ں���
        const RECT* pMergeArea = m_oSpotMerger.GetMergeArea(CameraIndex - 1);
        mergeAreas[0] = *pMergeArea;
        mergeAreaIndex[0] = CameraIndex - 1;

        //if (lightSpot.ptPosInScreen.x < pMergeArea->left)
        //{//��߳�����Ψһ�ں�����������ھ�Camera������,�������Լ��Ĺ�Ͻ��Χ��
        //    if (pbBeyondMergeArea)*pbBeyondMergeArea = TRUE;
        //}

        areaCount = 1;
    }
    else
    {//�������߸���һ���ں���
        const RECT* pLeftMergeArea  = m_oSpotMerger.GetMergeArea(CameraIndex -1);
        const RECT* pRightMergeArea = m_oSpotMerger.GetMergeArea(CameraIndex);

        mergeAreas[0] = *pLeftMergeArea;
        mergeAreas[1] = *pRightMergeArea;

        mergeAreaIndex[0] = CameraIndex - 1;
        mergeAreaIndex[1] = CameraIndex;

        areaCount = 2;


        //if(lightSpot.ptPosInScreen.x < pLeftMergeArea->left)
        //{
        //    // ��߳��������ں����������������ھ�Camera������,�������Լ��Ĺ�Ͻ��Χ��
        //    if (pbBeyondMergeArea)*pbBeyondMergeArea = TRUE;
        //}

        //else if (lightSpot.ptPosInScreen.x > pRightMergeArea->right)
        //{
        //    // ��߳��������ں������Ҳ������ұ��ھ�Camera������, �������Լ��Ĺ�Ͻ��Χ��
        //    if (pbBeyondMergeArea)*pbBeyondMergeArea = TRUE;
        //}
        
    }
    
    for (int i = 0; i < _countof(mergeAreas); i++)
    {
        const RECT& area = mergeAreas[i];
		const POINT& ptPosInScreen = lightSpot.spotParam.ptPosInScreen;
	
        if (area.left < ptPosInScreen.x && ptPosInScreen.x < area.right
			 &&
			area.top < ptPosInScreen.y && ptPosInScreen.y < area.bottom
			)
        {//������ں�����Χ����
            if (pMergeAreaIndex)
            {
                *pMergeAreaIndex = mergeAreaIndex[i];
            }
            return TRUE;
        }//if
        
    }//for

    return FALSE;
}


//@�� ��:�жϹ���Ƿ����ڵ��ֵ�����ͷ���ں����ڿ���
//@�� ��:lightSpot, �������
//       CameraIndex, ������ߵľ�ͷ��ID��
//@����ֵ:TRUE, �����ڵ��ֵ�����ͷ����
//       FALSE, �ֵ����δ����
//BOOL CSpotListProcessor::SeenInMergeAreaByBuddyCamera(const TLightSpot& spotTarget, UINT CameraIndex)
BOOL CSpotListProcessor::SeenByBuddyCamera(const TLightSpot& spotTarget, UINT CameraIndex)
{
    if (m_uCameraCount == 1) return FALSE;
    if (CameraIndex >= m_uCameraCount) return FALSE;

    int buddyCameraIndex[2];
    int nBuddyCameraCount = 0;

    if (CameraIndex == 0)
    {//ֻ���ұߵ�һ��Buddy
        buddyCameraIndex[0] = CameraIndex + 1;
        nBuddyCameraCount   = 1;
    }
    else if (CameraIndex == m_uCameraCount - 1)
    {//ֻ����ߵ�һ��Buddy
        buddyCameraIndex[0] = CameraIndex - 1;
        nBuddyCameraCount = 1;
    }
    else
    {//�������߸���һ��Buddy

        UINT leftBuddyIndex  = CameraIndex - 1;
        UINT rightBuddyIndex = CameraIndex + 1;

        buddyCameraIndex[0] = leftBuddyIndex;
        buddyCameraIndex[1] = rightBuddyIndex;
        nBuddyCameraCount = 2;
    }

    int threshold = this->GetSpotMerger().GetMergeDistThreshold();
    threshold = threshold * threshold;

    for (int i = 0; i < nBuddyCameraCount; i++)
    {
        DWORD dwBuddyCameraId = buddyCameraIndex[i];
        const TLightSpot* pBuddyCameraSpotList = &m_InputSpotListGroup.aryLightSpots[dwBuddyCameraId][0];
        int nBuddyCameraSpotCount = m_InputSpotListGroup.aryLightSpotsCount[dwBuddyCameraId];


        for (int j = 0; j < nBuddyCameraSpotCount; j++)
        {
            const TLightSpot& buddySpot = pBuddyCameraSpotList[i];

            //if (AppearInMergeArea(buddySpot, dwBuddyCameraId))
            //
            {//buddySpotҲλ���ں���, �ж�Ŀ�����buddy��֮��ľ����Ƿ�С���ںϾ�������
                int dx = spotTarget.spotParam.ptPosInScreen.x - buddySpot.spotParam.ptPosInScreen.x;
                int dy = spotTarget.spotParam.ptPosInScreen.y - buddySpot.spotParam.ptPosInScreen.y;
                int dist = dx*dx + dy*dy;

                if (dist < threshold)
                {
                    return TRUE;
                }
            }
        }//for(j)

    }//for(i)

    return FALSE;
}

BOOL CSpotListProcessor::SeenByBuddyCamera1(const TLightSpot& spotTarget, UINT CameraIndex)
{
    if (m_uCameraCount == 1) return FALSE;
    if (CameraIndex >= m_uCameraCount) return FALSE;

    int buddyCameraIndex[2];
    int nBuddyCameraCount = 0;

    if (CameraIndex == 0)
    {//ֻ���ұߵ�һ��Buddy
        buddyCameraIndex[0] = CameraIndex + 1;
        nBuddyCameraCount = 1;
    }
    else if (CameraIndex == m_uCameraCount - 1)
    {//ֻ����ߵ�һ��Buddy
        buddyCameraIndex[0] = CameraIndex - 1;
        nBuddyCameraCount = 1;
    }
    else
    {//�������߸���һ��Buddy

        UINT leftBuddyIndex = CameraIndex - 1;
        UINT rightBuddyIndex = CameraIndex + 1;

        buddyCameraIndex[0] = leftBuddyIndex;
        buddyCameraIndex[1] = rightBuddyIndex;
        nBuddyCameraCount = 2;
    }

    int threshold = this->GetSpotMerger().GetMergeDistThreshold();
    threshold = threshold * threshold;

    for (int i = 0; i < nBuddyCameraCount; i++)
    {
        DWORD dwBuddyCameraId = buddyCameraIndex[i];
        const TLightSpot* pBuddyCameraSpotList = &m_InputSpotListGroup.aryLightSpots1[dwBuddyCameraId][0];
        int nBuddyCameraSpotCount = m_InputSpotListGroup.aryLightSpotsCount1[dwBuddyCameraId];


        for (int j = 0; j < nBuddyCameraSpotCount; j++)
        {
            const TLightSpot& buddySpot = pBuddyCameraSpotList[i];

            //if (AppearInMergeArea(buddySpot, dwBuddyCameraId))
            //
            {//buddySpotҲλ���ں���, �ж�Ŀ�����buddy��֮��ľ����Ƿ�С���ںϾ�������
                int dx = spotTarget.spotParam.ptPosInScreen.x - buddySpot.spotParam.ptPosInScreen.x;
                int dy = spotTarget.spotParam.ptPosInScreen.y - buddySpot.spotParam.ptPosInScreen.y;
                int dist = dx * dx + dy * dy;

                if (dist < threshold)
                {
                    return TRUE;
                }
            }
        }//for(j)

    }//for(i)

    return FALSE;
}

//@��  ��:�ж��ֵ�����Ƿ����ں����ڿ����˹��
//@��  ��:��ǰ�����Id, ������ȷ���ֵ������id��
//@����ֵ: 
//         TRUE, �ֵ���������ں����ڿ����˹��
//        FALSE, �ֵ���������ں�����û�з��ֹ��
BOOL CSpotListProcessor::BuddyCameraFoundSpotInMergeArea(UINT CameraIndex)
{
    if (m_uCameraCount == 1) return FALSE;
    if (CameraIndex >= m_uCameraCount) return FALSE;

    RECT mergeAreas[2];
    int  areaCount = 0;

    int buddyCameraIndex[2];
    int nBuddyCameraCount = 0;

    if (CameraIndex == 0)
    {//ֻ���ұߵ�һ��Buddy
        buddyCameraIndex[0] = CameraIndex + 1;
        nBuddyCameraCount = 1;

        const RECT* pMergeArea = m_oSpotMerger.GetMergeArea(CameraIndex);
        mergeAreas[0] = *pMergeArea;
        areaCount = 1;

    }
    else if (CameraIndex == m_uCameraCount - 1)
    {//ֻ����ߵ�һ��Buddy
        buddyCameraIndex[0] = CameraIndex - 1;
        nBuddyCameraCount = 1;

        const RECT* pMergeArea = m_oSpotMerger.GetMergeArea(CameraIndex - 1);
        mergeAreas[0] = *pMergeArea;
        areaCount = 1;
    }
    else
    {//�������߸���һ��Buddy

        UINT leftBuddyIndex = CameraIndex - 1;
        UINT rightBuddyIndex = CameraIndex + 1;

        buddyCameraIndex[0] = leftBuddyIndex;
        buddyCameraIndex[1] = rightBuddyIndex;
        nBuddyCameraCount = 2;


        const RECT* pLeftMergeArea = m_oSpotMerger.GetMergeArea(CameraIndex - 1);
        const RECT* pRightMergeArea = m_oSpotMerger.GetMergeArea(CameraIndex);

        mergeAreas[0] = *pLeftMergeArea;
        mergeAreas[1] = *pRightMergeArea;
        areaCount = 2;
    }

    for (int i = 0; i < areaCount; i++)
    {
        const RECT& area = mergeAreas[i];
               
        DWORD dwBuddyCameraId = buddyCameraIndex[i];

        const TLightSpot* pBuddyCameraSpotList = &m_InputSpotListGroup.aryLightSpots[dwBuddyCameraId][0];
        int nBuddyCameraSpotCount = m_InputSpotListGroup.aryLightSpotsCount[dwBuddyCameraId];

        for (int j = 0; j < nBuddyCameraSpotCount; j++)
        {
            const TLightSpot& buddySpot = pBuddyCameraSpotList[j];
            if (area.left < buddySpot.spotParam.ptPosInScreen.x && buddySpot.spotParam.ptPosInScreen.x < area.right)
            {
                return TRUE;
            }
          
        }//for(j)

    }

    return FALSE;

}



//@����:������б�������д�����б���Ϣ
//@����:pLightSpots��ָ�����б��ָ��
//      nLightSpotCount, ��߸���
//      dwCameraId, ��0��ʼ������ͷ���
//@����:�ú���������߳�ͬʱ���ʣ������Ҫȷ���̰߳�ȫ
//
BOOL CSpotListProcessor::WriteSpotList(TLightSpot* pLightSpots, int nLightSpotCount, TLightSpot* pLightSpots1, int nLightSpotCount1, DWORD dwCameraId, E_CALIBRATE_MODEL eCalibrateModel)
{
    //�������̰߳�ȫ����
    CComCritSecLock<CComAutoCriticalSection> lock(m_csForSpotListGroupFIFO);
    if (nLightSpotCount1 == 1)
    {
        wsprintf(szTmp, L"WriteSpotList nLightSpotCount:%d, nLightSpotCount1:%d\n", nLightSpotCount,
            nLightSpotCount1);
        OutputDebugString(szTmp);
    }

	m_eCalibrateModel = eCalibrateModel;

    if(dwCameraId >= MAX_CAMERA_NUMBER) return FALSE;


#ifdef _DEBUG
    static int s_BatchNo = 0;
    s_BatchNo++;

    char szData[128];
    SYSTEMTIME localTime;
    GetLocalTime(&localTime);

    if (g_hDebugSampleFile1)
    {

        //�����߸���
        sprintf_s(
            szData,
            _countof(szData),
            "[%02d.%02d.%02d.%03d]<%d>[%d]:",
            localTime.wHour,
            localTime.wMinute,
            localTime.wSecond,
            localTime.wMilliseconds,
            dwCameraId,
            nLightSpotCount);

        fwrite(szData, 1, strlen(szData), g_hDebugSampleFile1);

        for (int i = 0; i < nLightSpotCount; i++)
        {
            sprintf_s(
                szData,
                _countof(szData),
                "%d,%d,%d,%d,%d;",
                pLightSpots[i].spotParam.ptPosInScreen.x,
                pLightSpots[i].spotParam.ptPosInScreen.y,
                pLightSpots[i].lStdSpotAreaInVideo,
                pLightSpots[i].mass,
                pLightSpots[i].aux.bOutsideOwnedArea
            );
            fwrite(szData, 1, strlen(szData), g_hDebugSampleFile1);
        }
        fwrite("\n", 1, 1, g_hDebugSampleFile1);
        fflush(g_hDebugSampleFile1);

    }

#endif

    if(nLightSpotCount >  MAX_OBJ_NUMBER)
    {
        nLightSpotCount = MAX_OBJ_NUMBER;
    }

    //<<debug
    static  BOOL s_bLastDelayProcess = FALSE;
    //>>

    BOOL bDelayProcess = FALSE;//��Ҫ�ӳٴ����־

    BOOL bAllOutsideMergeArea = TRUE;

    if(theApp.ScreenMode() >= EScreenModeDouble)
    {//����ƴ��ģʽ
        for (int i = 0; i < nLightSpotCount; i++)
        {
            TLightSpot& spot = pLightSpots[i];
            //<<debug
            spot.spotParam.dwCameraId = dwCameraId;
            //debug>>
            static bool s_bDebug = 0;
            if (dwCameraId == 1)
            {
                s_bDebug  = true;
            }

            spot.aux.uMergeAreaIndex = UINT(-1);
            //spot.aux.bBeyondMergeArea = false;

            if (spot.aux.bOutsideOwnedArea)
            {//��������������Ͻ��Ļ������,������������
                continue;
            }

            //if (AppearInMergeArea(spot, dwCameraId, &spot.aux.bBeyondMergeArea, &spot.aux.uMergeAreaIndex))
            if (m_bAutoMerge && AppearInMergeArea(spot, dwCameraId, &spot.aux.uMergeAreaIndex))
            {
                bAllOutsideMergeArea = FALSE;

                //���λ���ں�����, ��Ҫ�ж�����һ������ͷ�Ĺ���Ƿ����ںϾ����������ڣ���֤��������ͷͬʱ���,
                //������Ϊ��������ͷ���ݲ����Ĳ�ͬ��,һ������ͷ�ɼ��Ĺ����Ļ�������ں�����,����һ������ͷ�ɼ��Ĺ����Ļ������֮�����Զ,���ᱻ�ںϣ����
                //��ߴ���������յ��������, ������һ֧��, ����Ĺ�߿����������ɵı�ƥ�䣬���ԭ�еıʵ��𡣲�����Ч�����Ǳʼ�����
                //if(SeenInMergeAreaByBuddyCamera(spot, dwCameraId))
                if (SeenByBuddyCamera(spot, dwCameraId))
                {
                    //AtlTrace(
                    //    _T("Buddy Camera also see the spot(%d,%d) saw by camera %d\n"),
                    //    spot.ptPosInScreen.x,
                    //    spot.ptPosInScreen.y,
                    //    dwCameraId);

                }
                else
                {//�ֵ������û�������, ��Ҫ�Ӻ���
                    bDelayProcess = TRUE;
#ifdef _DEBUG
                    AtlTrace(
                        _T("BatchNo=%d,Spot(%d,%d) from camera %d delayed process case A\n"),
                        s_BatchNo,
                        pLightSpots[0].spotParam.ptPosInScreen.x,
                        pLightSpots[0].spotParam.ptPosInScreen.y,
                        dwCameraId);
#endif
                    break;
                }
            }
        }//for(i)



        for (int i = 0; i < nLightSpotCount1; i++)
        {
            TLightSpot& spot = pLightSpots1[i];
            //<<debug
            spot.spotParam.dwCameraId = dwCameraId;
            //debug>>
            static bool s_bDebug = 0;
            if (dwCameraId == 1)
            {
                s_bDebug = true;
            }

            spot.aux.uMergeAreaIndex = UINT(-1);
            //spot.aux.bBeyondMergeArea = false;

            if (spot.aux.bOutsideOwnedArea)
            {//��������������Ͻ��Ļ������,������������
                continue;
            }

            //if (AppearInMergeArea(spot, dwCameraId, &spot.aux.bBeyondMergeArea, &spot.aux.uMergeAreaIndex))
            if (m_bAutoMerge && AppearInMergeArea(spot, dwCameraId, &spot.aux.uMergeAreaIndex))
            {
                bAllOutsideMergeArea = FALSE;

                //���λ���ں�����, ��Ҫ�ж�����һ������ͷ�Ĺ���Ƿ����ںϾ����������ڣ���֤��������ͷͬʱ���,
                //������Ϊ��������ͷ���ݲ����Ĳ�ͬ��,һ������ͷ�ɼ��Ĺ����Ļ�������ں�����,����һ������ͷ�ɼ��Ĺ����Ļ������֮�����Զ,���ᱻ�ںϣ����
                //��ߴ���������յ��������, ������һ֧��, ����Ĺ�߿����������ɵı�ƥ�䣬���ԭ�еıʵ��𡣲�����Ч�����Ǳʼ�����
                //if(SeenInMergeAreaByBuddyCamera(spot, dwCameraId))
                if (SeenByBuddyCamera1(spot, dwCameraId))
                {
                    //AtlTrace(
                    //    _T("Buddy Camera also see the spot(%d,%d) saw by camera %d\n"),
                    //    spot.ptPosInScreen.x,
                    //    spot.ptPosInScreen.y,
                    //    dwCameraId);

                }
                else
                {//�ֵ������û�������, ��Ҫ�Ӻ���
                    bDelayProcess = TRUE;
#ifdef _DEBUG
                    AtlTrace(
                        _T("BatchNo=%d,Spot(%d,%d) from camera %d delayed process case A\n"),
                        s_BatchNo,
                        pLightSpots1[0].spotParam.ptPosInScreen.x,
                        pLightSpots1[0].spotParam.ptPosInScreen.y,
                        dwCameraId);
#endif
                    break;
                }
            }
        }//for(i)


        //��ǰ����ͷ�Ĺ�����ݶ��ںϲ�������ʱ,Ҫ��֤buddy����ͷ�Ĺ��Ҳ���ں�������,
        //������Ϊ��������ͷ���ݲ����Ĳ�ͬ��,һ������ͷ�ɼ��Ĺ����Ļ�������ں�����,����һ������ͷ�ɼ��Ĺ����Ļ�������ں�������,���ᱻ�ںϣ����
        //��ߴ���������յ��������, ������һ֧��, ����Ĺ�߿����������ɵı�ƥ�䣬���ԭ�еıʵ���
        if (m_bAutoMerge && bAllOutsideMergeArea)
        {
            if (BuddyCameraFoundSpotInMergeArea(dwCameraId))
            {

#ifdef _DEBUG
                AtlTrace(
                    _T("BatchNo=%d,Spot(%d,%d) from camera %d delayed process case B\n"),
                    s_BatchNo,
                    pLightSpots[0].spotParam.ptPosInScreen.x,
                    pLightSpots[0].spotParam.ptPosInScreen.y,
                    dwCameraId);
#endif

                bDelayProcess = TRUE;
            }
        }

    }

    for (int index = 0; index < nLightSpotCount; index++)
    {
        wsprintf(szTmp, L" xxxxxx dwCameraId:%d, x:%d, y:%d\n", pLightSpots[index].spotParam.dwCameraId,
            pLightSpots[index].spotParam.ptPosInScreen.x,
            pLightSpots[index].spotParam.ptPosInScreen.y);
        OutputDebugString(szTmp);
    }

    memcpy(&m_InputSpotListGroup.aryLightSpots[dwCameraId], pLightSpots, nLightSpotCount*sizeof(TLightSpot));
    m_InputSpotListGroup.aryLightSpotsCount[dwCameraId] = nLightSpotCount;

    memcpy(&m_InputSpotListGroup.aryLightSpots1[dwCameraId], pLightSpots1, nLightSpotCount1 * sizeof(TLightSpot));
    m_InputSpotListGroup.aryLightSpotsCount1[dwCameraId] = nLightSpotCount1;

    wsprintf(szTmp, L"22 WriteSpotList nLightSpotCount:%d, nLightSpotCount1:%d\n", m_InputSpotListGroup.aryLightSpotsCount[dwCameraId],
        m_InputSpotListGroup.aryLightSpotsCount1[dwCameraId]);
    OutputDebugString(szTmp);

    if(bDelayProcess)
    {//��Ҫ�Ӻ���,
        return FALSE;
    }
    
    //�����д��FIFO��ȥ
    m_SpotListGroupFIFO.Write(m_InputSpotListGroup);
    //����д������¼�
    SetEvent(m_hWriteEvent);

    return TRUE;
}


//@����:������
void CSpotListProcessor::ProcessLightSpots()
{
    //�������̰߳�ȫ����
    CComCritSecLock<CComAutoCriticalSection> lock(m_csForSpotListGroupFIFO);

    while(!m_SpotListGroupFIFO.IsEmpty())
    {//����FIFO����д�����������

        //char szBuf[1024] = { 0 };
        //sprintf(szBuf, "77 CSpotListProcessor ProcessLightSpots m_nID:%d\n", m_nID);
        //OutputDebugStringA(szBuf);

        const TSpotListGroup* pSpotListGroup = m_SpotListGroupFIFO.Read();
        if(pSpotListGroup == NULL) break;
        //��������ͷ�Ĺ������
        TLightSpot allLightSpots[MAX_CAMERA_NUMBER*MAX_OBJ_NUMBER];
        //���й�߸���
        int nAllLightSpotCount = 0;

        //��������ͷ�Ĺ������
        TLightSpot allLightSpots1[MAX_CAMERA_NUMBER * MAX_OBJ_NUMBER];
        //���й�߸���
        int nAllLightSpotCount1 = 0;

        BOOL bDoubleScreenTouchMergeTemp = g_tSysCfgData.globalSettings.bDoubleScreenTouchMerge;
             
        /*

        //#0�Ŵ�����(λ��ƴ���������)�Ĺ��
        for(int i=0; i<  pSpotListGroup->aryLightSpotsCount[0]; i++)
        {
            const TLightSpot& spot = pSpotListGroup->aryLightSpots[0][i];

            //bDoubleScreenTouchMergeTemp = g_tSysCfgData.globalSettings.bDoubleScreenTouchMerge;

            if(theApp.GetScreenMode() == EScreenModeDouble && bDoubleScreenTouchMergeTemp  && spot.ptPosInScreen.x > this->GetSpotMerger().GetMergeAreaRightBorder())
            {//���λ���ں����ұ߽�����, ���Թ���
                continue;
            }

            allLightSpots[nAllLightSpotCount] = spot;
            nAllLightSpotCount ++;
        }

        //#1�Ŵ�����(λ��ƴ�������ұ�)�Ĺ��
        for(int i=0; i<  pSpotListGroup->aryLightSpotsCount[1]; i++)
        {
            const TLightSpot& spot = pSpotListGroup->aryLightSpots[1][i];
            if(theApp.GetScreenMode() == EScreenModeDouble && bDoubleScreenTouchMergeTemp && spot.ptPosInScreen.x < this->GetSpotMerger().GetMergeAreaLeftBorder())
            {//���λ���ں�����߽�����, ���Թ���
                continue;
            }
            allLightSpots[nAllLightSpotCount] = spot;
            nAllLightSpotCount ++;
        }
        */
              
        for (int nCameraIndex = 0; nCameraIndex < MAX_CAMERA_NUMBER; nCameraIndex++)
        {
            for (int i = 0; i < pSpotListGroup->aryLightSpotsCount[nCameraIndex]; i++)
            {
                const TLightSpot& spot = pSpotListGroup->aryLightSpots[nCameraIndex][i];
                
                //���λ���ں��������߽����⣬���Թ�
                //if (spot.aux.bBeyondMergeArea) continue;
                //��������������Ͻ����Ļ��������,���Թ���
                if (spot.aux.bOutsideOwnedArea) continue;

                wsprintf(szTmp, L"ProcessLightSpots nCameraIndex:%d, aryLightSpotsCount:%d\n", nCameraIndex, pSpotListGroup->aryLightSpotsCount[nCameraIndex]);
                OutputDebugString(szTmp);

                allLightSpots[nAllLightSpotCount] = spot;
                nAllLightSpotCount++;
            }
        }

        for (int nCameraIndex = 0; nCameraIndex < MAX_CAMERA_NUMBER; nCameraIndex++)
        {
            for (int i = 0; i < pSpotListGroup->aryLightSpotsCount1[nCameraIndex]; i++)
            {
                const TLightSpot& spot = pSpotListGroup->aryLightSpots1[nCameraIndex][i];

                //���λ���ں��������߽����⣬���Թ�
                //if (spot.aux.bBeyondMergeArea) continue;
                //��������������Ͻ����Ļ��������,���Թ���
                if (spot.aux.bOutsideOwnedArea) continue;

                wsprintf(szTmp, L"ProcessLightSpots nCameraIndex:%d, nAllLightSpotCount1:%d\n", nCameraIndex, pSpotListGroup->aryLightSpotsCount1[nCameraIndex]);
                OutputDebugString(szTmp);

                allLightSpots1[nAllLightSpotCount1] = spot;
                nAllLightSpotCount1++;
            }
        }

        /*if (nAllLightSpotCount > 0)
        {
            char szBuf[1024] = { 0 };
            sprintf(szBuf, "999 OnPostProcess ProcessLightSpots nAllLightSpotCount:%d\n", nAllLightSpotCount);
            OutputDebugStringA(szBuf);
        }*/
        /*else
        {
            char szBuf[1024] = { 0 };
            sprintf(szBuf, "999 11 OnPostProcess ProcessLightSpots nAllLightSpotCount:%d\n", nAllLightSpotCount);
            OutputDebugStringA(szBuf);
        }*/

        //���й�ߵĺ�������
        OnPostProcess(&allLightSpots[0], nAllLightSpotCount, &allLightSpots1[0], nAllLightSpotCount1);
    }
}


void DebugContactInfo(const TContactInfo* contactInfos, int nCount)
{

    for (int i = 0; i < nCount; i++)
    {

        const TCHAR*  szEvent;
        if (contactInfos[i].ePenState == E_PEN_STATE_DOWN)
        {
            szEvent = _T("Down");
        }
        else if (contactInfos[i].ePenState == E_PEN_STATE_UP)
        {
            szEvent = _T("Up");
        }
        else
        {
            szEvent = _T("Unknown");
        }

/*
        TCHAR szDebug[1024];
        _stprintf_s(
            szDebug,
            _countof(szDebug),
            _T("pen_%01d:<x,y>=<%d,%d>,%s\n"),
            contactInfos[i].uId,
            contactInfos[i].pt.x,
            contactInfos[i].pt.y,
            szEvent);

        OutputDebugString(szDebug);
*/
        char szData[128];
        sprintf_s(
            szData,
            _countof(szData),
            "%d,%d,%d,%d\n",
            contactInfos[i].uId,
            contactInfos[i].pt.x,
            contactInfos[i].pt.y,
            contactInfos[i].ePenState);
        OutputDebugStringA(szData);
        fwrite(szData, 1, strlen(szData), g_hDebugRawInputData);
        fflush(g_hDebugRawInputData);

    }//for-each(i)

}
//@����:���й�ߵĺ����������
//@����:pLightSpots, ָ�����б��ָ��
//      nLightSpotCount, ��߸���
//@˵��:�ú���������ProcessLightSpots�������߳�ProcessThreadProc��ִ��
//      
void CSpotListProcessor::OnPostProcess(TLightSpot* pLightSpots, int nLightSpotCount, TLightSpot* pLightSpots1, int nLightSpotCount1)
{
    //˫��ƴ��ʱ���ں����ڵĹ�ߺϲ���
    if(theApp.ScreenMode() >= EScreenModeDouble && m_bAutoMerge)
    {
        m_oSpotMerger.DoMerge(pLightSpots, &nLightSpotCount);
        m_oSpotMerger.DoMerge(pLightSpots1, &nLightSpotCount1);
    }

    //��λ���ƴ�����־
    m_bIsTriggeringGuesture = FALSE;
	
	//���ص�ƥ����,��ֵ��,�������Ƿ�λ��־
	BOOL bNeedReset = FALSE;

    TContactInfo penInfo[PEN_NUM];
    int penCount = PEN_NUM;

    TContactInfo penInfo1[PEN_NUM];
    int penCount1 = PEN_NUM;

	/////add by zhaown 2019.10.09
	if(GetActualTouchType() == E_DEVICE_PALM_TOUCH_CONTROL 
		&& (   theApp.GetPalmTouchType() == E_PLAM_TOUCHCONTROL_P1
			|| theApp.GetPalmTouchType() == E_PLAM_TOUCHCONTROL_P2
			|| theApp.GetPalmTouchType() == E_PLAM_TOUCHCONTROL_P3
	        || theApp.GetPalmTouchType() == E_PLAM_TOUCHCONTROL_P4 
			|| theApp.GetPalmTouchType() == E_PLAM_TOUCHCONTROL_P5)  )
	{
		if (m_eCalibrateModel == E_CALIBRATE_MODEL_GENERICAL_CAMERA)
		{
		     /////��������ƻ���ʱ����ô�Ͳ���ƽ������Ͳ�ֵ����ֱ����ԭʼ�Ĺ��ֵ���д���
		     int  nScreenX = GetSystemMetrics(SM_CXSCREEN);
		     int  nScreenY = GetSystemMetrics(SM_CYSCREEN);

		     //POINT pts[MAX_CAMERA_NUMBER*MAX_OBJ_NUMBER];
             TLightSpotParam spotParam[MAX_CAMERA_NUMBER * MAX_OBJ_NUMBER];
		     //��Ĭ��ֵ80Ӣ���ʱ��ƫ���ֵPixelNumber =12 ��
		     //��200Ӣ���ʱ��ƫ���ֵPixelNumber =6�������г�һ�����Է���ʽ
		     double  screenDigonalInMM = g_tSysCfgData.globalSettings.fScreenDiagonalPhysicalLength;
//		     double  PixelNumber = (10936 - screenDigonalInMM) / 296;
			 double  PixelNumber = 12 - (screenDigonalInMM / 720);
		     if (PixelNumber< 0 )
		     {
			      PixelNumber = 0;
		     }
		     for (int i = 0; i< nLightSpotCount; i++)
		     {
			     //pts[i] = pLightSpots[i].spotParam.ptPosInScreen;
                 spotParam[i] = pLightSpots[i].spotParam;
			     if ( (spotParam[i].ptPosInScreen.x > nScreenX/8 && spotParam[i].ptPosInScreen.x < nScreenX*7/8) && (spotParam[i].ptPosInScreen.y > nScreenY/8 && spotParam[i].ptPosInScreen.y < (nScreenY*7)/8) )
			     {
                     spotParam[i].ptPosInScreen.x  = spotParam[i].ptPosInScreen.x + (int)PixelNumber;
                     spotParam[i].ptPosInScreen.y  = spotParam[i].ptPosInScreen.y - (int)PixelNumber;
			     }
		      }

		      m_oSmartPenMatch.DoMatch(spotParam, nLightSpotCount);
		      int nElementCount = 0;
		      const TMatchInfo* pMatchInfo = m_oSmartPenMatch.GetAllMatchInfo(&nElementCount);
		      penCount = nElementCount;
			  //ƽ���ʼ�
		      m_oStrokFilter.DoFilter(penInfo, penCount, m_dbSmoothCoefficient);

		     for (int i = 0 ; i < penCount; i++ )
		     {
			     const TMatchInfo &refMInfo = pMatchInfo[i];
			     penInfo[i].ePenState = (refMInfo.eMatchState == E_MISMATCHED) ? E_PEN_STATE_UP : E_PEN_STATE_DOWN;
			     penInfo[i].uId = refMInfo.uId;
			     penInfo[i].pt = refMInfo.ptPos;
                 penInfo[i].dwCameraId = refMInfo.dwCameraId;
                 penInfo[i].gunId = refMInfo.gunId;
		     }

             /********************************************************/
             for (int i = 0; i < nLightSpotCount1; i++)
             {
                 //pts[i] = pLightSpots[i].spotParam.ptPosInScreen;
                 spotParam[i] = pLightSpots1[i].spotParam;
                 if ((spotParam[i].ptPosInScreen.x > nScreenX / 8 && spotParam[i].ptPosInScreen.x < nScreenX * 7 / 8) && (spotParam[i].ptPosInScreen.y > nScreenY / 8 && spotParam[i].ptPosInScreen.y < (nScreenY * 7) / 8))
                 {
                     spotParam[i].ptPosInScreen.x = spotParam[i].ptPosInScreen.x + (int)PixelNumber;
                     spotParam[i].ptPosInScreen.y = spotParam[i].ptPosInScreen.y - (int)PixelNumber;
                 }
             }

             m_oSmartPenMatch.DoMatch(spotParam, nLightSpotCount1);
             int nElementCount1 = 0;
             const TMatchInfo* pMatchInfo1 = m_oSmartPenMatch.GetAllMatchInfo(&nElementCount1);
             penCount1 = nElementCount1;
             //ƽ���ʼ�
             m_oStrokFilter.DoFilter(penInfo1, penCount1, m_dbSmoothCoefficient);

             for (int i = 0; i < penCount; i++)
             {
                 const TMatchInfo& refMInfo = pMatchInfo1[i];
                 penInfo1[i].ePenState = (refMInfo.eMatchState == E_MISMATCHED) ? E_PEN_STATE_UP : E_PEN_STATE_DOWN;
                 penInfo1[i].uId = refMInfo.uId;
                 penInfo1[i].pt = refMInfo.ptPos;
                 penInfo1[i].dwCameraId = refMInfo.dwCameraId;
                 penInfo1[i].gunId = refMInfo.gunId;
             }
             /*******************************************************/

             char szBuf[1024] = { 0 };
             sprintf(szBuf, "777 OnPostProcess InputPoints pLightSpots dwCameraId:%d\n", pLightSpots->spotParam.dwCameraId);
             OutputDebugStringA(szBuf);

		     m_oVirtualHID.InputPoints(penInfo, penCount, penInfo1, penCount1, pLightSpots->spotParam.dwCameraId, pLightSpots->nMonitorId);
		    return;
		}//if
	}

	//////���GLBoard�װ��Ƿ��Ǵ򿪵�
    bool bHandHID2Me = DoGLBoardGestureRecognition(pLightSpots, nLightSpotCount);


    if (bHandHID2Me)
    {//GLBoard����δ����
        if (!m_bLastHIDOwnnerAfterGR)
        {//��һ���ǰװ�����ʶ�����������豸�����Ȱ��豸Reset
            //m_oVirtualHID.Reset();
            g_oGLBoardGR.ResetSmartMathch();
        }
        //m_bIsSmartPenReset = false;

        if (S_OK != g_hr13 || m_oVirtualHID.GetTouchTUIOMode() == false)
        {
            if ((!DoWindowsGestureRecognition(pLightSpots, nLightSpotCount, penInfo, penCount)) && (!DoWindowsGestureRecognition(pLightSpots1, nLightSpotCount1, penInfo1, penCount1)))
            {

                //for (int index2 = 0; index2 < penCount; index2++)
                //{
                //    char szBuf[1024] = { 0 };
                //    sprintf(szBuf, "888 111 penInfo dwCameraId:%d, gunId:%d, penCount:%d,pt x:%d, y:%d, uId:%d, ePenState:%d\n",
                //        penInfo[index2].dwCameraId,
                //        penInfo[index2].gunId,
                //        penCount,
                //        penInfo[index2].pt.x,
                //        penInfo[index2].pt.y,
                //        penInfo[index2].uId,
                //        penInfo[index2].ePenState);
                //    OutputDebugStringA(szBuf);
                //}

               //ƽ���ʼ�
               //m_oStrokFilter.DoFilter(penInfo, penCount, m_dbSmoothCoefficient);

#ifdef _DEBUG

                if (g_hDebugSampleFile2)
                {
                    for (int i = 0; i < nLightSpotCount; i++)
                    {
                        char szData[128];

                        sprintf_s(
                            szData,
                            _countof(szData),
                            "%d,%d------%d,%d------%d\n",
                            pLightSpots[i].spotParam.ptPosInScreen.x,
                            pLightSpots[i].spotParam.ptPosInScreen.y,
                            penInfo[i].pt.x,
                            penInfo[i].pt.y,
                            nLightSpotCount);

                        fwrite(szData, 1, strlen(szData), g_hDebugSampleFile2);
                        fflush(g_hDebugSampleFile2);
                    }
                }
#endif

                TSensorModeConfig* TSensorModeConfig = NULL;
                EProjectionMode eProjectionMode = g_tSysCfgData.globalSettings.eProjectionMode;

                TSensorModeConfig = &g_tSysCfgData.vecSensorConfig[0].vecSensorModeConfig[eProjectionMode];

                BOOL bEnableStrokeInterpolateTemp = TSensorModeConfig->advanceSettings.bEnableStrokeInterpolate;

                if (FALSE == bEnableStrokeInterpolateTemp)
                {
                    //if (penCount > 0)
                    //for(int index=0; index < nLightSpotCount; index++)
                    //{
                    //    char szBuf[1024] = { 0 };
                    //    sprintf(szBuf, "888 pLightSpots dwCameraId:%d, gunId:%d, nLightSpotCount:%d, ptPosInScreen x:%d, y:%d\n", 
                    //        pLightSpots[index].spotParam.dwCameraId, 
                    //        pLightSpots[index].spotParam.gunId,
                    //        nLightSpotCount,
                    //        pLightSpots[index].spotParam.ptPosInScreen.x,
                    //        pLightSpots[index].spotParam.ptPosInScreen.y);
                    //    OutputDebugStringA(szBuf);
                    //}

                    //for (int index1 = 0; index1 < penCount; index1++)
                    //{
                    //    char szBuf[1024] = { 0 };
                    //    sprintf(szBuf, "888 penInfo dwCameraId:%d, gunId:%d, penCount:%d,pt x:%d, y:%d, uId:%d, ePenState:%d\n", 
                    //        penInfo[index1].dwCameraId, 
                    //        penInfo[index1].gunId, 
                    //        penCount,
                    //        penInfo[index1].pt.x,
                    //        penInfo[index1].pt.y,
                    //        penInfo[index1].uId,
                    //        penInfo[index1].ePenState);
                    //    OutputDebugStringA(szBuf);
                    //}

                    //����ֵ
                    m_oVirtualHID.InputPoints(penInfo, penCount, penInfo1, penCount1, pLightSpots->spotParam.dwCameraId, pLightSpots->nMonitorId);
#ifdef _DEBUG
                    //  DebugContactInfo(penInfo, penCount);
#endif
                }
                else
                {
                    //��Bezier��ֵ
                    m_oBezierInterpolator.DoInterpolate(penInfo, penCount);
                    
                    const CBezierInterpolator<PEN_NUM>::InterpolateContainer& container = m_oBezierInterpolator.GetInterpolateResults();

                    int nSlotCount = container.GetSlotCount();

                    for (int slot = 0; slot < nSlotCount; slot++)
                    {
                        const TContactInfo* pInterpolateContact;
                        int nItemCount = container.GetSlotData(slot, &pInterpolateContact);

                        if (nItemCount > 0)
                        {
#ifdef _DEBUG
                            //DebugContactInfo(pInterpolateContact, nItemCount);
#endif
                            m_oInterpolateDispatcher.PreProcess(pInterpolateContact, nItemCount);

                            int allPenCount;
                            const TContactInfo* pAllContactInfo = m_oInterpolateDispatcher.GetAllContactData(&allPenCount);

                            char szBuf[1024] = { 0 };
                            sprintf(szBuf, "999 OnPostProcess InputPoints pLightSpots dwCameraId:%d\n", pLightSpots->spotParam.dwCameraId);
                            OutputDebugStringA(szBuf);

                            m_oVirtualHID.InputPoints((TContactInfo*)pAllContactInfo, allPenCount, NULL, 0, pLightSpots->spotParam.dwCameraId, pLightSpots->nMonitorId);

                            m_oInterpolateDispatcher.PostProcess();

                            //�ӳ�1ms
                            Sleep(1);
                        }
                        else
                        {
                            break;
                        }
                    }//for

                    m_oBezierInterpolator1.DoInterpolate(penInfo1, penCount1);
                    const CBezierInterpolator<PEN_NUM>::InterpolateContainer& container1 = m_oBezierInterpolator1.GetInterpolateResults();
                    int nSlotCount1 = container1.GetSlotCount();
                    for (int slot = 0; slot < nSlotCount1; slot++)
                    {
                        const TContactInfo* pInterpolateContact;
                        int nItemCount = container1.GetSlotData(slot, &pInterpolateContact);

                        if (nItemCount > 0)
                        {
#ifdef _DEBUG
                            //DebugContactInfo(pInterpolateContact, nItemCount);
#endif
                            m_oInterpolateDispatcher1.PreProcess(pInterpolateContact, nItemCount);

                            int allPenCount;
                            const TContactInfo* pAllContactInfo = m_oInterpolateDispatcher1.GetAllContactData(&allPenCount);

                            char szBuf[1024] = { 0 };
                            sprintf(szBuf, "999 OnPostProcess InputPoints pLightSpots dwCameraId:%d\n", pLightSpots->spotParam.dwCameraId);
                            OutputDebugStringA(szBuf);

                            m_oVirtualHID.InputPoints( NULL, 0, (TContactInfo*)pAllContactInfo, allPenCount, pLightSpots->spotParam.dwCameraId, pLightSpots->nMonitorId);

                            m_oInterpolateDispatcher1.PostProcess();

                            //�ӳ�1ms
                            Sleep(1);
                        }
                        else
                        {
                            break;
                        }
                    }//for

                }
            }
            else
            {//Windows�����Ѵ���
             //��λ���ƴ�����־

                if (!m_bIsTriggeringGuesture)
                {
                    bNeedReset = TRUE;
                    m_bIsTriggeringGuesture = TRUE;
                }

            }
        }
        else
        {
            //if (!DoWindowsGestureRecognition(pLightSpots, nLightSpotCount, penInfo, penCount))
            if (TRUE)
            {
                for (int spotIndex = 0; spotIndex < nLightSpotCount; spotIndex++)
                {
                    penInfo[spotIndex].dwCameraId = pLightSpots[spotIndex].spotParam.dwCameraId;
                    penInfo[spotIndex].gunId = pLightSpots[spotIndex].spotParam.gunId;
                    penInfo[spotIndex].pt = pLightSpots[spotIndex].spotParam.ptPosInScreen;
                    penInfo[spotIndex].uId = m_uid++;
                    penInfo[spotIndex].ePenState = E_PEN_STATE_DOWN;
                }
                penCount = nLightSpotCount;

                /*****************************************************************/
                for (int spotIndex1 = 0; spotIndex1 < nLightSpotCount1; spotIndex1++)
                {
                    penInfo1[spotIndex1].dwCameraId = pLightSpots1[spotIndex1].spotParam.dwCameraId;
                    penInfo1[spotIndex1].gunId = pLightSpots1[spotIndex1].spotParam.gunId;
                    penInfo1[spotIndex1].pt = pLightSpots1[spotIndex1].spotParam.ptPosInScreen;
                    penInfo1[spotIndex1].uId = m_uid++;
                    penInfo1[spotIndex1].ePenState = E_PEN_STATE_DOWN;
                }
                penCount1 = nLightSpotCount1;
                /****************************************************************/

                for (int index2 = 0; index2 < penCount; index2++)
                {
                    char szBuf[1024] = { 0 };
                    sprintf(szBuf, "888 111 penInfo dwCameraId:%d, gunId:%d, penCount:%d,pt x:%d, y:%d, uId:%d, ePenState:%d\n",
                        penInfo[index2].dwCameraId,
                        penInfo[index2].gunId,
                        penCount,
                        penInfo[index2].pt.x,
                        penInfo[index2].pt.y,
                        penInfo[index2].uId,
                        penInfo[index2].ePenState);
                    OutputDebugStringA(szBuf);
                }

                //ƽ���ʼ�
                //m_oStrokFilter.DoFilter(penInfo, penCount, m_dbSmoothCoefficient);

#ifdef _DEBUG

                if (g_hDebugSampleFile2)
                {
                    for (int i = 0; i < nLightSpotCount; i++)
                    {
                        char szData[128];

                        sprintf_s(
                            szData,
                            _countof(szData),
                            "%d,%d------%d,%d------%d\n",
                            pLightSpots[i].spotParam.ptPosInScreen.x,
                            pLightSpots[i].spotParam.ptPosInScreen.y,
                            penInfo[i].pt.x,
                            penInfo[i].pt.y,
                            nLightSpotCount);

                        fwrite(szData, 1, strlen(szData), g_hDebugSampleFile2);
                        fflush(g_hDebugSampleFile2);
                    }
                }
#endif

                TSensorModeConfig* TSensorModeConfig = NULL;
                EProjectionMode eProjectionMode = g_tSysCfgData.globalSettings.eProjectionMode;

                TSensorModeConfig = &g_tSysCfgData.vecSensorConfig[0].vecSensorModeConfig[eProjectionMode];

                BOOL bEnableStrokeInterpolateTemp = TSensorModeConfig->advanceSettings.bEnableStrokeInterpolate;

                if (FALSE == bEnableStrokeInterpolateTemp)
                {
                    //if (penCount > 0)
                    for (int index = 0; index < nLightSpotCount; index++)
                    {
                        char szBuf[1024] = { 0 };
                        sprintf(szBuf, "888 pLightSpots dwCameraId:%d, gunId:%d, nLightSpotCount:%d, ptPosInScreen x:%d, y:%d\n",
                            pLightSpots[index].spotParam.dwCameraId,
                            pLightSpots[index].spotParam.gunId,
                            nLightSpotCount,
                            pLightSpots[index].spotParam.ptPosInScreen.x,
                            pLightSpots[index].spotParam.ptPosInScreen.y);
                        OutputDebugStringA(szBuf);
                    }

                    for (int index1 = 0; index1 < penCount; index1++)
                    {
                        char szBuf[1024] = { 0 };
                        sprintf(szBuf, "888 penInfo dwCameraId:%d, gunId:%d, penCount:%d,pt x:%d, y:%d, uId:%d, ePenState:%d\n",
                            penInfo[index1].dwCameraId,
                            penInfo[index1].gunId,
                            penCount,
                            penInfo[index1].pt.x,
                            penInfo[index1].pt.y,
                            penInfo[index1].uId,
                            penInfo[index1].ePenState);
                        OutputDebugStringA(szBuf);
                    }

                    //����ֵ
                    m_oVirtualHID.InputPoints(penInfo, penCount, penInfo1, penCount1, pLightSpots->spotParam.dwCameraId, pLightSpots->nMonitorId);
#ifdef _DEBUG
                    //  DebugContactInfo(penInfo, penCount);
#endif
                }
                else
                {
                    //��Bezier��ֵ
                    m_oBezierInterpolator.DoInterpolate(penInfo, penCount);
                    const CBezierInterpolator<PEN_NUM>::InterpolateContainer& container = m_oBezierInterpolator.GetInterpolateResults();
                    int nSlotCount = container.GetSlotCount();
                    for (int slot = 0; slot < nSlotCount; slot++)
                    {
                        const TContactInfo* pInterpolateContact;
                        int nItemCount = container.GetSlotData(slot, &pInterpolateContact);
                        if (nItemCount > 0)
                        {
#ifdef _DEBUG
                            //DebugContactInfo(pInterpolateContact, nItemCount);
#endif
                            m_oInterpolateDispatcher.PreProcess(pInterpolateContact, nItemCount);
                            int allPenCount;
                            const TContactInfo* pAllContactInfo = m_oInterpolateDispatcher.GetAllContactData(&allPenCount);
                            char szBuf[1024] = { 0 };
                            sprintf(szBuf, "999 OnPostProcess InputPoints pLightSpots dwCameraId:%d\n", pLightSpots->spotParam.dwCameraId);
                            OutputDebugStringA(szBuf);
                            m_oVirtualHID.InputPoints((TContactInfo*)pAllContactInfo, allPenCount, NULL, 0, pLightSpots->spotParam.dwCameraId, pLightSpots->nMonitorId);
                            m_oInterpolateDispatcher.PostProcess();
                            //�ӳ�1ms
                            Sleep(1);
                        }
                        else
                        {
                            break;
                        }
                    }//for

                    /*****************************************/
                    m_oBezierInterpolator1.DoInterpolate(penInfo1, penCount1);
                    const CBezierInterpolator<PEN_NUM>::InterpolateContainer& container1 = m_oBezierInterpolator1.GetInterpolateResults();
                    int nSlotCount1 = container1.GetSlotCount();
                    for (int slot1 = 0; slot1 < nSlotCount1; slot1++)
                    {
                        const TContactInfo* pInterpolateContact;
                        int nItemCount = container.GetSlotData(slot1, &pInterpolateContact);
                        if (nItemCount > 0)
                        {
#ifdef _DEBUG
                            //DebugContactInfo(pInterpolateContact, nItemCount);
#endif
                            m_oInterpolateDispatcher1.PreProcess(pInterpolateContact, nItemCount);
                            int allPenCount;
                            const TContactInfo* pAllContactInfo = m_oInterpolateDispatcher1.GetAllContactData(&allPenCount);
                            char szBuf[1024] = { 0 };
                            sprintf(szBuf, "999 OnPostProcess InputPoints pLightSpots dwCameraId:%d\n", pLightSpots->spotParam.dwCameraId);
                            OutputDebugStringA(szBuf);
                            m_oVirtualHID.InputPoints(NULL, 0, (TContactInfo*)pAllContactInfo, allPenCount, pLightSpots->spotParam.dwCameraId, pLightSpots->nMonitorId);
                            m_oInterpolateDispatcher1.PostProcess();
                            //�ӳ�1ms
                            Sleep(1);
                        }
                        else
                        {
                            break;
                        }
                    }//for
                    /*****************************************/
                }
            }
            else
            {//Windows�����Ѵ���
             //��λ���ƴ�����־

                if (!m_bIsTriggeringGuesture)
                {
                    bNeedReset = TRUE;
                    m_bIsTriggeringGuesture = TRUE;
                }

            }
        }
    }
    else
    {//GLBoard�����Ѵ���

        //��λ���ƴ�����־
        if (!m_bIsTriggeringGuesture)
        {
			bNeedReset = TRUE;
            m_oSmartPenMatch.Reset();
			m_bIsTriggeringGuesture = TRUE;
        }
    }


	if (bNeedReset)
	{	
		m_oBezierInterpolator.Reset();
        m_oBezierInterpolator1.Reset();
		m_oInterpolateDispatcher.Reset();
        m_oInterpolateDispatcher1.Reset();
	}

    m_bLastHIDOwnnerAfterGR = bHandHID2Me;
}

bool CSpotListProcessor::DoGLBoardGestureRecognition(TLightSpot* pLightSpots, int &nLightSpotCount)
{
    bool bHandHID2Me = true;

    //<<commented by Jiqw : 2015/04/20
    //<<commented reason : �ʴ���ģʽ��֧�ֶ��
    //if (GetActualTouchType() == E_DEVICE_PEN_TOUCH_WHITEBOARD)
    //{//�ʴ���ģʽ�²�֧������ʶ��
    //    return true;
    //}
    //<<commented by Jiqw : 2015/04/20

     if (g_oGLBoardGR.FindTboardWnd(pLightSpots, nLightSpotCount))
    {
        //g_oGLBoardGR.SetIsTouchPadMode(m_oVirtualHID.GetHIDMode() == E_DEV_MODE_TOUCHSCREEN);
        bool isMultiPenMode = g_oGLBoardGR.IsMultiPenMode();
		switch (GetActualTouchType())
		{
		   case E_DEVICE_PEN_TOUCH_WHITEBOARD:
			   //�ʴ���ģʽ�²�֧������ʶ��
			   if (isMultiPenMode == false) return true;
			   g_oGLBoardGR.SetIsPenTouchDevice(true);

			   break; 
		   case E_DEVICE_FINGER_TOUCH_WHITEBOARD:

			   g_oGLBoardGR.SetIsPenTouchDevice(false);
			   break;
		   case E_DEVICE_FINGER_TOUCH_CONTROL:

			   g_oGLBoardGR.SetIsPenTouchDevice(true);
			   break;
		   case E_DEVICE_PALM_TOUCH_CONTROL:

			   g_oGLBoardGR.SetIsPenTouchDevice(true);
			   break;
		   default:
			  break;
		}
//        if (GetActualTouchType() == E_DEVICE_PEN_TOUCH_WHITEBOARD)
//        {//�ʴ���ģʽ�²�֧������ʶ��
//            if (isMultiPenMode == false) return true;
//            g_oGLBoardGR.SetIsPenTouchDevice(true);
//        }
//        else
//        {
//            g_oGLBoardGR.SetIsPenTouchDevice(false);
//        }

        bHandHID2Me = g_oGLBoardGR.DoDetection(pLightSpots, nLightSpotCount);
    }    
    /*else
    {
    if (!m_bLastHIDOwnnerAfterGR)
    {
    g_oGLBoardGR.ResetSmartMathch();
    }
    }*/

    return bHandHID2Me;
}

bool CSpotListProcessor::DoWindowsGestureRecognition(const TLightSpot* pLightSpots, int nLightSpotCount, TContactInfo *penInfo, int &nPenCount)
{
    //POINT pts[MAX_CAMERA_NUMBER*MAX_OBJ_NUMBER];
    TLightSpotParam spotParam[MAX_CAMERA_NUMBER * MAX_OBJ_NUMBER];
    for(int i=0; i< nLightSpotCount; i++)
    {
        //pts[i] = pLightSpots[i].spotParam.ptPosInScreen;
        spotParam[i] = pLightSpots[i].spotParam;
    }

    m_oSmartPenMatch.DoMatch(spotParam, nLightSpotCount);

    int nElementCount = 0;
    const TMatchInfo* pMatchInfo =  m_oSmartPenMatch.GetAllMatchInfo(&nElementCount); 
	bool bContinedType = false ;
	if (GetActualTouchType() == E_DEVICE_FINGER_TOUCH_WHITEBOARD || GetActualTouchType() == E_DEVICE_FINGER_TOUCH_CONTROL)
	{
		bContinedType = true;
	}
    bool toBeContinued = (bContinedType && !g_oGLBoardGR.IsInputInGLBorad()) ? g_oWinGR.DetermineWhethertoEnterWGR(pLightSpots, nLightSpotCount) : false;

    if (toBeContinued)
    {    
        //CPerfDetector perf(_T("**********CalcMoveDirectionByStatistics()"));
        for(int i = 0; i < nElementCount; i++)
        {
            const TMatchInfo &refMInfo = pMatchInfo[i];

            if (refMInfo.eMatchState == E_MISMATCHED)
            {
                g_oWinGR.AddTouchAfterMatch(refMInfo.uId, refMInfo.ptPos, 0);
                penInfo[i].ePenState = E_PEN_STATE_UP;
            }
            else
            {
                if (refMInfo.eMatchState == E_MATCHED)
                {
                    g_oWinGR.AddTouchAfterMatch(refMInfo.uId, refMInfo.ptPos, 1);
                }
                else
                {
                    g_oWinGR.AddTouchAfterMatch(refMInfo.uId, refMInfo.ptPos, 2);
                }

                penInfo[i].ePenState = E_PEN_STATE_DOWN;
            }

            penInfo[i].uId       = refMInfo.uId;
            penInfo[i].pt        = refMInfo.ptPos;

            penInfo[i].dwCameraId = refMInfo.dwCameraId;
            penInfo[i].gunId = refMInfo.gunId;

            //penInfo[i].bIgnored = FALSE;
        }

        /*CPerfDetector perf(_T("****DoWindowsGestureRecognition()"));*/
        toBeContinued = g_oWinGR.DoRecongition();
    }
    else
    {
        for(int i = 0; i < nElementCount; i++)
        {
            const TMatchInfo &refMInfo = pMatchInfo[i];
            penInfo[i].ePenState = (refMInfo.eMatchState == E_MISMATCHED)?E_PEN_STATE_UP:E_PEN_STATE_DOWN;
            penInfo[i].uId       = refMInfo.uId;
            penInfo[i].pt        = refMInfo.ptPos;  

            penInfo[i].dwCameraId = refMInfo.dwCameraId;
            penInfo[i].gunId = refMInfo.gunId;
            //penInfo[i].bIgnored  = FALSE;

        }
    }    

    nPenCount = nElementCount;

    return toBeContinued;    
}

//@����:��λ��ߴ�����
void CSpotListProcessor::Reset()
{
    for(int i = 0; i < MAX_CAMERA_NUMBER; i++)
    {
        //m_ValidSpotListGroup.aryLightSpotsCount[i] = 0 ;
        m_InputSpotListGroup.aryLightSpotsCount[i] =0;
        m_InputSpotListGroup.aryLightSpotsCount1[i] = 0;
    }

    //ƥ�临λ
    m_oSmartPenMatch.Reset();

    //Bezier��ֵ����λ
    this->m_oBezierInterpolator.Reset();

    this->m_oBezierInterpolator1.Reset();

    m_oInterpolateDispatcher.Reset();

    m_oInterpolateDispatcher1.Reset();

    m_SpotListGroupFIFO.Reset();
}

//@����:��������ͷ�ĸ���
//@����:uCameraCount,����ͷ����
//void CSpotListProcessor::SetCameraCount(UINT uCameraCount)
//{
//    m_uCameraCount = uCameraCount;
//}

//@����:���������߳�
BOOL CSpotListProcessor::StartProcessThread()
{
    assert(NULL == m_hProcessThread);
    DWORD dwThreadId = 0;//�߳�Id
	
	ResetEvent(this->m_hExitEvent);
    m_hProcessThread = CreateThread(
        NULL,//lpThreadAttributes
        0,   //dwStackSize
        ProcessThreadProc,//lpStartAddress,
        (LPVOID)this,     //lpParameter,
        0,                //dwCreationFlags
        &dwThreadId       //lpThreadId
        );

    return NULL != m_hProcessThread;
}

//BOOL CSpotListProcessor::StartProcessThread1()
//{
//    assert(NULL == m_hProcessThread1);
//    DWORD dwThreadId1 = 0;//�߳�Id
//
//    ResetEvent(this->m_hExitEvent);
//    m_hProcessThread1 = CreateThread(
//        NULL,//lpThreadAttributes
//        0,   //dwStackSize
//        ProcessThreadProc1,//lpStartAddress,
//        (LPVOID)this,     //lpParameter,
//        0,                //dwCreationFlags
//        &dwThreadId1       //lpThreadId
//    );
//
//    return NULL != m_hProcessThread;
//}

//@����:ֹͣ�����߳�
void CSpotListProcessor::StopProcessThread()
{
    //�����߳��˳��¼�
    SetEvent(m_hExitEvent);
    WaitForSingleObject(m_hProcessThread, INFINITE);
	CloseHandle(m_hProcessThread);
    m_hProcessThread = NULL;    
}


//@����:�̴߳�����
ULONG _stdcall CSpotListProcessor::ProcessThreadProc(LPVOID lpCtx)
{
    //����̵߳����ȼ���
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

    CSpotListProcessor* lpThis = reinterpret_cast<CSpotListProcessor*>(lpCtx);

    HANDLE hEvents[2];
    hEvents[0] = lpThis->m_hWriteEvent;
    hEvents[1] = lpThis->m_hExitEvent;

    BOOL bExit = FALSE;

    for(;!bExit;)
    {
        DWORD dwWaitResult = WaitForMultipleObjects(_countof(hEvents), hEvents, FALSE, INFINITE);

        switch(dwWaitResult)
        {   
        case WAIT_OBJECT_0://�µ�д���¼�
            if(lpThis->m_bSimulateMode)
            {
                lpThis->SimulateProcess();
            }
            else
            {
                lpThis->ProcessLightSpots();
            }

            break;

        case WAIT_OBJECT_0 + 1://�߳��˳��¼�
            bExit = TRUE;
            break;

        }//switch
    }
    return 0UL;
}

//ULONG _stdcall CSpotListProcessor::ProcessThreadProc1(LPVOID lpCtx)
//{
//    //����̵߳����ȼ���
//    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
//
//    CSpotListProcessor* lpThis = reinterpret_cast<CSpotListProcessor*>(lpCtx);
//
//    HANDLE hEvents[2];
//    hEvents[0] = lpThis->m_hWriteEvent;
//    hEvents[1] = lpThis->m_hExitEvent;
//
//    BOOL bExit = FALSE;
//
//    for (; !bExit;)
//    {
//        DWORD dwWaitResult = WaitForMultipleObjects(_countof(hEvents), hEvents, FALSE, INFINITE);
//
//        switch (dwWaitResult)
//        {
//        case WAIT_OBJECT_0://�µ�д���¼�
//            if (lpThis->m_bSimulateMode)
//            {
//                lpThis->SimulateProcess();
//            }
//            else
//            {
//                lpThis->ProcessLightSpots();
//            }
//
//            break;
//
//        case WAIT_OBJECT_0 + 1://�߳��˳��¼�
//            bExit = TRUE;
//            break;
//
//        }//switch
//    }
//    return 0UL;
//}

//���Ժ���
void CSpotListProcessor::SimulateProcess()
{
    TLightSpot allLightSpots[MAX_CAMERA_NUMBER*MAX_OBJ_NUMBER];
    FILE* file = NULL;
    fopen_s(&file, "simul_input_data.txt", "r");
    int nCount = 0;
    int nGroupId = 0;
    if(file)
    {
        char szText[1024];
        int nLastNo = 0;
        int nNo = 0;


        TLightSpot spot;
        memset(&spot, 0, sizeof(spot));

        int nSpotCount = 0;

        while(fgets(szText, _countof(szText),file))
        {

            sscanf_s(szText, 
                "%d,%d,%d,%d,%d",
                &nNo,
                &spot.lStdSpotAreaInVideo,
                &spot.spotParam.ptPosInScreen.x,
                &spot.spotParam.ptPosInScreen.y,
                &spot.mass
                );
            if(nNo != nLastNo)
            {

                if(nSpotCount != 0)
                {
                    nGroupId ++;

                    char szBuf[1024] = { 0 };
                    sprintf(szBuf, "999 SimulateProcess OnPostProcess\n");
                    OutputDebugStringA(szBuf);

                    OnPostProcess(&allLightSpots[0], nSpotCount, NULL, 0);
                    Sleep(1);
                }

                nSpotCount = 0;
            }

            allLightSpots[nSpotCount] = spot;
            nSpotCount ++;

            nLastNo = nNo;

            nCount ++;

        }
    }

    if(file)
    {
        fclose(file);
    }

    m_bSimulateMode = FALSE;
}

//@����:��Ļ�ֱ��ʱ仯�¼���Ӧ����
//@����:nScreenWidth, �µ���Ļ���
//      nScreenHeight,�µ���Ļ�߶�
void CSpotListProcessor::OnDisplayChange(int nScreenWidth, int nScreenHeight)
{
    //�Խ��߳ߴ磬��λ:����(mm)
	int nDiagonalPhysicalLength = int(g_tSysCfgData.globalSettings.fScreenDiagonalPhysicalLength);

    SIZE sizeScreen;
    sizeScreen.cx = nScreenWidth;
    sizeScreen.cy = nScreenHeight;

    //�����������������Ļ����ߴ����Ļ�ֱ���
    this->m_oVirtualHID.OnSetTouchScreenDimension(nDiagonalPhysicalLength, sizeScreen);

    //���ʼ�����㷨������Ļ����ߴ����Ļ�ֱ���
    this->m_oSmartPenMatch.OnSetTouchScreenDimension(nDiagonalPhysicalLength, sizeScreen);

    //���ʼ��˲���������Ļ����ߴ����Ļ�ֱ���
    this->m_oStrokFilter.OnSetTouchScreenDimension(nDiagonalPhysicalLength, sizeScreen);

    //֪ͨ"��ߺϲ�ģ��"��Ļ�ֱ��ʷ����˱仯
    if(theApp.ScreenMode() >= EScreenModeDouble)
    {
        this->m_oSpotMerger.OnDisplayChange(nScreenWidth,  nScreenHeight);
    }

    //��GLBoard���ƺͱʼ�������Ļ����ߴ����Ļ�ֱ���
    g_oGLBoardGR.OnSetTouchScreenDimension(nDiagonalPhysicalLength, sizeScreen);

	this->m_oVirtualHID.SetTUIOScreenDisplayChange(sizeScreen.cx, sizeScreen.cy);

}

//@����:�ж��Ƿ����ڴ�������
BOOL CSpotListProcessor::IsTriggeringGuesture()const
{
    return m_bIsTriggeringGuesture;
}


//@����:��ȡ�����������Ļ�������򡣸�������������Ͻ������ڽӵ��ں���
//@����:uCameraIndex, id of camera sensor
//     monitorArea, �����Ч��Ͻ��������
RECT CSpotListProcessor::GetVisibleScreenArea(UINT uCameraIndex, const RECT& monitorArea) const
{
    RECT rcVisibleArea = monitorArea;
#if 0    
    if (theApp.ScreenMode() >= EScreenModeDouble)
    {
        int nMergeAreaIndex = 0;
        if (uCameraIndex == 0)
        {  //ֻ���ұߵ�һ���ں���
            const RECT* pMergeArea = m_oSpotMerger.GetMergeArea(nMergeAreaIndex);
            if(pMergeArea) rcVisibleArea.right = pMergeArea->right;
        }
        else if (uCameraIndex == m_uCameraCount - 1)
        {//ֻ����ߵ�һ���ں���
            nMergeAreaIndex = uCameraIndex - 1; 
            const RECT* pMergeArea = m_oSpotMerger.GetMergeArea(nMergeAreaIndex);
            if (pMergeArea) rcVisibleArea.left = pMergeArea->left;
        }
        else
        {//�������߸���һ���ں���

            //��ߵ��ں���
            nMergeAreaIndex = uCameraIndex - 1;
            const RECT* pLeftMergeArea = m_oSpotMerger.GetMergeArea(nMergeAreaIndex);
            
            if(pLeftMergeArea) rcVisibleArea.left = pLeftMergeArea->left;
            
            //�ұߵ��ں���
            nMergeAreaIndex = uCameraIndex;
            const RECT* pRightMergeArea = m_oSpotMerger.GetMergeArea(nMergeAreaIndex);
            if(pRightMergeArea) rcVisibleArea.right = pRightMergeArea->right;
        }
    }
#endif
    return rcVisibleArea;
}


CToleranceDistribute& CSpotListProcessor::GetToleranceDistribute()
{
    return this->m_oStrokFilter.GetToleranceDistribute();
}

void CSpotListProcessor::SetSmoothCoefficient(int nSmoothCoff)
{
	m_dbSmoothCoefficient = (double)nSmoothCoff / 10;
}


void CSpotListProcessor::EnableAutoMerge(bool bEnable)
{
    m_bAutoMerge = bEnable;
}