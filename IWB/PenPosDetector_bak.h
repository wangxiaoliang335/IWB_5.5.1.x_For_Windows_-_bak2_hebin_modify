#pragma once

//�ڴ����
template<int MAX_BUF_NUMBER, class T>
class CMemPool
{
public:
	CMemPool()
	{
		m_nBufCount = MAX_BUF_NUMBER;

		m_vecObjs.resize(m_nBufCount);

		for(unsigned int i=0; i< m_nBufCount; i++)
		{
			m_vecObjs[i] = new T();
		}

	}

	~CMemPool()
	{

		for(unsigned int i=0; i< m_nBufCount; i++)
		{
			m_vecObjs[i]->Release();//
		}
	}

	T* GetFreeBuf()
	{
		T* pObj= NULL;

		for(unsigned int i=0; i< m_vecObjs.size(); i++)
		{


			assert(m_vecObjs[i]);
			if(m_vecObjs[i]->IsFree())
			{
				pObj = m_vecObjs[i];

				break;
			}
		}
		return pObj;
	}


protected:
	unsigned int m_nBufCount;
	std::vector<T*> m_vecObjs;

};

//֡�ʼ����(Frames Per Second)
class CFPSDetector
{
public:
	CFPSDetector(int StatisticThreshold = 50)
		:
		m_fCurrentFps(0.0f),
		m_fAvgFps(0.0f),
		m_nStatisticThresholdForCurrentFps(StatisticThreshold)
	{

		QueryPerformanceFrequency(&m_liFreq);
		QueryPerformanceCounter  (&m_liStart);
		QueryPerformanceCounter  (&m_liLastThreshold);
	}

	~CFPSDetector()
	{

	}

	void Reset()
	{
		m_fCurrentFps = 0.0f;
		m_fAvgFps     = 0.0f;
		m_nFrameCounterForCurrentFps = 0;
		m_nTotalFrameCount = 0;

		QueryPerformanceFrequency(&m_liFreq);
		QueryPerformanceCounter  (&m_liStart);
		QueryPerformanceCounter  (&m_liLastThreshold);
	}


	float GetCurrentFps()const
	{
		return m_fCurrentFps;
	}

	float GetAvgFps() const
	{
		return m_fAvgFps;
	}


	void Trigger()
	{
		m_nFrameCounterForCurrentFps++;
		m_nTotalFrameCount++;
		
		if(m_nFrameCounterForCurrentFps == m_nStatisticThresholdForCurrentFps)
		{
			LARGE_INTEGER liNow;
			QueryPerformanceCounter(&liNow);
			//���㵱ǰ֡��
			m_fCurrentFps = (float)((double)m_nFrameCounterForCurrentFps * m_liFreq.QuadPart /(double)(liNow.QuadPart - m_liLastThreshold.QuadPart));
			
			m_liLastThreshold = liNow;
			m_nFrameCounterForCurrentFps = 0;

			//����ƽ��֡��
			m_fAvgFps = (float)((double)m_nTotalFrameCount * ((double)m_liFreq.QuadPart /(double)(liNow.QuadPart - m_liStart.QuadPart)));
		
		}
		
	}


private:
	float m_fCurrentFps     ;//��ǰ֡��
	float m_fAvgFps         ;//ƽ��֡��
	int   m_nFrameCounterForCurrentFps      ;//
	INT64   m_nTotalFrameCount              ;//
	int   m_nStatisticThresholdForCurrentFps;//��ǰ֡��ͳ������
	LARGE_INTEGER  m_liFreq;
	LARGE_INTEGER  m_liStart;
	LARGE_INTEGER  m_liLastThreshold;
};

class CDib;

enum EManualScreenAreadMode
{
	E_ManualScreenAreaEditMode  ,//�༭ģʽ
	E_ManualScreenAreaNormalMode,//����ģʽ
};

//���ڲ�ͬ֡�ʵ�����ͷ���Լ�����Ʒ��ͶӰ����ʾ�ӳ�����Ĵ��ڡ�
//���ӳٵȴ���֡����Ϊ����ʱ��
class CWaitTimer
{
public:
	CWaitTimer()
		:
		m_dwVideoDisplayDelay(0)
	{
		Init(1, 0, FALSE);
	}

	//@����:dwLatencyTimes�� ʱ���ӳٱ���
	void Init(DWORD dwTimeMagnification, DWORD dwVideoDisplayDelay, BOOL bUseAbsTime = FALSE)
	{
		m_dwTimeBegin = GetTickCount();

		if (dwTimeMagnification <= 0)
		{
			dwTimeMagnification = 1;
		}
		if (dwTimeMagnification > 10)
		{
			dwTimeMagnification = 10;
		}
		m_dwTimeMagnification = dwTimeMagnification;

		m_dwVideoDisplayDelay = dwVideoDisplayDelay;
		m_dwFrameCount = 0;

		m_bUseAsbTime = bUseAbsTime;
	}


	void Reset()
	{
		m_dwTimeBegin = GetTickCount();
		m_dwFrameCount = 0;
	}

	BOOL IsWaitTimeout(DWORD dwWaitTimeInMs)
	{
		if (m_bUseAsbTime)
		{
			DWORD dwTimeElapsed = GetTimeElapsed();

			return (dwTimeElapsed >= dwWaitTimeInMs * m_dwTimeMagnification + m_dwVideoDisplayDelay) ? TRUE : FALSE;
		}
		else
		{
			const int MILLI_SECOND_PER_FRAME = 16;
			DWORD dwNeeedWaitFrames = (dwWaitTimeInMs + (MILLI_SECOND_PER_FRAME >> 1)) / MILLI_SECOND_PER_FRAME;
			dwNeeedWaitFrames *= m_dwTimeMagnification;
			dwNeeedWaitFrames += m_dwVideoDisplayDelay / MILLI_SECOND_PER_FRAME;
			return (m_dwFrameCount >= dwNeeedWaitFrames) ? TRUE : FALSE;
		}
	}


	void Trigger()
	{
		if (!m_bUseAsbTime)
		{
			m_dwFrameCount++;
		}

	}

	DWORD GetTimeMagnification()
	{
		return m_dwTimeMagnification;
	}
protected:
	DWORD GetTimeElapsed()
	{
		DWORD dwNow = GetTickCount();
		DWORD dwElpase = 0;

		if (dwNow >= m_dwTimeBegin)
		{
			dwElpase = dwNow - m_dwTimeBegin;
		}
		else
		{//ϵͳ������������
			dwElpase = (0xFFFFFFFF - m_dwTimeBegin) + dwNow;
		}
		return dwElpase;
	}


protected:
	DWORD m_dwTimeBegin;//������ʼʱ��
	DWORD m_dwTimeMagnification;//ʱ��Ŵ���
	DWORD m_dwFrameCount;
	DWORD m_bUseAsbTime;//ʹ�þ���ʱ��
	DWORD m_dwVideoDisplayDelay;//��Ƶ��ʾ�ӳ٣���λ:ms

};

struct LightSpotBounds
{
	RECT  m_aryLightSpotBounds;
	bool  valid;

public:
	LightSpotBounds()
	{
		memset(&m_aryLightSpotBounds,0,sizeof(m_aryLightSpotBounds));
		valid = true;
	}
};

//�˶������
class CIWBSensor;
class CPenPosDetector
{
public:
	CPenPosDetector(int nID);

	~CPenPosDetector();


	//��ʼ���
	BOOL StartDetect(HWND hNotifyWnd, int nSrcImageWidth, int nSrcImageHeight, CIWBSensor* pSensor);

	//ֹͣ���
	BOOL StopDetect();

	//д������ԭʼ��Ƶ����
	//BOOL WriteSourceImageData(BYTE* pData, int nWidth, int nHeight, int cBytesPerPixel);

	//@����:����߳�
	static ULONG __stdcall MotionDetectorProc(LPVOID lpCtx);

	//BOOL IsDetecting();

	//@����:��ʾԭʼ����Ƶ����
	//@����:hWnd,��ʾ��Ƶ�Ĵ���
	void ViewSrcImage(HWND hWnd, BOOL bShow);

	//@����:�鿴����ͼ��
	//@����:hWnd,��ʾ��Ƶ�Ĵ���
	void ViewBkgndImage(HWND hWnd, BOOL bShow);

	//@����:�鿴���ͼ��
	//@����:hWnd,��ʾ��Ƶ�Ĵ���
	void ViewDiffImage(HWND hWnd, BOOL bShow);


	//@����:��⺯��
	//BOOL DoDetect(const CYUY2Frame* pYUVFrame);
    BOOL DoDetect(const CImageFrame* pGrayFrame, ESensorLensMode eSensorLenMode, int & nID_Para);

	//@����:��⺯��
	//BOOL DoDetect_RedLeaf(const CYUY2Frame* pYUVFrame);

	//void DrawTrackTargets(const std::vector<CMoveTargets>& targets);

	//@����:����֪ͨ�����Ա���ռ����
	void SetNotifyWindow(HWND hWnd, UINT uMsg);

	//
	UINT GetObjCount() const
	{
		return m_nObjCount;
	}

    //��ʹ�ú���
//	const RECT* GetLightSpotBounds() const
//	{
//		return &m_aryLightSpotBounds[0];
//	}

	////
	const LightSpotBounds* GetLightSpotInfo() const
	{
		return &m_aryLightSpotBoundsInfo[0];
	}

	void ResetWaitTimer();

	//@����:�������ȼ������
	void SetYThreshold(BYTE threshold);

	//@����:���ȼ������
	BYTE GetYThreshold()const;

	//@����:���ö�̬��������
	void SetDynamicMaskThreshold(BYTE threshold);

	//@����:������Ƶ��������
	void ResetVideoClipArea();

	//@����:������Ƶ��������
	void SetVideoClipArea(const RECT& rcClipArea);

	//@����:��ȡ��Ƶ��������
	const RECT& GetVideoClipArea()const;

	//@����:��ȡ�ֶ�ָ������Ļ����
	const RECT&  GetManualScreenArea()const;

	//@����:�趨�ֶ�ָ������Ļ����
	void UpdateScreenMaskArea(const RECT& rcNewArea);

	//@����:�����Ƿ���þ�̬������
//	BOOL IsDisableStaticScreenMask()const;

	//@����:�����ֶ�ָ������Ļ����
//    void DisableManualScreenArea(BOOL bDisable);


	//@����:�����ֶ�ָ������Ļ����ǰģʽ
	EManualScreenAreadMode GetManualScreenAreaMode()const;

	//@����:�趨"�ֶ�ָ����Ļ����"��ģʽ
	//void SetManualScreenAreaMode(EManualScreenAreadMode eNewMode);

	//@����:�ж��ֶ�ָ������Ļ�����Ƿ���ȷ
	//BOOL IsValidManualScreenArea();

	//@����:��ȡ�ֶ�ָ����Ļ��������ͼ
	const CImageFrame& GetScreenAreaMask();

	//@����:�����ֶ�ָ����Ļ��������ͼ
//	void UpdateManualScreenAreaMask();

	//@��  ��:����8λλͼ����YUY2��ʽ������λͼ
	//@��  ��:maskFrame,8λ����λͼ
	//@����ֵ:�ɹ�����TRUE
	//        ʧ�ܷ���FALSE
	BOOL SetStaticMaskFrame(const CImageFrame& maskFrame);

	//@����:������������
	BOOL AddStaticMaskArea(const RECT* pAddArea);

	//@����:ɾ����������
	BOOL EraseSaticMaskArea(const RECT* pEraseArea);

	//@����:�����������
	BOOL ClearStaticMaskArea();
	
    //@��  ��:��������λͼ
    //@��  ��:lpFileName, ����λͼ�ļ�����
    //        maskFrame, ���������֡
	//@����ֵ:�ɹ�����TRUE,
	//        ʧ�ܷ���FALSE
    //BOOL LoadMaskFrame(LPCTSTR lpszFileName, CYUY2Frame& maskFrame);
    BOOL LoadMaskFrame(LPCTSTR lpszFileName, CImageFrame& maskFrame);
	
    //BOOL InitStatisticsFrame(CYUY2Frame& maskFrame);
    BOOL InitStatisticsFrame(CImageFrame& maskFrame);

	//@��  ��:��������λͼ
	//@����ֵ:�ɹ�����TRUE,
	//        ʧ�ܷ���FALSE
	BOOL LoadMaskFrame();

	//@��  ��:��������λͼ
    //@��  ��:lpszFileName, ������ļ�����
    //        maskFrame, ����֡
	//@����ֵ:�ɹ�����TRUE,
	//        ʧ�ܷ���FALSE
     //BOOL SaveMaskFrame(LPCTSTR lpszFileName, const CYUY2Frame& maskFrame);
    BOOL SaveMaskFrame(LPCTSTR lpszFileName, const CImageFrame& maskFrame);
	
    BOOL SaveDynamicMaskFrame();
	BOOL SaveStaticMaskFrame();
    

	//@��  ��:���θ��ŵ�
	//@��  ��:pGrayFrame,�Ҷ�ʵʱͼƬ֡��
	//        YThreshold, �������ޡ�
	//@����ֵ:TRUE, ���µ������������
	//        FALSE,���µ������������
    BOOL MaskClutter(const CImageFrame* pGrayFrame, BYTE YThreshold);

	const int GetSrcImageWidth()const { return m_nSrcImageWidth;}
	const int GetSrcImageHeight()const{ return m_nSrcImageHeight;}

	//add by vera_zhao 2018.11.29
	void SetSrcImageSize(int Width, int Height); 

	//@����:�����Ź��,�����¾�̬����ͼ
	void StartClutterDetection(BOOL bStart, BYTE cAutoMaskingThreshold = DEFAULT_AUTO_MAKSING_THRESHOLD);
    
    //@����:���ڸ��ŵ����
    BOOL IsClutterDetecting()const {return m_bIsClutterDetecting;}

    //@����:�����������ο��λ��
    void SetGuideRectangle(const RECT& rcGuideRectangel, DWORD dwRGBColor);

    void GetGuideRectangle(RECT* pRect, DWORD* pRGBColor)const;
	void RegulateGuideRectangle(CIWBSensor* pSensor,int ActiveImageWidth, int ActiveImageHaight);
    //@����:��ʾ���������������ο�
    void ShowGuideRectangle(BOOL bShow);
    //@����:�б��������ο��Ƿ�ɼ�
    BOOL IsGuideRectangleVisible() const;

	//////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////zhaown
	void SetSpotProportion(int nSpot)                {m_nSpotProportion  = nSpot ;}
	int  GetSpotProportion()const                    {return  m_nSpotProportion  ;}
	void SetMultiEraser(int nEraser)                 {m_nMultiEraser  = nEraser  ;}
	int  GetMultiEraser()const                       { return  m_nMultiEraser    ;}
	void SetFixedBlobSetTime(int nFixedBlobSetTime)  {m_nFixedBlobAutoMaskTime = nFixedBlobSetTime ;}
	int  GetFixedBlobSetTime()const                  {return m_nFixedBlobAutoMaskTime              ;}



    void  EnableDynamicMasking(BOOL bEnableDynamicMasking) {
        m_bIsDynamicMasking = bEnableDynamicMasking; 
        m_bDynamicMaskingChangedEvent = TRUE;
    }
    BOOL  IsDynamicMasking()const                               { return m_bIsDynamicMasking; }

	/////////modify by vera_zhao 2019.10.24
	void  EnableAntiJamming(BOOL bEnableAntiJamming)            { m_bIsAntiJamming = bEnableAntiJamming; }
	BOOL  IsAntiJamming()const                                  { return m_bIsAntiJamming; }

    void EnableStaticMasking(BOOL bEnableStaticMasking)         { m_bIsStaticMasking = bEnableStaticMasking; }
	BOOL IsStaticMasking()const                                 { return m_bIsStaticMasking;}

	void EnableOnlineScreenArea(BOOL bEnableManualMaskFrame) { m_bEnableOnlineScreenArea = bEnableManualMaskFrame; }
	BOOL IsEnableOnlineScreenArea()const                           { return m_bEnableOnlineScreenArea; }

	void DisableReflectionPoint(BOOL bDisableReflectionPoint) { m_bDisableReflectionSpot = bDisableReflectionPoint; }
	BOOL IsDisableReflectionPoint()const                      { return m_bDisableReflectionSpot; }

	void SetCalibrateFailed(int CalibrateFailed)                {  m_bCalibratorFailed = CalibrateFailed ;  }

    //Added by toxuke@gmail.com, 2014/09/10
    //void EnbleGestureRecognition(BOOL bEnable)                  { m_bGestureRecognitionEnable = bEnable;}

    BOOL IsGestureRecognitionEnable() {return (m_eDeviceTouchType == E_DEVICE_FINGER_TOUCH_WHITEBOARD || m_eDeviceTouchType == E_DEVICE_PALM_TOUCH_CONTROL)?TRUE:FALSE; };

    //2014/12/18, added by toxuke@gmail.com
    //@����:���ô�������(�ʴ�/�ִ�)
    //@˵��:���ݲ�ͬ�Ĵ������ͣ����벻ͬ�Ĺ�߲����ļ���
    void SetTouchType(EDeviceTouchType eType);
    //>>


	//@����:���ù�������Ļ�ĳߴ�
	void SetAttachedMonitorSize(const RECT& rcMonitor);
    
    //const CYUY2Frame& GetStaticMaskFrame() const   { return this->m_oStaticMask;}
    const CImageFrame& GetStaticMaskFrame() const   { return this->m_oStaticMask;}
	//const CYUY2Frame& GetDynamicMaskFrame() const  {return m_oDynamicMask      ;}
    const CImageFrame& GetDynamicMaskFrame() const  {return m_oDynamicMask      ;}
	//CYUY2Frame&  GetStaticMask()                   { return this->m_oStaticMask;}
	const  CImageFrame& GetManualOnLineScreenArea() const { return this->m_oManualOnLineScreenArea; }

    //<<2014/12/16
    //@����:�����Ƿ���ʾ��߳ߴ���Ϣ
    //@����:bShow, TRUE:��ʾ; FALSE:���ء�
    void ShowSpotSizeInfo(BOOL bShow) {m_bDisplaySpotSizeInfo = bShow;}

    BOOL IsSpotSizeInfoVisible()const {return m_bDisplaySpotSizeInfo;}

    //@���������Ƿ���ʾ�����Ӿ���
    //@����:bShow, TRUE:��ʾ; FALSE:���ء�
    void ShowSpotRect(BOOL bShow) {m_bDisplaySpotRect = bShow;}

    //@����:�жϹ����Ӿ����Ƿ���ʾ
    BOOL IsSpotRectVisible()const { return m_bDisplaySpotRect;}
    //>>

      CVideToScreenMap& GetVideoToScreenMap()            { return this->m_oVideo2ScreenMap;}
      const CVideToScreenMap& GetVideoToScreenMap()const { return this->m_oVideo2ScreenMap;}
     // CMouseEventGenerator& GetMouseEventGenerator() { return this->m_oMouseEventGen;}

      //added by toxuke@gmail.co, 2014/11/17
      void SetSpotListProcessor(ISpotListProcessor* pSpotListProcessor)
      {
            m_pSpotListProcessor = pSpotListProcessor;
      }

	  ISpotListProcessor* GettSpotListProcessor()
	  {
		  return m_pSpotListProcessor;
	  }


     //<<У����غ���
    //added by toxuke@gmail.com, 2014/11/21
    //@����:����У��ģʽ
	//@����:
	void EnterCalibrateMode(HWND hCalibrateWnd, E_CALIBRATE_MODE eCalibrateMode);

	void EntryShootCalibrateMode(HWND hCalibrateWnd, int isShootCalibrate);

	//@����:�뿪У��ģʽ
	void LeaveCalibrateMode();

	void LeaveShootCalibrateMode();
   
	void EnableOpticalPenControl(BOOL bEnable)
	{
		m_bPenControl = bEnable;
	}

	BOOL IsOpticalPenControlling() const
	{
		return m_bPenControl;
	}

    
	E_CALIBRATE_MODE GetCalibrateMode()const
	{
		return m_eCalibrateMode;
	}

	HWND GetCalibrateHWnd()const
	{
		return m_hCalibrateWnd;
	}

	BOOL IsCalibrating()const
	{
		return m_bIsCalibrating;
	}


    //=====��߲�����غ���======
    void EnterSpotSamplingMode(HWND hSamplingWnd/*,E_COLLECTSPOT_MODE eSpotSettingMode*/)
    {
        m_hSpotSamplingWnd = hSamplingWnd;
        m_bIsSamplingSpot =TRUE;
        //m_eSpotSamplingMode = eSpotSettingMode;
    }

	void LeaveSpotSamplingMode()
    {
        m_hSpotSamplingWnd = NULL;
        m_bIsSamplingSpot = FALSE;
    }
	
    //E_COLLECTSPOT_MODE GetCollectSpotMode()const{return m_eSpotSamplingMode;}
	
    HWND  GetSpotHWnd()const {return m_hSpotSamplingWnd;}
	
    BOOL  IsCollectingSpot()const {return m_bIsSamplingSpot;}

    //>>2014/11/21
    void UpdateDynamicMaskFrame(const CImageFrame& refSatisticFrame);

    void  SetLightSpotSampleSize(const ALL_LIGHTSPOT_SAMPLE_SIZE& sampleSize);//�����ֶ���߲ɼ�ʱ������
    void SetMinimumLightSpotSize(SIZE sizeMinimum) {this->m_szLightSpotMinimumSize = sizeMinimum; }
	SIZE GetMinimumLightSpotSize() { return m_szLightSpotMinimumSize;     }

    int GetID()const {return this->m_nID;}

     //======����ͼƬ֡��غ���======
    BOOL IsImageCaputredDone() const;
    
    //@����:��ʼ����ͼƬ֡
    //@����:nDelay
    void StartImageCapture(DWORD dwDelay);

    //@����:���ز����ͼƬ֡
    //const CYUY2Frame& GetCapturedFrame()const;
    const CImageFrame& GetCapturedFrame()const;

    //======���Ӻ���=====
    //@����:���þ�ͷ��������
    //@����:eLensFocalType, ��ͷ��������ö��ֵ
    void SetLensFocalType(ELensType eLensType);

    //<<added by toxuke@gmail.com, 2015/03/09
    //@����:�ж��Ƿ����ֶ�У��״̬��
    BOOL IsManualCalibrating() const { return this->m_bIsCalibrating && (this->m_eCalibrateMode == CALIBRATE_MODE_MANUAL);}
    //>>
	BOOL IsAutoCalibrating() const { return this->m_bIsCalibrating && (this->m_eCalibrateMode == CALIBRATE_MODE_AUTO); }


    //<<2015/04/07
    //@����:�ϲ��ֶ�У���õ�����Ļ����ͼ����̬����ͼ��ȥ
    //@����:maskFrame, �ֶ�У���õ�����ĻͶӰ����ͼ
    void MergeManualCalibScreenMask(const CImageFrame& maskFrame);
    //>>

    void SetHasDetectSVStarted(bool bStart)
    {
        m_bHasDetectSVStarted = bStart;
    }

    //2015/11/03
    //<<
    void EnterManualResample(HWND hResampleWnd);

    void LeaveManualResample();
    //>>
    CBlobDetector m_oBlobCounter;

	//@���ܣ���Ļ�ߴ�ĸı�
//	void SetScreenSize(int ScreenWidth, int ScreenHeight);
    //<<debug

    //const POINT (& GetContourCrossPoints() const)  [MAX_OBJ_NUMBER][4] 
    //{
    //            return m_aryContourCrossPoints;
    // }
        
    //debug>>
	void ShowCalibratePoints(BOOL bShow)
	 {
		 m_bShowCalibratePoints = bShow;
	 }

	 BOOL IsShowingCalibratePoints()const
	 {
		 return m_bShowCalibratePoints;
	 }

	 const TCameraDebugData* GetCameraDebugData()const
	 {
		 return &m_vecDebugData[0];
	 }

     BOOL IsTriggeringGuesture()const
     {
         if (m_pSpotListProcessor)
         {

             return m_pSpotListProcessor->IsTriggeringGuesture();
         }
         return FALSE;
     }

	 void    SetCurrentOnLineScreenAreaPt(CPoint &point);
	 void    GetCurrentOnLineScreenAreaPt(std::vector<CPoint> &pt);

	 void    DeleteOnLineScreenArea();
	 void    ClearOnLineScreenArea();

	 void    SaveOnLineScreenArea();
	 void    LoadOnLineScreenArea();

	 void    DeleteOnLineFile();
	 bool    LoadOnLinePt();
	 bool    SaveOnLinePt();


     //@����:����4��궨ģʽ
     void EnterMarkStoneBaseMode(HWND hNotifyWnd);

     //@����:�뿪�ĵ�궨ģʽ
     void LeaveMarkStoneBaseMode();

     //@����:���ر궨����
     CBaseStoneMarker& GetBaseStoneMarker();

     //@����:�ж��Ƿ����ڽ���4��궨
     BOOL IsMarkingBasestone()const;



protected:

	BOOL SimulateMannalCalibrate(LPCTSTR lpszAVIFilePath);    
	static ULONG __stdcall SimulateMannalCalibrateProc(LPVOID lpCtx);


    //@����:�����߲ɼ�����
    //@����:szDeviceType, �豸����,_T("3D"):�ʴ���; _T("FP"):��ָ����
    //BOOL LoadSpotData(LPCTSTR szDeviceType);
    //@����:�����߲ɼ�����
    //@����:eDeviceTouchType, �豸����
    //      eLensType, ��ͷ����
    BOOL LoadSpotData(EDeviceTouchType eDeviceTouchType, ELensType eLensType);

	BOOL SaveSpotData(const NORMALIZED_SMAPLE_SIZE& normalizedSampleData );

	//CString  m_strszXmlSystemFileName ;   //�����ڰ�װϵͳ�µ�·��



    //<added by jiqw
    //<added reason:��������ֵ����ʽ��������������Լ�����ľ��������ֵ
    //@���ܣ���ȡ��ĳ������ľ���������±�
    //@������nMonitorId, ���������Ļ���
    //      ptCurentΪ��ǰ������Ļ����λ��    
    int GetNearestRectIndex(const int &nMonitorId, POINT &ptCurent);
    //addedy by jiqw>
    

    //@����:���ݲ���ֵ�͹����Ļ���꣬��˫���Բ�ֵ�㷨���ҳ���Ļ���괦�Ĺ����������
    //@����:CurrentPt, ���λ��
    //      nMonitorId, ��ĻId
    //      nDefaultSize, �������ֵ��ʧ���ȱʡ�ߴ硣
    //@˵��:���������õ�����λ�õĹ��������������ɸѡС��ߡ�
	LONG  GetLightSpotThreshold(POINT ptCurrent ,int nMonitorId, int nAreaIndex, int nDefaultSize);
	float GetPercentageThreshold(POINT ptCurrent ,int nMonitorId, int nAreaIndex, float DefaultPercentage);
	
    //RECT  m_rcCalculateArea[8];                       //��¼˫���Բ�ֵ���ĸ����λ��

    typedef std::vector<RECT>  MONITOR_SUBAREA              ;
    typedef std::vector<MONITOR_SUBAREA> ALL_MONITOR_SUBAREA;//������Ļ���ֵ�������
    ALL_MONITOR_SUBAREA m_AllMonitorSubArea                 ;

	//LONG  m_lScreeenAreas_Actual;                     //ʵ�ʵõ���Ļ�����           
	//LONG  m_lScreeenAreas_stardard;                   //��׼����Ļ�����
	//LONG  GetScreenRealArea();                        //�õ���Ļ����ʵֵ
	//void  SetScreenRealArea(LONG lScreenArea);        //������Ļ����ʵֵ

  
    //@����:�ڴ�������ʾ��Ƶ֡
    //HWND m_hDisplayWnd;//��ʾ����
    BOOL DisplayFrame(HWND hWnd, const BYTE* pYUY2Frame, int nWidth, int nHeight);

     //@����:ִ�й�߲ɼ�����
    //@����:pObj, ָ���������ָ��
    //      nCount, ��߸���
    void DoCollectSpot(const TBlobObject* pObjs, size_t nCount);

     //2015/03/05
    //<<���Ժ���
    void LoadSpotDataFromFile();
    //>>

	//@���ܣ����˵������ʵĵ�
	void  FilterOutUnqualifiedSpot(const CImageFrame& grayFrame ,TBlobObject* pObjs, size_t nObjCount);
	//@���ܣ�Ѱ������Ч����й�������Ч����ó���Ч���
	void  InvalidateSpotInNeighborhood(TBlobObject* pObjs, size_t nCount);
	//@���ܣ����µ�����������Ч��߽��ж����ж���
	void  UpdateObjectWithNewThreshold(const CImageFrame& grayFrame, BYTE newYThreshold, TBlobObject& obj);
	void  SplitInValidSpot(RECT rArea, E_SpotDirection  eSplitDirectioon, int nSplitNum,  std::vector<TObjDimension> & splitinvaliddata);
	void  FlashingSpotInvalidityCheck(TBlobObject* pObjs, size_t nCount);
	void  DetectSpotSlopeDirection(const CImageFrame& grayFrame, TBlobObject& pObj,int CalArea);
	void  SpecialInvalidSpot(TBlobObject& pObj);

	void  FilterMaxNeighborhoodSpot(TBlobObject* pObjs, size_t nObjCount);



 protected:
    HANDLE m_hSimulateManualThread;
	CAviReader  m_oAVIInput;            //AVI����Դ
	CImageFrame m_SimFrame;
	BOOL  m_bSimCalibrate;

	CWaitTimer m_oWaitTimer;

     //int m_nSamplePerRow   ;//ÿ�еĹ�߲��������
     int m_nSamplePerCol ;//ÿ�еĹ�߲��������

    

    ALL_LIGHTSPOT_SAMPLE_SIZE m_allSampleSize;//���еĹ�߲����ߴ硣
    NORMALIZED_SMAPLE_SIZE    m_allNormalizedSampleSize;//���й�߲���ֵ����Ļ�ߴ��һ�����ֵ��


	static const int MAX_FRAME_BUFFER = 2;
	CMemPool<MAX_FRAME_BUFFER, CYUY2Frame> m_YUV2FramePool;//YUY2ͼ��֡�ڴ��

	//HANDLE m_hEventNewImage;
	//HANDLE m_hEventExit;
	//HANDLE m_hDetectThread;


	//��������ͼ��֡���С�
	//std::queue<CYUY2Frame*> m_NewFrameQueue;

	//������������ͼ��֡���С��Ĺؼ���
	//CComAutoCriticalSection m_csForNewFrameQueue;

	//debug����
	HWND m_hWndImageShow              ;//��ʾԭʼ��Ƶ�Ĵ�����
	//HWND m_hWndBkgndShow            ;//��ʾ�����Ĵ�����
	//HWND m_hWndDiffImageShow        ;//��ʾ���ͼ�Ĵ�����
	HWND m_hNotifyWnd                 ;//���ռ�����Ĵ�����
	UINT m_uNotifyMsg                 ;//֪ͨ��Ϣ

	std::map<HWND,CDib*> m_MapWnd2Dib;//Ϊ�˼�����ʾ,�����Ĵ�������Dib�����ӳ��

    void RestrictArea(RECT& rcArea);
	CImageFrame m_CurrentFrame;

	//CBlobDetector m_oBlobCounter;

	BOOL               m_BkgndInitialized;
	static const int   m_nBkgndPixelChangeStep = 8;
	static const int   m_nDiffMinimumTheshold  = 15;


	HBRUSH  m_hRedBrush;
	HBRUSH  m_hGreenBrush;
	HBRUSH  m_hWhiteBrush;
	HBRUSH  m_hCyanBrush;
	HPEN    m_hRedPen;

	LOGFONT m_LogFont;
	HFONT   m_hFont;

	//ÿ�μ�⵽�Ľ��
    //	RECT  m_aryLightSpotBounds[MAX_OBJ_NUMBER];
	//<<<<<<begin
	//add by vera_zhao 2018.12.24
	//��һ����������ʾ������Ч�Ĺ�ߺ���Ч�Ĺ���ò�ͬ����ɫ���ֿ���
	LightSpotBounds m_aryLightSpotBoundsInfo[MAX_OBJ_NUMBER];  
	std::vector <TInvalidSpotList> m_vecInvalidList;
	//<<<<<<end


    //<<debug
    //POINT m_aryContourCrossPoints[MAX_OBJ_NUMBER][4];
    //debug>>

	//��⵽��Ŀ�����
	UINT           m_nObjCount;

	//2010-12-18,����Ŀ���������޿ɵ�
	BYTE          m_YThreshold;

	//2018/01/03,��̬���ε�����, ��ʱ����ڴ����޵Ĺ�߱��Զ����Ρ�
	BYTE          m_YDynamicMaskThreshold;
	//

	//2010-12-18 21:00 ������Ƶ���ڵļ�������
	RECT         m_rcVideoClipArea;

	//CFPSDetector m_oFpsDetector;
		

	int m_nSrcImageWidth;
	int m_nSrcImageHeight;

	//������С�ߴ�
	SIZE m_szLightSpotMinimumSize ;


	//2011-11-01  zhaoweina modify 2013-06-18
	BOOL m_bDisableStaticScreenMask           ;//���ý�̬������

	RECT m_rcManualScreenArea                 ;//�ֶ�ָ������Ļ����
	BOOL m_bValidManualScreenArea             ;//��Ļ��������

	CComAutoCriticalSection m_csScreenAreaMask;

	//CYUY2Frame m_oStaticMask          ;//��Ļ����̬����ͼ
    CImageFrame m_oStaticMask           ;//��Ļ����̬����ͼ
	
    //CYUY2Frame m_oDynamicMask         ;//��Ļ����̬����ͼ
    CImageFrame m_oDynamicMask         ;//��Ļ����̬����ͼ

	//CYUY2Frame m_oClutterMask         ;//��Ļ�����Ӳ�����ͼ
    CImageFrame m_oClutterMask         ;//��Ļ�����Ӳ�����ͼ

	//CYUY2Frame m_oStatisticalFrame  ;//ͳ��ͼ
	CImageFrame m_oStatisticalFrame  ;//ͳ��ͼ
	EManualScreenAreadMode m_eManualScreenAreaMode;//�ֶ�ָ������Ļ����ǰģʽ


	BOOL m_bMaskingClutter     ;   //�Զ����θ��Ź�߱�־
	BYTE m_MaskingClutter      ;   //�Զ����θ��Ź��ʱʹ�õ��ж�����

	CImageFrame  m_oManualOnLineScreenArea;   //���Ƶľ�̬����ͼ


	//<<added by toxuke@gmail.com
	BOOL m_bInitialStage;//��ʼ�׶�
	BOOL m_bInitialFindSpot; //��ʼ���ֹ��
	UINT m_nIntialStageCount;//��ʼ�׶μ�����
	static const UINT  INITIAL_STAGE_WAIT_COUNT = 50;//��ʼ״̬�ȴ�����
	//>>

	//<<ˮƽ������
	//int  m_nFirstGuidelineYPos ;//��һ��ˮƽ�����ߵ�λ��
	//BOOL m_bShowFirstGuideline ;//��ʾ��һ��ˮƽ������
	//int m_nSecondGuidelineYPos ;//�ڶ���ˮƽ�����ߵ�λ��
	//BOOL m_bShowSecondGuideline;//��ʾ�ڶ���ˮƽ������
	RECT   m_rcGuideRectangle    ;//�������ο������
//  RECT   m_rcGuideAbs          ;//�������������������ο������
//	RectF  m_rcGuideRel          ;//��Ե��������ο������

    DWORD  m_dwGuideRectangleColor;//�������ο����ɫ
    BOOL   m_bShowGuideRectangle  ;//�������ο��Ƿ�ɼ���־


	int  m_nSpotProportion     ;
	int  m_nMultiEraser        ;

	BOOL m_bIsDynamicMasking       ; //�Ƿ������Զ�����
    BOOL m_bDynamicMaskingChangedEvent;//�Զ�����״̬�л��¼�


	/////////modify by vera_zhao   2019.10.24
	BOOL  m_bIsAntiJamming          ; //�Ƿ���п����Ŵ���

	BOOL   m_bEnableOnlineScreenArea; //�Ƿ������ֶ����Ƶľ�̬����ͼ

	BOOL   m_bExistOnlineScreenArea; //�Ƿ�����ֶ����ƾ�̬����ͼ�����ݣ�

	BOOL   m_bDisableReflectionSpot;   //�Ƿ���Ӧ�����

	BOOL m_bIsStaticMasking        ;//�Ƿ�̬���θ��ŵ��־

    BOOL m_bIsClutterDetecting       ;//���ŵ���
    BYTE m_nDetectingClutterThreshold;//�Զ������Ź��ʱʹ�õ��ж�����


    //added by toxuke@gmail.com, 2013/04/19
    //����ʶ��ʹ�ܱ�־
    //BOOL m_bGestureRecognitionEnable;

    BOOL m_bCalibratorFailed ;//У��ʧ�ܱ�־����ʱ��Ҫ��������ͼ

    int m_nFPSNotifyCounter;//֡��֪ͨ������  

    //<added by jiqw 2015/04/28
    //<added reason:
        /*
        �ڰ�װ�򵼵ĵ��Լ�����ʱ�����⵲���Ƿ����������ķ�ʽΪ��
        ��1����ģ��Post���������͹�߸�����֪ͨ����
        ��2��֪ͨ���ڻ����ƽ��ֵ�����������������t�Ƚϣ����С��t������Ϊ������ʧ

        Ϊ�����Ч�ʣ�������ÿһ֡��post���ݣ����Ǽ��30֡postһ��
        */
    //>
    int m_nBackSlashDataNotifyCounter;
    //>
    

    int m_nDynamicMaskUpdateCounter;//��̬���θ��¼�����

    //CMouseEventGenerator   m_oMouseEventGen  ;//����¼���������
    CVideToScreenMap       m_oVideo2ScreenMap;//��Ƶ���굽��Ļ�����ӳ�����
    //CGestureEventGenerator m_oGestureEventGen;//����ʶ����

    //const CIWBSensor* m_pIWBSensor         ;//������IWB����������ָ��
    int m_nID;//���

    //��߼����һ�������߳������, ��߲������ݴ����߳������á�
    //�����Ҫ�߳�ͬ��
    CComAutoCriticalSection m_csForLightSpotSampleSize;


    //<<added by toxuke@gmail.com, 2014/11/21, 
    //У����ر���
	BOOL m_bIsCalibrating ;//����У����־

	volatile BOOL m_bIsShootCalibrateing;//  �������У����־
	HWND m_hCalibrateWnd  ;//У�����ھ��
	BOOL m_bPenControl    ;//���Ŀ���״̬
    E_CALIBRATE_MODE m_eCalibrateMode;//У��ģʽ

    //��߲�����ر���
    //E_COLLECTSPOT_MODE m_eSpotSamplingMode;
    BOOL m_bIsSamplingSpot             ;
    HWND m_hSpotSamplingWnd            ;
    //>>

    //CContactPtFinder m_oContactPtFinder;
   
    //POINT m_ptContactVideoPos;//��ʵĴ���λ��(��Ƶ����), Ϊ����CInterceptFilter::Transform����ʾ��ӡ��λ��,�������ɳ�Ա������


    ISpotListProcessor* m_pSpotListProcessor;//ָ���ߴ������ӿڵ�ָ��


    //<<added by toxuke@gmail.com, 2014/12/16
    BOOL m_bDisplaySpotSizeInfo ;//�Ƿ���ʾ��߳ߴ���Ϣ
    BOOL m_bDisplaySpotRect    ;//�Ƿ���ʾ�����Ӿ��Ρ�
    //>>

  	UINT  m_nFixedBlobAutoMaskTime   ;   //�̶�����Զ����ε�����ʱ��

    EDeviceTouchType m_eDeviceTouchType;//��������

    //<<2015/01/14
    ELensType   m_eLensType ;//��ͷ��������
    //>>
  
    //֡������
    CImageFrame m_oCapturedFrame         ;//����֡��
    DWORD       m_dwImageCaptureStartTime;//������ʼʱ�̡�
    DWORD       m_dwImageCaptureDelayTime;//�����ӳ٣���λ:���롣
    BOOL        m_bImageCaptureDone      ;//������ɱ�־�� 

	CImageFrame m_oShootFrame;//�����������֡��

    //
    //<added by jiqw 2015/05/11 15:21
    //�ڵ��Լ�����ʱ�����ܼ�����Ƿ���ʧ
    bool        m_bHasDetectSVStarted;

   //<<debug
    //2015/11/03
    //�ֶ��ٲ���:ָ�Զ�У����ɺ�, ���ù�ʻ�����ָ��У�����Ŵ����£��ɼ���ߵ���Ƶ���꣬�Ա�����Զ�У����ʵ��ʹ��ʱ��λ������ϵ��
    HWND m_hManualResampleWnd;//�ֶ��ٲ���������
    BOOL m_bManualResample   ;//�ֶ��ٲ�����־    
    //debug>>

    //std::vector<POINT> m_vecLightSpotContourPoints;//�����������������.
	RECT m_rcMonitor;
	//<<2017/11/16
	BOOL m_bShowCalibratePoints;//���Թ���, �ڻ�������ʾУ����λ��
	//>>

	//<<���Թ���
	TCameraDebugData m_vecDebugData[MAX_OBJ_NUMBER];
	//>>
	std::vector<CPoint>  m_vecOnLinePt ;    //���������������ͼʱ�Ĳɼ���

	int m_frameCount = 0;
	map<int, int>  m_IndexToHaveSpot;

	//���ǹ�������������ǹ
	POINT  spot_Point_[5];

	static const  double SCREEN_DISTANCETWOSPOT_WIDTH_IN_MM;
	double m_ScreenMinDistanceWidthInPixel ;


    BOOL m_bMarkingBaseStone;//4��궨״̬��־
    CBaseStoneMarker m_oBaseStoneMarker;//4��궨ִ����

    //����4��궨���
    void SaveBaseStoneMarker();

    //����4��궨���
    void LoadBaseStoneMarker();

    void InitBaseStoneFilePath(TCHAR* buf, int nBufSize);
};

