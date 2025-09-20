#include "stdafx.h"
//#include "headers.h"

//CVideToScreenMap g_oVideo2ScreenMap;
const  double CVideToScreenMap::SCREEN_OUT_AREA_WIDTH_IN_MM = 10.0;

//class CVideToScreenMap
CVideToScreenMap::CVideToScreenMap(int nID)
:
m_bManualCorrectDataIsValid(FALSE),
m_nMouseXDeviationCompensationVal(0),
m_nMouseYDeviationCompensationVal(0),
m_nID(nID),
m_bIsRearProjectMode(FALSE),
m_ScreenOutAreaWidthInPixel(0)
{
	int nCxScreen    = GetSystemMetrics(SM_CXSCREEN);
	int nCyScreen    = GetSystemMetrics(SM_CYSCREEN);

    m_tCalibData.szImage.cx = DEFAULT_VIDEO_WIDTH;
    m_tCalibData.szImage.cy = DEFAULT_VIDEO_HEIGHT;

    //const MonitorInfo* pMonitorInfo = theApp.GetMonitorFinder().GetMonitorInfo(nID);
    //if(pMonitorInfo)
    //{
    //    m_rcMonitorResolution = pMonitorInfo->rcMonitor;
    //}
    //else
    //{
    //    m_rcMonitorResolution.left   = 0;
    //    m_rcMonitorResolution.top    = 0;
    //    m_rcMonitorResolution.right  = GetSystemMetrics(SM_CXSCREEN);
    //    m_rcMonitorResolution.bottom = GetSystemMetrics(SM_CYSCREEN);
    //}
    //m_rcMonitorResolution��CIWBSensor����
	m_rcMonitorResolution.left   = 0;
	m_rcMonitorResolution.top    = 0;
	m_rcMonitorResolution.right  = 0;
	m_rcMonitorResolution.bottom = 0;


    LoadScreenMap();
}


CVideToScreenMap::~CVideToScreenMap()
{
	//if(m_pMapData != NULL) delete[] m_pMapData;

	//PROFILE::SaveLenType(g_eCalibrateType);
    SaveScreenMap();
}

//@����:����У�������Ļ�����ʵ������Ĺ�ʵ�����(���ձ�������Ļ�ߴ����)
//
//@˵��:��������ݶ������˳ߴ�Ŵ�(2013/04/11)
//BOOL CVideToScreenMap::SetCalibrateData(int nPos, const POINT& ptScreen, const POINT& ptImage ,BOOL RearProjection)
BOOL CVideToScreenMap::SetCalibrateData(const TCalibData& calibData )
{
    m_tCalibData = calibData;
   
	if (m_bIsRearProjectMode && calibData.eCalibType == E_CALIBRATE_TYPE_MANUAL)
	{
		//��Ͷģʽ��,�ֶ��ɼ�����У�����ڼ������Ļ�ϵ�����������ϵ�ģ����ǵõ���ͼ���е�����
		//��ֲ�ȴ������ϵ�ģ� �����Ҫ��תУ�������Ļˮƽ���ꡣ
        UINT nMonitorCount = m_tCalibData.allMonitorCalibData.size();
        for(UINT nDispId = 0; nDispId < nMonitorCount; nDispId ++)
        {
			const RECT& rcMonitor = m_tCalibData.allMonitorCalibData[nDispId].rcMonitor;
			int nMonitorWidth = rcMonitor.right - rcMonitor.left;

            std::vector<TCalibCoordPair>& cliabData = m_tCalibData.allMonitorCalibData[nDispId].calibData;
            size_t nCalibDataCount = cliabData.size();

            for(UINT i = 0; i < nCalibDataCount; i++)
            {
                int nOldScreenX = cliabData[i].ptScreenCoord.x;
                 cliabData[i].ptScreenCoord.x = nMonitorWidth - nOldScreenX;
            }

        }//for-each(nDispId)

    }


    return TRUE;
}



//BOOL CVideToScreenMap::SetAdjustPtNumber(size_t nNewNumber)
//{
//	if(m_tCalibrateData.vecCalibratePoints.size() != nNewNumber)
//	{
//		m_tCalibrateData.vecCalibratePoints.resize(nNewNumber);
//
//	}
//	return TRUE;
//}

//void CVideToScreenMap::SetCalibrateData(const TCalibrateData& adjustData)
//{
//	m_tCalibrateData = adjustData;
//}

const TCalibData& CVideToScreenMap::GetCalibrateData() const
{
	return m_tCalibData;
}



BOOL CVideToScreenMap::ChangeImageSize(int nNewImageWidth, int nNewImageHeight)
{
    if(m_tCalibData.szImage.cx != nNewImageWidth || m_tCalibData.szImage.cy != nNewImageHeight)
	{
		//�����µ���Ƶ�ߴ����У������
		// std::vector<CalibratePair> & vecAdjustData = m_tCalibrateData.vecCalibratePoints;
		//for(int i= 0; i < MANUAL_CALIBRATE_POINT_NUMBER; i++)
		//{
		//	vecAdjustData[i].second.x  = vecAdjustData[i].second.x * nNewImageWidth / m_tCalibrateData.nImageWidth;
		//	vecAdjustData[i].second.y  = vecAdjustData[i].second.y * nNewImageHeight / m_tCalibrateData.nImageHeight;
		//}

		//delete[]m_pMapData;

		//m_pMapData = new POINT[nNewImageWidth * nNewImageHeight];

		m_tCalibData.szImage.cx  = nNewImageWidth;
		m_tCalibData.szImage.cy  = nNewImageHeight;

		//<<!
		//DoCalibrate();
		//!>>

	}

	return TRUE;
}

//BOOL CVideToScreenMap::OnOnChangeScreenSize(int nNewScreenWidth, int nNewScreenHeight)
//{
//   // if(m_tCalibData.szScreen.cx != nNewScreenWidth || m_tCalibData.szScreen.cy != nNewScreenHeight)
//	{
//		//comment:2010-03-08
//		//m_pMapData�е������ǹ�һ����0~65535�ģ� 
//		//
//		//�����µ���Ƶ�ߴ����У������
//		//std::vector<CalibratePair> & vecAdjustData = m_tCalibrateData.vecCalibratePoints;
//		//for(int i= 0; i < E_ADJUST_POINT_NUMBER; i++)
//		//{
//		//	vecAdjustData[i].second.x  = vecAdjustData[i].second.x * nNewScreenWidth / m_tCalibrateData.nScreenWidth;
//		//	vecAdjustData[i].second.y  = vecAdjustData[i].second.y * nNewScreenHeight / m_tCalibrateData.nScreenHeight;
//		//}
//
//		/*	int nVideoWidth  = m_tCalibrateData.nImageWidth;
//		int nVideoHeight = m_tCalibrateData.nImageHeight;
//
//		for(int y = 0; y < nVideoHeight; y ++)
//		{
//		for(int x = 0; x < nVideoWidth; x ++)
//		{
//		POINT& pt = m_pMapData[x + y*nVideoWidth];
//		pt.x = pt.x * nNewScreenWidth  / m_tCalibrateData.nScreenWidth;
//		pt.y = pt.y * nNewScreenHeight / m_tCalibrateData.nScreenHeight;
//		}
//		}*/
//
//		//m_tCalibrateData.nScreenWidth  = nNewScreenWidth;
//		//m_tCalibrateData.nScreenHeight = nNewScreenHeight;
//
//  //      m_tCalibData.szScreen.cx  = nNewScreenWidth;
//		//m_tCalibData.szScreen.cy = nNewScreenHeight;
//
//	}
//
//	return TRUE;
//}

//void CVideToScreenMap::SetScreenSize(int nNewScreenWidth, int nNewScreenHeight)
//{
//
//  //      m_tCalibData.szScreen.cx  = nNewScreenWidth;
//		//m_tCalibData.szScreen.cy = nNewScreenHeight;
//}


////@����:�����ṩ����Ƶ��������� �õ���Ӧ����Ļ���ꡣ
////@����:x, ˮƽ��Ƶ����
////      y, ��ֱ��Ƶ����
////      bOutSide, ָ�򱣴��Ƿ�����Ļ����֮���־��ָ��
////@����ֵ:��Ļ����ֵ
//POINT CVideToScreenMap::GetPt(int x, int y, bool* pbIsOutside)
//{
//    //CPerfDetector perf(_T("CVideToScreenMap::GetPt"));
//    CComCritSecLock<CComAutoCriticalSection> lock(m_csForCalibrateThreadSafe);
//
//    int nActualMonitorLeft, nActualMonitorTop, nActualMonitorRight, nActualMonitorBottom, nActualMonitorWidth, nActualMonitorHeight;
//    //�̰߳�ȫ����
//    {
//        
//        nActualMonitorLeft   = this->m_rcMonitorResolution.left  ;
//        nActualMonitorTop    = this->m_rcMonitorResolution.top   ;
//        nActualMonitorRight  = this->m_rcMonitorResolution.right ;
//        nActualMonitorBottom = this->m_rcMonitorResolution.bottom;
//
//        nActualMonitorWidth  = this->m_rcMonitorResolution.right  - this->m_rcMonitorResolution.left;
//        nActualMonitorHeight = this->m_rcMonitorResolution.bottom - this->m_rcMonitorResolution.top ;
//    }
//
//
//
//    POINT pt;
//    pt.x = 0;
//    pt.y = 0;
//
//
//    if(pbIsOutside) *pbIsOutside = false;
//    
//	//m_bIsExScreenPositionToMouse = FALSE;
//
//    //ӳ����δ����
//    //int nScaledVideoWidth  = m_tCalibData.szImage.cx  << INT_SCALE_SHIFT_SIZE;
//    //int nScaledVideoHeight = m_tCalibData.szImage.cy << INT_SCALE_SHIFT_SIZE;
//    const TCalibParams* pCalibParams =  GetCalibParams();
//    if(NULL == pCalibParams ) 
//    {
//         return pt;
//    }
//
//
//    //if(pbIsOutside) *pbIsOutside = false;
//    if(  0 <= x && x <  pCalibParams->szImage.cx &&  0<= y && y < pCalibParams->szImage.cy )
//    {
//
//            pt.x = x;
//            pt.y = y;
//            //�жϹ����������һ����Ļ.
//             int nMonitorId = MointorFromPt(pt);
//             if(nMonitorId != -1)
//             {
//	            //pt = this->m_oCalibrateAlog.GetScreenCoord(pt, nMonitorId);
//                BOOL bRet = this->m_oCalibrateAlog.GetScreenCoord(pt, &pt, nMonitorId);
//                if(!bRet)
//                {
//                    *pbIsOutside = TRUE;
//                    return pt;
//                }
//
//                //�����������Ǹ���У��ʱ����Ļ�ֱ��ʵõ���, �����Ҫ
//                //����ʵ�ʵ���Ļ�ֱ��ʽ������Ե���
//                RECT rcCalibrateMonitor     = pCalibParams->allCalibCoefs[nMonitorId].rcMonitor;
//                int nCalibrateMonitorLeft   = rcCalibrateMonitor.left;
//                int nCalibrateMonitorTop    = rcCalibrateMonitor.top ;
//                int nCalibrateMonitorWidth  = rcCalibrateMonitor.right  -  rcCalibrateMonitor.left;
//                int nCalibrateMonitorHeight = rcCalibrateMonitor.bottom -  rcCalibrateMonitor.top ;
//
//                pt.x = nActualMonitorLeft + ((pt.x - nCalibrateMonitorLeft) * nActualMonitorWidth  + (nCalibrateMonitorWidth  >> 1)/*��������*/)/nCalibrateMonitorWidth;
//                pt.y = nActualMonitorTop  + ((pt.y - nCalibrateMonitorTop ) * nActualMonitorHeight + (nCalibrateMonitorHeight >> 1)/*��������*/)/nCalibrateMonitorHeight;
//
//
//
//                //������Ļλ����Ϣ(��λ:����)
//                 int nCXVScreen    = GetSystemMetrics(SM_CXVIRTUALSCREEN);
//                 int nCYVScreen    = GetSystemMetrics(SM_CYVIRTUALSCREEN);
//                 int nVSLeft       = GetSystemMetrics(SM_XVIRTUALSCREEN);
//                 int nVSTop        = GetSystemMetrics(SM_YVIRTUALSCREEN);
//                 int nVSRight      = nVSLeft + nCXVScreen;
//                 int nVSBottom     = nVSTop  + nCYVScreen;
//
//                //�ж�λ���Ƿ�λ��������Ļ���⣬���λ��������Ļ���⣬����λ��־*pbIsOutside
//			    if (pt.x < nVSLeft) 
//                {
//                    if(pbIsOutside) *pbIsOutside= true;
//                    pt.x = nVSLeft;
//                }
//
//                else if(pt.x > nVSRight)
//			    {
//                    if(pbIsOutside) *pbIsOutside= true;
//                    pt.x = nVSRight;
//				    
//			    }
//
//                if (pt.y < nVSTop)
//                {
//                    if(pbIsOutside) *pbIsOutside= true;
//                    pt.y = nVSTop;
//                }
//
//                else if(pt.y > nVSBottom)
//			    {
//                    if(pbIsOutside) *pbIsOutside= true;
//                    pt.y = nVSBottom;
//			    }
//
//             }
//             else
//             {
//                if(pbIsOutside) *pbIsOutside = true;
//             }
//
//            //AtlTrace(_T("obj.x=%d, obj.y=%d;pt.x = %d, pt.y=%d\r\n"), x, y, pt.x * this->m_ScreenSize.cx /65535, pt.y * this->m_ScreenSize.cy /65535);
//    }
//    else
//    {
//
//        if(pbIsOutside)
//        {
//            * pbIsOutside = true;
//        }
//        //assert(FALSE);
//    }    
//    return pt;
//
//}




////@����:�ھ�ͷ�������ص�������,���ݹ�ʹ�ߵ����ĺ������ε�λ�ã���ȡ���Ӧ����Ļ����
////@����:pPtCentroid, ָ�����������������ָ��
////      prcBound, ָ������Ӿ��������ָ��
////      pbIsOutside, ָ���жϹ���Ƿ�λ����Ļ�������������ָ��
// //added by xuke, 2016/11/10
//BOOL CVideToScreenMap::GetPt_Aug(const POINT* pPtCentroid, const RECT* prcBound, int nPtNumber, POINT* pPtScreen, bool* pbIsOutsideScreen)
//{
//    //CPerfDetector perf(_T("CVideToScreenMap::GetPt"));
//    CComCritSecLock<CComAutoCriticalSection> lock(m_csForCalibrateThreadSafe);
//
//    int nActualMonitorLeft, nActualMonitorTop, nActualMonitorRight, nActualMonitorBottom, nActualMonitorWidth, nActualMonitorHeight;
//    //�̰߳�ȫ����
//    {
//
//        nActualMonitorLeft   = this->m_rcMonitorResolution.left  ;
//        nActualMonitorTop    = this->m_rcMonitorResolution.top   ;
//        nActualMonitorRight  = this->m_rcMonitorResolution.right ;
//        nActualMonitorBottom = this->m_rcMonitorResolution.bottom;
//
//        nActualMonitorWidth  = this->m_rcMonitorResolution.right  - this->m_rcMonitorResolution.left;
//        nActualMonitorHeight = this->m_rcMonitorResolution.bottom - this->m_rcMonitorResolution.top ;
//    }
//
//
//    int nMonitorId = 0;
//
//    BOOL bRet = this->m_oCalibrateAlog.GetScreenCoord_Aug(pPtCentroid,  prcBound, nPtNumber, pPtScreen, nMonitorId);
//
//    if(!bRet)
//    {
//        if(pbIsOutsideScreen)
//        {
//            for(int i=0; i < nPtNumber; i++)
//            {
//                pbIsOutsideScreen[i] = false;
//            }
//        }
//        return FALSE;
//    }
//
//    const TCalibParams* pCalibParams =  GetCalibParams();
//
//    if(pCalibParams->allCalibCoefs.size() == 0) return FALSE;
//
//    //�����������Ǹ���У��ʱ����Ļ�ֱ��ʵõ���, �����Ҫ
//    //����ʵ�ʵ���Ļ�ֱ��ʽ������Ե���
//    RECT rcCalibrateMonitor     = pCalibParams->allCalibCoefs[nMonitorId].rcMonitor;
//    int nCalibrateMonitorLeft   = rcCalibrateMonitor.left;
//    int nCalibrateMonitorTop    = rcCalibrateMonitor.top ;
//    int nCalibrateMonitorWidth  = rcCalibrateMonitor.right  -  rcCalibrateMonitor.left;
//    int nCalibrateMonitorHeight = rcCalibrateMonitor.bottom -  rcCalibrateMonitor.top ;
//
//    for(int i=0; i < nPtNumber; i++)
//    {
//
//        POINT pt;
//
//        pt.x = nActualMonitorLeft + ((pPtScreen[i].x - nCalibrateMonitorLeft) * nActualMonitorWidth  + (nCalibrateMonitorWidth  >> 1)/*��������*/)/nCalibrateMonitorWidth;
//        pt.y = nActualMonitorTop  + ((pPtScreen[i].y - nCalibrateMonitorTop ) * nActualMonitorHeight + (nCalibrateMonitorHeight >> 1)/*��������*/)/nCalibrateMonitorHeight;
//
//
//
//        //������Ļλ����Ϣ(��λ:����)
//        int nCXVScreen    = GetSystemMetrics(SM_CXVIRTUALSCREEN);
//        int nCYVScreen    = GetSystemMetrics(SM_CYVIRTUALSCREEN);
//        int nVSLeft       = GetSystemMetrics(SM_XVIRTUALSCREEN);
//        int nVSTop        = GetSystemMetrics(SM_YVIRTUALSCREEN);
//        int nVSRight      = nVSLeft + nCXVScreen;
//        int nVSBottom     = nVSTop  + nCYVScreen;
//
//        //�ж�λ���Ƿ�λ��������Ļ���⣬���λ��������Ļ���⣬����λ��־*pbIsOutside
//        if (pt.x < nVSLeft) 
//        {
//            if(pbIsOutsideScreen) pbIsOutsideScreen[i] = true;
//            pt.x = nVSLeft;
//        }
//
//        else if(pt.x > nVSRight)
//        {
//            if(pbIsOutsideScreen) pbIsOutsideScreen[i] = true;
//            pt.x = nVSRight;
//
//        }
//
//        if (pt.y < nVSTop)
//        {
//            if(pbIsOutsideScreen) pbIsOutsideScreen[i] = true;
//            pt.y = nVSTop;
//        }
//
//        else if(pt.y > nVSBottom)
//        {
//            if(pbIsOutsideScreen) pbIsOutsideScreen[i] = true;
//            pt.y = nVSBottom;
//        }
//
//        else
//        {
//            if(pbIsOutsideScreen) pbIsOutsideScreen[i] = false;
//        }
//    }
//
//    return TRUE;
//}


//@����:���ݹ���������õ������������е�λ��
//@����:pCentroid, �����������
//      pContourPoints, ָ��߽��������б��ָ��
//      nContourPointCount, ������
//      pPtWorld, �������,����ӳ�������ڴ��ַ
//      pbIsOutsideScreen, ����ӳ�������Ļ��ı�־��
//BOOL CVideToScreenMap::GetPt_WithLightSpotContour(const POINT& ptCentroid,  const POINT* pContourPoints, int nContourPointCount, POINT* pPtWorld, bool* pbIsOutsideScreen,  POINT* debugPtContourCross)

BOOL CVideToScreenMap::GetPt(const TPoint2D& ptImage, TPoint2D* pPtScreen, bool* pbIsOutsideScreen, BOOL bWithoutAutoCalibCompensate,TCameraDebugData* pDebugData)
{
    //�̰߳�ȫ����
    CComCritSecLock<CComAutoCriticalSection> lock(m_csForCalibrateThreadSafe);

    int nActualMonitorLeft, nActualMonitorTop, nActualMonitorRight, nActualMonitorBottom, nActualMonitorWidth, nActualMonitorHeight;
    
    nActualMonitorLeft   = this->m_rcMonitorResolution.left  ;
    nActualMonitorTop    = this->m_rcMonitorResolution.top   ;
    nActualMonitorRight  = this->m_rcMonitorResolution.right ;
    nActualMonitorBottom = this->m_rcMonitorResolution.bottom;

    nActualMonitorWidth  = this->m_rcMonitorResolution.right  - this->m_rcMonitorResolution.left;
    nActualMonitorHeight = this->m_rcMonitorResolution.bottom - this->m_rcMonitorResolution.top ;



    int nMonitorId = 0;

    //BOOL bRet = this->m_oCalibrateAlog.GetScreenCoord_WithLightSpotContour(ptCentroid,  pContourPoints, nContourPointCount, pPtWorld, nMonitorId, debugPtContourCross);
    BOOL bRet = this->m_oCalibrateAlog.GetScreenCoord(ptImage, pPtScreen, nMonitorId, bWithoutAutoCalibCompensate, pDebugData);

    if(!bRet)
    {
        if(pbIsOutsideScreen)
        {
            *pbIsOutsideScreen= false;
        }
        return FALSE;
    }


    const TCalibParams* pCalibParams =  GetCalibParams();

    if(pCalibParams->allCalibCoefs.size() == 0) return FALSE;

    //�����������Ǹ���У��ʱ����Ļ�ֱ��ʵõ���, �����Ҫ
    //����ʵ�ʵ���Ļ�ֱ��ʽ������Ե���
    RECT rcCalibrateMonitor     = pCalibParams->allCalibCoefs[nMonitorId].rcMonitor;
    int nCalibrateMonitorLeft   = rcCalibrateMonitor.left;
    int nCalibrateMonitorTop    = rcCalibrateMonitor.top ;
    int nCalibrateMonitorWidth  = rcCalibrateMonitor.right  -  rcCalibrateMonitor.left;
    int nCalibrateMonitorHeight = rcCalibrateMonitor.bottom -  rcCalibrateMonitor.top ;

    {
        pPtScreen ->d[0] = nActualMonitorLeft + ((pPtScreen ->d[0] - nCalibrateMonitorLeft) * nActualMonitorWidth  + (nCalibrateMonitorWidth  >> 1)/*��������*/)/nCalibrateMonitorWidth;
        pPtScreen ->d[1] = nActualMonitorTop  + ((pPtScreen ->d[1] - nCalibrateMonitorTop ) * nActualMonitorHeight + (nCalibrateMonitorHeight >> 1)/*��������*/)/nCalibrateMonitorHeight;
   
        //������Ļλ����Ϣ(��λ:����)
		int nCXVScreen = GetSystemMetrics(SM_CXSCREEN);
		int nCYVScreen = GetSystemMetrics(SM_CYSCREEN);
		int nVSLeft    = 0;
		int nVSTop     = 0;

		if (g_tSysCfgData.globalSettings.bSupportExtendScreen)
		{//��չ��
			nCXVScreen = GetSystemMetrics(SM_CXVIRTUALSCREEN);
			nCYVScreen = GetSystemMetrics(SM_CYVIRTUALSCREEN);
			nVSLeft    = GetSystemMetrics(SM_XVIRTUALSCREEN);
			nVSTop     = GetSystemMetrics(SM_YVIRTUALSCREEN);
		}
#if 0
        int nCXVScreen    = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int nCYVScreen    = GetSystemMetrics(SM_CYVIRTUALSCREEN);
        int nVSLeft       = GetSystemMetrics(SM_XVIRTUALSCREEN);
        int nVSTop        = GetSystemMetrics(SM_YVIRTUALSCREEN);
#else  

#endif 
        int nVSRight      = nVSLeft + nCXVScreen;
        int nVSBottom     = nVSTop  + nCYVScreen;

		if (m_bIsRearProjectMode)//��Ͷģʽ
		{
			pPtScreen->d[0] = nCXVScreen - pPtScreen->d[0];
		}

		if (pbIsOutsideScreen) *pbIsOutsideScreen = false;
        

        //�ж�λ���Ƿ�λ��������Ļ���⣬���λ��������Ļ���⣬����λ��־*pbIsOutside
        if (pPtScreen->d[0] < nVSLeft)
        {
			if (pPtScreen->d[0] <(nVSLeft - m_ScreenOutAreaWidthInPixel) ) 
			{
               if(pbIsOutsideScreen) *pbIsOutsideScreen = true;
			}
            pPtScreen->d[0] = nVSLeft;
        }

        else if(pPtScreen->d[0]  > nVSRight)
        {
			if (pPtScreen->d[0]  > (nVSRight + m_ScreenOutAreaWidthInPixel))
			{
               if(pbIsOutsideScreen) *pbIsOutsideScreen  = true;
			}
            pPtScreen->d[0] = nVSRight;
        }

        if (pPtScreen->d[1]  < nVSTop)
        {
			if (pPtScreen->d[1]  < (nVSTop - m_ScreenOutAreaWidthInPixel))
			{
                if(pbIsOutsideScreen) *pbIsOutsideScreen = true;
			}
            pPtScreen->d[1]= nVSTop;
        }

        else if(pPtScreen->d[1] > nVSBottom)
        {
			if (pPtScreen->d[1] > (nVSBottom + m_ScreenOutAreaWidthInPixel))
			{
               if(pbIsOutsideScreen) *pbIsOutsideScreen = true;
			}
            pPtScreen->d[1]  = nVSBottom;
        }
    }

    return TRUE;
}

// BOOL CVideToScreenMap::GetExMouseScreenPosition()
// {
// 	return m_bIsExScreenPositionToMouse;
// }


//const SIZE CVideToScreenMap::GetScreenSize()const
//{
//    SIZE szScreen;
//    szScreen.cx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
//    szScreen.cy = GetSystemMetrics(SM_CYVIRTUALSCREEN);
//
//    const TCalibParams* pCalibParams =  GetCalibParams();
//    if(pCalibParams)
//    {
//        szScreen = pCalibParams->szScreen;
//    }
//
//	return  szScreen;
//}


////@����:��������У����־
//void CVideToScreenMap::SetAdjustingTag(BOOL bIsAdjusting)
//{
//	m_bAdjusted = !bIsAdjusting;
//}


//RECT CVideToScreenMap::GetVideoClipArea()const
//{
//	return m_rcVideoClipArea;
//}


//@����:У���㷨
//BOOL CVideToScreenMap::DoCalibrate(E_CALIBRATE_MODE eCalibrateMode, int N)
//{
//
//	if(eCalibrateMode == CALIBRATE_MODE_MANUAL)
//	{
//		switch(g_eCalibrateType)
//		{
//			case PROFILE::E_LEN_FAR_FOCUS:
//			
//				return Calibrate_LSLK<2>(N);
//				break;
//
//			case PROFILE::E_LEN_NEAR_FOCUS_1:
//				return Calibrate_LSLK<5>(N);
//				break;
//
//		}
//	}
//	else if(eCalibrateMode == CALIBRATE_MODE_AUTO)
//	{
//		return Calibrate_LSLK<5>(N);
//	}
//
//	return FALSE;
//}


#include <fstream>
//@����:У���㷨
BOOL CVideToScreenMap::DoCalibrate()
{
    CComCritSecLock<CComAutoCriticalSection> lock(m_csForCalibrateThreadSafe);

	 //����У������	
	BOOL bDebugModeTemp= g_tSysCfgData.globalSettings.bDebugMode;

	BOOL bRet = m_oCalibrateAlog.CaclCalibrationParams(m_tCalibData, bDebugModeTemp);

    return bRet;
}

void CVideToScreenMap::ScreenOutAreaWidthInPixel(double screenDigonalInMM)
{
	 m_ScreenDigonalInMM = screenDigonalInMM;

     int nCxVScreen = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	 int nCyVScreen = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	 double screenDigonalInPixel = sqrt((double)(nCxVScreen*nCxVScreen + nCyVScreen*nCyVScreen));
	 m_ScreenOutAreaWidthInPixel = int(SCREEN_OUT_AREA_WIDTH_IN_MM / screenDigonalInMM * screenDigonalInPixel);
}


//2012/07/25, �˹������������

//@����:�����˹���������
//void CVideToScreenMap::UpdateManualCorrectData(const std::vector<CalibratePair>& newManualCorrectData)
//{
//	m_vecManualCorrectData = newManualCorrectData;
//	m_bManualCorrectDataIsValid = TRUE;
//}

//@����:�����˹���������
//void CVideToScreenMap::InvalidateManualCorrectData()
//{
//	m_bManualCorrectDataIsValid = FALSE;
//}
//
//
////@����:�ж��˹����������Ƿ���Ч
//BOOL CVideToScreenMap::IsValidManualCorrectData()
//{
//	return m_bManualCorrectDataIsValid;
//}


//@����:�������ˮƽ�ʹ�ֱλ��ƫ���������������
//@����:nXDeviationCompensation, ˮƽƫ������ֵ, ��λ����,�����ɸ�
//      nYDeviationCompensation, ��ֱƫ������ֵ, ��λ����,�����ɸ�
void CVideToScreenMap::SetDeviationCompensationValue(int nXDeviationCompensationVal, int nYDeviationCompensationVal)
{
    int nCxScreen = GetSystemMetrics(SM_CXSCREEN);
    int nCyScreen = GetSystemMetrics(SM_CYSCREEN);

    if(nXDeviationCompensationVal > MAX_PERMIT_DEVIATION_COMPENSATION)
    {
        nXDeviationCompensationVal = MAX_PERMIT_DEVIATION_COMPENSATION;
    }
    else if(nXDeviationCompensationVal < -MAX_PERMIT_DEVIATION_COMPENSATION)
    {
        nXDeviationCompensationVal = - MAX_PERMIT_DEVIATION_COMPENSATION;
    }


    if(nYDeviationCompensationVal > MAX_PERMIT_DEVIATION_COMPENSATION)
    {
        nYDeviationCompensationVal = MAX_PERMIT_DEVIATION_COMPENSATION;
    }
    else if(nYDeviationCompensationVal < -MAX_PERMIT_DEVIATION_COMPENSATION)
    {
        nYDeviationCompensationVal = - MAX_PERMIT_DEVIATION_COMPENSATION;
    }


	//<<beginModified by toxuke@gmail.com, 2013/03/27
    //��ƫ������ֵ��һ����0~65535
    //m_nMouseXDeviationCompensationVal = nXDeviationCompensationVal*65535/nCxScreen;
    //m_nMouseYDeviationCompensationVal = nYDeviationCompensationVal*65535/nCyScreen;

    m_nMouseXDeviationCompensationVal = nXDeviationCompensationVal;
    m_nMouseYDeviationCompensationVal = nYDeviationCompensationVal;

	//>>

}

//@����:��ȡ���ˮƽ�ʹ�ֱλ��ƫ��������趨ֵ��
//@����:
//      pXDeivationCompensationVal, ����ˮƽƫ������ֵ�Ļ�������ַ��
//      pYDeviationCompensationVal, ���洹ֱƫ������ֵ�û�������ַ��
void CVideToScreenMap::GetDeviationCompensationValue(int* pXDeivationCompensationVal, int* pYDeviationCompensationVal)
{
    if(pXDeivationCompensationVal == NULL) *pXDeivationCompensationVal = m_nMouseXDeviationCompensationVal;
    if(pYDeviationCompensationVal == NULL) *pYDeviationCompensationVal = m_nMouseYDeviationCompensationVal;
}

//@����:����У�����̲���
const TCalibParams* CVideToScreenMap::GetCalibParams()const
{
    //CComCritSecLock<CComAutoCriticalSection> lock(m_csForCalibrateThreadSafe);

    return m_oCalibrateAlog.GetCalibParams();
}

 //@����:����У�����̲���
 void CVideToScreenMap::SetCalibParams(const TCalibParams& param)
 {
     CComCritSecLock<CComAutoCriticalSection> lock(m_csForCalibrateThreadSafe);

    m_oCalibrateAlog.SetCalibParams(param);
 }

 
 //@����:���ù��λ�õ���Ļ��ŵ�ӳ������,�Ծ�����ߴ����ĸ���Ļ�� 
 void CVideToScreenMap::SetScreenMap(const CImageFrame& screenMap)
 {
    CComCritSecLock<CComAutoCriticalSection> lock(m_csForCalibrateThreadSafe);
    m_oScreenMap = screenMap;
     
    //��������ӳ���ϵͼ
    SaveScreenMap();
 }

//@����:�жϵ㴦���ĸ���Ļ
//@����:pt, �����ͼ���е�����
//@����ֵ:-1,����
//        0,1,2,...7,Ϊ��Ļ���
 int CVideToScreenMap::MointorFromPt(const POINT& pt)
 {
    int W = m_oScreenMap.Width();
    int H = m_oScreenMap.Height();

    POINT ptImage  = pt;
    ptImage.x = pt.x ;
    ptImage.y = pt.y ;

    if(ptImage.x < 0 || ptImage.x >= W) return  -1;
    if(ptImage.y < 0 || ptImage.y >= H) return  -1;

    BYTE data = *(m_oScreenMap.GetData() + ptImage.x + ptImage.y *W);

    for(int i=0; i < 8; i++)
    {
        BYTE mask = 1<<i;

        if(mask & data)
        {
            return i;
        }

    }

    return  -1;

 }


 static COLORREF color_map[256]=
 {
  RGB(0,      0,    0),
  RGB(255,    6,    0),
  RGB(  0,   255,  255),
  RGB(12,    255,   0),
  RGB(255,   18,    0),
  RGB(255,   24,    0),
  RGB(255,   30,    0),
  RGB(255,   36,    0),
  RGB(255,   42,    0),
  RGB(255,   48,    0),
  RGB(255,   54,    0),
  RGB(255,   60,    0),
  RGB(255,   66,    0),
  RGB(255,   72,    0),
  RGB(255,   78,    0),
  RGB(255,   84,    0),
  RGB(255,   90,    0),
  RGB(255,   96,    0),
  RGB(255,  102,    0),
  RGB(255,  108,    0),
  RGB(255,  114,    0),
  RGB(255,  120,    0),
  RGB(255,  126,    0),
  RGB(255,  131,    0),
  RGB(255,  137,    0),
  RGB(255,  143,    0),
  RGB(255,  149,    0),
  RGB(255,  155,    0),
  RGB(255,  161,    0),
  RGB(255,  167,    0),
  RGB(255,  173,    0),
  RGB(255,  179,    0),
  RGB(255,  185,    0),
  RGB(255,  191,    0),
  RGB(255,  197,    0),
  RGB(255,  203,    0),
  RGB(255,  209,    0),
  RGB(255,  215,    0),
  RGB(255,  221,    0),
  RGB(255,  227,    0),
  RGB(255,  233,    0),
  RGB(255,  239,    0),
  RGB(255,  245,    0),
  RGB(255,  251,    0),
  RGB(253,  255,    0),
  RGB(247,  255,    0),
  RGB(241,  255,    0),
  RGB(235,  255,    0),
  RGB(229,  255,    0),
  RGB(223,  255,    0),
  RGB(217,  255,    0),
  RGB(211,  255,    0),
  RGB(205,  255,    0),
  RGB(199,  255,    0),
  RGB(193,  255,    0),
  RGB(187,  255,    0),
  RGB(181,  255,    0),
  RGB(175,  255,    0),
  RGB(169,  255,    0),
  RGB(163,  255,    0),
  RGB(157,  255,    0),
  RGB(151,  255,    0),
  RGB(145,  255,    0),
  RGB(139,  255,    0),
  RGB(133,  255,    0),
  RGB(128,  255,    0),
  RGB(122,  255,    0),
  RGB(116,  255,    0),
  RGB(110,  255,    0),
  RGB(104,  255,    0),
  RGB( 98,  255,    0),
  RGB( 92,  255,    0),
  RGB( 86,  255,    0),
  RGB( 80,  255,    0),
  RGB( 74,  255,    0),
  RGB( 68,  255,    0),
  RGB( 62,  255,    0),
  RGB( 56,  255,    0),
  RGB( 50,  255,    0),
  RGB( 44,  255,    0),
  RGB( 38,  255,    0),
  RGB( 32,  255,    0),
  RGB( 26,  255,    0),
  RGB( 20,  255,    0),
  RGB( 14,  255,    0),
  RGB(  8,  255,    0),
  RGB(  2,  255,    0),
  RGB(  0,  255,    4),
  RGB(  0,  255,   10),
  RGB(  0,  255,   16),
  RGB(  0,  255,   22),
  RGB(  0,  255,   28),
  RGB(  0,  255,   34),
  RGB(  0,  255,   40),
  RGB(  0,  255,   46),
  RGB(  0,  255,   52),
  RGB(  0,  255,   58),
  RGB(  0,  255,   64),
  RGB(  0,  255,   70),
  RGB(  0,  255,   76),
  RGB(  0,  255,   82),
  RGB(  0,  255,   88),
  RGB(  0,  255,   94),
  RGB(  0,  255,  100),
  RGB(  0,  255,  106),
  RGB(  0,  255,  112),
  RGB(  0,  255,  118),
  RGB(  0,  255,  124),
  RGB(  0,  255,  129),
  RGB(  0,  255,  135),
  RGB(  0,  255,  141),
  RGB(  0,  255,  147),
  RGB(  0,  255,  153),
  RGB(  0,  255,  159),
  RGB(  0,  255,  165),
  RGB(  0,  255,  171),
  RGB(  0,  255,  177),
  RGB(  0,  255,  183),
  RGB(  0,  255,  189),
  RGB(  0,  255,  195),
  RGB(  0,  255,  201),
  RGB(  0,  255,  207),
  RGB(  0,  255,  213),
  RGB(  0,  255,  219),
  RGB(  0,  255,  225),
  RGB(  0,  255,  231),
  RGB(  0,  255,  237),
  RGB(  0,  255,  243),
  RGB(  0,  255,  249),
  RGB(  0,  249,  255),
  RGB(  0,  243,  255),
  RGB(  0,  237,  255),
  RGB(  0,  231,  255),
  RGB(  0,  225,  255),
  RGB(  0,  219,  255),
  RGB(  0,  213,  255),
  RGB(  0,  207,  255),
  RGB(  0,  201,  255),
  RGB(  0,  195,  255),
  RGB(  0,  189,  255),
  RGB(  0,  183,  255),
  RGB(  0,  177,  255),
  RGB(  0,  171,  255),
  RGB(  0,  165,  255),
  RGB(  0,  159,  255),
  RGB(  0,  153,  255),
  RGB(  0,  147,  255),
  RGB(  0,  141,  255),
  RGB(  0,  135,  255),
  RGB(  0,  129,  255),
  RGB(  0,  124,  255),
  RGB(  0,  118,  255),
  RGB(  0,  112,  255),
  RGB(  0,  106,  255),
  RGB(  0,  100,  255),
  RGB(  0,   94,  255),
  RGB(  0,   88,  255),
  RGB(  0,   82,  255),
  RGB(  0,   76,  255),
  RGB(  0,   70,  255),
  RGB(  0,   64,  255),
  RGB(  0,   58,  255),
  RGB(  0,   52,  255),
  RGB(  0,   46,  255),
  RGB(  0,   40,  255),
  RGB(  0,   34,  255),
  RGB(  0,   28,  255),
  RGB(  0,   22,  255),
  RGB(  0,   16,  255),
  RGB(  0,   10,  255),
  RGB(  0,    4,  255),
  RGB(  2,    0,  255),
  RGB(  8,    0,  255),
  RGB( 14,    0,  255),
  RGB( 20,    0,  255),
  RGB( 26,    0,  255),
  RGB( 32,    0,  255),
  RGB( 38,    0,  255),
  RGB( 44,    0,  255),
  RGB( 50,    0,  255),
  RGB( 56,    0,  255),
  RGB( 62,    0,  255),
  RGB( 68,    0,  255),
  RGB( 74,    0,  255),
  RGB( 80,    0,  255),
  RGB( 86,    0,  255),
  RGB( 92,    0,  255),
  RGB( 98,    0,  255),
  RGB(104,    0,  255),
  RGB(110,    0,  255),
  RGB(116,    0,  255),
  RGB(122,    0,  255),
  RGB(128,    0,  255),
  RGB(133,    0,  255),
  RGB(139,    0,  255),
  RGB(145,    0,  255),
  RGB(151,    0,  255),
  RGB(157,    0,  255),
  RGB(163,    0,  255),
  RGB(169,    0,  255),
  RGB(175,    0,  255),
  RGB(181,    0,  255),
  RGB(187,    0,  255),
  RGB(193,    0,  255),
  RGB(199,    0,  255),
  RGB(205,    0,  255),
  RGB(211,    0,  255),
  RGB(217,    0,  255),
  RGB(223,    0,  255),
  RGB(229,    0,  255),
  RGB(235,    0,  255),
  RGB(241,    0,  255),
  RGB(247,    0,  255),
  RGB(253,    0,  255),
  RGB(255,    0,  251),
  RGB(255,    0,  245),
  RGB(255,    0,  239),
  RGB(255,    0,  233),
  RGB(255,    0,  227),
  RGB(255,    0,  221),
  RGB(255,    0,  215),
  RGB(255,    0,  209),
  RGB(255,    0,  203),
  RGB(255,    0,  197),
  RGB(255,    0,  191),
  RGB(255,    0,  185),
  RGB(255,    0,  179),
  RGB(255,    0,  173),
  RGB(255,    0,  167),
  RGB(255,    0,  161),
  RGB(255,    0,  155),
  RGB(255,    0,  149),
  RGB(255,    0,  143),
  RGB(255,    0,  137),
  RGB(255,    0,  131),
  RGB(255,    0,  126),
  RGB(255,    0,  120),
  RGB(255,    0,  114),
  RGB(255,    0,  108),
  RGB(255,    0,  102),
  RGB(255,    0,   96),
  RGB(255,    0,   90),
  RGB(255,    0,   84),
  RGB(255,    0,   78),
  RGB(255,    0,   72),
  RGB(255,    0,   66),
  RGB(255,    0,   60),
  RGB(255,    0,   54),
  RGB(255,    0,   48),
  RGB(255,    0,   42),
  RGB(255,    0,   36),
  RGB(255,    0,   30),
  RGB(255,    0,   24),
  RGB(255,    0,   18),
  RGB(255,    0,   12),
  RGB(255,    0,    6)
};




 //@����:������Ļӳ�����ݣ���㵽������Ļ��ŵ�ӳ���ϵ)
 BOOL CVideToScreenMap::LoadScreenMap()
 {
    CComCritSecLock<CComAutoCriticalSection> lock(m_csForCalibrateThreadSafe);

    TCHAR szFullPath[MAX_PATH];
    _stprintf_s(szFullPath, _countof(szFullPath), _T("%s\\Sensor%02d\\%s"), (LPCTSTR)PROFILE::SETTINGS_BASE_DIRECTORY, m_nID, (LPCTSTR)PROFILE::SCREEN_MAP_NAME);

     CDib  dib;
     if(!dib.Open(szFullPath))
     {
         return FALSE;
     }

     int W = dib.GetWidth();
     int H = dib.GetHeight();
     m_oScreenMap.SetSize(W,H,1);

    int   nLineStride = dib.GetLineStride();
     
     BYTE* pDibCursor = dib.GetBits() + (H - 1)*nLineStride;

     BYTE* pMapData   = this->m_oScreenMap.GetData();
     for(int r = 0; r < H; r++)
     {
        pDibCursor = dib.GetBits() + (H - 1 - r)*nLineStride;
        for(int c = 0; c < W; c++)
        {
            COLORREF rgb =  pDibCursor[0] |  pDibCursor[1] << 8 | pDibCursor[2] << 16;


            BYTE mapData = 0;

            for(UINT i = 0; i < _countof(color_map); i++)
            {
                if(color_map[i] == rgb)
                {
                    mapData = (BYTE)i;
                    break;
                }
            }

            *pMapData = mapData;

            pDibCursor += 3;
            pMapData ++;


        }


     }

     return TRUE;
 }

  //@����:������Ļӳ�����ݣ���㵽������Ļ��ŵ�ӳ���ϵ)
 // @˵��:Ϊ��ӳ���ϵ�Ŀ��ӻ�, ÿ����Ļ�����Ӧһ������ɫ
 BOOL CVideToScreenMap::SaveScreenMap()
 {
     CDib  dib;

     int W = this->m_oScreenMap.Width();
     int H = this->m_oScreenMap.Height();

     dib.Create(W, H);

     int   nLineStride = dib.GetLineStride();
     
     BYTE* pDibCursor = dib.GetBits() + (H - 1)*nLineStride;

     BYTE* pMapData   = this->m_oScreenMap.GetData();
     for(int r = 0; r < H; r++)
     {
        pDibCursor = dib.GetBits() + (H - 1 - r)*nLineStride;
        for(int c = 0; c < W; c++)
        {
            COLORREF rgb = color_map[*pMapData];
            pDibCursor[0] = GetRValue(rgb);
            pDibCursor[1] = GetGValue(rgb);
            pDibCursor[2] = GetBValue(rgb);
            pDibCursor += 3;
            pMapData ++;
        }
     }

    TCHAR szFullPath[MAX_PATH];
    _stprintf_s(szFullPath, _countof(szFullPath), _T("%s\\Sensor%02d\\%s"), (LPCTSTR)PROFILE::SETTINGS_BASE_DIRECTORY, m_nID, (LPCTSTR)PROFILE::SCREEN_MAP_NAME);

    return dib.Save(szFullPath);
 }



//<<added by toxuke@gmail.com, 2015/02/09
//���Ը�����������У�������ļ��ж�ȡУ������
BOOL CVideToScreenMap::CalibrateFromDataFile(LPCTSTR lpszFileName)
{
    CComCritSecLock<CComAutoCriticalSection> lock(m_csForCalibrateThreadSafe);
    errno_t err;
    FILE* file = NULL;
    err = fopen_s(&file, CT2A(lpszFileName), "r");
    if(err == 0)
    {
       char szText[1024];
       int nRet = 0;

       //��Ƶͼ��ߴ�
       int nImageWidth, nImageHeight;

       //��Ļλ����Ϣ(����)
        RECT rcMonitor;
		int radius = 0;

        //У������Ļ����
        int nScreenX = 0;
        int nScreenY = 0;

        //У������Ƶ����
        double dbImageU  = 0;
        double dbImageV  = 0;


		//��ͷͶ���
		double throwRatio = 0.15;

        std::vector<TCalibCoordPair> calibData;

        int nStep = 0;
        while(fgets(szText, _countof(szText),file))
        {
            int nTextLength = strlen(szText);

            for(int i=0; i<nTextLength; i++)
            {
                if(szText[i] == '#')
                {//ע�ͺ�����ݽض�
                    szText[i]='\0';
                    break;
                }
            }

            nTextLength =  strlen(szText);
           if(nTextLength == 0) continue;


            switch(nStep)
            {
			case 0:
				nRet = sscanf_s(szText,
					"%lf",
					&throwRatio);

				if (nRet != 1) return FALSE;
				nStep = 1;
				break;

            case 1://��ȡͼ������
                nRet = sscanf_s(szText, 
                       "%d,%d",
                        &nImageWidth,
                        &nImageHeight
                        );
                if(nRet != 2) return FALSE;
                
                nStep = 2;
                break;


            case 2://��ȡ��Ļ�ߴ�
               nRet = sscanf_s(szText, 
                           "%d,%d,%d,%d",
                           &rcMonitor.left,
                           &rcMonitor.top,
                           &rcMonitor.right,
                           &rcMonitor.bottom
                           );

                if(nRet != 4) return FALSE;
                nStep = 3;
                break;

			case 3://��ȡУ��Բ�뾶
				nRet = sscanf_s(szText,
					"%d,",
					&radius);

				if (nRet != 1) return FALSE;
				nStep = 4;
				break;

            case 4:
                {
                    nRet = sscanf_s(szText, 
                           "%lf,%lf,%d,%d",
							&dbImageU,
							&dbImageV,
                            &nScreenX,
                            &nScreenY);
                    if(nRet != 4) break;

                    TCalibCoordPair data;
                    data.ptScreenCoord.x = nScreenX;
                    data.ptScreenCoord.y = nScreenY;
                    data.pt2DImageCoord.d[0]  = dbImageU;
                    data.pt2DImageCoord.d[1]  = dbImageV;

                    calibData.push_back(data);
                }
                break;

            }//switch


        }//while

        m_tCalibData.allMonitorCalibData.resize(1);
        m_tCalibData.allMonitorCalibData[0].calibData = calibData;
        m_tCalibData.allMonitorCalibData[0].rcMonitor = rcMonitor;
		m_tCalibData.allMonitorCalibData[0].radius = radius;

        m_tCalibData.eCalibType = E_CALIBRATE_TYPE_AUTO;
        m_tCalibData.szImage.cx = nImageWidth;
        m_tCalibData.szImage.cy = nImageHeight;

        fclose(file);

        BOOL bRet =  this->DoCalibrate();

        //���У������
        //OutputCalibParams(NULL);
        return bRet;
       
    }//if

    return FALSE;
}


//<<added by toxuke@gmail.com, 2015/02/09
//���Ը������������У�����̵�ϵ��
//void CVideToScreenMap::OutputCalibParams(LPCTSTR lpszFileName)
//{
//        char text[1024];
//        FILE* file;
//
//        const TCalibParams* pCalibParams =  GetCalibParams();
//        if(NULL == pCalibParams) return;
//
//        if(NULL == lpszFileName)
//        {
//            SYSTEMTIME sysTime;
//            GetLocalTime(&sysTime);
//            sprintf_s(
//                text, 
//                _countof(text), 
//                "CalibrateParam(%04d%02d%02d-%02d%02d%02d-%03d).txt", 
//                sysTime.wYear,
//                sysTime.wMonth,
//                sysTime.wDay,
//                sysTime.wHour,
//                sysTime.wMinute,
//                sysTime.wSecond,
//                sysTime.wMilliseconds);
//        }
//        else
//        {
//            strcpy_s(text, _countof(text), CT2A(lpszFileName));
//        }
//
//        errno_t err = fopen_s(&file, text, "w");
//        if(err == 0)
//        {
//            int nLen = 0;
//
//            //���ͼ��ߴ���Ϣ
//            //ע��
//            strcpy_s(text, _countof(text), "#image width and height(expand 16 times)\n");
//            fwrite(text, 1, strlen(text), file);
//
//            nLen = sprintf_s(
//                text, 
//                _countof(text), 
//                "%d,%d\n", 
//                pCalibParams->szImage.cx,
//                pCalibParams->szImage.cy );
//
//            fwrite(text, 1, nLen, file);
//
//            //�����Ļ��������
//            strcpy_s(text, _countof(text), "#screen dimension in pixel\n");
//            fwrite(text, 1, strlen(text), file);
//
//
//            nLen = sprintf_s(
//                text, 
//                _countof(text), 
//                "%d,%d, %d, %d \n", 
//                pCalibParams->allCalibCoefs[0].rcMonitor.left,
//                pCalibParams->allCalibCoefs[0].rcMonitor.top,
//                pCalibParams->allCalibCoefs[0].rcMonitor.right,
//                pCalibParams->allCalibCoefs[0].rcMonitor.bottom);
//            fwrite(text, 1, nLen, file);
//
//            //���У������
//            strcpy_s(text, _countof(text), "#calibrate params\n");
//            fwrite(text, 1, strlen(text), file);
//            for(size_t i=0; i < pCalibParams->allCalibCoefs[0].calibCoefs.size(); i++)
//            {
//                const double& dbCoef = pCalibParams->allCalibCoefs[0].calibCoefs[i];
//                nLen = 
//                sprintf_s(
//                    text,
//                    _countof(text),
//                    "%le\n",
//                    dbCoef);
//                fwrite(text, 1, nLen, file);
//            }//for
//
//            fclose(file);
//
//        }//if(err == 0)
//
//}

//>>

//added by xuke, 2020/03/05
//@��  ��:�ж���Ļ��������Ļ����
//@����ֵ:true, ���������Ļ�ڲ�
//        false, ���������Ļ�ⲿ
bool CVideToScreenMap::IsOutsideOwnedArea(const POINT& ptScreen)const
{
    bool bOutSide = false;
    if (ptScreen.x < m_rcMonitorResolution.left)
    {
        bOutSide = true;
    }
    
    if (ptScreen.x > m_rcMonitorResolution.right)
    {
        bOutSide = true;
    }

    if (ptScreen.y < m_rcMonitorResolution.top)
    {
        bOutSide = true;
    }

    if (ptScreen.y > m_rcMonitorResolution.bottom)
    {
        bOutSide = true;
    }

    return bOutSide;
}