#pragma once
//@Date:2011/04/24
//@Author: toxuke@gmail.com

#ifdef ALGOLIB_EXPORTS
    #define ALGOLIB_API _declspec(dllexport)
#else
    #define ALGOLIB_API _declspec(dllimport)
#endif

#include <vector>
//Part 3. typedef std::vector<POINT> Points;
#include "CommonDefine.h"
#include "ImageFrame.h"
#include "BitFrame.h"
#include <atlstr.H>
#include "CameraSpecs.h"


#define ERR_SOFTKEY_VERSION  (-8)

#define ERR_SOFTKEY_APP_TYPE (-9)

#define ERR_START_CALIBRATING_FAILED (-10)

#define ERR_INVALID_PARAMETER (-11)



//Used In Part 1 and Part 2.
//
//================================================
//Part 1. �Զ�У��ͼ��ģʽ
enum E_AutoCalibratePattern
{

    E_CALIBRATE_PATTERN_5_COLS =  0,
    E_CALIBRATE_PATTERN_6_COLS  = 1,
    E_CALIBRATE_PATTERN_7_COLS  = 2,
    E_CALIBRATE_PATTERN_8_COLS  = 3,
    E_CALIBRATE_PATTERN_9_COLS  = 4,
    E_CALIBRATE_PATTERN_10_COLS = 5,
    E_CALIBRATE_PATTERN_11_COLS = 6,
    E_CALIBRATE_PATTERN_12_COLS = 7,
    E_CALIBRATE_PATTERN_13_COLS = 8,
    E_CALIBRATE_PATTERN_14_COLS = 9,
    E_CALIBRATE_PATTERN_15_COLS = 10,
    E_CALIBRATE_PATTERN_16_COLS = 11,
    E_CALIBRATE_PATTERN_17_COLS = 12,
    //E_CALIBRATE_SEARCH_SCREEN_AREA_ONLY = 9//ֻ������Ļ����ģʽ
};

inline int GetPatternCols(E_AutoCalibratePattern pattern)
{
    return  9 + int(pattern);
}

inline E_AutoCalibratePattern GetAutoCalibratePattern(int cols)
{

    return E_AutoCalibratePattern(cols - 5);

}

//�Զ�У���������
enum EAutoCalibrateError
{
    E_AUTO_CALIBRATOR_OK,
    E_AUTO_CALIBRATOR_ERROR_DETECT_DISPLAY_AREA_FAILED,//δ��⵽��Ļ����
    E_AUTO_CALIBRATOR_ERROR_CORNER_DETECT_FAILED,//�ǵ���ʧ��
    E_AUTO_CALIBRATOR_ERROR_CORNER_MATCH_FAILED,//�ǵ�ƥ��ʧ��
    E_AUTO_CALIBRATOR_ERROR_UNDEF,//δ֪����    

    E_AUTO_MASK_ERROR_SEARCH_UPPER_SCREEN_BORDER_FAILED,
    E_AUTO_MASK_ERROR_SEARCH_LOWER_SCREEN_BORDER_FAILED,
    E_AUTO_CALIBRATOR_ERROR_NOT_FOUND_DEVICE,
};


//�Զ�У���������
enum EAutoMaskingError
{
    E_AUTO_MASKING_OK,
    E_AUTO_MASKING_ERROR_DETECT_DISPLAY_AREA_FAILED,//δ��⵽��Ļ����
    E_AUTO_MASKING_ERROR_UNDEF,//δ֪����
    E_AUTO_MASKING_NOT_FOUND_DEVICE,

};



//У�����Լ���
enum ECalibDebugLevel
{
    E_CALIB_DEBUG_LEVEL_CONCISE,//������
    E_CALIB_DEBUG_LEVEL_DEBUG,  //�������
    E_CALIB_DEBUG_LEVEL_VERBOSE, //��ϸ���
};


 //�Զ�У����Ԫ����
struct TCalibCoordPair
{
    POINT    ptScreenCoord  ;//
    TPoint2D pt2DImageCoord ;//�߾���
   
    TCalibCoordPair()
    {
        ptScreenCoord.x = 0;
        ptScreenCoord.y = 0;

        pt2DImageCoord.d[0] = 0.0;
        pt2DImageCoord.d[1] = 0.0;

   }
};

//��ĻУ������ϵ��
struct TMonitorCalibCoefs
{
    RECT rcMonitor;
    std::vector<double> calibCoefs;
    TMonitorCalibCoefs()
    {
        rcMonitor.left   = 0;
        rcMonitor.right  = 0;
        rcMonitor.top    = 0;
        rcMonitor.bottom = 0;
    }


};

//��λУ��ģ��
enum E_CALIBRATE_MODEL
{
    E_CALIBRATE_MODEL_GENERICAL_CAMERA     = 0,//ͨ�����ģ��
    E_CALIBRATE_MODEL_4_POINST_PERSPECTIVE = 1,//4��͸��ģ��
    E_CALIBRATE_MODEL_COUNT = 2                //У��ģ����Ŀ
};

//У������
enum E_CALIBRATE_TYPE
{
    E_CALIBRATE_TYPE_MANUAL = 0,//�ֶ�У��
    E_CALIBRATE_TYPE_AUTO   =1, //�Զ�У��
};

//������Ļ��У������ϵ��
typedef std::vector<TMonitorCalibCoefs>  ALL_CALIB_COEFS;
struct TCalibParams
{
    TCalibParams()
    {
        eCalibType = E_CALIBRATE_TYPE_AUTO;
        szImage.cx = 640;
        szImage.cy = 480;

        eCalibrateModel = E_CALIBRATE_MODEL_GENERICAL_CAMERA;


        bUsingScreenPhysicalDimensions = FALSE;//ʹ����Ļ����ߴ��־
        nScreenWidthInmm  = 0;//��Ļ������
        nScreenHeightInmm = 0;//��Ļ����߶�

    }

    E_CALIBRATE_MODEL eCalibrateModel;//��λУ��ʹ�õ�ģ��

    E_CALIBRATE_TYPE eCalibType;//�ֶ�/�Զ�У��
    SIZE szImage;
    ALL_CALIB_COEFS allCalibCoefs;

    BOOL bUsingScreenPhysicalDimensions;//ʹ����Ļ����ߴ��־
    LONG nScreenWidthInmm;//��Ļ������
    LONG nScreenHeightInmm;//��Ļ����߶�

};

struct TMonitorCalibData
{
    int radius;//У��Բ�İ뾶
    RECT rcMonitor;//��ʾ����������
    std::vector<TCalibCoordPair> calibData;

    TMonitorCalibData()
    {
        radius = 0;
        rcMonitor.left   = 0;
        rcMonitor.top    = 0;
        rcMonitor.right  = 0;
        rcMonitor.bottom = 0;
    }
};

typedef  std::vector<TMonitorCalibData> ALL_MOITOR_CALIB_DATA ;

struct TCalibData
{

    TCalibData()
    {
        eCalibrateModel = E_CALIBRATE_MODEL_GENERICAL_CAMERA;
        eCalibType = E_CALIBRATE_TYPE_AUTO;
        szImage.cx = 1920;
        szImage.cy = 1080;
        lpCtx = NULL;//

        bUsingScreenPhysicalDimensions = false;
        nScreenWidthInmm  = 0;
        nScreenHeightInmm = 0;
    }

    E_CALIBRATE_MODEL eCalibrateModel;//��λУ��ʹ�õ�ģ��

    E_CALIBRATE_TYPE eCalibType;
    SIZE szImage;

    ALL_MOITOR_CALIB_DATA allMonitorCalibData;
    LPVOID lpCtx;//�����ĸ�������

    BOOL bUsingScreenPhysicalDimensions;//ʹ����Ļ����ߴ��־
    LONG nScreenWidthInmm ;//��Ļ������
    LONG nScreenHeightInmm;//��Ļ����߶�


};

enum EChangeCalibCameraParams
{
    E_CAMERA_SHARPNESS          ,//��������ͷ��������, param1����ָ��������
    E_CAMERA_SHARPNESS_MAX      ,//��������ͷ�����������
    E_CAMERA_SHARPNESS_MIN      ,//��������ͷ����С������
    
    E_CAMERA_BRIGHTNESS_SET     ,//��������ͷ����, param1����ָ������
    E_CAMERA_BRIGHTNESS_INCREASE,//��������ͷ������
    E_CAMERA_BRIGHTNESS_DECREASE,//��������ͷ������

    E_CAMERA_CONTRAST_MAX       ,//��������ͷ�����Աȶ�
    E_CAMERA_CONTRAST_DEFAULT   ,//��������ͷ��ȱʡ�Աȶ�
    E_CAMERA_CONTRAST           ,//��������ͷ�Աȶ�
    E_CAMERA_AUTO_CALIBRATE     ,//�Զ�У��ʱ���������
    E_CAMERA_AUTO_MASK          ,//�Զ�����ʱ���������
};


//�Զ�У��ʱ,������������ͷ��Ƶ�����Ļص�����
typedef BOOL (*CHANGECAMERAPARAMSPROC)(EChangeCalibCameraParams eCtrlMode, LPVOID lpCtx, BYTE param1, BYTE param2);

//��̬����ǰ�Ļص�������У��ģ�����ͨ��������������Ĳ������˹�Ƭ�ıպϡ�
typedef BOOL   (CALLBACK *PRE_STATIC_MASKING_ROUTINE)(LPVOID lpCtx);



//�Զ�У��ʱָ���Ļ������
struct AutoCalibrateImageParams
{
    AutoCalibrateImageParams()
    {
        autoCalibrateExpectedBrightness = 0x80;
        autoCalibrateHilightGray = 255;
        autoCalibrateSpeed = 1;
        videoDislayDelay = 0;

    }
    BYTE autoCalibrateExpectedBrightness; //�Զ�У�������Զ�����ʱ������ƽ����������(��Χ0~255)
    BYTE autoCalibrateHilightGray       ; //�Զ�У��ͼ���и�����ĻҶ�ֵ(0~255)
    BYTE autoCalibrateSpeed             ; //�Զ�У���ٶ�(1~10), 1��죬10����
    BYTE videoDislayDelay               ;//��Ƶ��ʾ�ӳ�
};

struct AutoCalibrateParams
{
    AutoCalibrateImageParams  autoCalibrateImageParams;
    TVideoProcAmpProperty     videoprocampproperty;
};

//��������б�, ÿһ�������Ӧһ��У�����ԵĻ������
//typedef std::vector<AutoCalibrateImageParams> AutoCalibrateImageParamsList;

typedef std::vector<AutoCalibrateParams> AutoCalibrateParamsList;


//@����:��Ļ��Ϣ
struct TScreenInfo
{
    RECT rcArea;
};
//@����:������չ����ʾ����Ϣ
//�Զ�У������
struct TAutoCalibrateParams
{
    //======У������======
    HWND hNotifyWnd                              ;//��Ϣ֪ͨ������
    E_AutoCalibratePattern ePattern              ;//У������ö��ֵ    
    CHANGECAMERAPARAMSPROC ChangeCameraParamsProc;//��������ͷ�����Ļص�����
    LPVOID  lpCtx                                ;//ChangeCameraParamsProc�Ļص��������Ĳ���    
    ECalibDebugLevel eDebugLevel                 ;//���Լ���
    BOOL bSaveInermediatFile                     ;//�Ƿ񱣴�У�����м���ͼƬ
    BOOL bRecordVideo                            ;//�Ƿ��¼У�������е�ԭʼ��Ƶ
    std::vector<TScreenInfo> vecScreenInfos      ;//��������ʾ����Ϣ


    //BYTE cAvgBrightness                        ;//У��ʱ��ƽ������
    //COLORREF clrGridHighlight                  ;//У��ͼ���и��������ɫֵ

//    AutoCalibrateImageParamsList imageParamsList ;//��������б�,

    AutoCalibrateParamsList   autocalibrateparamslist;   //�Զ�У�������б�

	BOOL   bEnableOnlineScreenArea;

    std::vector<POINT> vecOnlineAreaVertices;//��������Ķ��㡣

};


//�Զ����β���
struct TAutoMaskingParams
{
    //======У������======
    HWND hNotifyWnd                              ;//��Ϣ֪ͨ������
    CHANGECAMERAPARAMSPROC ChangeCameraParamsProc;//��������ͷ�����Ļص�����
    LPVOID  lpCtx                                ;//ChangeCameraParamsProc�Ļص��������Ĳ���    
    ECalibDebugLevel eDebugLevel                 ;//���Լ���
    BOOL bSaveInermediatFile                     ;//�Ƿ񱣴�У�����м���ͼƬ
    BOOL bRecordVideo                            ;//�Ƿ��¼У�������е�ԭʼ��Ƶ
    std::vector<TScreenInfo> vecScreenInfos       ;//��������ʾ����Ϣ


    //BYTE cAvgBrightness                        ;//У��ʱ��ƽ������
    //COLORREF clrGridHighlight                  ;//У��ͼ���и��������ɫֵ

//    AutoCalibrateImageParamsList imageParamsList ;//��������б�,
    AutoCalibrateParamsList       autocalibrateparamslist;   //�Զ�У�������б�

    BOOL                    bDoStaticMaskingOnly ;//ֻ����̬���α�־

};



//��̬���β���
struct TStaticMaskingParams
{
    //=======��̬���β���=======
    PRE_STATIC_MASKING_ROUTINE fpPreStaticMaskingProc ;//��̬����ǰ�ص�����    
    LPVOID lpPreStaticMaskingCtx                      ;//��̬����ǰ�ص������������Ĳ���
    BYTE   cStaticMaskThreshold                       ;//��̬��������
    int    nMaskEroseSize                             ;//����ͼ��ʴ�ߴ硣

    TStaticMaskingParams()
    {
        fpPreStaticMaskingProc = NULL;
        lpPreStaticMaskingCtx  = NULL;
        cStaticMaskThreshold   = 180;
        nMaskEroseSize         = 10;
    }

};



class CAutoCalibratorImpl2;
class ALGOLIB_API CAutoCalibrator
{
public:
    CAutoCalibrator();

    ~CAutoCalibrator();
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
   // HRESULT StartCalibrating(HWND hNotifyWnd, E_AutoCalibratePattern eGridPattern=E_CALIBRATE_PATTERN_11_COLS, BYTE cBrightness = 50, CHANGECAMERAPARAMSPROC ChangeCameraParamsProc = NULL, LPVOID lpCtx = NULL, COLORREF clrGridHighlight=RGB(255,255,255),ECalibDebugLevel eDebugLevel=E_CALIB_DEBUG_LEVEL_CONCISE, BOOL bSaveInermediatFile = FALSE ,BOOL bRecordVideo = FALSE);
    
    //@����:��ʼУ��, ���ⲿģ�����
    //@����:autoCalibrateParams, �Զ�У������
    //      staticMaskingParam, ��̬���β���
     BOOL StartCalibrating(const TAutoCalibrateParams& autoCalibrateParams, const TStaticMaskingParams& staticMaskingParams);

     //@����:��ʼ�Զ�����,���ⲿģ�����
     BOOL StartMasking(const TAutoMaskingParams& autoMaskingParams, const TStaticMaskingParams& staticMaskingParams, int nSrcWidth, int nSrcHeight);

    
    ////@����:�ⲿ�������Զ�У����, ����������ͷ��ͼƬ֡�����Զ�У��ģ��, �����Զ�У��״̬�������С�
    ////@����:pYUVFrame, ָ��YUY2��ʽ��λͼ֡
    ////@����ֵ:�ɹ�����TRUE
    ////        ʧ�ܷ���FALSE
    //BOOL ProcessImage(const CImageFrame* pYUVFrame);

    //@����:�����Զ�У��,
    //@˵��:
    //void EndCalibrating();
    

     //@����:��ȡ�Զ�У�����ɵļ�����ĻӰ�������ͼ
     //@˵��:�Զ�У���㷨���Զ�ʶ����������Ļ������ͷͼ������ռ������
    //       ����ͼ��һ���ڰ׶�ֵͼ, ֵΪ0XFF�������Ӧ��Ļ����0x00Ϊ
    //       ��Ҫ�ڱ�����
     const CImageFrame& GetMaskFrame()const;
	 
	 ///@����:��ȡ����Զ����ε�����ͼ
	 ///vera_zhao 2020.2.20
	 const CImageFrame& GetMaskFinderFrame(CImageFrame& maskframe)const;


      //@����:���ع��λ�õ���Ļ��ŵ�ӳ������,�Ծ�����ߴ����ĸ���Ļ��
      const CImageFrame& GetScreenMap()const;


    //@����:������Ļ����
    int GetMonitorCount()const;

     //@����:��ȡָ����Ļ��У������
     //@����:nMonitorID�� ��ʾ��Id
     //@˵��:
     //const TMonitorCalibData& GetMonitorCalibData(int nMonitorID) const;


    //@����:��ȡУ������ľ��
    //@˵��:�ⲿ�����ȡ��У��ͼƬ֡ͨ��������ϢWM_MANUAL_CALIBRATE_DATA֪ͨ��У������, У������
    //      �������ͽ�"У��״̬��", ����״̬�������С�
    HWND GetCalibrateHWnd()const;

    //@����:��ȡУ��ʱ����Ļ�ߴ�
    //@˵��:��ģ��У��ʱ,У��ʱ����Ļ�ߴ�ͼ������ǰ��Ļ�ߴ���ܲ�ͬ��
     SIZE GetScreenSize()const;

     
    //@����:��ȡУ��ʱ����Ƶͼ��ߴ�
    //@˵��:
    SIZE GetImageSize()const;


    CAtlString  GetCalibrateIntermediataDirName();
    //���Ժ���
    //@����:ͨ������У��������¼�Ƶ�¼���ļ�, ����У������, �Ա��ڵ���У���㷨��
    //@����:lpszAVIFilePath, ¼���µ�AVI�ļ�·��
    //      hNotifyWnd,  ��У��������, ����У�������ϢWM_AUTO_CALIBRATE_DONE�Ĵ�������
    //      eGridNumber, �Զ�У��ʱ, ��ʾ�����̸�У��ͼ����ģʽ

    BOOL DoSimulateCalibrate(LPCTSTR lpszAVIFilePath, HWND hNotifyWnd,  UINT uCx, UINT uCy, BYTE cBrightness, E_AutoCalibratePattern eGridsNumber=E_CALIBRATE_PATTERN_11_COLS,  ECalibDebugLevel eDebugLevel=E_CALIB_DEBUG_LEVEL_DEBUG);


    //���Ժ���:���������Զ����ڹ���
    void TestAutoBrightnessControl(CHANGECAMERAPARAMSPROC ChangeCameraParamsProc = NULL, LPVOID lpCtx = NULL);

    //���Ժ���:�鿴У��ͼ��        
    void ViewCalibratePattern();


     //@����:��ȡ������Ļ��У������
      const TCalibData* GetCalibrateData() const;

    //@����:���ص�����¼���ļ�����·��
     const TCHAR* GetDebugVideoFilePath()const;

     void OnDeviceMissing();



protected:
    CAutoCalibratorImpl2* m_pAutoCalibratorImpl;//ָ���Զ�У�������ʵ��

};


//================================================
//�жϹ���Ǵ�������ƫ���Ǵ�������ƫ��
//���ƫ�����õĻ�����ô�������Ҫ���ָ�ģ����ƫ��û�б����õĻ�������ǲ���Ҫ���ָ�ġ�
enum E_SpotDirection
{
    E_SPOT_NODIRECTION,
    E_SPOT_LEFTDIRECTION,
    E_SPOT_RIGHTDIRECTION,
};

struct TObjDimension
{
    RECT rcArea;
    int radius;
};

struct  TInvalidSpotList
{
    RECT  rOriginalArea;    //�������Ч���
    RECT  rExpansionInvalidArea ;    //��չ����Ч���
    int   nCount ;          //����
    bool  bMatch   ;   //��Ч����Ч�Ƿ�ƥ�䡣�����ƥ��ģ�count��2���������ƥ��Ļ���������1
    bool  bInvalid ;   //�ظ��ı��Ϊ��Ч�ġ�
    E_SpotDirection   eSplitDirection;
    int   nSplitNum;  //���ָ�ĸ���

};
//struct TMatchSpot
//{
//	RECT  rInvalidArea;
//	
//};


//Part 2. ������㷨
//Ŀ����Ϣ
struct TBlobObject
{
    //int nObjectId      ;//Ŀ��Id
    unsigned int mass    ;//����
    unsigned int mx      ;//��x�������
    unsigned int my      ;//��y�������

    POINT GPos         ;//��������
    RECT rcArea        ;//����
    BOOL bIsValid      ;//��ȷ��־
    BOOL bReferenced   ;//�Ƿ�������Ϊ�ο��㡣//��С�Ĺ�ߺ��ⲿ�Ĺ�߲���������Ϊ��Ч��ߵĲο�
    int  objToInvalidNumber;   //��Ч����б��Ӧ
    E_SpotDirection   eSplitDirection;  //���ָ�ķ���
    int   nSplitNum   ;  //���ָ�ĸ���

    bool ShootCalibrateing = false;

    int gunId = -1; //���������ǹ֧Id

    //
    //LONG ActualArea    ;//��ߵ�ʵ�����    
    //LONG CalculateArea ;//��������=
    //POINT debugPtContourCross[2];//

    bool operator < (TBlobObject& right)
    {
        return (mass < right.mass);
    }

    bool operator <= (TBlobObject& right)
    {
        return (mass <= right.mass);
    }


    bool operator > (TBlobObject& right)
    {
        return (mass > right.mass);
    }

    bool operator >= (TBlobObject& right)
    {
        return (mass >= right.mass);
    }

};

#define MAX_OBJ_NUMBER 256

template<int MAX_OBJECT_COUNT= MAX_OBJ_NUMBER,int MAX_IMAGE_WIDTH = 1920, int MAX_IMAGE_HEIGHT = 1080>
class CConnectedComponentScanlineAlogrithm;
class m_pBitFrame;
class m_YUY2Frame;
class ALGOLIB_API CBlobDetector
{
public:

    CBlobDetector();

    ~CBlobDetector();

    //@����:���λͼ�����ȴ���ָ������ֵ�Ĳ���������������Ӿ���, ���������������ġ�
    //@����:frame, YUY2��ʽ��λͼ֡
    //      YThreshold, ������������
    //      pClipArea, ��ѡ����, ���βü�����ߴ�
    //@����:��⵽��Ŀ��ĸ���
    //@˵��:
    //UINT operator() (const CYUY2Frame& YUY2Frame, BYTE YThreshold, CYUY2Frame& StatisticalFrame,CYUY2Frame& DynamicFrame, UINT FixedSpotTime,const RECT* pClipArea=NULL/* const CBitFrame** ppBitFrame=NULL*/);
    //UINT CBlobDetector::operator() (const CYUY2Frame& YUY2Frame, BYTE YThreshold, CImageFrame& StatisticalFrame, const CYUY2Frame& DynamicMaskFrame, int StatisticStep = 1);
    UINT CBlobDetector::operator() (const CImageFrame& GrayFrame, BYTE YThreshold, CImageFrame& StatisticalFrame, const CImageFrame& DynamicMaskFrame, int StatisticStep = 1, BYTE YStatisicThreshold = 0,bool bDesktopMode= true);

    //@����:��ȡĿ���б�
    const TBlobObject* GetObjs()const;
    //@���ܣ����Ŀ���б����Ǿ�̬��
    TBlobObject* GetObjs();

    //	void AnalysisFixedBolb(CYUY2Frame& YUY2Frame);
    const CImageFrame* GetBinarizedImage()const;
protected:

    CConnectedComponentScanlineAlogrithm<>* m_pBlobScanAlgo;
    CBitFrame*  m_pBitFrame;
    //CYUY2Frame* m_pYUY2Frame;
    CImageFrame*  m_pGrayFrame;
    unsigned  int m_nFrameCount;
    

};
//====================================================
//struct TBolbInfo
//{
//	RECT rcArea          ;        //����
//	POINT Gpt            ;        //��������
//	DWORD dwStartTime    ;        //�ɼ��������ߵ�ʱ��
//	BOOL  bSMatched      ;        //ƥ���״̬
//};


//====================================================
//Part 3.����У���㷨
typedef std::vector<POINT> Points;

//
//@����:��������ͷͼ�����굽��Ļ�����ӳ��ĵ�Ӧ����(Homography Matrix),
//@����:screenPts ,����, ��Ļ����
//      imagePts  ,����, ͼ������
//      screenSize,����, ��Ļ�ߴ�
//      imageSize ,����, ͼ��ߴ�
//      mapMatrix ,���,ͼ��ӳ�����, ��ߴ�=sizeof(POINT)*IMAGE_WIDTH*IAMGE_HEIGHT.
//      lpClipRect,���,������Ļ������Ӿ���
//@���:�������
//      0:�ɹ�, ����:ʧ�ܡ�
//      ������   ����
//        -1,����������,������ܹ��ӿڿ�
//         ...
//        -7, ����������,������ܹ��ӿڿ�
//        -10,�������ݴ���
//
//
//@˵��:screenPts[i] ��imagePts[i](i=1,...,N)��һ��ƥ��㡣
//      mapMatrix[i]�е������Ѿ���һ����0~65535��Χ��  
//extern "C" ALGOLIB_API LONG CoordCalibrate(const Points& screenPts, const Points& imagePts, const SIZE& screenSize, const SIZE& imageSize, SHORT_POINT* mapMatrix, RECT* lpClipRect);

class ICalibrate;

class  ALGOLIB_API CalibrateAlgo
{
public:

        CalibrateAlgo();

        ~CalibrateAlgo();

        //@����:����У������
        //@����:data ,����, У������
       BOOL CaclCalibrationParams(const TCalibData& data, BOOL bDebug = FALSE);

       //@����:��������ͷ����Ƶ�������ó���Ļ����
       BOOL GetScreenCoord(const TPoint2D& ptVideo, TPoint2D* ptScreen, int nMonitorId = 0,BOOL bWithoutAutoCalibCompensate = FALSE, TCameraDebugData* pDebugOutput = NULL);
       
       /*
       //@����:��������ͷ����Ƶ�������ó���Ļ����
       //@����:pImgPts, ָ���ߵ��������������ָ��
       //      prcBounds, ָ���ߵ������������ָ��
       //      nPtNumber, ��߸���
       //      pScreenPts, ָ�򱣴���Ļ���������ָ��
       //      nMonitorId, ��ĻId,��һ����ͷ�������Ļʱ����Ч��
       BOOL GetScreenCoord_Aug(const POINT* pImgPts,  const RECT* prcBounds, int nPtNumber, POINT* pScreenPts, int nMonitorId = 0);
       */

       //@����:��3D��������ӳ��Ϊ2D���ͼ������
       BOOL MapWorldToImage(const TPoint3D* ptWorld, int nPtCount, TPoint2D* ptImage, int nMonitorId = 0);

       /*
       //@����:��������ͷ����Ƶ�������ó���Ļ����
       //@����:pCentroid, ��ߵ�����
       //      pContourPoints, ָ���������������
       //      nContourPointCount, ������������
       //      pScreenPts, ָ�򱣴���Ļ������ڴ��ַ��ָ��
       //      nMonitorId, ��ĻId,��һ����ͷ�������Ļʱ����Ч��
       BOOL GetScreenCoord_WithLightSpotContour(const POINT& pCentroid,  const POINT* pContourPoints, int nContourPointCount, POINT* pScreenPts, int nMonitorId = 0, POINT* debugPtContourCross=NULL);
       */

    //@����:���þ�ͷ�������
    void SetLensSpecification(const TLensSpecification& lensSpec);

    //@����:����CMOSоƬ�������
    void SetCMOSChipSpecification(const TCMOSChipSpecification& cmosChipSpec);

    //@����:�����Զ�У����������
    void SetAutoCalibCompCoefs(const TAutoCalibCompCoefs& autoCalibCompCoefs);

    //@����:���þ�ͷ����֪���ڲ������ͶԳƻ������
    void SetLensInternalAndSymmmetricDistortParams(const TInternalAndSymmetricDistortParams* pParams);


     //@����:����У�����̲���
     const TCalibParams* GetCalibParams()const;

      //@����:����У�����̲���
      void SetCalibParams(const TCalibParams& param);

      //@����:����ָ�����͵�У��ʵ��
      void GenCalibratorInst(E_CALIBRATE_MODEL eCalibrateModel);
protected:
    void CreateCalibrateInst(E_CALIBRATE_MODEL eCalibrateModel);
    ICalibrate* m_pCalibrateInst;
    CComAutoCriticalSection* m_pcsForParam;//���������̰߳�ȫ�Ĺؼ���

};

//====================================================
#include "NewFrameSink.h"
//Part 4. ��Ļ�������
class ALGOLIB_API CScreenAreaDetector
{
public:
    //@�� ��:�������,ͬʱ��ʾȫ������
    //@�� ��:hNotifyWnd, ����������֪ͨ�Ĵ�����
    //@����ֵ;	
    BOOL StartDetect(HWND hNotifyWnd);

    //@��  ��:������⹦��,
    //@��  ��:
    //@����ָ:
    BOOL End();


    //@����:������Ļ��������λͼ
    const CImageFrame& GetMaskFrame()const;

    operator INewFrameEventSink*();
    operator INewFrameEventSink&();

};


//====================================================
//Part 5. ���Ŀ¼���ú���
extern "C" ALGOLIB_API void ALGOAPI_SetIntermediateDir(const TCHAR* szIntermediateDir);
extern "C" ALGOLIB_API const TCHAR* ALGOAPI_GetIntermediateDir();