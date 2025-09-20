#pragma once


class CUsbCameraDeviceList;
class CIWBSensor;


//IWB����������Ĺ�����
class CIWBSensorManager
{
public:

    CIWBSensorManager();


    ~CIWBSensorManager();

    //@����:��ʼ��
    //@����: nSensorCount, CMOS����������
    void Init(int nSensorCount);

    //@����:����ʼ��
    void Uninit();


    //@����:��IWBSensor�����������
    //@����:Camera�豸�б�
    void AssignCamera(const CUsbCameraDeviceList& CameraList);


	//@����:������Ƶ��ʾ����
	//@����:hDispWnd,  ��Ƶ��ʾ������
	//      hNotifyWnd, ������Ϣ���մ�����
	//      rcVideoDispArea, ��Ƶ��ʾ����
	void SetVideoDispParams(HWND hDispWnd, HWND hNotifyWnd, RECT& rcVideoDispArea);


    //@����:��ʼ����
    //@����:
	//		nSensorID, ���������(0~n), -1ʱָȫ�崫����
    BOOL StartRunning(int nSensorID = -1);



    //@����:ֹͣ����
    //@����:nSensorID, ���������(0~n), -1ʱָȫ�崫����
    BOOL StopRunning(int nSensorID=-1);


    //@����:�ı���Ƶ��ʾ����ĳߴ�
    void SetVideoDisplayArea(const RECT& rcNewDisplayArea);


    //@����:�л�����ͷ��ָ����ģʽ
    void SwitchToMode(ESensorLensMode eMode, int nSensorID=-1);

    //@����:��ȡ����ģʽ
    //@˵��:ȫ�ֵĹ���ģʽ���һ���������һ��
    ESensorLensMode GetLensMode()const;

    //@����:�ж��Ƿ��д�������ʹ��ģʽ�µ����
    BOOL HasNormalUsageCamera()const;

    //����:ִ���Զ�У������
    //@����:nSensorID, ���������(0~n), -1ʱָȫ�崫����  
    //BOOL DoAutoCalibrate(int nSensorID);


    //����:ִ���ֶ�У������
    //@����:nSensorID, ���������(0~n), -1ʱָȫ�崫����
    //BOOL DoManualCliabrate(int nSensorID);



    //@����:������Ƶ��ʾ����ȷ����Ƶ������ʵ����
    CIWBSensor* FindSensorInstance(const RECT& rcDisplayArea);

	const CIWBSensor* GetSensor(int nID) const;
	CIWBSensor* GetSensor(int nID);

    //@����:���Ʋ��ֿ��
    //@����:hWnd, ������
    void DrawLayoutFrame(HWND hWnd = NULL);

    UINT GetSensorCount()const
    {
        return m_vecSensors.size();
    }


    //@����:������������
    //@����:sysCfgData, ����ͼ�񴫸�����������Ϣ
    void SetCfgData( TSysConfigData& sysCfgData);

	void SetGlobalCfgData(TSysConfigData& sysCfgData);

    //@����:��ȡ��������
    //@����:sysCfgData, ����ͼ�񴫸�����������Ϣ
    BOOL GetCfgData(TSysConfigData& sysCfgData);


    //@����:USB����ͷ�����¼���Ӧ
    //@����:lpszDevicePath, �����USB����ͷ���豸·��
    //void OnCameraPlugIn(LPCTSTR lpszDevicePath);
    void OnCameraPlugIn(const TCaptureDeviceInstance& devInst);

    //@����:USB����ͷ�γ��¼���Ӧ,  �����USB����ͷ���豸·��
    void OnCameraPlugOut(LPCTSTR lpszDevicePath);

    //@����:����λ��ȷ��ͼ���������ĸ���������
    //@����:�ǿգ�����ָ�򴫸��������ָ��
    //      NULL,δ�ҵ���
    CIWBSensor* SensorFromPt(const POINT& ptPos);

    CIWBSensor* GetSensor();
	//CIWBSensor* GetSensor0();
	void SelectAsCurrentSensor(CIWBSensor* pSensor);

	int   CurrentSelectSensor();

    //@����:ʹ�ܹ��
    //@����:bEnable,ʹ�ܱ�־
    void EnableOpticalPen(BOOL bEnable);

    //@����:�жϹ���Ƿ��ڿ�����
    //@˵��:ֻҪ��һ֧����ڿ�����,�򷵻�TRUE
    BOOL IsOpticalPenControlling();


    //@����:��ʼͼ�񴫸������Զ�У������
    //@����:ePattern, У��ͼ��ģʽ
    //      hNotifWindow, У���������֪ͨ��Ϣ"WM_AUTO_CALIBRATE_DONE"�Ľ��մ���
    //      nSensorID, -1,ȫ��������У��
    void  StartAutoCalibrate(E_AutoCalibratePattern ePattern, HWND hNotifWindow, int nSensorID=-1);

    //@����:һ��ͼ�񴫸����Զ�У����ɺ���¼���Ӧ����
    //@����:bSuccess, �ɹ�/ʧ�ܱ�־
    //      bSimulateMode,ģ��У��ģʽ
    void OnIWBSensorAutoCalibrateDone(BOOL bSuccess, BOOL bSimulateMode);


    //@����:��ʼͼ�񴫸������ֶ�У������
    //@����:
    //      hNotifWindow, У���������֪ͨ��Ϣ"WM_FINISH_MAUNUAL_CALIBRATING"�Ľ��մ���
    //      nPtsInRow, ÿ��У��������� -1: ��ʾ�������ļ��ж�ȡ
    //      nPtsInCol, ÿ��У��������� -1: ��ʾ�������ļ��ж�ȡ
    //      nSensorID, -1,ȫ��������У��
    void  StartManualCalibrate(HWND hNotifyWindow, int nPtsInRow = -1, int nPtsInCol = -1, int nSensorID=-1, EManualCalibrateType eManualCalibType = E_MUNUAL_CALIB_FULL_SCREEN);

    void  StartShootCalibrate(HWND hNotifyWindow, int nPtsInRow = -1, int nPtsInCol = -1, int nSensorID = -1, EManualCalibrateType eManualCalibType = E_MUNUAL_CALIB_FULL_SCREEN);

    void  LeaveShootCalibrateMode(int nSensorID);

    //@����:һ��ͼ�񴫸����ֶ�У������ɺ���¼���Ӧ����
    //@����:bSuccess, �ɹ�/ʧ�ܱ�־    
    void OnIWBSensorManualCalibrateDone(BOOL bSuccess, DWORD dwCtxData);

    //@����:�ж�У���Ƿ�ɹ�
    BOOL IsCalibarateOk();

    //@����:��̬������Ļ�ڵĸ��Ź��
    void StaticMaskClutter();


    //@����:�����Զ���������
    //@����:hNotifWindow, У���������֪ͨ��Ϣ"WM_SEARCH_SCREEN_AREA_DONE"�Ľ��մ���
    //      nSensorID, ������ID; -1ʱ,Ϊȫ��������У����
    void StartSearchMaskArea(HWND hNotifWindow, int nSensorID=-1);



    //@����:��"�������������"�¼�����Ӧ����
    //@����:bSuccess, �ɹ�/ʧ�ܱ�־
    void OnIWBSensorSearchMaskAreaDone(BOOL bSuccess);


    //@����:������߲ɼ�����
    void StartLightSpotSampling(HWND hNotifWindow, int nSensorID=-1);


    //@����:��߲ɼ������¼�����Ӧ����
    //@����:bSuccess, �ɹ�ʧ�ܱ�־
    void OnIWBSensorLightSpotSamplingDone(BOOL bSuccess,int nSensorId);

    //@����:�ж����еĲ����Ѿ����
    BOOL AllSamplingIsDone();


    //@����:���ع���б�����
    CSpotListProcessor& GetSpotListProcessor(){return *m_pSpotListProcessor;}

    //CSpotListProcessor& GetSpotListProcessor1() { return *m_pSpotListProcessor1; }

    //@����:�жϴ������豸�Ƿ�������
    //@����;��
    //@˵��:ֻҪ��һ�������������У�����Ϊ��������״̬
    BOOL IsRunning()const;


    //@����:�ж����д������豸ֹͣ����
    BOOL IsAllSensorStopped()const;


    //���Թ�����غ���
    //@����:�ж��Ƿ�����¼��
	BOOL IsRecording() const;

	//@����:��ʼ¼��
	BOOL StartRecording();

	//@����:ֹͣ¼��
	BOOL StopRecording();

    //@����:������Ƶ����ΪAVI¼���ļ�
    //@����:lpszVideoPath1,������1��·��
    //      lpszVideoPath2,������2��·��
    void LoadSrcFromAVI(LPCTSTR lpszVideoPath1, LPCTSTR lpszVideoPath2);

    //@����:��������ͼ�񴫸�������ʾ����
    //void SwapSensorImage();


    //@����:��������ͼ�񴫸�������ʾ����
    //@����:��һ��ͼ�񴫸�����Id
    //      �ڶ���ͼ�񴫸�����id
    void SwapSensorImage(UINT firstSensorId, UINT secondSensorId);

    //@����:ͨ����ȡ¼���ļ������Զ�У������
    //@����:nSensorId, ������Id
    //      hNotifyWnd, У����Ϻ����Ϣ֪ͨ����
    //      lpszAVIFileName, ¼���ļ�
    //      cBrightness, У��ʱ�Ļ���ƽ������
    //      eGridNumber, У�����̸�����
    BOOL DoSimulateAutoCalibrate(int nSensorId,  HWND hNotifyWnd, LPCTSTR lpszAVIFileName, SIZE szScreen, BYTE cBrightness, E_AutoCalibratePattern eGridsNumber);


    //@����:��Ļ�ֱ��ʱ仯�¼���Ӧ����
    //@����:nScreenWidth, �µ���Ļ���
    //      nScreenHeight,�µ���Ļ�߶�
    void OnDisplayChange(int nScreenWidth, int nScreenHeight);

    /*
    @���ܣ��������ܼ�⵲���Ƿ���ʧ
    */
    void StartDetectBackSplashVanished();

    /*
    @���ܣ�ֹͣ���ܼ�⵲���Ƿ���ʧ
    */
    void StopDetectBackSplashVanished();


      //@����:�鿴У������λ��
      //@����:bShow,TRUE,��ʾ����
      //            FALSE, ���ط���
      //@Author:15077727@qq.com, 2015/09/09
      BOOL CheckCalibrateSymbols(BOOL bShow);



      //�ж��Ƿ�У�������Ƿ��Ѿ���ʾ
     BOOL IsCalibrateSymbolVisible()const;

      //>>

    BOOL CalibrateSymbolManualResample();


    //Added by 15077726@qq.com
    //2015/11/9
    //����:�ж��Ƿ��Ѿ����У��
    BOOL IsCalibrated()const;

    //����:�ж��Ƿ���У���ɹ����������
    BOOL HasCalibratedCamera()const;


	void OnTimer(LPVOID lpCtx);

    CScreenLayoutDesigner& GetScreenLayoutDesigner() {return m_oScreenLayoutDesigner;}


    void ApplyScreenLayout();

    void  Start4BasePointMarking(HWND hNotifyWindow, int nSensorID);

    //@����:4��궨������Ӧ����
    void OnIWBSensor4BasePointMarkingDone(BOOL bSuccess);
	void EnableOnlineScreenArea(BOOL bEnable);
	BOOL IsEnableOnlineScreenArea();

protected:
    void UpdateCalibrateToleranceDistribute();
    void UpdateVideoLayout(const RECT& rcDisplayArea);
	void DrawSelectBound(HWND hWnd);
	static const int SELECT_BOUND_WIDTH = 5;

    std::vector<CIWBSensor*> m_vecSensors;
    
    //������һ�δ�����״̬������
    std::vector<ESensorLensMode> m_vecLastSensorLensMode;
    
    //��ʾ�������ʵ��
    //CDispDevFinder      m_oDispMonitorFinder;

    //�Զ�У������
    //CAutoCalibrator          m_oAutoCalibrator;

    
    //��ߴ�����,����������ơ���ꡢ�����¼�
    //<<����ľ�1M���ҵĶ�ջ����, ��Ϊ�ڶ�(heap)������CSpotListProcessor����
    //CSpotListProcessor      m_oSpotListProcessor;
    CSpotListProcessor*       m_pSpotListProcessor;
    //>>

    //CSpotListProcessor* m_pSpotListProcessor1;

    //��߲ɼ�����
    CCollectSpotSize   m_wndLightSpotSampling;


    static const int SPLITTER_WIDTH = 5;

    //���������ʾ����Ĳ���
    std::vector<RECT> m_vecVideoLayout;//��Ƶ����
    std::vector<RECT> m_vecSplitter   ;//�ָ�������


    //��Ļ������ƹ���, ����ÿ����Ļ��С��һ��
     CScreenLayoutDesigner m_oScreenLayoutDesigner;


    HWND m_hVideoDispWnd;//��Ƶ��ʾ����

    CWGRUtilContextImpl m_execContext;  //windows ϵͳ������ʶ��������ִ�л���


    //======�Զ�У����ز���========================
    enum EOperationMode
    {
        E_MODE_ALL_SENSOR,   //�Զ�У�����д�����
        E_MODE_SINGLE_SENSOR //У������������
    };

    EOperationMode  m_eOperationMode;
    HWND m_hNotifyWindow;
    E_AutoCalibratePattern m_eAutoCalibratePattern;

    int m_nAutoCalibrateTryTimes                    ;  //�Զ�У�����Դ���
    int m_nCurrentSensorID                          ;  //����У����ͼ�񴫸�����ID
    static const int MAX_AUTOCALIBRATE_TRY_TIMES = 1;  //�Զ�У�����Դ�����
    std::vector<BOOL> m_vecCalibrateResults         ;  //����ÿ��������У�����������,˵����ҪԤ�ȱ���ÿ����������У���������Ϊ�������˹�Ƭ��Ҫ
                                                       //�����д�������У����ɺ���ܹ�һ����ϡ�
    //BOOL m_bAllScreenMergedOnOnePlane               ;//������Ļ�ں���һ��ƽ���ϵı�־
	UINT m_uCurrentSelectSensordId;                    //��ǰѡ�е�Sensor id


};