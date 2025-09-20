#pragma once

#define BLACK RGB(0,    0,   0)
#define WHITE RGB(255,255, 255)
#define RED   RGB(255,  0,   0)
#define BLUE  RGB(0,    0, 255)
#define GREEN RGB(0,  255,   0)

//ǰ����ɫ
#define FOREGROUND_COLOR WHITE

#define BACKGROUND_COLOR BLACK
class CIWBSensor
{
public:
    CIWBSensor(int nID=0);

    virtual ~CIWBSensor();

    //@����:������Ƶ����ʾ�����Ϣ
    //@����:hDispWnd, ��Ƶ���Ŵ���ľ��
    //      rcDispArea, ��Ƶ���Ƶ�����  
    //      hNotifyWnd, ������Ϣ���մ�����
    void SetVideoDispParams(HWND hDispWnd, RECT& rcDispArea, HWND hNotifyWnd);

    //@����:���д�����
    BOOL Run();

    BOOL Stop();

    void ShowMissStatusInfo();


    //@����:������Ƶ�����豸��Ϣ
    void SetDeviceInfo(const TCaptureDeviceInstance& devInfo);

    //@����:������Ƶ�����豸��Ϣ
    const TCaptureDeviceInstance& GetDeviceInfo()const;


    void SetFavoriateMediaType(const VideoMediaType& vmt);

    CVideoPlayer* GetVideoPlayer()
    {
        return &m_oVideoPlayer;
    }

    CPenPosDetector* GetPenPosDetector()
    {
        return &m_oPenPosDetector;
    }

    CInterceptFilter*  GetInterceptFilter()
    {
        return this->m_pInterceptFilter;
    }
    
     CAutoCalibrator* GetAutoCalibrator()
     {
         return &this->m_oAutoCalibrator;
     }
    //@����:����ָ���ľ�ͷģʽ,��ִ��ϵ�ж���
    //@����:eMode, ����ģʽö��ֵ
    void SwitchLensMode(ESensorLensMode eMode);


    //@����:ֻ����������ľ�ͷģʽ������ʵ��ִ�ж���
    //@����:eMode, ��ͷģʽö��ֵ
    void SetLensMode(ESensorLensMode eMode){ m_tCfgData.eLensMode = eMode;}
    ESensorLensMode GetLensMode()const{ return m_tCfgData.eLensMode;}


    
    //@����:������������
    //@����:cfgData, �������, ͼ�񴫸�����������Ϣ
    //      pGlobalSettings, �������, ָ��ȫ��������Ϣ��ָ��
    void SetCfgData( const TSensorConfig& cfgData, const GlobalSettings* pGlobalSettings = NULL);
    void SetGlobalCfgData(const GlobalSettings* pGlobalSettings = NULL);

    void SetlenCfgData(const TLensConfig& lencfgData);

    //@����:��ȡ��������
    //@����:cfgData, �������, ͼ�񴫸�����������Ϣ
    //void GetCfgData(TSensorConfig& cfgData)const;
    const TSensorConfig& GetCfgData()const;

    //@����:�жϴ������Ƿ����ڹ���
    BOOL IsDetecting();//


    //@����:ʹ�ܹ��
    //@����:bEnable,ʹ�ܱ�־
    void EnableOpticalPen(BOOL bEnable);

    void Reset_Start_Offset();

    //@����:�жϹ���Ƿ��ڿ�����
    //@˵��:ֻҪ��һ֧����ڿ�����,�򷵻�TRUE
    BOOL IsOpticalPenControlling();

    //@����:��ʼͼ�񴫸������Զ�У������
    //@����:ePattern, У��ͼ��ģʽ
    //      hNotifyWnd, У���������ͬ־��Ϣ�Ľ��մ���
    void  StartAutoCalibrate(E_AutoCalibratePattern ePattern, HWND hNotifyWnd);



    //@����:��ʼ�Զ�����
    //@����:hNotifyWnd, У���������ͬ־��Ϣ�Ľ��մ���
    void  StartAutoMasking(HWND hNotifyWnd);

    //@����:�������Զ�У����ɺ���¼���Ӧ����
    //@����:bSuccess, �ɹ�/ʧ�ܱ�־
    void OnAutoCalibrateDone(BOOL bSuccess);


    //@����:����Ƶ��������ʾ������Ϣ
    void ShowErrInfo(LPCTSTR lpszInfo);


    //@����:��ʼͼ�񴫸������ֶ�У������
    //@����:
    //      nPtsInRow, ÿ�е�У�������
    //      nPtsInCol, ÿ�е�У�������
    //      hNotifyWnd, У���������ͬ־��Ϣ�Ľ��մ���
    void  StartManualCalibrate(HWND hNotifyWnd, int nPtsInRow = -1, int nPtsInCol = -1,  EManualCalibrateType eManualCalibType = E_MUNUAL_CALIB_FULL_SCREEN);

    void  StartShootCalibrate(HWND hNotifyWnd);

    void LeaveShootCalibrateMode();

    //@����:�������ֶ�У����ɺ���¼���Ӧ����
    //@����:bSuccess, �ɹ�/ʧ�ܱ�־
    void OnManualCalibrateDone(BOOL bSuccess);


    //@����:�������Զ�����������������¼���Ӧ����
    //@����:bSuccess, �ɹ�/ʧ�ܱ�־
    void OnAutoSearchMaskAreaDone(BOOL bSuccess);

    int GetID()const
    {
        return m_nID;
    }

    //@����:������߲ɼ�����
    //@����: hSampleWnd, ����������
    BOOL StartLightSpotSampling(HWND hSampleWnd);


    //@����:��߲ɼ������¼�����Ӧ����
    //@����:allSampleSize, ���еĲ���ֵ
    ///     bSuccess, �ɹ�ʧ�ܱ�־
    void OnLightSpotSamplingDone(const ALL_LIGHTSPOT_SAMPLE_SIZE& allSampleSize, BOOL bSuccess);



    //@����:��Ļ�ֱ��ʷ����仯ʱ���¼���Ӧ����
    ///@����:rcNewMonitorResolution, ��������Ļ�ֱ�����Ϣ
    //void OnMonitorResolutionChange(const RECT&  rcNewMonitorResolution);
    void OnMonitorResolutionChange();

    /*
    @���ܣ��������ܼ�⵲���Ƿ���ʧ
    */
    void OnStartDetectBackSplashVanished();

    /*
    @���ܣ�ֹͣ���ܼ�⵲���Ƿ���ʧ
    */
    void OnStopDetectBackSplashVanished();


    //@����:��̬���ο�ʼ�ص�������
    //@����:lpCtx, �ص������������ݡ�
    static  BOOL CALLBACK OnPreStaticMasking(LPVOID lpCtx);



    //@����:�豸��ʧ�¼���Ӧ����
    void OnDeviceIsMissing();

    static BOOL  OnAutoCalibChangeCameraParams(EChangeCalibCameraParams eCtrlMode, LPVOID lpCtx, BYTE param1, BYTE param2);


      //@����:�鿴У������λ��
      //@����:bShow,TRUE,��ʾ����
      //            FALSE, ���ط���
      //@Author:15077727@qq.com, 2015/09/09
      BOOL CheckCalibrateSymbolPos(BOOL bShow);

      //�ж��Ƿ�У�������Ƿ��Ѿ���ʾ
      BOOL IsCalibrateSymbolVisible()const;
      //>>

      //<<added by 15077726@qq.com
      BOOL CalibrateSymbolManualResample();
      //>>

      CCalibrateSymbolManualSampler& GetCalibrateSymbolManualSampler() { return this->m_oCalibSymbolManualSampler;}


      //����;�жϴ������Ƿ���У������
      BOOL IsCalibrated()const;

      void SetAttachedScreenArea(const RECT& rcMonitor);

      //@����:��ȡ��������Ļ����ߴ�
      BOOL GetAttachedScreenArea(RECT& rcMonitor)const;


      //@����:��ȡ�����������Ļ��������, ������������ʵ�ʹ�Ͻ����Ļ����
      //@����:uID, id of camera sensor
       const RECT& GetVisibleScreenArea() const;


      void  OnTimer(LPVOID lpCtxData);
      void  SetStrokeInterpolate(bool bEnableStrokeInterpolate);
      void  SetOnlineScreenArea(bool bEnableOnlineScreenArea) ;

      void GetPidVid(INT* pPID, INT* pVID)const;

      ECameraType GetCameraType()const { return m_eCameraType; }

      //@����:��ʼ������4��궨
      void Start4BasePointMarking(HWND hNotifyWnd);

      //@����:������4��궨��������¼���Ӧ����
      //@����:bSuccess, �ɹ�/ʧ�ܱ�־
      void On4BasePointMarkingDone(BOOL bSuccess);


      //��ʼ��У��ʵ��
      void ReinitCalibrateInst(E_CALIBRATE_MODEL eCalibrateModel);

	  void SetResolutionType(CAtlString  Value);

	  void OnStartDrawOnlineScreenArea();
	  void OnClearDrawOnlineScreenArea();
	  void OnEnableDrawOnlineScreenArea(BOOL bEnable);

	  BOOL IsEnableOnlineScreenArea();

	  BOOL GenerateMaskFrameWithCalibrateData(CImageFrame& maskFrame, const RECT& rect, int nMonitorId = 0);
	  void GetCollectSpotShowPath(TCHAR *lpszbuf, unsigned int numberOfElements);

	  void UpdateTouchTypeFromUSBKey();


	  void UpdateThrowRatioDisplayInfo();


      UINT GetScreenAreaNo();
      void SetScreenAreaNo(UINT areaNo);
protected:
	  
	  ELensType MapThrowRatioToLensType(const double& throwRatio);

	  void UpdateAutoCalibrateCompensateCoefs(const TLensConfig& lensConfig);

protected:
  //��Ƶ���Ŷ���
  CVideoPlayer     m_oVideoPlayer;

  //��Ƶ���Ų���
  //HWND             m_hVideoDispWnd  ;//��Ƶ���Ŵ�����
  //RECT             m_rcVideoDispArea;//��Ƶ��������
  //HWND             m_hNotifyWnd     ;//��Ƶ����֪ͨ���մ���



  //��ΪCInterceptFilter�̳���CTransformFilter�࣬
  //��CTransformFilter���а�����ֹ�˿������캯���͸�ֵ����������
  //CCritSec  
  CInterceptFilter* m_pInterceptFilter;


  //��߼�����
  CPenPosDetector  m_oPenPosDetector;


  //����ͷ��Ϣ
  TCaptureDeviceInstance m_tDeviceInfo;

  //
  VideoMediaType        m_tFavoriteMediaType;



  //��ͼ�񴫸������ӵ���Ļ����
  RECT  m_rcMonintorArea;


  //��Ƶ���굽��Ļ�����ӳ�����.
//  CVideToScreenMap     m_oVideoToScreenMap;


  //����¼�������
  //CMouseEventGenerator m_oMouseEventGenerator;
  int m_nID;

  TSensorConfig m_tCfgData; 

  //�Զ�У������
   CAutoCalibrator m_oAutoCalibrator;


   //�ֶ�УУ������
   CManualCalibrateWnd   m_oManualCalibrator;

   CShootCalibrateWnd    m_oShootCalibrator;

  //��Ļ������
  //static CDispMonitorFinder m_oDispMonitorFinder;

  //��߲ɼ�����
//  CCollectSpotSize   m_wndLightSpotSampling;

//  ISpotListProcessor* m_pSpotListProcessor;

  //��������ͷģʽ
  //ESensorLensMode m_eLensMode;
  ECameraType     m_eCameraType;

  //<<���Թ���, �鿴У����
  //2015/09/09, by 15077726@qq.com
  CCalibrateSymbolChecker m_oCalibSymbolChecker;

  //2015/11/13, by 15077726@qq.com
  CCalibrateSymbolManualSampler m_oCalibSymbolManualSampler;

  CSensorTimerActionExecuter m_oTimerActionExecuter;
  static void _stdcall OnManualSampleDoneCallBackProc(LPVOID lpCtx, BOOL bSuccess);
  //>>

  static const int SONIX_PID = 0x9230;
  static const int SONIX_VID = 0x05a3;

  class ThrowRatioInfo
  {
  public:
	  ThrowRatioInfo()
	  {
		  m_dbThrowRatioInFirmware = 0.0;
		  m_dbThrowRatioSelected   = 0.0;
	  }

	  double m_dbThrowRatioInFirmware;
	  double m_dbThrowRatioSelected;

  }m_ThrowRatioInfo;

};
