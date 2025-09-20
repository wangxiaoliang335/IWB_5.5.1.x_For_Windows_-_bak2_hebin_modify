#include "stdafx.h"
//#include "headers.h"
#include "../png/pngapi.h"

//extern unsigned char ascii_font_5X7[][5];
//void RgbToYuv(BYTE R,BYTE G,BYTE B, BYTE *y,BYTE *u,BYTE *v)
//{
//
//    double rr=R,bb=B,gg=G;
//    double yy,uu,vv;
//
//    yy = 0.299*rr  + 0.587*gg   + 0.114*bb;
//    uu = -0.169*rr + -0.331*gg  +  0.5*bb;
//    vv  =0.5*rr    + -0.419*gg  + -0.081*bb;
//
//
//    if(uu>127) uu=127;
//    if(uu<-127) uu=-127;
//    *u=(BYTE)(uu);
//
//    if(vv>127) vv=127;
//    if(vv<-127) vv=-127;
//    *v=(BYTE)(vv);
//
//    if(yy>255) yy=255;
//    if(yy<0) yy=0;
//    *y=(BYTE)(yy);
//}


//;��      ��   ��                    ����    ��
//;�� Y-16 ��   ��    66    129    25 ���� R  ��
//;�� U-128�� =(��   -38   -74    112 ���� G  ��) >> 8
//;�� V-128��   ��    112   -94   -18 ���� B  ��
//;��      ��   ��                    ����    ��
void RgbToYuv(BYTE R,BYTE G,BYTE B, BYTE *y,BYTE *u,BYTE *v)
{

    int Y, U, V;


    Y = 66 * (int)R + 129 * (int)G +  25 * (int)B;
    U =-38 * (int)R -  74 * (int)G + 112 * (int)B;
    V =112 * (int)R -  94 * (int)G - 18  * (int)B;

    Y >>= 8;
    U >>= 8;
    V >>= 8;

    Y += 16;
    U += 128;
    V += 128;

    *y = Y & 0xFF;
    *u = U & 0xFF;
    *v = V & 0xFF;
}


BOOL SaveBGRAFrame(const CBRGAFrame& frame)
{

    SYSTEMTIME st;
    GetLocalTime(&st);
    TCHAR szFilePath[MAX_PATH];

    _stprintf_s(
        szFilePath,
        _countof(szFilePath),
        _T(".\\Intermediate\\Capture\\%4d-%2d-%2d\\%02d-%02d-%02d-%3d.png"),
        st.wYear,
        st.wMonth,
        st.wDay,
        st.wHour,
        st.wMinute,
        st.wSecond,
        st.wMilliseconds);

    CAtlString strFullPath  = CheckAndCreatePath(szFilePath);

    bool bRet = SaveBGRAAsPng(CT2CA(strFullPath), frame.GetData(), frame.Width(), frame.Height());

    return bRet?TRUE:FALSE;
}

//<<2015-08-18
//@����:���ȿ��ƻص�����
//@����:bInc, 
//      absDiff,
//      lpCtxData, ����������
bool CInterceptFilter::BrightnessControl(bool bInc, BYTE absDiff,BYTE nIndex, LPVOID lpCtxData)
{
#ifdef _DEBUG
    //CPerfDetector perf(_T("BrightnessControl"));
#endif

    CIWBSensor* lpSensor = reinterpret_cast<CIWBSensor*>(lpCtxData);
    if(lpSensor == NULL) return false;
    BOOL bRet = CIWBSensor::OnAutoCalibChangeCameraParams(bInc?E_CAMERA_BRIGHTNESS_INCREASE:E_CAMERA_BRIGHTNESS_DECREASE, (LPVOID)lpSensor , absDiff,nIndex);
    return bRet?true:false;
}

//@����:��YUY2��ʽͼƬת��Ϊ8λ�Ҷȸ�ʽ
inline _declspec(naked)  void YUY2ToGray(const BYTE* pYUY2, int nPixelCount, BYTE* pGray)
{
    static const unsigned char mask[16]={0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF, 0x00,0xFF, 0x00,0xFF, 0x00,0xFF, 0x00, 0xFF,0x00};

    _asm
    {

        pushad
            //reset the floating-point unit
            finit

            mov esi, [esp + 32 + 4 ] ;pointer to YUY2 source data
            mov ecx, [esp + 32 + 8 ] ;pixel count
            mov edi, [esp + 32 + 12] ;pointer to gray destination data
            movdqu xmm2, xmmword ptr mask;


next_16_pixel:

        //xmm0 = V3 Y7 U3 Y6 V2 Y5 U2 Y4 V1 Y3 U1 Y2 V0 Y1 U0 Y0
        //movdqa:Stores 128-bit value. Address p must be 16 byte aligned
        movdqa xmm0, xmmword ptr[esi];//SSE
        add esi, 16;

        movdqa xmm1, xmmword ptr[esi];//SSE
        add esi, 16;

        pand xmm0, xmm2 //pand yuy2 with mask
        pand xmm1, xmm2 //pand yuy2 with mask

        //xmm0=Y15 Y14...Y7 Y6...Y1 Y0
        packuswb xmm0, xmm1

        movapd [edi], xmm0
        add edi,16;//eight pixels;

        sub ecx,16;//sixteen pixels are ok

        jnz next_16_pixel;

        //Empty the multimedia state.
        emms

        popad

        ret

    }//asm

}

//>>


static YUVColor  YUV_RED     = {76,  84,   255};
static YUVColor  YUV_WHITE   = {255, 128,  128}; 
static YUVColor  YUV_GREEN   = {149, 43,   21}; 
static YUVColor  YUV_BLUE    = {29 , 255,  107}; 
static YUVColor  YUV_YELLOW  = {225 , 0,  148}; 
static YUVColor  YUV_BLACK  =  {16 , 128, 128}; 
static YUVColor  YUV_CYAN   =  {178 ,171, 0  }; 
static YUVColor  YUV_PURPLE =  {105, 212, 234}; 

#define ARGB_RED    0xFFFF0000
#define ARGB_YELLOW 0xFFFFFF00
#define ARGB_BLUE   0xFF0000FF
#define ARGB_PURPLE 0xFFFF00FF
#define ARGB_CYAN   0xFF00FFFF
#define ARGB_GREEN  0xFF00FF00
/*
CInterceptFilter::CInterceptFilter(CPenPosDetector* pPenPosDetector,CVideoPlayer* pVideoPlayer)
:
CTransformFilter(_T("Motion Detect Filter"),NULL, CLSID_MotionDetectFilter),
m_nRawImageWidth(640),
m_nRawImageHeight(480),
m_bIsRecording(FALSE),
m_bSrcFromAVI(FALSE),
m_pPenPosDetector(pPenPosDetector),
m_pVideoPlayer(pVideoPlayer),
m_nDisplayCounter(0),
m_dwImageType(MJPG),
m_oAutoBrightnessRegulator(100, BrightnessControl, NULL),
m_pSensor(NULL)
{

}
*/

CInterceptFilter::CInterceptFilter(CIWBSensor* pSensor)
:
CTransformFilter(_T("Motion Detect Filter"),NULL, CLSID_MotionDetectFilter),
m_nRawImageWidth(640),
m_nRawImageHeight(480),
m_bIsRecording(FALSE),
m_bSrcFromAVI(FALSE),
m_pPenPosDetector(pSensor->GetPenPosDetector()),
m_pVideoPlayer(pSensor->GetVideoPlayer()),
m_nDisplayCounter(0),
m_dwImageType(MJPG),
m_pSensor(pSensor),
m_oAutoBrightnessRegulator(50,BrightnessControl, pSensor),
m_bEnableBrightnessAutoRegulating(FALSE),
m_bCaptureImage(FALSE),
m_nFrameSkipCount(0),
m_oFpsDetector(60),
m_bStartDrawOnlineScreenArea(false),
m_bIsDetecting(FALSE)
{


}
CInterceptFilter::~CInterceptFilter(void)
{
    NOTE("CInterceptFilter::~CInterceptFilter");

    if(m_bSrcFromAVI)
    {
        m_oAviInput.Close();
    }

    if(this->IsRecording())
    {
        StopRecording();
    }

    //if(m_pPenPosDetector->IsDetecting())
    //{
    m_pPenPosDetector->StopDetect();
    //}

}

void CInterceptFilter::AttachPenPosDetector(CPenPosDetector* pPenPosDetector)
{
    m_pPenPosDetector = pPenPosDetector;
}


void CInterceptFilter::AttachVideoPlayer(CVideoPlayer* pVideoPlayer)
{
    m_pVideoPlayer = pVideoPlayer;
}

void CInterceptFilter::SetFrameSkipCount(int nFrameskipcount)
{
	if (nFrameskipcount > m_nFrameSkipCount)
	{
		m_nFrameSkipCount = nFrameskipcount;
	}

}



//The folloing 4 virtual function must be implemented
// check if you can support mtIn
HRESULT  CInterceptFilter:: CheckInputType(const CMediaType* mtIn) 
{
    DebugMediaType(mtIn);
    return NOERROR;
}

// check if you can support the transform from this input to this output
HRESULT  CInterceptFilter::CheckTransform(const CMediaType* mtIn, const CMediaType* mtOut) 
{
    DebugMediaType(mtIn);
    DebugMediaType(mtOut);

    return NOERROR;
}


// this goes in the factory template table to create new instances
// static CCOMObject * CreateInstance(LPUNKNOWN, HRESULT *);

// call the SetProperties function with appropriate arguments

//
//Noted:
//     Called by CTransformOutputPin::DecideBufferSize
//
//
HRESULT  CInterceptFilter::DecideBufferSize(
    IMemAllocator * pAllocator,
    ALLOCATOR_PROPERTIES *pprop)  
{
    //LOG_INFO("CInterceptFilter::DecideBufferSize");

    CMediaType mt(m_pInput->CurrentMediaType());


    pprop->cBuffers = 2;
    pprop->cbBuffer = mt.GetSampleSize();;
    pprop->cbAlign  = 2;

    ALLOCATOR_PROPERTIES actual;
    HRESULT hr = pAllocator->SetProperties(pprop, &actual);
    if(FAILED(hr))
    {
        return hr;
    }

    // Ask the allocator to reserve us some sample memory, NOTE the function
    // can succeed (that is return NOERROR) but still not have allocated the
    // memory that we requested, so we must check we got whatever we wanted
    if(actual.cBuffers < pprop->cBuffers || actual.cbBuffer < pprop->cbBuffer)
    {
        return E_FAIL;
    }
    return NOERROR;
}



// override to suggest OUTPUT pin media types
HRESULT  CInterceptFilter::GetMediaType(int iPosition, CMediaType *pMediaType)  
{
    NOTE("CInterceptFilter::GetMediaType");
    ASSERT(m_pInput->IsConnected());
    if (iPosition < 0)
    {
        return E_INVALIDARG;
    }
    if (iPosition == 0)
    {
        HRESULT hr = m_pInput->ConnectionMediaType(pMediaType);
        if (FAILED(hr))
        {
            return hr;
        }

        //FOURCCMap fccMap = FCC('MRLE'); 
        //pMediaType->subtype = static_cast<GUID>(fccMap);
        //pMediaType->SetVariableSize();
        //pMediaType->SetTemporalCompression(FALSE);

        ASSERT(pMediaType->formattype == FORMAT_VideoInfo);
        VIDEOINFOHEADER *pVih =
            reinterpret_cast<VIDEOINFOHEADER*>(pMediaType->pbFormat);

        //<<added by toxuke@gmail.com, 2013/02/27
        pVih->bmiHeader.biCompression = YUY2;
        pVih->bmiHeader.biBitCount    = 16;
        pVih->bmiHeader.biSizeImage   = pVih->bmiHeader.biHeight * pVih->bmiHeader.biWidth * 2;
        pMediaType->SetSubtype(&MEDIASUBTYPE_YUY2);
        //>>

        if (m_pPenPosDetector)
        {
            if (pVih->bmiHeader.biWidth >= 1200)
            {
                m_pPenPosDetector->LoadConfig(TRUE);
            }
            else
            {
                m_pPenPosDetector->LoadConfig(FALSE);
            }
        }

        return S_OK;
    }
    // else
    return VFW_S_NO_MORE_ITEMS;
}

void CInterceptFilter::CaptureImage()
{

    this->m_bCaptureImage = TRUE;
}

BOOL SaveGrayFrameExt3(const CImageFrame& grayFrame, CAtlString& strFullPath, int index)
{

    SYSTEMTIME st;
    GetLocalTime(&st);
    TCHAR szFilePath[MAX_PATH];

    _stprintf_s(
        szFilePath,
        _countof(szFilePath),
        _T(".\\Intermediate\\Capture\\ext3-%4d-%2d-%2d\\i-%d-%02d-%02d-%02d-%3d.png"),
        st.wYear,
        st.wMonth,
        st.wDay,
        index,
        st.wHour,
        st.wMinute,
        st.wSecond,
        st.wMilliseconds);

    //OutputDebugString(szFilePath);
    //OutputDebugString(_T("\n"));

    strFullPath = CheckAndCreatePath(szFilePath);

    bool bRet = SaveGrayAsPng(CT2CA(strFullPath), grayFrame.GetData(), grayFrame.Width(), grayFrame.Height());

    return bRet ? TRUE : FALSE;
}

// Transform place holder - should never be called
HRESULT CInterceptFilter::Transform(IMediaSample * pIn, IMediaSample *pOut)
{
    if (!m_bIsDetecting) return S_OK;

    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

    m_oLostFrameDetector.DoDetect();

    //֡�ʴ���
    m_oFpsDetector.Trigger();

    BYTE* pDestBuf;
    BYTE* pSrcBuf;
    HRESULT hr = S_OK;
    hr = pOut->GetPointer(&pDestBuf);
    if (FAILED(hr))
    {
        return hr;
    }


	hr = pIn->GetPointer(&pSrcBuf);
    if (FAILED(hr))
    {
        return hr;
    }

    //�ж���Ƶ��ʽ
    unsigned int bytesPerPixel = 2;

    CMediaType mt(m_pInput->CurrentMediaType());

    int nMJPGDataLength = 0;
    int nImageDataLength = 0;
    CAtlString strFullPath;
    static int index = 0;

    //ģ������
    if (m_bSrcFromAVI)
    {
        const BITMAPINFOHEADER* bmpInfoHeader = m_oAviInput.GetAVIFormat();

        if (bmpInfoHeader)
        {
            m_MJPGFrame.SetSize(bmpInfoHeader->biHeight, bmpInfoHeader->biWidth, bmpInfoHeader->biBitCount / 8);

            LONG lBytes, lSamples;
            if (!m_oAviInput.Read((BYTE*)m_MJPGFrame.GetData(), m_MJPGFrame.Size(), &lBytes, &lSamples))
            {
                m_oAviInput.Close();
                m_bSrcFromAVI = FALSE;
            }
            if (bmpInfoHeader->biCompression == MJPG)
            {
                m_oMJPGDecoder.ProcessData(m_MJPGFrame.GetData(), lBytes, (BYTE *)m_GraySrcFrame.GetData(), &nMJPGDataLength);
                pSrcBuf = m_MJPGFrame.GetData();
                index++;
                //SaveGrayFrameExt3(m_GraySrcFrame, strFullPath, index);  //wxl modify test
            }
            else if (bmpInfoHeader->biCompression == YUY2)
            {
                YUY2ToGray(m_MJPGFrame.GetData(), bmpInfoHeader->biHeight * bmpInfoHeader->biWidth, m_GraySrcFrame.GetData());
                pSrcBuf = m_MJPGFrame.GetData();
            }
            else
            {
                return E_FAIL;//��֧��
            }
        }
    }
    else
    {
        if (FORMAT_VideoInfo2 == mt.formattype)
        {
            VIDEOINFOHEADER2* pVideoInfoHeader2 = reinterpret_cast<VIDEOINFOHEADER2*>(mt.pbFormat);

            m_nRawImageWidth = pVideoInfoHeader2->bmiHeader.biWidth;
            m_nRawImageHeight = pVideoInfoHeader2->bmiHeader.biHeight;
            m_dwImageType = pVideoInfoHeader2->bmiHeader.biCompression;
            nImageDataLength = pVideoInfoHeader2->bmiHeader.biSizeImage;

            m_pPenPosDetector->SetSrcImageSize(m_nRawImageWidth, m_nRawImageHeight);

            m_GraySrcFrame.SetSize(m_nRawImageWidth, m_nRawImageHeight, 1);

            if (pVideoInfoHeader2->bmiHeader.biCompression == MJPG/*MJPG*/)
            {
                //   m_dwImageType= MJPG;

                if (pSrcBuf[0] != 0xFF || pSrcBuf[1] != 0xd8 || pSrcBuf[2] != 0xFF || pSrcBuf[3] != 0xdb)
                {
                    AtlTrace(_T("Invalid JPEG Frame\r\n"));
                    return E_FAIL;
                }

                //��MJPGͼƬ֡����Ϊ�Ҷ�ͼ��
                m_oMJPGDecoder.ProcessData(pSrcBuf, pIn->GetSize(), (BYTE *)m_GraySrcFrame.GetData(), &nMJPGDataLength);
                nImageDataLength = nMJPGDataLength;
            }

            else if (pVideoInfoHeader2->bmiHeader.biCompression == YUY2)
            {
                //m_dwImageType= YUY2;
                YUY2ToGray(pSrcBuf, m_nRawImageWidth*m_nRawImageHeight, m_GraySrcFrame.GetData());
            }
            else
            {
                return E_FAIL;//��֧��
            }
        }
        else if (FORMAT_VideoInfo == mt.formattype)
        {
            VIDEOINFOHEADER* pVideoInfoHeader = reinterpret_cast<VIDEOINFOHEADER*>(mt.pbFormat);

            m_nRawImageWidth = pVideoInfoHeader->bmiHeader.biWidth;
            m_nRawImageHeight = pVideoInfoHeader->bmiHeader.biHeight;
            m_dwImageType = pVideoInfoHeader->bmiHeader.biCompression;
            nImageDataLength = pVideoInfoHeader->bmiHeader.biSizeImage;

            m_pPenPosDetector->SetSrcImageSize(m_nRawImageWidth, m_nRawImageHeight);

            m_GraySrcFrame.SetSize(m_nRawImageWidth, m_nRawImageHeight, 1);

            if (pVideoInfoHeader->bmiHeader.biCompression == MJPG/*MJPG*/)
            {
                //	CPerfDetector pfer(_T("CPerfDetector:: MJPG"));
                //0xd8:M_SOI, Start of Image
                //0xd9:M_EOI, End of Image
                //0xdb:M_DQT
                //0xE0:M_APP0, Application-specific marker, type N
                //if(pSrcBuf[0] != 0xFF || pSrcBuf[1] != 0xd8 || pSrcBuf[2] != 0xFF || pSrcBuf[3] != 0xdb)
                if (pSrcBuf[0] != 0xFF || pSrcBuf[1] != 0xd8 || pSrcBuf[2] != 0xFF)
                {
                    AtlTrace(_T("Invalid JPEG Frame\r\n"));
                }

                //				static bool bDebug = false;
                //				if (bDebug)
                //				{
                //					std::fstream dbgFile;
                //					static int debug_count = 0;
                //					char szFileName[MAX_PATH];
                //					sprintf_s(szFileName, _countof(szFileName), "k:\\temp\\debug_%d.jpg", debug_count++);
                //					dbgFile.open(szFileName, std::ios_base::binary | std::ios_base::out);
                //
                //					dbgFile.write((const char*)pSrcBuf, pIn->GetSize());
                //
                //					dbgFile.close();
                //				}

                //��MJPGͼƬ֡����Ϊ�Ҷ�ͼ��
                m_oMJPGDecoder.ProcessData(pSrcBuf, pIn->GetSize(), (BYTE*)m_GraySrcFrame.GetData(), &nMJPGDataLength);

                //������ݻ�ѹ������˻��С��
                nImageDataLength = nMJPGDataLength;
                //				if (bDebug)
                //				{
                //					std::fstream dbgFile;
                //					static int debug_count = 0;
                //					char szFileName[MAX_PATH];
                //					sprintf_s(szFileName, _countof(szFileName), "k:\\temp\\debug2_%d.jpg", debug_count++);
                //					dbgFile.open(szFileName, std::ios_base::binary | std::ios_base::out);
                //
                //					dbgFile.write((const char*)pSrcBuf, nMJPGDataLength);
                //
                //					dbgFile.close();
                //				}

            }

            else if (pVideoInfoHeader->bmiHeader.biCompression == YUY2)   //˵��ͼ��ĸ�ʽ��YUY2
            {
                YUY2ToGray(pSrcBuf, m_nRawImageWidth*m_nRawImageHeight, m_GraySrcFrame.GetData());

            }
            else
            {
                return E_FAIL;//��֧��
            }
        }

    }//else

#if 1
    static bool bSimulateObj = false;
    if (bSimulateObj && m_pSensor->GetID() == 0)
    {
        int x = 358;
        int y = 303;
        BYTE* pData = m_GraySrcFrame.GetData();
        int nSize = 6;
        
        int nStartRow = y - nSize / 2;
        int nEndRow = nStartRow + nSize;
        int nStartCol = x- nSize / 2;
        int nEndCol = nStartCol + nSize;
        BYTE* pRowStartData = pData + m_nRawImageWidth*nStartRow + nStartCol;

        for (int r = nStartRow; r < nEndRow; r++)
        {
            BYTE* p = pRowStartData;
            for (int c = nStartCol; c < nEndCol; c++)
            {
                *p = 0xFF;
                p++;
            }
            pRowStartData += m_nRawImageWidth;
        }
    }
#endif
    //<<2015-08-18
    //�����Զ����ȿ���
    if (m_bEnableBrightnessAutoRegulating)
    {
        static unsigned int  s_AutoControlCount = 0;
        if(s_AutoControlCount % 6== 0)
        {
			int PID = 0;
			int VID = 0;
			m_pSensor->GetPidVid(&PID, &VID);
		    if (PID == 0x9186 && VID == 0x18ec )
		   {
                DoAutoBrightnessControl((const BYTE*)m_GraySrcFrame.GetData(), m_nRawImageWidth, m_nRawImageHeight);
		   }        
        }
        s_AutoControlCount ++;
    }
    //>>


    //¼����Ƶ
    if (IsRecording())
    {
        m_oAviRecorder.Write((const BYTE*)pSrcBuf, nImageDataLength);   //֧��YUV2¼��ģʽ
    }

    //����,����ͼƬ�����ص����ֵ����Сֵ
    //static BYTE MaxY = 0, MinY = 255, MaxU = 0, MinU= 255, MaxV = 0, MinV = 255;
    //FindMaxValue(pIn,MaxY, MinY, MaxU, MinU, MaxV, MinV);

    ////���������ͷģʽ�Ļ��Ͳ���Ҫ����ߵĴ���ֻ��Ҫ��ʾ����
    ////����ʹ��ģʽ�Ļ���Ҫ����ߴ����

    //ʹ�þ�̬����ͼ��������
    if (this->m_pPenPosDetector->GetManualScreenAreaMode() == E_ManualScreenAreaNormalMode &&  m_pPenPosDetector->IsStaticMasking())
    {
        //	CPerfDetector perf(_T("CInterceptFilter::static"));
        //������Դ���о�̬����
        //˵������ͼ�ĳߴ���ʵ�ʵ�ͼ�ĳߴ粻�����
        MaskFilterData((BYTE*)m_GraySrcFrame.GetData(), m_GraySrcFrame.Size(), this->m_pPenPosDetector->GetStaticMaskFrame());
    }

	if(this->m_pPenPosDetector->IsEnableOnlineScreenArea())
	{
		MaskFilterData((BYTE*)m_GraySrcFrame.GetData(), m_GraySrcFrame.Size(), this->m_pPenPosDetector->GetManualOnLineScreenArea());
	}

    if (m_nFrameSkipCount > 0)
    {
        m_nFrameSkipCount--;
        return S_OK;
    }
    //֪ͨ��Ҫ֡���ݵ��¼���
    NotifySink(&m_GraySrcFrame);
    ////���������ͷģʽ�Ļ�������߼�⹤����
    map<int, pointRect> point_nID;
    m_pPenPosDetector->DoDetect(&m_GraySrcFrame, m_pSensor->GetLensMode(), point_nID, strFullPath, index);
    //wchar_t szTmp[1024] = { 0 };
    //wsprintf(szTmp, L" xxxxxx m_pPenPosDetector:%p\n", m_pPenPosDetector);
    //OutputDebugString(szTmp);

    if (this->m_pVideoPlayer)
    {
        HWND hWnd = m_pVideoPlayer->GetPlayWnd();

        //���Ŵ��岻�ɼ�,���й�����ʾ�Ĳ����������Խ�Լ�����CPU��Դ��
        if (!IsWindowVisible(hWnd))
        {
            return S_OK;
        }

        m_nDisplayCounter++;

        if (m_nDisplayCounter % 4 != 0)//������ʾ,����CPU��Դ����  //m_nDisplayCounter % 2 == 0
        {
            return S_OK;
        }

        m_pVideoPlayer->UpdateVideoStreamForamtInfo(m_nRawImageWidth, m_nRawImageHeight, m_dwImageType, m_oFpsDetector.GetCurrentFps(), m_pSensor->GetID());

    }

    if (m_pPenPosDetector->IsStaticMasking())
    {
        if (m_nRawImageWidth != m_pPenPosDetector->GetStaticMaskFrame().Width() || m_nRawImageHeight != m_pPenPosDetector->GetStaticMaskFrame().Height())
        {
            UpdateARGBFrameWithoutMask();
        }
        else 
		{
            UpdateARGBFrameWithMask();//��������ͼ��Ϣ��ͼƬ��Ϣ��ARGBͼƬ֡�С�        
		}
    }
    else
    {
        UpdateARGBFrameWithoutMask();
    }

    //������һ�μ�⵽��Ŀ����Ӿ���
    int nObjCount = m_pPenPosDetector->GetObjCount();
    //    const RECT* pBounds   = m_pPenPosDetector->GetLightSpotBounds();

    const LightSpotBounds* LightspotInfo = m_pPenPosDetector->GetLightSpotInfo();

    //const POINT (&aryContourCrossPoints)[MAX_OBJ_NUMBER][4]  = m_pPenPosDetector->GetContourCrossPoints();
    char szSize[256] = "";
    for (int i = 0; i < nObjCount; i++)
    {
        if (m_pPenPosDetector->IsSpotRectVisible())
        {
            //DrawRectBound(pDestBuf,pBounds[i], YUV_BLUE);
            if (LightspotInfo[i].valid)
            {
                m_BGRAFrame.DrawRectange(LightspotInfo[i].m_aryLightSpotBounds, ARGB_BLUE);
            }
            else 
            {
                m_BGRAFrame.DrawRectange(LightspotInfo[i].m_aryLightSpotBounds, ARGB_PURPLE);
            }

        }

        if (m_pPenPosDetector->IsSpotSizeInfoVisible())
        {
			SIZE MinSize = m_pPenPosDetector->GetMinimumLightSpotSize();
            int w = LightspotInfo[i].m_aryLightSpotBounds.right  - LightspotInfo[i].m_aryLightSpotBounds.left;
            int h = LightspotInfo[i].m_aryLightSpotBounds.bottom - LightspotInfo[i].m_aryLightSpotBounds.top;
            if (w > MinSize.cx && h > MinSize.cy )
            {
                bool bFindID = false;
                //wsprintf(szTmp, L" point_nID size:%d\n", point_nID.size());
                //OutputDebugString(szTmp);
                for (auto iter = point_nID.begin(); iter != point_nID.end(); iter++)
                {
                    //wsprintf(szTmp, L" rc left:%d, top:%d, right:%d, bottom:%d, m_aryLightSpotBounds left:%d, top:%d, right:%d, bottom:%d\n",
                    //    iter->second.rc_.left, iter->second.rc_.top, iter->second.rc_.right, iter->second.rc_.bottom,
                    //    LightspotInfo[i].m_aryLightSpotBounds.left, LightspotInfo[i].m_aryLightSpotBounds.top,
                    //    LightspotInfo[i].m_aryLightSpotBounds.right, LightspotInfo[i].m_aryLightSpotBounds.bottom);
                    //OutputDebugString(szTmp);

                    if (iter->second.rc_.left == LightspotInfo[i].m_aryLightSpotBounds.left &&
                        iter->second.rc_.top == LightspotInfo[i].m_aryLightSpotBounds.top &&
                        iter->second.rc_.right == LightspotInfo[i].m_aryLightSpotBounds.right &&
                        iter->second.rc_.bottom == LightspotInfo[i].m_aryLightSpotBounds.bottom)
                    {
                        //if (-1 != nID)
                        //{
                            sprintf_s(szSize, _countof(szSize), "<%d*%d>, nID:%d", w, h, iter->first);
                            bFindID = true;
                            break;
                        //}
                        //else
                        //{
                            //sprintf_s(szSize, _countof(szSize), "<%d*%d>", w, h);
                        //}
                    }
                }

                if (false == bFindID)
                {
                    sprintf_s(szSize, _countof(szSize), "<%d*%d> ", w, h);
                }
                m_BGRAFrame.PutStr(LightspotInfo[i].m_aryLightSpotBounds.right, LightspotInfo[i].m_aryLightSpotBounds.bottom, szSize, ARGB_YELLOW, 16);
            }
        }
 
    }

    if (m_pPenPosDetector->IsGuideRectangleVisible() && m_pSensor->GetLensMode() == E_VIDEO_TUNING_MODE)
    {
        ////////�Ƚ��е����ٽ��еõ�������
        m_pPenPosDetector->RegulateGuideRectangle(m_pSensor, m_nRawImageWidth, m_nRawImageHeight);

		RECT rcGuideRectangle;
        DWORD dwRGBColor;
        m_pPenPosDetector->GetGuideRectangle(&rcGuideRectangle, &dwRGBColor);

        //LOG_INF("left=%d ��right=%d,top=%d,bottom=%d...", rcGuideRectangle.left, rcGuideRectangle.right, rcGuideRectangle.top, rcGuideRectangle.bottom);
        //YUVColor yuv;

        BYTE R = (dwRGBColor >> 16) & 0xFF;
        BYTE G = (dwRGBColor >> 8) & 0xFF;
        BYTE B = dwRGBColor & 0xFF;
        //RgbToYuv(R,G,B, &yuv.Y, &yuv.U, &yuv.V);
        DWORD dwColor = (0xFF << 24) | (R << 16) | (G << 8) | B;

        //DrawRect(pDestBuf, rcGuideRectangle, yuv);
        m_BGRAFrame.DrawRectange(rcGuideRectangle, dwColor);
    }


    if (m_pPenPosDetector->IsShowingCalibratePoints())
    {
        const TCalibData& calibData = m_pPenPosDetector->GetVideoToScreenMap().GetCalibrateData();

        for (size_t index = 0; index < calibData.allMonitorCalibData.size(); index++)
        {
            const std::vector<TCalibCoordPair>& data = calibData.allMonitorCalibData[index].calibData;

            for (size_t i = 0; i < data.size(); i++)
            {

                POINT ptPixel;
                ptPixel.x = (int)(data[i].pt2DImageCoord.d[0] + .5);
                ptPixel.y = (int)(data[i].pt2DImageCoord.d[1] + .5);

                m_BGRAFrame.SetPixel(
                    ptPixel,
                    ARGB_YELLOW);

            }//for-each(i)

        }

        const TCameraDebugData* pCameraDebugData = m_pPenPosDetector->GetCameraDebugData();
        for (int i = 0; i < nObjCount; i++)
        {
            POINT ptPixel;
            ptPixel.x = int(pCameraDebugData[i].pt2DContactInImage.d[0] + .5);
            ptPixel.y = int(pCameraDebugData[i].pt2DContactInImage.d[1] + .5);
            m_BGRAFrame.SetPixel(
                ptPixel,
                ARGB_CYAN);


            ptPixel.x = int(pCameraDebugData[i].pt2DCentroid.d[0] + .5);
            ptPixel.y = int(pCameraDebugData[i].pt2DCentroid.d[1] + .5);
            m_BGRAFrame.SetPixel(
                ptPixel,
                ARGB_RED);

        }
    }


    int nBasePointCount = 0;
    CBaseStoneMarker&  baseStoneMarker = m_pPenPosDetector->GetBaseStoneMarker();

    //����4��궨��׼��
    if (baseStoneMarker.GetMachineState()  != CBaseStoneMarker::E_MACHINE_STATE_END)
    {
            //��õ�ǰ���c��������
            int baseStoneIndex = baseStoneMarker.GetCurrentBaseStoneIndex();

            TCHAR szText[1024];
            _stprintf_s(szText, _countof(szText), _T("˫��#%d�궨�c"), baseStoneIndex + 1);
            m_pVideoPlayer->SetDisplayInfo(szText, 30);

            nBasePointCount = baseStoneIndex;
    }
    else
    {
        nBasePointCount = 0;
        if (baseStoneMarker.IsDataValid())
        {
            nBasePointCount = 4;
        }
    }
   
    const TCalibParams* pCalibParams = m_pPenPosDetector->GetVideoToScreenMap().GetCalibParams();

    if (m_pPenPosDetector->IsMarkingBasestone() 
        || 
        (pCalibParams &&  pCalibParams->eCalibrateModel == E_CALIBRATE_MODEL_4_POINST_PERSPECTIVE))
    {
        const TPoint2D* pBasePoints = baseStoneMarker.GetBasePoints();

        if (nBasePointCount)
        {//��ʾ�������cʮ��
            for (int i = 0; i < nBasePointCount; i++)
            {
                int SIZE = 10;
                POINT center;
                center.x = pBasePoints[i].d[0];
                center.y = pBasePoints[i].d[1];

                m_BGRAFrame.DrawCross(
                    center,
                    SIZE,
                    ARGB_PURPLE);
            }


            if (1 < nBasePointCount)
            {
                POINT start, end;
                start.x = pBasePoints[0].d[0];
                start.y = pBasePoints[0].d[1];


                int nEdgeCount = nBasePointCount - 1;

                if (nBasePointCount == 4)
                {
                    nEdgeCount = 4;
                }

                int nLineNo = 0;

                int ptEndIdx = 1;

                while (nLineNo < nEdgeCount)
                {
                    end.x = pBasePoints[ptEndIdx].d[0];
                    end.y = pBasePoints[ptEndIdx].d[1];
                    m_BGRAFrame.Line(
                        start,
                        end,
                        ARGB_CYAN);

                    ptEndIdx++;
                    if (ptEndIdx == 4)
                    {
                        ptEndIdx = 0;
                    }

                    start = end;
                    nLineNo++;
                }//while

            }//if

        }//if(nBasePointCount)
    }

	if (m_bStartDrawOnlineScreenArea)
	{
	     std::vector<CPoint> vecpt;
	     m_pPenPosDetector->GetCurrentOnLineScreenAreaPt(vecpt);
	     int nCount = vecpt.size();
	     for (int Index = 0; Index < nCount-1; Index++ )
	     {
		     m_BGRAFrame.Line(vecpt[Index],vecpt[Index+1], ARGB_RED);
	     }
	     if (nCount > 0)
	     {
		     m_BGRAFrame.Line(vecpt[nCount-1], m_PtMove, ARGB_RED);
	     }
	}
	else
	{
		//if (m_pSensor->GetLensMode() == E_VIDEO_TUNING_MODE && this->m_pPenPosDetector->IsEnableOnlineScreenArea())
        if(this->m_pPenPosDetector->IsEnableOnlineScreenArea())
		{
			 std::vector<CPoint> vecFinishpt;
			 m_pPenPosDetector->GetCurrentOnLineScreenAreaPt(vecFinishpt);
			 int nCount = vecFinishpt.size();
			 for (int Index = 0; Index < nCount; Index++)
			 {
				 if (Index == nCount-1)
				 {
					  m_BGRAFrame.Line(vecFinishpt[Index], vecFinishpt[0], ARGB_GREEN);
				 }
				 else
				{                  
					 m_BGRAFrame.Line(vecFinishpt[Index], vecFinishpt[Index + 1], ARGB_GREEN);
				}
			}
		}
	}

    //��ʾ"���ƴ����¼�����"
    if (m_pPenPosDetector->IsTriggeringGuesture())
    {
        m_BGRAFrame.PutStr(
            10, 
            10,
            "Triggering Guesture",
            ARGB_RED, 
            16);

    }


#ifdef _DEBUG
    //����Ƶ����ʾУ����
    //const TCalibrateData& refCalibData =  g_oVideo2ScreenMap.GetCalibrateData();
    //for(size_t i=0; i<refCalibData.vecCalibratePoints.size(); i++)
    //{
    //    int x = refCalibData.vecCalibratePoints[i].second.x >> INT_SCALE_SHIFT_SIZE;
    //    int y = refCalibData.vecCalibratePoints[i].second.y >> INT_SCALE_SHIFT_SIZE;
    //    SetPixel(pDestBuf, x, y, YUV_CYAN);
    //}
#endif

    //SetPixel(pDestBuf, m_pPenPosDetector->m_ptContactVideoPos.x >> INT_SCALE_SHIFT_SIZE, m_pPenPosDetector->m_ptContactVideoPos.y >>INT_SCALE_SHIFT_SIZE, YUV_PURPLE);

    //<<aded by toxuke@gmail.com, 2013/05/17
    if(m_pVideoPlayer)
    {
        m_pVideoPlayer->DisplayFrame((const BYTE*)m_BGRAFrame.GetData(), m_nRawImageWidth, m_nRawImageHeight);
    }
    //>>

    if(m_bCaptureImage)
    {
        SaveBGRAFrame(m_BGRAFrame);
        m_bCaptureImage = FALSE;
    }

    return S_OK;
}



BOOL CInterceptFilter::StartDetect(HWND hDisplayWnd, int nSrcImageWidth, int nSrcImageHeight)
{
    m_oLostFrameDetector.Reset(30);

    m_oFpsDetector.Reset();

    m_pPenPosDetector->StartDetect(hDisplayWnd, nSrcImageWidth, nSrcImageHeight, m_pSensor);
    
     m_bIsDetecting = TRUE;

    return TRUE;

}

BOOL CInterceptFilter::StopDetect()
{
     m_pPenPosDetector->StopDetect();
     m_bIsDetecting = FALSE;
     return TRUE;
}

void CInterceptFilter::ViewMonoImage(HWND hWnd)
{
    m_pPenPosDetector->ViewSrcImage(hWnd, TRUE);
}


//@����:���ý���֪ͨ��Ϣ�Ĵ����֪ͨ��Ϣ
void CInterceptFilter::SetNotifyWindow(HWND hWnd, UINT uMsg)
{
    m_pPenPosDetector->SetNotifyWindow(hWnd, uMsg);
}



CBasePin * CInterceptFilter::GetPin(int n)
{
    HRESULT hr = S_OK;

    // Create an input pin if necessary

    if (m_pInput == NULL) {

        m_pInput = new CInterceptInputPin(NAME("Transform input pin"),
            this,              // Owner filter
            &hr,               // Result code
            L"XForm In");      // Pin name


        //  Can't fail
        ASSERT(SUCCEEDED(hr));
        if (m_pInput == NULL) {
            return NULL;
        }
        m_pOutput = (CTransformOutputPin *)
            new CTransformOutputPin(NAME("Transform output pin"),
            this,            // Owner filter
            &hr,             // Result code
            L"XForm Out");   // Pin name


        // Can't fail
        ASSERT(SUCCEEDED(hr));
        if (m_pOutput == NULL) {
            delete m_pInput;
            m_pInput = NULL;
        }
    }

    // Return the appropriate pin

    if (n == 0) {
        return m_pInput;
    } else
        if (n == 1) {
            return m_pOutput;
        } else {
            return NULL;
        }

}

const CPenPosDetector* CInterceptFilter::GetPenPosDetector()const
{
    return m_pPenPosDetector;
}

CPenPosDetector* CInterceptFilter::GetPenPosDetector()
{
    return m_pPenPosDetector;
}

const CVideoPlayer* CInterceptFilter::GetVideoPlayer()const
{
    return m_pVideoPlayer;
}

CVideoPlayer* CInterceptFilter::GetVideoPlayer()
{
    return m_pVideoPlayer;
}



//@����:������ͼ����ͼƬ֡
//@����:pGrayData, �Ҷ�ͼƬ֡
//      srcSize, ͼƬ֡�ߴ�
//      mask, ����ͼ
void CInterceptFilter::MaskFilterData(BYTE* pGrayData, LONG srcSize, const CImageFrame & mask )
{
    if(srcSize != mask.Height()* mask.Width())
    {
        return;
    }
    if(mask.Height() != m_nRawImageHeight  || mask.Width() != m_nRawImageWidth)
    {
        return;
    }

    DWORD* pdwSrcData        = reinterpret_cast<DWORD*>(pGrayData);
    const DWORD* pdwMaskData = reinterpret_cast<const DWORD*>(mask.GetData());

    int nLoopCount = mask.Height()* mask.Width();//
    _asm
    {
        pushad
            finit

            mov esi, pdwMaskData
            mov edi, pdwSrcData
            mov ecx, nLoopCount

_next_16_pixles:

        //һ���Դ���16�����ء�һ����������16���ֽ���ɡ�
        movapd xmm0,[esi];
        add esi,16

        movapd xmm4,[edi];
        andpd xmm4,xmm0
        movapd [edi],xmm4;

        add edi,16
        sub ecx,16 ;

        jnz _next_16_pixles;
        emms
            popad

    }
}


//@����:�ж��Ƿ�����¼��
BOOL CInterceptFilter::IsRecording()const
{
    return m_bIsRecording;
}

//@����:��ʼ¼��
BOOL CInterceptFilter::StartRecording(LPCTSTR lpszFilePath)
{
    if(m_bIsRecording)
    {
        StopRecording();
        m_bIsRecording = FALSE;
    }

    CAtlString strFileName;

    if(lpszFilePath == NULL)
    {
        SYSTEMTIME localTime;
        GetLocalTime(&localTime);
        strFileName.Format(
            _T("Sensor%d_%04d%02d%02d-%02d%02d%02d%03d.avi"),
            this->m_pPenPosDetector->GetID(),
            localTime.wYear,
            localTime.wMonth,
            localTime.wDay,
            localTime.wHour,
            localTime.wMinute,
            localTime.wSecond,
            localTime.wMilliseconds);

    }
    else
    {
        strFileName = lpszFilePath;
    }

    BOOL bOk = m_oAviRecorder.OpenFile(strFileName, m_nRawImageWidth, m_nRawImageHeight, 60, m_dwImageType);  //640,480,60

    if(bOk)
    {
        m_bIsRecording = TRUE;

        return TRUE;
    }	

    return FALSE;
}


//@����:ֹͣ¼��
BOOL CInterceptFilter::StopRecording()
{
   if(m_bIsRecording)
    {
        m_oAviRecorder.Close();
        m_bIsRecording = FALSE;

        return TRUE;

    }
    return FALSE;

}

//@����:��AVI�ļ���������Դ
BOOL  CInterceptFilter::InputFromAVIFile(LPCTSTR szAVIFilePath)
{

    if(m_bSrcFromAVI)
    {
        m_oAviInput.Close();
        m_bSrcFromAVI = FALSE;

    }
    BOOL bRet = m_oAviInput.OpenFile(szAVIFilePath);

    if(bRet)
    {
        m_bSrcFromAVI = TRUE;

        return TRUE;
    }
    return FALSE;
}


void CInterceptFilter::AddEventSink(INewFrameEventSink* pSink)
{
    CComCritSecLock<CComAutoCriticalSection> lock(m_csEventSinks);
    m_mapEventSinks[pSink] = pSink;

}

void CInterceptFilter::RemoveSink(INewFrameEventSink* pSink)
{
    CComCritSecLock<CComAutoCriticalSection> lock(m_csEventSinks);
    std::map<INewFrameEventSink*, INewFrameEventSink*>::iterator  it = m_mapEventSinks.find(pSink);
    if(it != m_mapEventSinks.end())
    {
        m_mapEventSinks.erase(it);
    }
}

void CInterceptFilter::NotifySink(CImageFrame* pFrame)
{
    CComCritSecLock<CComAutoCriticalSection> lock(m_csEventSinks);
    std::map<INewFrameEventSink*, INewFrameEventSink*>::iterator  it = m_mapEventSinks.begin();
    for(;it!=m_mapEventSinks.end(); it++)
    {
        it->second->OnNewFrame(pFrame);
    }
}


void CInterceptFilter::DoAutoBrightnessControl(const BYTE* pGrayData, int nWidth, int nHeight)
{
    this->m_oAutoBrightnessRegulator.ProcessImage(pGrayData, nWidth, nHeight);
}

//@����:ʹ�ܻ��������Զ�����
//@����:bEnable, ʹ�ܱ�־
void CInterceptFilter::EnableBrightnessAutoRegulating(BOOL bEnable)
{

    this->m_bEnableBrightnessAutoRegulating = bEnable;
}

//@����:���û����ƽ������
//@����:brightness:����ֵ
void CInterceptFilter::SetImageAverageBrightness(BYTE brightness)
{
    this->m_oAutoBrightnessRegulator.SetExpectedBrightness(brightness);
}
//

void CInterceptFilter::SetAutoCalibrateParamsIndex(BYTE nIndex)
{
	this->m_oAutoBrightnessRegulator.SetAutoCalibrateParamsIndex(nIndex);
}

//////

//@����:����ARGBͼƬ֡,��ʹ������ͼ
void CInterceptFilter::UpdateARGBFrameWithoutMask()
{
    m_BGRAFrame.SetSize(m_nRawImageWidth, m_nRawImageHeight, sizeof(DWORD));

    int nLoopCount = m_nRawImageWidth * m_nRawImageHeight;//ѭ������

    DWORD* pdwARGBData = m_BGRAFrame.GetData();
    const BYTE*  pbGrayData       =  m_GraySrcFrame.GetData();

    __declspec( align( 16) ) static const unsigned char AlphaCode[16] =
    {
        0x00,0x00,0x00,0xFF,
        0x00,0x00,0x00,0xFF,
        0x00,0x00,0x00,0xFF,
        0x00,0x00,0x00,0xFF
    };
    _asm
    {
        pushad
            finit

            mov esi, pbGrayData
            mov edi, pdwARGBData
            mov ecx, nLoopCount
            ;//xxm7 = FF 00 00 00 FF 00 00 00 FF 00 00 00 FF 00 00 00
        movdqa xmm7, AlphaCode

_next_16_pixles:

        ;//xmm0 = PF PE PD PC PB PA P9 P8 P7 P6 P5 P4 P3 P2 P1 P0
        movdqa xmm0,[esi];

        //------0~7����-------
        ;//xmm1 = PF PE PD PC PB PA P9 P8 P7 P6 P5 P4 P3 P2 P1 P0
        movdqa xmm1, xmm0

            ;//xmm1 = P7P7 P6P6 P5P5 P4P4 P3P3 P2P2 P1P1 P0P0
        punpcklbw xmm1, xmm1

            //0~3 ����
            movdqa xmm2, xmm1

            ;//xmm2 = P3P3P3P3 P2P2P2P2 P1P1P1P1 P0P0P0P0
        punpcklbw xmm2, xmm2

            ;//xmm2 = FFP3P3P3 FFP2P2P2 FFP1P1P1 FFP0P0P0
        por xmm2, xmm7;

        movdqa [edi], xmm2;


        //4~7 ����
        ;//xmm1 = P7P7P7P7 P6P6P6P6 P5P5P5P5 P4P4P4P4
        punpckhbw xmm1,xmm1

            ;//xmm1 = FFP7P7P7 FFP6P6P6 FFP5P5P5 FFP4P4P4
        por xmm1, xmm7;
        movdqa [edi + 16], xmm1;


        //----8~11����----
        //xmm0 = PFPF PEPE PDPD PCPC PBPB PAPA P9P9 P8P8
        punpckhbw xmm0,xmm0

            //8~11����
            ;//xmm1 = PFPF PEPE PDPD PCPC PBPB PAPA P9P9 P8P8
        movdqa xmm1, xmm0;

        ;//xmm1 = PBPBPBPB PAPAPAPA P9P9P9P9 P8P8P8P8
        punpcklbw xmm1,xmm1

            ;//xmm1 = FFPBPBPB FFPAPAPA FFP9P9P9 FFP8P8P8
        por xmm1, xmm7
            movdqa [edi + 32], xmm1;

        ;//12~15����
        ;//xmm0 = PFPFPFPF PEPEPEPE PDPDPDPD PCPCPCPC
        punpckhbw xmm0,xmm0

            ;//xmm0 =  FFPFPFPF FFPEPEPE FFPDPDPD FFPCPCPC
        por xmm0, xmm7
            movdqa [edi + 48], xmm0;

        add esi,16
            add edi,64

            sub ecx,16 ;

        jnz _next_16_pixles;
        emms
            popad

    }

}


//@����:����ARGBͼƬ֡,ʹ������ͼ
void CInterceptFilter::UpdateARGBFrameWithMask()
{
    m_BGRAFrame.SetSize(m_nRawImageWidth, m_nRawImageHeight, sizeof(DWORD));

    int nLoopCount = m_nRawImageWidth * m_nRawImageHeight;//ѭ������

    DWORD* pdwARGBData = m_BGRAFrame.GetData();

    const BYTE*  pbGrayData       =  m_GraySrcFrame.GetData();
    //const BYTE*  pbGrayData       = m_pPenPosDetector->m_oBlobCounter.GetBinarizedImage()->GetData();
    const BYTE*  pbStaticMaskData  = m_pPenPosDetector->GetStaticMaskFrame().GetData();
    const BYTE*  pbDynamicMaskData = m_pPenPosDetector->GetDynamicMaskFrame().GetData();

    __declspec( align( 16) ) static const unsigned char AlphaCode[16] =
    {
        0x00,0x00,0x00,0xFF,
        0x00,0x00,0x00,0xFF,
        0x00,0x00,0x00,0xFF,
        0x00,0x00,0x00,0xFF
    };

    __declspec( align( 16) ) static const unsigned char MaskColor[16] =
    {
        //
        //  BB   GG   RR   AA
        0x00,0x7F,0x00,0xFF,
        0x00,0x7F,0x00,0xFF,
        0x00,0x7F,0x00,0xFF,
        0x00,0x7F,0x00,0xFF
    };

    _asm
    {
        pushad
            finit

            mov esi, pbGrayData
            mov edi, pdwARGBData
            mov ecx, nLoopCount
            mov edx, pbStaticMaskData
            mov ebx, pbDynamicMaskData

            ;//xxm7 = FF 00 00 00 FF 00 00 00 FF 00 00 00 FF 00 00 00
        movdqa xmm7, AlphaCode ;//����

        ;
        movdqa xmm6, MaskColor ;//����

_next_16_pixles:

        ;//xmm0 = PF PE PD PC PB PA P9 P8 P7 P6 P5 P4 P3 P2 P1 P0
        movdqa xmm0,[esi];

        ;//xmm3 = ��̬��������
        movdqa xmm3, [edx]

        ;//xmm4 = ��̬��������
        movdqa xmm4, [ebx]


        ;//xmm3 = ��������
        ;//xmm3 = M[F] M[E] M[D] M[C] M[B] M[A] M[9] M[8] M[7] M[6] M[5] M[4] M[3] M[2] M[1] M[0]
        pand   xmm3, xmm4


            //------0~7����-------

            ;//�Ҷ�����
        ;//xmm1 = PF PE PD PC PB PA P9 P8 P7 P6 P5 P4 P3 P2 P1 P0
        movdqa xmm1, xmm0


            ;//��������
        ;//xmm4 =  M[F] M[E] M[D] M[C] M[B] M[A] M[9] M[8] M[7] M[6] M[5] M[4] M[3] M[2] M[1] M[0]
        movdqa xmm4, xmm3

            ;//�Ҷ�����
        ;//xmm1 = P7P7 P6P6 P5P5 P4P4 P3P3 P2P2 P1P1 P0P0
        punpcklbw xmm1, xmm1

            ;//��������
        ;//xmm4 = M[77] M[66] M[55] M[44] M[33] M[22] M[11] M[00]
        punpcklbw xmm4,xmm4

            ;//�Ҷ�����
        ;//0~3 ����
        movdqa xmm2, xmm1

            ;//��������
        ;//xmm5 = M[77] M[66] M[55] M[44] M[33] M[22] M[11] M[00]
        movdqa xmm5, xmm4

            ;//�Ҷ�����
        ;//xmm2 = P3P3P3P3 P2P2P2P2 P1P1P1P1 P0P0P0P0
        punpcklbw xmm2, xmm2

            ;//�Ҷ�����(0~3����)
        ;//xmm2 = FFP3P3P3 FFP2P2P2 FFP1P1P1 FFP0P0P0
        por xmm2, xmm7;


        ;//��������(0~3����)
        ;//xmm5 = M[7777] M[6666] M[5555] M[4444] M[3333] M[2222] M[1111] M[0000]
        punpcklbw xmm5,xmm5

            ;//���β���(0~3����)
        pand xmm2, xmm5

            ;//������ȡ��(0~3����),��������������ɫ
        ;//xmm5=����������ɫ
        pandn xmm5,xmm6

            ;//���������ɫ(0~3����)
        paddb xmm2, xmm5
            ;//---------------
        movdqa [edi], xmm2;//����ARGB����


        ;//4~7 ����
        ;//�Ҷ�����(4~7����)
        ;//xmm1 = P7P7P7P7 P6P6P6P6 P5P5P5P5 P4P4P4P4
        punpckhbw xmm1,xmm1

            ;//xmm1 = FFP7P7P7 FFP6P6P6 FFP5P5P5 FFP4P4P4
        por xmm1, xmm7;

        ;//��������(4~7����)
        ;//xmm4 = M[7777] M[6666] M[5555] M[4444]
        punpckhbw xmm4,xmm4

            ;//���β���(4~7����)
        pand xmm1, xmm4

            ;//������ȡ��(4~7����)
        ;//xmm4=����������ɫ
        pandn xmm4,xmm6

            ;//���������ɫ(4~7����)
        paddb xmm1, xmm4

            ;//---------------
        movdqa [edi + 16], xmm1;//����ARGB����


        ;//----8~11����----
        ;//�Ҷ�����
        ;//xmm0 = PFPF PEPE PDPD PCPC PBPB PAPA P9P9 P8P8
        punpckhbw xmm0, xmm0

            ;//��������
        ;//xmm3 = M[FF] M[EE] M[DD] M[CC] M[BB] M[AA] M[99] M[88]
        punpckhbw xmm3, xmm3

            ;//�Ҷ�����
        ;//8~11����
        ;//xmm1 = PFPF PEPE PDPD PCPC PBPB PAPA P9P9 P8P8
        movdqa xmm1, xmm0;

        ;//��������
        ;//xmm4 = M[FF] M[EE] M[DD] M[CC] M[BB] M[AA] M[99] M[88]
        movdqa xmm4,xmm3;

        ;//�Ҷ�����(8~11����)
        ;//xmm1 = PBPBPBPB PAPAPAPA P9P9P9P9 P8P8P8P8
        punpcklbw xmm1,xmm1

            ;//xmm1 = FFPBPBPB FFPAPAPA FFP9P9P9 FFP8P8P8
        por xmm1, xmm7

            ;//��������(8~11����)
        ;//xmm4 = M[BBBB] M[AAAA] M[9999] M[8888]
        punpcklbw xmm4, xmm4


            ;//���β���(8~11����)
        pand xmm1, xmm4

            ;//������ȡ��(8~11����)
        ;//xmm4=����������ɫ
        pandn xmm4,xmm6

            ;//���������ɫ(8~11����)
        paddb xmm1, xmm4


            ;//---------------
        movdqa [edi + 32], xmm1;//����ARGB����

        ;//12~15����
        ;//�Ҷ�����(12~15����)
        ;//xmm0 = PFPFPFPF PEPEPEPE PDPDPDPD PCPCPCPC
        punpckhbw xmm0,xmm0

            ;//xmm0 =  FFPFPFPF FFPEPEPE FFPDPDPD FFPCPCPC
        por xmm0, xmm7


            ;//��������(12~15����)
        ;//xmm3 =  M[FFFF] M[EEEE] M[DDDD] M[CCCC]
        punpckhbw xmm3,xmm3

            ;//���β���(12~15����)
        pand xmm0, xmm3

            ;//������ȡ��(12~15����)
        ;//xmm3=����������ɫ
        pandn xmm3,xmm6

            ;//���������ɫ(12~15����)
        paddb xmm0, xmm3

            ;//---------------
        movdqa [edi + 48], xmm0;//����ARGB����


        add esi, 16
            add edx, 16
            add ebx, 16
            add edi,64

            sub ecx,16 ;

        jnz _next_16_pixles;
        emms
            popad

    }
}
