#pragma once
#include <GlobalDefine.h>
#include "InterpolateDispatcher.h"
class ISpotListProcessor
{
public:
     //@����:������б�������д�����б���Ϣ
    //@����:pLightSpots��ָ�����б��ָ��
    //      nLightSpotCount, ��߸���
    //      dwCameraId, ��0��ʼ������ͷ���    
    virtual BOOL WriteSpotList(TLightSpot* pLightSpots, int nLightSpotCount, TLightSpot* pLightSpots1, int nLightSpotCount1, DWORD dwCameraId, E_CALIBRATE_MODEL eCalibrateModel) = 0;

    //@����:��������
    virtual void StartProcess() = 0;

    //@����:ֹͣ����
    virtual void StopProcess() = 0;

    //@����:��Ļ�ֱ��ʱ仯�¼���Ӧ����
    //@����:nScreenWidth, �µ���Ļ���
    //      nScreenHeight,�µ���Ļ�߶�
    virtual void OnDisplayChange(int nScreenWidth, int nScreenHeight)=0;

    //@����:�ж��Ƿ����ڴ�������
    virtual BOOL IsTriggeringGuesture()const = 0;



    //@����:��ȡ�����������Ļ�������򡣸�������������Ͻ������ڽӵ��ں���
    //@����:uSensorID, id of camera sensor
    //     monitorArea, �����Ч��Ͻ��������
    virtual RECT GetVisibleScreenArea(UINT uSensorID, const RECT& monitorArea) const = 0;

	virtual void SetSmoothCoefficient(int nSmoothCoff) = 0;
};


//����б����߳�
class CSpotListProcessor:public ISpotListProcessor
{
public:
    CSpotListProcessor();

    ~CSpotListProcessor();

    //@����:��ʼ��
    void Init(UINT nSensorCount);

    //@����:��������
    //@˵��:���ü���Ϊ0ʱ���������߳�, ����,���ü���+1,
    virtual void StartProcess();

    //@����:ֹͣ����
    //@˵��:����һ��,���ü���-1�����ü���Ϊ0ʱ�������߳̽���
    virtual  void StopProcess();

    //@����:������б�������д�����б���Ϣ
    //@����:pLightSpots��ָ�����б��ָ��
    //      nLightSpotCount, ��߸���    
    //      dwCameraId, ��0��ʼ������ͷ���
    //@����:�ú���������߳�ͬʱ���ʣ������Ҫȷ���̰߳�ȫ
    virtual BOOL WriteSpotList(TLightSpot* pLightSpots, int nLightSpotCount, TLightSpot* pLightSpots1, int nLightSpotCount1, DWORD dwCameraId, E_CALIBRATE_MODEL eCalibrateModel);
    

    //@����:��λ������
    void Reset();

    //@����:��������ͷ�ĸ���
    //@˵��:��ȷ����ͷ����,���������ʵʱ�����ٶ�
    //void SetCameraCount(UINT nCameraCount);


     CSpotMerger& GetSpotMerger() { return this->m_oSpotMerger;}
     static CVirtualHID& GetVirtualHID() { return m_oVirtualHID;}



    //@����:��Ļ�ֱ��ʱ仯�¼���Ӧ����
    //@����:nScreenWidth, �µ���Ļ���
    //      nScreenHeight,�µ���Ļ�߶�
    virtual void OnDisplayChange(int nScreenWidth, int nScreenHeight);


    //@����:�ж��Ƿ����ڴ�������
    virtual BOOL IsTriggeringGuesture()const;


    //@����:��ȡ�����������Ļ�������򡣸�������������Ͻ������ڽӵ��ں���
    //@����:uCameraIndex, id of camera sensor
    //     monitorArea, �����Ч��Ͻ��������
    RECT GetVisibleScreenArea(UINT uCameraIndex, const RECT& monitorArea) const;

    CToleranceDistribute& GetToleranceDistribute();

	virtual void SetSmoothCoefficient(int nSmoothCoff);

    void EnableAutoMerge(bool bEnable);
protected:
    //@����:���������߳�
    BOOL StartProcessThread();

    //BOOL StartProcessThread1();

    //@����:ֹͣ�����߳�
    void StopProcessThread();

    //@����:������
    void ProcessLightSpots(); 


    //@����:���й�ߵĺ����������
    //@����:pLightSpots, ָ�����б��ָ��
    //      nLightSpotCount, ��߸���
    //@˵��:
    //      ��ߺ�������������:
    //      a)λ���ں����ڵĹ�ߺϲ�
    //      b)ǰ��ʱ�̵Ĺ��ƥ��
    //      c)����ʶ��
    //      d)����, ����¼��Ĳ���
    void OnPostProcess(TLightSpot* pLightSpots, int nLightSpotCount, TLightSpot* pLightSpots1, int nLightSpotCount1);

    /*
     @���ܣ�GLBoard����ʶ��
     @����:pLightSpots, ָ�����б��ָ��
           nLightSpotCount, ��߸���
     @����ֵ�����ݷ���ֵ�����Ƿ�������ݴ�����������¼�
    */
    bool DoGLBoardGestureRecognition(TLightSpot* pLightSpots, int &nLightSpotCount);

    /*
    @���ܣ�Windowsϵͳ������ʶ��
    @����:pLightSpots, ָ�����б��ָ��
          nLightSpotCount, ��߸���
    @����ֵ�����ݷ���ֵ�����Ƿ�������ݴ�����������¼�
    */
    bool DoWindowsGestureRecognition(const TLightSpot* pLightSpots, int nLightSpotCount, TContactInfo *penInfo, int &nPenCount);

    //@�� ��:�жϹ���Ƿ�����ڴ����ں�����
    //@�� ��:lightSpot, ������ݡ�
    //       CameraIndex, ������ߵľ�ͷ��ID�š�
    //       pMergeAreaIndex, ָ�򱣴��ں����������ڴ��ָ�룬�ڸô����ں���������Ҫ�жϵĹ�ߡ�
    //       bBeyondMergeArea, ��Խ�ں�������߽��־
    //@����ֵ:TRUE, ���ں����ڳ��֡�
    //       FALSE, δ���ں����ڳ���
    BOOL AppearInMergeArea(const TLightSpot& lightSpot, UINT CameraIndex, /*bool* bBeyondMergeArea,*/ UINT* pMergeAreaIndex = NULL);


    //@�� ��:�жϹ���Ƿ����ڵ��ֵ�����ͷ���ں����ڿ���
    //@�� ��:lightSpot, ������ݡ�
    //       CameraIndex, ������ߵľ�ͷ��ID�š�
    //@����ֵ:TRUE, �����ڵ��ֵ�����ͷ������
    //       FALSE, �ֵ����δ����
    //BOOL SeenInMergeAreaByBuddyCamera(const TLightSpot& lightSpot, UINT CameraIndex);
    BOOL SeenByBuddyCamera(const TLightSpot& spotTarget, UINT CameraIndex);

    BOOL SeenByBuddyCamera1(const TLightSpot& spotTarget, UINT CameraIndex);

    //@��  ��:�ж��ֵ�����Ƿ����ں����ڿ����˹��
    //@��  ��:��ǰ�����Id, ������ȷ���ֵ������id��
    //@����ֵ: 
    //         TRUE, �ֵ���������ں����ڿ����˹��
    //        FALSE, �ֵ���������ں�����û�з��ֹ��
    BOOL BuddyCameraFoundSpotInMergeArea(UINT CameraIndex);




protected:
    HANDLE m_hProcessThread;

    HANDLE m_hProcessThread1;
   
    //BOOL HasNeignborInSpotList(const POINT& ptPos, const TLightSpot* pSpotList, int nSpotCount);

    static ULONG _stdcall ProcessThreadProc(LPVOID lpCtx);

    //static ULONG _stdcall ProcessThreadProc1(LPVOID lpCtx);

    //FIFOԪ������״̬����
    static const int MAX_CAMERA_NUMBER = 6;


    //static const int STATUS_DATA_ALL_INVALID = 0x00;//������Ч��־    
    //static const int STATUS_DATA_ALL_VALID   = (1<< CAMERA_NUMBER) - 1;//����ȫ��Ч��־
    
    UINT m_uCameraCount;//����ͷ����

    //FIFOԪ�ؽṹ
    struct TSpotListGroup
    {
        TLightSpot aryLightSpots     [MAX_CAMERA_NUMBER][MAX_OBJ_NUMBER];//[����ͷ][����б�]��ά����
        int        aryLightSpotsCount[MAX_CAMERA_NUMBER]                ;//ÿ������ͷ��������׽���Ĺ�߸���

        TLightSpot aryLightSpots1[MAX_CAMERA_NUMBER][MAX_OBJ_NUMBER];//[����ͷ][����б�]��ά����
        int        aryLightSpotsCount1[MAX_CAMERA_NUMBER];//ÿ������ͷ��������׽���Ĺ�߸���
    };



    //����Ĺ���б�(
    TSpotListGroup    m_InputSpotListGroup;


    //��Ч�Ĺ���б�����
    //TSpotListGroup    m_ValidSpotListGroup;

    //<<added by toxuke@gmail.cim ,2014/12/25
    CRoundRobinQueue<TSpotListGroup, 16>  m_SpotListGroupFIFO;//
    //>>

  
    CComAutoCriticalSection m_csForSpotListGroupFIFO;//

    LONG m_lReferenceCount;//���ü�����


    HANDLE m_hWriteEvent;//д������¼�
    //HANDLE m_hWriteEvent1;//д������¼�

    HANDLE m_hExitEvent ;//�˳��¼�
    //HANDLE m_hExitEvent1;//�˳��¼�

    static const int SAMPLE_CORRELATION_INTERVAL = 48 ;//����ͷ����������������, ��λ:ms
	////��¼���У��ģʽ���ĵ㻹������У��
	E_CALIBRATE_MODEL m_eCalibrateModel;



    //�ں������ڵĹ�ߺϲ���
    CSpotMerger m_oSpotMerger;//

    static const int PEN_NUM = MAX_SUPPORT_TOUCH_COUNT;
    //���ܹ��ƥ��
    CSmartMatch<PEN_NUM> m_oSmartPenMatch;//

    //����HID�豸
    static CVirtualHID m_oVirtualHID;//  

    bool  m_bLastHIDOwnnerAfterGR;     //�ϴ�ʶ��󣬻�ȡVirtualHID����Ȩ���Ƿ��Ǳ����ʵ��
   // bool  m_bIsSmartPenReset;    

    int m_uid = 0;

    //<<debug 
    //���Ժ���
    void SimulateProcess();
    BOOL m_bSimulateMode;
    
    //debug>>

    CStrokeFilter<PEN_NUM>  m_oStrokFilter;//�ʼ�������
	CBezierInterpolator<PEN_NUM> m_oBezierInterpolator;//

    CBezierInterpolator<PEN_NUM> m_oBezierInterpolator1;//

    CInterpolateDispatcher<PEN_NUM> m_oInterpolateDispatcher;

    CInterpolateDispatcher<PEN_NUM> m_oInterpolateDispatcher1;

    //���ڴ������Ʊ�־
    BOOL m_bIsTriggeringGuesture;
	BOOL m_bEnableStrokeInterpolateTemp;

	double  m_dbSmoothCoefficient;

    BOOL m_bAutoMerge;//ʹ�ܴ����Զ��ں�

    wchar_t szTmp[1024] = { 0 };
};

///////////////////////////////////////////////////////////////////////////
//����ʶ��
extern CGestureEventGenerator g_oGLBoardGR;
extern CWindowsGestureRecognition g_oWinGR;
//extern bool g_oIsFingerTouchMode;           //ֻ������ָ����ģʽ�²ſ�������ʶ��