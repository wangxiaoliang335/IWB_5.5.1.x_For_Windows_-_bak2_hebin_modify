// AlogLib.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

//#include "GlobalVar.h"
//
//#include "../inc/DispMonitorFinder.h"
////�����ӿ��༰����������
//#include "../inc/AlgoLib.h"
//#include "../inc/EasiSDKReg_Interface.h"
////#include "./AutoCalibrator/AutoCalibratorImpl.h"
//#include "./AutoCalibrator/AutoCalibratorImpl2.h"
//#include "./BlobDetector/BlobDetectorImpl.h"
//#include "./Calibrator/CalibrateAlgo.h"
//#include "./ScreenAreaDetector/ScreenAreaDetectorImpl.h"
#include "headers.h"

#include "../inc/EasiSDKReg_Interface.h"

#pragma comment(lib,"../lib/EasiSDKReg_Interface.lib")



//@����:���ϵͳ�е�USB��������ʽ�Ƿ���ȷ
#ifdef _DEBUG
HRESULT CheckUSBKey()
{
    return S_OK;
}

#else
HRESULT CheckUSBKey()
{
    return S_OK;

    HRESULT hr = S_OK;
    float fVersion = 0.0f;
    hr = SDKREG_GetVersion(&fVersion);

    if(hr!= S_OK)
    {
        return hr;
    }

    hr = SDKREG_IsBasicFunctionEnabled(14);
    if(hr != S_OK)
    {
        return hr;
    }


    // 	if(fVersion < 0.20111018f)
    // 	{
    // 		return ERR_SOFTKEY_VERSION;//�汾����
    // 	}
    // 
    // 	int nAppType = 0;
    // 	hr = SDKREG_GetAppType(&nAppType);
    // 
    // 	if(hr != S_OK)
    // 	{
    // 		return hr;		
    // 	}
    // 
    // 	if(nAppType != 1)
    // 	{
    // 		return ERR_SOFTKEY_APP_TYPE;
    // 	}

    return S_OK;
}
#endif

//Part 1.�Զ�У������
//@����:�����Զ�У�������ʵ����
//
//
//IAutoCalibratorInterface* GetAutoCalibratorInstance()
//{
//    static CAutoCalibratorImpl2 s_AutoCalibratorInstance;
//    //static CAutoCalibratorImp s_AutoCalibratorInstance;
//
//    return &s_AutoCalibratorInstance;
//}

CAutoCalibrator::CAutoCalibrator()
:
m_pAutoCalibratorImpl(NULL)
{
    m_pAutoCalibratorImpl = new CAutoCalibratorImpl2();

}


CAutoCalibrator::~CAutoCalibrator()
{
    delete m_pAutoCalibratorImpl;
}


//@����:��ʼУ��, ���ⲿģ�����
//@����:
//      hNotifyWnd, ��Ϣ֪ͨ������
//      E_AutoCalibratePattern, У������ö��ֵ
//      cBrightness, У��ʱ��������ƽ��ֵ
//      ChangeCameraParamsProc, ��������ͷ�����Ļص�����
//      lpCtx, ChangeCameraParamsProc�������Ĳ���
//      clrGridHighlight, У��ͼ���и��������ʾ��ɫ
//      eDebugLevel, ���Լ���
//      bSaveInermediatFile, �Ƿ񱣴�У�����м���ͼƬ
//      bRecordVideo���Ƿ��¼У�������е�ԭʼ��Ƶ
//@����ֵ:�ɹ����� 0;
//@˵��:��ʼ�Զ�У����, ������У������, "�Զ�У��"״̬���ص���ʼ״̬��
//HRESULT CAutoCalibrator::StartCalibrating(HWND hNotifyWnd, E_AutoCalibratePattern eGridPattern, BYTE cBrightness, CHANGECAMERAPARAMSPROC ChangeCameraParamsProc, LPVOID lpCtx, COLORREF clrGridHighlight, ECalibDebugLevel eDebugLevel, BOOL bSaveInermediatFile,BOOL bRecordVideo)

//@����:��ʼУ��, ���ⲿģ�����
//@����:autoCalibrateParams �Զ�У������
BOOL CAutoCalibrator::StartCalibrating(const TAutoCalibrateParams& autoCalibrateParams, const TStaticMaskingParams& staticMaskingParams)
{
    
     BOOL bRet = m_pAutoCalibratorImpl->StartCalibrating(autoCalibrateParams, staticMaskingParams);
    return bRet;

}


//@����:��ʼ�Զ�����,���ⲿģ�����
BOOL CAutoCalibrator::StartMasking(const TAutoMaskingParams& autoMaskingParams, const TStaticMaskingParams& staticMaskingParams,int nSrcWidth ,int nSrcHeight)
{
    BOOL bRet = m_pAutoCalibratorImpl->StartMasking(autoMaskingParams, staticMaskingParams, nSrcWidth, nSrcHeight);

    return bRet;
}


BOOL CAutoCalibrator::DoSimulateCalibrate(LPCTSTR lpszAVIFilePath, HWND hNotifyWnd, UINT nCx, UINT nCy, BYTE cBrightness, E_AutoCalibratePattern eGridsNumber, ECalibDebugLevel eDebugLevel)
{
    m_pAutoCalibratorImpl->SetDebugLevel(eDebugLevel);
    return m_pAutoCalibratorImpl->DoSimulateCalibrate(lpszAVIFilePath, hNotifyWnd, nCx, nCy, cBrightness, eGridsNumber);
}

const CImageFrame& CAutoCalibrator::GetMaskFrame()const
{
    return m_pAutoCalibratorImpl->GetMaskFrame();

}

const CImageFrame& CAutoCalibrator::GetMaskFinderFrame(CImageFrame& maskframe)const
{
	return m_pAutoCalibratorImpl->GetMaskFinderFrame(maskframe);
}


//@����:���ع��λ�õ���Ļ��ŵ�ӳ������,�Ծ�����ߴ����ĸ���Ļ��
const CImageFrame& CAutoCalibrator::GetScreenMap()const
{
    return m_pAutoCalibratorImpl->GetScreenMap();
}

//@����:������Ļ����
int CAutoCalibrator::GetMonitorCount()const
{
    return m_pAutoCalibratorImpl->GetMonitorCount();
}


//@����:��ȡָ����Ļ��У������
//@����:nMonitorID�� ��ʾ��Id
//@˵��:
//const TMonitorCalibData& CAutoCalibrator::GetMonitorCalibData(int nMonitorID) const
//{
//    return m_pAutoCalibratorImpl->GetMonitorCalibData(nMonitorID);
//}



HWND CAutoCalibrator::GetCalibrateHWnd()const
{
    return m_pAutoCalibratorImpl->GetCalibrateHWnd();
}

//@���ܣ����У��ʧ�ܵ��ļ���·��
//@˵�������Զ�У��ʱ�����У����ȫʧ�ܺ���Ҫ��ʧ��ʱ�����ͼƬ����ʱ����Ҫ�õ�ʧ�ܺ��ļ���·��
CAtlString CAutoCalibrator::GetCalibrateIntermediataDirName()
{
	return m_pAutoCalibratorImpl->GetCalibrateIntermediataDirName();
}

//@����:��ȡУ��ʱ����Ļ�ߴ�
//@˵��:��ģ��У��ʱ,У��ʱ����Ļ�ߴ�ͼ������ǰ��Ļ�ߴ���ܲ�ͬ��
SIZE CAutoCalibrator::GetScreenSize()const
{
    return m_pAutoCalibratorImpl->GetScreenSize();
}



//@����:��ȡУ��ʱ����Ƶͼ��ߴ�
//@˵��:
SIZE CAutoCalibrator::GetImageSize()const
{
    return m_pAutoCalibratorImpl->GetImageSize();
}




//���Ժ���:���������Զ����ڹ���
void CAutoCalibrator::TestAutoBrightnessControl(CHANGECAMERAPARAMSPROC ChangeCameraParamsProc , LPVOID lpCtx)
{
    return m_pAutoCalibratorImpl->TestAutoBrightnessControl(ChangeCameraParamsProc, lpCtx);
}



//���Ժ���:�鿴У��ͼ��
void CAutoCalibrator::ViewCalibratePattern()
{
    m_pAutoCalibratorImpl->ViewCalibratePattern();
}



//@����:��ȡ������Ļ��У������
const TCalibData* CAutoCalibrator::GetCalibrateData()const
{
        if(m_pAutoCalibratorImpl)
        {
            return m_pAutoCalibratorImpl->GetCalibrateData();
        }
        return  NULL;
}


//@����:���ص�����¼���ļ�����·��
const TCHAR* CAutoCalibrator::GetDebugVideoFilePath()const
{
        if(m_pAutoCalibratorImpl)
        {
            return m_pAutoCalibratorImpl->GetDebugVideoFilePath();
        }
        return  NULL;
}

void CAutoCalibrator::OnDeviceMissing()
{
	if (m_pAutoCalibratorImpl)
	{
		m_pAutoCalibratorImpl->OnDeviceMissing();
	}
	return;
}


CBlobDetector::CBlobDetector()
:
m_nFrameCount(0)
{
    m_pBlobScanAlgo = new CConnectedComponentScanlineAlogrithm<>();
    m_pBitFrame     = new CBitFrame();
    //m_pYUY2Frame    = new CYUY2Frame();
    m_pGrayFrame    = new CImageFrame();
}


CBlobDetector::~CBlobDetector()
{
    delete m_pBlobScanAlgo;
    delete m_pBitFrame;
    //delete m_pYUY2Frame;
    delete m_pGrayFrame;

}


//@����:1.ͳ�Ʋ�����
//      2.���λͼ�����ȴ���ָ������ֵ�Ĳ���������������Ӿ���, ���������������ġ�
//@����:frame, 8Bit�Ҷȸ�ʽ��λͼ֡
//      YThreshold, ������������
//      StatisticalFrame, ���ش������޵Ĵ�����ͳ�Ʊ�ÿ��������һ���ֽڼ�¼ͳ��ֵ
//      DynamicMaskFrame, ��̬����λͼ
//      StatisticStep,ͳ��ͼ������,��λ:֡, ��ÿ��StatisticStep֡������һ�θ���һ��ͳ�Ʊ�
//      YStatisicThreshold, ��̬����ͼ������
//@����:��⵽��Ŀ��ĸ���
//@˵��:

UINT CBlobDetector::operator() (const CImageFrame& grayFrame, BYTE YThreshold, CImageFrame& StatisticalFrame, const CImageFrame& DynamicMaskFrame, int StatisticStep, BYTE YStatisicThreshold,bool bDynamicMasking)
{

    //CPerfDetector perf(_T("CBlobDetector::operator()"));
    int nWidth  = grayFrame.Width();
    int nHeight = grayFrame.Height();

    int nPixelCount = nWidth * nHeight;

	//CPerfDetector perf(_T("YUY2ToBits"));
	m_pBitFrame->SetSize(nWidth, nHeight);
	*m_pGrayFrame = grayFrame;

    //ͳ���Զ�����λͼ
	//�����Ҫ��̬���εĻ�����ô�ͽ��ж�̬����
	if(bDynamicMasking)
    {
		//static  unsigned int s_FrameCount = 0;
		if (StatisticStep == 0)
		{
			StatisticStep = 1;
		}
		if (m_nFrameCount % StatisticStep == 0)
		{
			//CPerfDetector perf(_T("DynamicMaskStatistic_MMX"));
			if (YStatisicThreshold == 0)
			{
				YStatisicThreshold = YThreshold - 40;
			}
			///////���ж�̬����״̬ͼ��ͳ�ƹ���
			///////ֻ�ǽ���ͳ��ͼ�ļ��㣬����grayFrame�����ݽ��иı䡣
			DynamicMaskStatistic_Gray_MMX(grayFrame.GetData(), StatisticalFrame.GetData(), YStatisicThreshold, nPixelCount);

			m_nFrameCount = 0;
		}
		m_nFrameCount++;

        //���ö�̬����ͼ����Դ����
        //YUY2FrameMask_MMX(m_pYUY2Frame->GetData(),DynamicMaskFrame.GetData(), nPixelCount);
        GrayFrameMask_MMX(m_pGrayFrame->GetData(), DynamicMaskFrame.GetData(),nPixelCount);
	}

    //YUY2ͼƬ֡��������תΪ��λλͼ
    {    
        //ת��Ϊ��λλͼ
        //YUY2ToBitFrame_SSE(m_pYUY2Frame->GetData(), (BYTE*)m_pBitFrame->GetData(),YThreshold, nPixelCount);
        //GrayToBitFrame_SSE2(m_pGrayFrame->GetData(), (BYTE*)m_pBitFrame->GetData(),YThreshold, nPixelCount);
        BinarizeGrayToBitFrame_SSE2(m_pGrayFrame->GetData(), (BYTE*)m_pBitFrame->GetData(),YThreshold, nPixelCount);
        BitToGrayFrame_MMX((BYTE*)m_pBitFrame->GetData(), m_pGrayFrame->GetData(), nPixelCount);

    }
    //�ڵ�λλͼ�м��Ŀ��
    {
        m_pBlobScanAlgo->ProcessImage((const BYTE*)m_pBitFrame->GetData(), m_pBitFrame->Width(), m_pBitFrame->Height());
    }
    UINT nObjCount = m_pBlobScanAlgo->GetObjCount();
    return nObjCount;
}

//@����:��ȡĿ���б�
const TBlobObject* CBlobDetector::GetObjs()const
{
    return m_pBlobScanAlgo->GetObjs();
}

TBlobObject* CBlobDetector::GetObjs()
{
	return m_pBlobScanAlgo->GetObjs();
}

const CImageFrame* CBlobDetector::GetBinarizedImage() const 
{
    return m_pGrayFrame;
}
//====================================================
//Part 3.����У���㷨


CalibrateAlgo::CalibrateAlgo()
:
m_pCalibrateInst(NULL)
{
    //m_pCalibrateInst = new Calibrator_PinHoleCameraModel();
    m_pcsForParam = new CComAutoCriticalSection();
}

CalibrateAlgo::~CalibrateAlgo()
{
    if(m_pCalibrateInst)
    {
        delete m_pCalibrateInst;
    }

    if(m_pcsForParam)
    {
        delete m_pcsForParam;
    }
}

void CalibrateAlgo::CreateCalibrateInst(E_CALIBRATE_MODEL eCalibrateModel)
{
    if (m_pCalibrateInst)
    {
        delete m_pCalibrateInst;
        m_pCalibrateInst = NULL;
    }

    switch (eCalibrateModel)
    {
    case E_CALIBRATE_MODEL_GENERICAL_CAMERA:
        m_pCalibrateInst = new Calibrator_GenericCameraModel();
        break;

    case E_CALIBRATE_MODEL_4_POINST_PERSPECTIVE:
        m_pCalibrateInst = new Calibrator_4PointsPerspectiveCameraModel();
        break;
        
    }

}

//@����:����У������
//@����:calibData ,����, ������Ļ��У������
//      bDebug, ���Ա�־
BOOL CalibrateAlgo::CaclCalibrationParams(const TCalibData& calibData, BOOL bDebug)
{
     CComCritSecLock<CComCriticalSection> lock(*m_pcsForParam);
    int N = calibData.allMonitorCalibData[0].calibData.size();
   
    if(NULL == m_pCalibrateInst || calibData.eCalibrateModel != m_pCalibrateInst->GetCalibrateModel())
    {
        CreateCalibrateInst(calibData.eCalibrateModel);
    }

    BOOL bRet = m_pCalibrateInst->CalcParams(calibData, bDebug);

    return bRet;

}

 //@����:����У�����̲���
const TCalibParams* CalibrateAlgo::GetCalibParams()const
{
    if(m_pCalibrateInst)
    {
        return m_pCalibrateInst->GetCalibParams();
    }
    return NULL;
}

//@����:����У�����̲���
void CalibrateAlgo::SetCalibParams(const TCalibParams& params)
{   
    CComCritSecLock<CComCriticalSection> lock(*m_pcsForParam);

    if( 0 == params.allCalibCoefs.size()) return;

    if (NULL == m_pCalibrateInst || params.eCalibrateModel != m_pCalibrateInst->GetCalibrateModel())
    {
        CreateCalibrateInst(params.eCalibrateModel);
    }

    m_pCalibrateInst->SetCalibParams(params);

}


//@����:��������ͷ����Ƶ�������ó���Ļ����
BOOL CalibrateAlgo::GetScreenCoord(const TPoint2D& ptVideo, TPoint2D* ptScreen, int nMonitorId, BOOL bWithoutAutoCalibCompensate, TCameraDebugData* pDebugOutput)
{
    CComCritSecLock<CComCriticalSection> lock(*m_pcsForParam);

    if(NULL == m_pCalibrateInst)
    {
        return FALSE;
    }
    return m_pCalibrateInst->GetScreenPt(ptVideo, ptScreen, nMonitorId, bWithoutAutoCalibCompensate, pDebugOutput);
}


//@����:��3D��������ӳ��Ϊ2D���ͼ������
BOOL CalibrateAlgo::MapWorldToImage(const TPoint3D* ptWorld, int nPtCount, TPoint2D* ptImage, int nMonitorId)
{
	CComCritSecLock<CComCriticalSection> lock(*m_pcsForParam);

	if (NULL == m_pCalibrateInst)
	{
		return FALSE;
	}

	return m_pCalibrateInst->MapWorldToImage(ptWorld, nPtCount, ptImage, nMonitorId);
}


/*
//@����:��������ͷ����Ƶ�������ó���Ļ����
//@����:pImgPts, ָ���ߵ��������������ָ��
//      prcBounds, ָ���ߵ������������ָ��
//      nPtNumber, ��߸���
//      pScreenPts, ָ�򱣴���Ļ���������ָ��
//      nMonitorId, ��ĻId,��һ����ͷ�������Ļʱ����Ч��
BOOL CalibrateAlgo::GetScreenCoord_Aug(const POINT* pImgPts,  const RECT* prcBounds, int nPtNumber, POINT* pScreenPts, int nMonitorId)
{
    CComCritSecLock<CComCriticalSection> lock(*m_pcsForParam);

    if(NULL == m_pCalibrateInst)
    {
        return FALSE;
    }

    return m_pCalibrateInst->GetScreenCoord_Aug(pImgPts, prcBounds, nPtNumber, pScreenPts, nMonitorId);
}
*/


/*
//@����:��������ͷ����Ƶ�������ó���Ļ����
//@����:pCentroid, ��ߵ�����
//      pContourPoints, ָ���������������
//      nContourPointCount, ������������
//      pScreenPts, ָ�򱣴���Ļ������ڴ��ַ��ָ��
//      nMonitorId, ��ĻId,��һ����ͷ�������Ļʱ����Ч��

BOOL CalibrateAlgo::GetScreenCoord_WithLightSpotContour(const POINT& pCentroid,  const POINT* pContourPoints, int nContourPointCount, POINT* pScreenPts, int nMonitorId, POINT* debugPtContourCross)
{
    CComCritSecLock<CComCriticalSection> lock(*m_pcsForParam);

    if(NULL == m_pCalibrateInst)
    {
        return FALSE;
    }

    return m_pCalibrateInst->GetScreenCoord_WithLightSpotContour(
        pCentroid,
        pContourPoints,
        nContourPointCount,
        pScreenPts,
        nMonitorId,
        debugPtContourCross);

}
*/

//@����:���þ�ͷ�������
void CalibrateAlgo::SetLensSpecification(const TLensSpecification& lensSpec)
{
    if(NULL == m_pCalibrateInst)
    {
         m_pCalibrateInst = new Calibrator_GenericCameraModel();
    }
    m_pCalibrateInst->SetLensSpecification(lensSpec);
}

//@����:����CMOSоƬ�������
void CalibrateAlgo::SetCMOSChipSpecification(const TCMOSChipSpecification& cmosChipSpec)
{
    if(NULL == m_pCalibrateInst)
    {
         m_pCalibrateInst = new Calibrator_GenericCameraModel();
    }
    m_pCalibrateInst->SetCMOSChipSpecification(cmosChipSpec);
}

//@����:�����Զ�У����������
void CalibrateAlgo::SetAutoCalibCompCoefs(const TAutoCalibCompCoefs& autoCalibCompCoefs)
{
    if(NULL == m_pCalibrateInst)
    {
         m_pCalibrateInst = new Calibrator_GenericCameraModel();
    }
    m_pCalibrateInst->SetAutoCalibCompCoefs(autoCalibCompCoefs);
}


//@����:���þ�ͷ����֪���ڲ������ͶԳƻ������
void CalibrateAlgo::SetLensInternalAndSymmmetricDistortParams(const TInternalAndSymmetricDistortParams* pParams)
{
	if (NULL == m_pCalibrateInst)
	{
		m_pCalibrateInst = new Calibrator_GenericCameraModel();
	}
	m_pCalibrateInst->SetLensInternalAndSymmetricDistortParams(pParams);

}


//@����:����ָ�����͵�У��ʵ��
void CalibrateAlgo::GenCalibratorInst(E_CALIBRATE_MODEL eCalibrateModel)
{
    if (NULL == m_pCalibrateInst || eCalibrateModel != m_pCalibrateInst->GetCalibrateModel())
    {

        CreateCalibrateInst(eCalibrateModel);
    }
}
//=====================================================
//Part 4.��Ļ�������
CScreenAreaDetectorImpl* GetAreaDetectorInstance()
{
    static CScreenAreaDetectorImpl s_ScreenAreaDetectorImpl;
    return &s_ScreenAreaDetectorImpl;
}



//@�� ��:�������,ͬʱ��ʾȫ������
//@�� ��:hNotifyWnd, ����������֪ͨ�Ĵ�����
//@����ֵ;	
BOOL CScreenAreaDetector::StartDetect(HWND hNotifyWnd)
{
    return GetAreaDetectorInstance()->StartDetect(hNotifyWnd);
}


//@��  ��:������⹦��,
//@��  ��:
//@����ָ:
BOOL CScreenAreaDetector::End()
{
    return  GetAreaDetectorInstance()->End();
}


//@����:������Ļ��������λͼ
const CImageFrame& CScreenAreaDetector::GetMaskFrame()const
{
    return GetAreaDetectorInstance()->GetMaskFrame();
}

CScreenAreaDetector::operator INewFrameEventSink*()
{
    return GetAreaDetectorInstance();
}


CScreenAreaDetector::operator INewFrameEventSink&()
{

    return *GetAreaDetectorInstance();
}



//=========================
//Part 5
//=========================
extern "C" ALGOLIB_API void ALGOAPI_SetIntermediateDir(const TCHAR* szIntermediateDir)
{
    g_strIntermediatRootDirName =  szIntermediateDir;
}

extern "C" ALGOLIB_API const TCHAR* ALGOAPI_GetIntermediateDir()
{
    return g_strIntermediatRootDirName;
}