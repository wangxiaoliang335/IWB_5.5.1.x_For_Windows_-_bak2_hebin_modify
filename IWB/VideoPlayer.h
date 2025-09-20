#pragma once

#ifndef SAFE_RELEASE

#define SAFE_RELEASE( x )  \
    if ( NULL != x )       \
    {                      \
        x->Release( );     \
        x = NULL;          \
    }

#endif



class CInterceptFilter;

//
//#define DT_TOP                      0x00000000
//#define DT_LEFT                     0x00000000
//#define DT_CENTER                   0x00000001
//#define DT_RIGHT                    0x00000002
//#define DT_VCENTER                  0x00000004
//#define DT_BOTTOM                   0x00000008
//#define DT_WORDBREAK                0x00000010
//#define DT_SINGLELINE               0x00000020
//#define DT_EXPANDTABS               0x00000040
//#define DT_TABSTOP                  0x00000080
//#define DT_NOCLIP                   0x00000100
//#define DT_EXTERNALLEADING          0x00000200
//#define DT_CALCRECT                 0x00000400
//#define DT_NOPREFIX                 0x00000800
//#define DT_INTERNAL                 0x00001000
//OSD(Over Screen Display Text)
class TOSDText
{
public:
    //struct RectF
    //{
    //    float left;
    //    float top;
    //    float right;
    //    float bottom;
    //};

	TOSDText()
		:
		m_strText(_T("")),
		m_dwDrawTextFormat(0U),
		m_lFontSize(10),
		m_strFontFaceName(_T("Times New Roman")),
		m_hFont(NULL),
		m_nDisplayTimesCounter(-1)
	{
		m_rcDisplayArea.left   = 0.0;
        m_rcDisplayArea.top    = 0.0;
		m_rcDisplayArea.right  = 1.0;
		m_rcDisplayArea.bottom = 1.0;

	}
	TOSDText(
		const TCHAR* szText,
		const RectF&  rcDisplayArea,
		UINT dwDrawTextFormat,
		LONG lFontSize,
		const TCHAR* lpszFontFaceName	
		)
		:
		m_strText(szText),
		m_dwDrawTextFormat(dwDrawTextFormat),
		m_lFontSize(lFontSize),
		m_strFontFaceName(lpszFontFaceName),
		m_hFont(NULL)

	{

		m_rcDisplayArea = rcDisplayArea;

	}

	~TOSDText()
	{
		if (m_hFont)
		{
			::DeleteObject(m_hFont);
		}
	}

	void SetFont(LONG lFontSize, const TCHAR* lpszFontFaceName)
	{
		if (lFontSize != m_lFontSize || _tcsicmp((LPCTSTR)m_strFontFaceName, lpszFontFaceName) != 0)
		{
			if (m_hFont)
			{
				::DeleteObject(m_hFont);
				m_hFont = NULL;
			}
			m_lFontSize = lFontSize;
			m_strFontFaceName = lpszFontFaceName;

			HDC hdcDesktop = GetDC(GetDesktopWindow());
			LOGFONT lf;
			memset(&lf, 0, sizeof(LOGFONT));
			lf.lfHeight = -MulDiv(m_lFontSize, GetDeviceCaps(hdcDesktop, LOGPIXELSY), 72);;
			lf.lfWidth = 0;
			lf.lfWeight = FW_BOLD;
			lf.lfCharSet = ANSI_CHARSET;
			lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
			lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
			memcpy(lf.lfFaceName, (LPCTSTR)m_strFontFaceName, _countof(lf.lfFaceName));
			m_hFont = CreateFontIndirect(&lf);
			ReleaseDC(GetDesktopWindow(), hdcDesktop);

		}
	}

	HFONT GetFont()
	{
		if (NULL == m_hFont)
		{
			HDC hdcDesktop = GetDC(GetDesktopWindow());
			LOGFONT lf;
			memset(&lf, 0, sizeof(LOGFONT));
			lf.lfHeight = -MulDiv(m_lFontSize, GetDeviceCaps(hdcDesktop, LOGPIXELSY), 72);;
			lf.lfWidth = 0;
			lf.lfWeight = FW_BOLD;
			lf.lfCharSet = ANSI_CHARSET;
			lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
			lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
			memcpy(lf.lfFaceName, (LPCTSTR)m_strFontFaceName, _countof(lf.lfFaceName));
			m_hFont = CreateFontIndirect(&lf);
			ReleaseDC(GetDesktopWindow(), hdcDesktop);
		}

		return m_hFont;
	}

	void SetText(const TCHAR* lpszText)
	{
		m_strText = lpszText;
	}

	const TCHAR* GetText()const
	{
		return (LPCTSTR)m_strText;
	}


	void SetDisplayArea(const RectF& rcDisplayArea, UINT dwDrawTextFormat)
	{
		m_rcDisplayArea    = rcDisplayArea;
		m_dwDrawTextFormat = dwDrawTextFormat;
	}

	const RectF& GetDisplayArea()const
	{
		return m_rcDisplayArea;
	}

	UINT GetDrawTextFormat()const
	{
		return m_dwDrawTextFormat;
	}


	void DecDisplayTimesCounter()
	{
		if (m_nDisplayTimesCounter >= 0)
		{
			m_nDisplayTimesCounter--;
		}

	}

	void SetDisplayTimes(int nTimes)
	{
		m_nDisplayTimesCounter = nTimes;
	}

	int GetDisplayTimesCounter()const 
	{
		return m_nDisplayTimesCounter;
	}
	LONG GetFontSize()const
	{
		return m_lFontSize;
	}

	LPCTSTR GetFontName()const
	{
		return (LPCTSTR)m_strFontFaceName;
	}
    
	
protected:
	CString      m_strText;
	HFONT        m_hFont;
    RectF        m_rcDisplayArea;//�����һ����[0,1]�ڵľ���
	UINT         m_dwDrawTextFormat;//API DrawText��Format����
	LONG         m_lFontSize;
	CString      m_strFontFaceName;

	int          m_nDisplayTimesCounter;//��ʾ������������-1������Σ�0��ֹͣ��ʾ
};



enum EOSDTextType
{
    E_OSDTEXT_TYPE_ERROR_INFO  = 0,//У����Ϣ
    E_OSDTEXT_TYPE_GUIDE_BOX   = 1,//������
    E_OSDTEXT_TYPE_FORMAT_INFO = 2,//��Ƶ��ʽ��Ϣ
	E_OSDTEXT_TYPE_SHOW_INFO   = 3,
	E_OSDTEXT_TYPE_LENS_INFO   = 4,//�̼��еľ�ͷ
    E_OSDTEXT_TYPE_COUNT       = 5
};

class CVideoPlayer
{
public:
	CVideoPlayer(int nID=0);

	~CVideoPlayer();

    void SetInterceptFilter(CInterceptFilter* pFilter);
    CInterceptFilter* GetInterceptFilter();


	HRESULT Stop();

	//@����:��ʼ���
	//@����:hPlayWnd,       ���Ŵ�����
	//      rcDispArea,     ��Ƶ��ʾ����
	//      hNotifyWnd,     ֪ͨ��Ϣ���մ���ľ��
	//      lpszDevicePath, �豸·��
	//      pVideoInfoHeader, ָ��ָ������Ƶ��ʽ��ָ��
	//@����ֵ:
	//   �ɹ�,����TRUE
	//   ʧ��,����FALSE
	BOOL StartDetect(HWND hPlayWnd, const RECT& rcDispArea, HWND hNotifyWnd, LPCTSTR lpszDevicePath,  const VIDEOINFOHEADER* pVideoInfoHeader);


	//@����:��ʼ���
	//@����:lpszDevicePath, �豸·��
	//      pVideoInfoHeader, ָ��ָ������Ƶ��ʽ��ָ��
	//@����ֵ:
	//   �ɹ�,����TRUE
	//   ʧ��,����FALSE
	BOOL StartDetect(LPCTSTR lpszDevicePath,  const VIDEOINFOHEADER* pVideoInfoHeader);

	BOOL StopDetect();

	void MissStatusInfo();

    //@����:�ڴ�������ʾ��Ƶ֡
    //
    //@����:pYUY2Frame, YUY2ͼƬ֡
    //      nImageWidth,  ͼƬ���
    //      nImageHeight, ͼƬ�߶�
    //BOOL CVideoPlayer::DisplayFrame(const BYTE* pYUY2Frame, int nImageWidht, int nImageHeight);

    //@����:�ڴ�������ʾ��Ƶ֡
    //@����:pARGBFrame  , ARGBͼƬ֡
    //      nImageWidth , ͼƬ���
    //      nImageHeight, ͼƬ�߶�
    BOOL DisplayFrame(const BYTE* pARGBFrame, int nImageWidth, int nImageHeight);

    const RECT& GetDisplayArea() const;
    void SetDisplayArea(const RECT& rcDispArea);
	void SetDisplayWnd(HWND hWnd);
	void SetNotifyWnd(HWND hWnd);


	////@����:�������ȼ������
	//BOOL SetYThreshold(BYTE threshold);

	////@����:���ȼ������
	//BYTE GetThreshold()const;


	//���ý���֪ͨ��Ϣ�Ĵ���
	void SetNotifyMessage(HWND hWnd, UINT uMsg);

	BOOL IsDetecting()const;
	BOOL ResizeVideo(const RECT& rcNewSize);

	////@����:������Ƶ��������
	//BOOL ResetVideoClipArea();

	////@����:������Ƶ��������
	//BOOL SetVideoClipArea(const RECT& rcClipArea);


	BOOL ChangeCaptureFreq(int  nFrequency);

	BOOL SetImageFormat(const VIDEOINFOHEADER& vih);

	HRESULT ShowPropertyPage(HWND hWndParent);

	//CPenPosDetector* GetPenPosDetector();
	//const CPenPosDetector* GetPenPosDetector() const;

	IBaseFilter* GetCaptureFilter();


	
	//@����:����Capture Filter��Gammaֵ
	//@����:lGammaValue, Specifies the gamma, as gamma * 100. Values range from 1 to 500. 
	//BOOL ChangeGammaValue(LONG nGammaValue);


    //@����:��������ͷ����    
    BOOL SetCameraParams(const TVideoProcAmpProperty& newParams);


    //@����:��ȡ����ͷ����
    BOOL GetCameraParams(TVideoProcAmpProperty& saveParams);

    //@����:��������ĵ�������
    BOOL SetCameraParams(long  lValue, VideoProcAmpProperty eProperty);

    //@����:��ȡ����ͷ����
    BOOL GetCameraBrightness(long & lValue, long& lMax, long& lMin, long& lSteppingDelta, long& lDefault);

    //@����:��ȡ����ͷ�Աȶ�
    BOOL GetCameraContrast(long & lValue, long& lMax, long& lMin, long& lSteppingDelta, long& lDefault);

    //@����:��ȡ����ͷGammaֵ
    BOOL GetCameraGamma(long & lValue, long& lMax, long& lMin, long& lSteppingDelta, long& lDefault);


    //@����:��������ͷ�ķŴ�����ϵ��
    //@˵��:0x33AA,��IRCUT���˹�Ƭ
    //@˵��:0x44AA,�ر�IRCUT���˹�Ƭ
    //BOOL SetCameraGain(const long & lGain);

    HWND GetPlayWnd()const
    {
        return m_hPlayWnd;
    }


    //@����:����Ƶ��������ʾ����
    void SetDisplayInfo(LPCTSTR lpszInfo, int nDisplayTimes=300, BOOL isSingleLine = TRUE);



	//@����:����OSD��ʾ����
	//
	BOOL AddOSDText(
		EOSDTextType eOSDTextType,
		const TCHAR* szContent,
		//const TOSDText::RectF&  rcDisplayArea,
        const RectF&  rcDisplayArea,
		UINT deDrawTextFormat,
		LONG lFontSize,
		const TCHAR* lpszFontFaceName,
		int nDisplayTimes);


	void ClearOSDText(EOSDTextType eOSDTextType);

	void UpdateVideoStreamForamtInfo(int nImageWidth, int nImageHeight, DWORD ImageType, float fps,int nId);
	CString   CurrentCameraResolution();


    BOOL GetVideoSize(SIZE&  size);

protected:
    //
	HRESULT PlayVideo(HWND hWnd = NULL, HWND hNotifyWnd=NULL);

	HRESULT InitializeVideo();

	void UnInitializeVideo();

	HRESULT GetInterfaces();
	HRESULT CloseInterfaces();

	HRESULT BuildGraph();
	HRESULT FindCaptureDevice(IBaseFilter **ppSrcFilter);

    //�ڻ�������ʾOSD������Ϣ
    void DisplayText(HDC hDC, int nImageWidth, int nImageHeight);


	//IVideoWindow* m_pVW ; 
	IMediaControl* m_pMC ;
	IMediaEventEx* m_pME ;	
	//ICaptureGraphBuilder2* m_pCaptureGraphBuilder;
	IGraphBuilder* m_pFilterGraph;

	IBaseFilter* m_pCaptureFilter;

	//IVMRWindowlessControl9* m_pWC;
    IVMRWindowlessControl* m_pWC;



	HWND m_hPlayWnd  ;//���Ŵ�����
    RECT m_rcDispArea;//��Ƶ��ʾ����
   // CDib m_oRealFrameDib;
   //CImageFrameT<DWORD> m_oRGB;
    CDibCanvas m_oRealFrameARGB;

    CDibCanvas m_oMemFrame;//�ڴ�DC, ���⻭����˸

    HWND m_hNotifyWnd;

    //ͼ����������
	CInterceptFilter* m_pInterceptFilter;


	DWORD m_dwRegister;
	BOOL m_bIsDetecting;

	//
	//CAtlString m_strCaptureDeviceName;
	//int        m_nCaptureDeviceInstNo;
	CAtlString   m_strCaptureDevicePath;
	VIDEOINFOHEADER m_tDesiredImageFormat;

	//

	DWORD m_dwGraphRegister;
	BOOL m_bIsClosing;

    enum EVideoState
    {
        E_VIDEO_RUNNING,//��Ƶ���ڲ���
        E_VIDEO_STOP   ,//��Ƶֹͣ
        E_VIDEO_ERR    ,//��Ƶ����
    }m_eVideoState;


    int m_nID;//�����ı��

    //CString  m_strDisplayText                ;//��ʾ����
    int      m_nDisplayTimesCounter            ;//��ʾ��������
    //BOOL     m_bTextIsValidate               ;//������Ч��־
    static const int  MAX_DISPLAY_TIMES = 300;
    //HFONT m_hFont;



	TOSDText m_osdtext[E_OSDTEXT_TYPE_COUNT];

	CString    m_sCurrentCameraResolution;

	TCHAR m_szStatusText[256];
};
