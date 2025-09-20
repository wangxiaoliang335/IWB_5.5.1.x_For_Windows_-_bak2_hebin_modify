#pragma once
//�Զ�У���ӿ�
class IAutoCalibratorInterface
{
public:
    //@����:��ʼУ��, ���ⲿģ�����
    //@����:
    //      hNotifyWnd, ��Ϣ֪ͨ������
    //      E_AutoCalibratePattern, У������ö��ֵ
    //      cBrightness, У��ʱ��������ƽ��ֵ
    //      ChangeCameraParamsProc, ��������ͷ�����Ļص�����
    //      lpCtx, ChangeCameraParamsProc�������Ĳ���
    //      clrGridHighlight, У��ͼ���и��������ʾ��ɫ
    //      bSaveInermediatFile, �Ƿ񱣴�У�����м���ͼƬ
    //      bRecordVideo���Ƿ��¼У�������е�ԭʼ��Ƶ
    //   
    //virtual BOOL StartCalibrating(HWND hNotifyWnd, E_AutoCalibratePattern eGridPattern=E_CALIBRATE_PATTERN_11_COLS, BYTE cBrightness = 50, CHANGECAMERAPARAMSPROC ChangeCameraParamsProc = NULL, LPVOID lpCtx = NULL, COLORREF clrGridHighlight=RGB(255,255,255),BOOL bSaveInermediatFile = FALSE ,BOOL bRecordVideo = FALSE)=0;


    //@����:��ʼУ��, ���ⲿģ�����
    //@����:autoCalibrateParams, �Զ�У������
    //      staticMaskingParams, ��̬���β���
     virtual BOOL StartCalibrating(const TAutoCalibrateParams& autoCalibrateParams, const TStaticMaskingParams& staticMaskingParams) = 0;


     //@����:��ʼ�Զ�����,���ⲿģ�����
     virtual BOOL StartMasking(const TAutoMaskingParams& autoMaskingParams, const TStaticMaskingParams& staticMaskingParams, int nSrcImageWidth, int nSrcImageHeight) = 0;



    //@����:����У��
    virtual void Finish() = 0;

    
    //@����:���Զ�У����ͶιͼƬ��������֮
    //@����ֵ:�쳣����FALSE
    virtual BOOL FeedImage(const CImageFrame* pGrayFrame) = 0;


     //@����:������Ļ����
    virtual int GetMonitorCount()const= 0;

	 //@����:��ȡָ����Ļ��У������
	 //@����:nMonitorID�� ��ʾ��Id
	 //@˵��:
	 //virtual  TMonitorCalibData GetMonitorCalibData(int nMonitorID) const = 0;


     //@����:��ȡ������Ļ��У������
     virtual  const TCalibData* GetCalibrateData() const = 0;


    virtual const CImageFrame& GetMaskFrame() const = 0;
    
    virtual const CImageFrame& GetScreenMap() const = 0;

    //@����:����У��������
    virtual HWND GetCalibrateHWnd()const=0;

    //@����:���õ��Լ���
    virtual void SetDebugLevel(ECalibDebugLevel eDebugLevel) = 0;

    //@����:������Ļ�ߴ�
    virtual SIZE GetScreenSize()const = 0;
    

   //@����:������Ļ�ߴ�
    virtual SIZE GetImageSize()const = 0;

    
    //@���ܣ������Զ�У����ʧ�ܵ��м���ͼƬ·��
    virtual  CAtlString GetCalibrateIntermediataDirName() = 0 ;


    //========���Ժ���========
    //@����:ͨ������AVI¼�񣬽���ģ��У��
    //
    virtual BOOL DoSimulateCalibrate(LPCTSTR lpszAVIFilePath, HWND hNotifyWnd, UINT uCx, UINT uCy, BYTE cBrightness, E_AutoCalibratePattern eGridsNumber)
    {
        return FALSE;
    }

    //@����:�鿴У��ͼ��
    //
    virtual void ViewCalibratePattern()
    {

    }

    //@����:�����Զ����ȵ���
    virtual void TestAutoBrightnessControl(CHANGECAMERAPARAMSPROC ChangeCameraParamsProc = NULL, LPVOID lpCtx = NULL)
    {

    }

    //@����:���ص�����¼���ļ�����·��
    virtual const TCHAR* GetDebugVideoFilePath()const = 0;
};