#pragma once

DEFINE_GUID (CLSID_MotionDetectFilter, 0x6378695c, 0x478e, 0x4096, 0xa7, 0x62, 0xbd, 0xce, 0xfd, 0x76, 0x3d, 0x47);
struct YUVColor
{
    BYTE Y;
    BYTE U;
    BYTE V;
};
class CPenPosDetector;
class CVideoPlayer;
class CIWBSensor;
#include "../AlgoLib/AutoCalibrator/AutoBrightnessRegulator.h"

class CInterceptFilter:
    public CTransformFilter
{


public:
    //CInterceptFilter(CPenPosDetector* pPenPosDetector = NULL,CVideoPlayer* pVideoPlayer = NULL);

    CInterceptFilter(CIWBSensor* pSensor = NULL);

    ~CInterceptFilter(void);

    void AttachPenPosDetector(CPenPosDetector* pPenPosDetector);
    void AttachVideoPlayer(CVideoPlayer* pPlayer);


    //The folloing 4 virtual function must be implemented
    // check if you can support mtIn
    virtual HRESULT CheckInputType(const CMediaType* mtIn) ;

    // check if you can support the transform from this input to this output
    virtual HRESULT CheckTransform(const CMediaType* mtIn, const CMediaType* mtOut) ;

    // this goes in the factory template table to create new instances
    // static CCOMObject * CreateInstance(LPUNKNOWN, HRESULT *);

    // call the SetProperties function with appropriate arguments
    virtual HRESULT DecideBufferSize(
        IMemAllocator * pAllocator,
        ALLOCATOR_PROPERTIES *pprop) ;

    // override to suggest OUTPUT pin media types
    virtual HRESULT GetMediaType(int iPosition, CMediaType *pMediaType) ;


    virtual HRESULT Transform(IMediaSample * pIn, IMediaSample *pOut);

    virtual CBasePin * CInterceptFilter::GetPin(int n);

    BOOL StartDetect(HWND hDisplayWnd, int nSrcImageWidth, int nSrcImageHeight);
    BOOL StopDetect ();

    void ViewMonoImage (HWND hWnd);


    //���ý���֪ͨ��Ϣ�Ĵ���
    void SetNotifyWindow(HWND hWnd, UINT uMsg);


    const CPenPosDetector* GetPenPosDetector()const;
    CPenPosDetector* GetPenPosDetector();


    const CVideoPlayer* GetVideoPlayer()const;
    CVideoPlayer* GetVideoPlayer();



    //void DrawManualScreenArea(BYTE* pYUV2Canvas);

    //@����:ʹ���ֶ�ָ������Ļ���������������
    //void MaskFilterData(BYTE* pSrcData, LONG srcSize,const CYUY2Frame& mask );
    void MaskFilterData(BYTE* pSrcData, LONG srcSize,const CImageFrame& mask );


    //¼����Թ���, 2012-06-20, by toxuke@gmail.com
    //@����:�ж��Ƿ�����¼��
    BOOL IsRecording() const;

    //@����:��ʼ¼��
    BOOL StartRecording(LPCTSTR lpszFilePath = NULL);

    //@����:ֹͣ¼��
    BOOL StopRecording();


    //@����:��AVI�ļ���������Դ
    BOOL  InputFromAVIFile(LPCTSTR aviFilePath);

    void AddEventSink(INewFrameEventSink* pSink);

    void RemoveSink(INewFrameEventSink* pSink);

    //@����:֪ͨ�¼���
    //void NotifySink(CYUY2Frame* pFrame);
     void NotifySink(CImageFrame* pFrame);


    //@����:ʹ�ܾ�̬���Ρ�
//    void EnableStaticMasking(BOOL bMasking);

    //@����:���ؾ�̬����״̬
//    BOOL IsStaticMasking()const;

    //@����:ʹ�ܻ��������Զ�����
    void EnableBrightnessAutoRegulating(BOOL bEnable);

    //@����:���û����ƽ������
    //@����:brightness:����ֵ
    void SetImageAverageBrightness(BYTE brightness);
	void SetAutoCalibrateParamsIndex(BYTE nIndex);

    void CaptureImage();

	void  SetFrameSkipCount(int nFrameskipcount);

	void  SetStartDrawMaskFrame(bool Start) { m_bStartDrawOnlineScreenArea= Start; }
	void  SetDrawMovePt(CPoint &point) { m_PtMove = point; }

private:
    //<<2015/8/18
    //���ȿ��ƺ���
    static bool BrightnessControl(bool bInc, BYTE absDiff, BYTE nIndex, LPVOID lpCtxData);

    //@����:ʵ��ִ�����ȿ���
    void DoAutoBrightnessControl(const BYTE* YUY2Frame, int nWidth, int nHeight);
    //>>

    //@����:����ARGBͼƬ֡,��ʹ������ͼ
    void UpdateARGBFrameWithoutMask();

    //@����:����ARGBͼƬ֡,ʹ������ͼ
    void UpdateARGBFrameWithMask();

protected:
    CPenPosDetector* m_pPenPosDetector;//��߼�����
    CVideoPlayer*    m_pVideoPlayer   ;//��Ƶ���Ŷ���
    CIWBSensor*      m_pSensor        ;//����������

    int             m_nRawImageWidth;
    int             m_nRawImageHeight;
    //��ʧ֡������
    CLostFrameCountDetector m_oLostFrameDetector;

    //֡�������
    CFPSDetector m_oFpsDetector;


    //¼����Թ���, 2012-06-20, by toxuke@gmail.com
    BOOL       m_bIsRecording;//����¼���־
    CAviWriter m_oAviRecorder;//¼��¼����


    BOOL       m_bSrcFromAVI ;//��ƵԴ����AVI
    CAviReader m_oAviInput   ;//¼���ȡ��



    std::map<INewFrameEventSink*, INewFrameEventSink*> m_mapEventSinks;
    CComAutoCriticalSection m_csEventSinks;

    CMJPG_Decoder m_oMJPGDecoder;//MJPG������


//    CYUY2Frame m_YUY2SrcFrame;
    CImageFrame m_MJPGFrame      ;
    CImageFrame m_GraySrcFrame   ;// 
    CBRGAFrame  m_BGRAFrame      ;//��ʾʱʹ�õ�ARGB��ʽ
    int         m_nDisplayCounter;//��ʾ����������
    DWORD       m_dwImageType    ;//ͼ������

    //<<2015/8/18 ��ʾ������Զ����ȵ��ڣ�ͬ�Զ�У������һ�¡�
    CAutoBrightnessRegulator<BYTE> m_oAutoBrightnessRegulator;

    //ʹ�������Զ����ڱ�־
    BOOL m_bEnableBrightnessAutoRegulating;


    BOOL m_bCaptureImage;
	
	int     m_nFrameSkipCount;
	bool    m_bStartDrawOnlineScreenArea;
	CPoint  m_PtMove;

    wchar_t szTmp[256] = { 0 };

    BOOL m_bIsDetecting;
};