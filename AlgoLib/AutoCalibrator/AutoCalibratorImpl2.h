#pragma once
#include <atlwin.h>
#include <Mf2StdGf.h>
#include <MsgDefine.h>
#include <ImageFrame.h>
#include <BitFrame.h>
#include <ImageDib.h>
#include <DIB/Dib.h>
#include <AVI/AVIWriter.h>
#include <AVI/AVIReader.h>
#include <limits>
#include <vector>

#include <ImageDebug.h>

#include <assert.h>
#include <AlgoLib.h>
#include <BlobDetectorImpl.h>
//#include <RobustCornerDetect.h>

#include "CalibratePattern.h"
#include "Histogram.h"
#include <ImageFrameAPI.h>
#include  <MyAPI.h>
//#include "../MorphologyAlgo/inc/MorphologyAlgo.h"
#include "AutoCalibratorInterface.h"
#include "AutoCalibrateWnd.h"
#include "GrayImageBinarize.h"
#include "CrossDetector.h"
#include "AutoBrightnessRegulator.h"
#include "DispMonitorFinder.h"
#include "../inc/SplitArea.h"
#include "../../inc/MJPG/MJPG_Decoder.h"
#include "../../inc/FillPolygon.h"
#include "../../libAVI/libAVI.h"

typedef std::vector<POINT> PolygonVertices;


#define BLACK RGB(0,    0,   0)
#define WHITE RGB(255,255, 255)
#define RED   RGB(255,  0,   0)
#define BLUE  RGB(0,    0, 255)
#define GREEN RGB(0,  255,   0)

//ǰ����ɫ
#define FOREGROUND_COLOR WHITE

#define BACKGROUND_COLOR BLACK


//�ڳ��̽���λʵ���У� ���ֲ���һ�����̸�ͼ����У����������ȱ�ݣ�
//A: ������ͷ�п�����ͼ����Ͽ���խ�����Σ� �ϱߵĿ�����±߿�ȵ�2����
// �ϱߵĸ߶����±߸߶ȵ�4���� ��ʹ��ͳһ�ı�׼�����ϲ��ֺ��²��ֵĽǵ�ʱ��
// �ᷢ���°벿�ֻ��кܶ����Ŀ�꣬ ��Щ���Ŀ��Ĵ���ʹ��Ѱ����ȷ�Ľǵ���
// ���ѡ�
//
//Ϊ�˿˷�������ȱ��, �Ľ�����Զ�У���㷨����Ļ�ֳ����ɸ��������� ÿ���������У��
//ʱֻ�ڶ�Ӧ��Ļ������������ʾ�����������̸���ʽ��У��ͼ�����Խǵ������Ҳ��Ϊ������������
//�����Զ�У���㷨���㲽���޸�����
//Step1:ͼ��Ԥ����,����
// 1.1 ����ɫͼƬ����ɻҶ�ͼƬ
// 1.2 ����ǵ�����ǰ��ͼƬ�м�ȥ��������ͼ�������������š�
//
//
//Step2:������������ҳ�������ĻӰ���Ӧ������, �������Ҳ�ں���ͼ������������ͼ
// 1.1 ���������Ļ����ˢ�ɰ�ɫ����ͼ���ֵ��, �õ�ֻ��������������Ķ�ֵλͼA��
// 1.2 ���������Ļ����ˢ�ɺ�ɫ����ͼ���ֵ��, �õ��ڰ׶�ֵͼB.
// 1.3 A��Ϊ1��������B�ж�ӦΪ0������Ϊ��������Ļ����
//
//Step3:�������Ļ������ͷ�����е�ƫת�Ƕ�
// 2.1 ���ڼ������Ļ����ʾ�ϰ��ºڵĺڰ����飬 ��������ͷ�����а�ɫ�������������G1.
// 2.2 ���ڼ������Ļ����ʾ�Ϻ��°׵ĺڰ����飬 ��������ͷ�����а�ɫ�������������G2.
// 2.3 ����ʸ��V<G1, G2>, VΪ��Ļ��ֱ��ʸ��������ͷ�����еķ�λ��
//
//
//Step3.����������У���׶�
// �ڼ������Ļ���������У������ͼ���� �������ҳ�����ͷ���г�����У���ǵ��λ�ã�������֪��
// ��Ļ���꽨��һ��һӳ���ϵ��
//
// �������������У����ϣ� ת����4�� �������ִ�в���3
//
//
//Step4.�������ҵ�����ĻУ����������ͷ����ǵ�Ķ�Ԫ�鼯�ϴ���У�����̣�����У������ϵ����
//


//���60֡�Ҷ�MJPG��������ͷ�ĸĽ���ʩ(2013/08/30)
//1.����֡�ʼӱ�, У��ʱʹ�õİ����ͺ�������������������ʾ, ��Ϊ������ʾ��
//
//2.У�������޸�
//2.1:����Ȧ��У����, ��Ļ��ÿ��ֻ���һ�����У����
//2.2:���ڲ�У����, ��Ļ�Ͻ�У��ͼ���ֳ����������ֱַ���ʾ��
//    ����У��ͼ����Ϊ������������ʾ, �������������������ֵ�����ͼ�ˡ�
//@˵��:pRect���Դ������Ͻ�Ϊ����ԭ��(ɾ��)
//      pRect, ������Ļ���Ͻ�Ϊ����ԭ��
inline void FillBoard(HWND hWnd, COLORREF clr, const RECT* pRect=NULL)
{
    HDC hDC = ::GetDC(hWnd);

    HBRUSH hBrush = ::CreateSolidBrush(clr);//


    RECT rcClient;

    if(pRect)
    {
        rcClient = *pRect;
        LPPOINT pPoint = (LPPOINT)&rcClient; 
        ScreenToClient(hWnd, pPoint);
        ScreenToClient(hWnd, pPoint + 1);

    }
    else
    {
        ::GetClientRect(hWnd,&rcClient);
    }

    
    BOOL bRet = FillRect(hDC, &rcClient, hBrush);
    ::DeleteObject(hBrush);
    ReleaseDC(hWnd, hDC);


}


//=========��̬���β�����=======
class CStaticMaskFinder
{
public:
        CStaticMaskFinder()
            :
        m_cMaskThreshold(0x080),
        m_nCountDown(0),
        m_hFont(NULL),
        m_hDisplayWnd(NULL),
        m_hFontOld(NULL),
        m_hDC(NULL),
        m_hbrWhite(NULL)
        {

        }
        
        ~CStaticMaskFinder()
        {
        
        }


        //@����:��λ
        //@����:nImageWidth,  ͼ����
        //      nImageHeight, ͼ��߶�
        //      cMaskThreshold, ��������, ������ֵ���ڴ�����ʱ����
        //      hDisplayWnd, ������ʾ����
        //      rcMonitor, ������Ļ���Ͻ�Ϊ����ԭ������ϵ�еľ�������
        void Reset(int nImageWidth, int nImageHeight, BYTE cMaskThreshold, HWND hDisplayWnd, const RECT& rcMonitor,bool bDispNumber)
        {  
            m_oStaticMaskFrame.SetSize(nImageWidth, nImageHeight, cMaskThreshold);
            
            //ȫ����ʼ��Ϊ0xFF�������κ�����״̬��
            m_oStaticMaskFrame.Clear(0xFF);

            m_cMaskThreshold = cMaskThreshold;

            m_hDisplayWnd = hDisplayWnd;

            m_rcMonitor = rcMonitor;
            LPPOINT pPoint = (LPPOINT)&m_rcMonitor;
            ScreenToClient(hDisplayWnd, pPoint);
            ScreenToClient(hDisplayWnd, pPoint + 1);

            InitGDIObject();

            /////add by vera_zhao 2018.12.29
            //�Ƿ���ʾ����
            if (bDispNumber)
            {
                m_nCountDown = MAX_COUNT_DOWN;
                DispNumber(m_nCountDown / 15);
            }
            else
            {
                m_nCountDown = 25;
            }      
        }


        //@����:������������ͷ��ÿһ֡���ݣ���δ��������ͼ���ӡ�
        //@����:scrFrame, 8λ�ĻҶ�ͼƬ
        //@����ֵ:TRUE, ��̬���ν���
        //        FALSE, ��̬������
        BOOL Process(const CImageFrame& srcFrame,bool bDispNumber)
        {
            if(m_nCountDown == 0) return  TRUE;//�Զ������Ѿ�������

            const BYTE* pSrc  = srcFrame.GetData();
                  BYTE* pMask = m_oStaticMaskFrame.GetData();
               int nLoopCount = srcFrame.GetPixelCount();
               while(nLoopCount)
               {
                   if(*pSrc > m_cMaskThreshold)
                   {
                        *pMask = 0x00;
                   }
                   
                   pSrc  ++;
                   pMask ++;
                   nLoopCount --;
               }//while

               wchar_t szTmp[1024] = { 0 };
               wsprintf(szTmp, L" xxxxxx m_nCountDown:%d\n", m_nCountDown);
               OutputDebugString(szTmp);

               m_nCountDown --;

               if (bDispNumber)
               {
                  if(m_nCountDown % 15 == 0)
                  {
                     int number = m_nCountDown / 15;
                     DispNumber(number);
                  }
               }
               else
               {
                   //��������
               }

               if(0 == m_nCountDown)
               {
                    ReleaseGDIObject();
                    return TRUE;
               }

               return FALSE;
        }//

        //����:���ؾ�̬����ͼ
        const CImageFrame& GetMaskFrame() { return m_oStaticMaskFrame;}

protected:
        void InitGDIObject()
        {
            RECT rcWnd;
            GetClientRect(m_hDisplayWnd, &rcWnd);
            int nWndHeight = rcWnd.bottom - rcWnd.top;

            LOGFONT lf;
            memset(&lf, 0, sizeof(LOGFONT));
            lf.lfHeight        = nWndHeight/2;
            lf.lfWidth         = 0;
            lf.lfWeight        = FW_BOLD;
            lf.lfCharSet       = ANSI_CHARSET;
            lf.lfOutPrecision  = OUT_DEFAULT_PRECIS;
            lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
            memcpy(lf.lfFaceName, _T("Times New Roman"), _countof(lf.lfFaceName));

            m_hFont    = CreateFontIndirect(&lf);
            m_hDC      = GetDC(m_hDisplayWnd);
            m_hFontOld = (HFONT)SelectObject(m_hDC, m_hFont);
            m_hbrWhite = ::CreateSolidBrush(RGB(255,255,255));

            m_clrOldText = SetTextColor(m_hDC, RGB(255,0,0));

        }

        void ReleaseGDIObject()
        {
            if(m_hFontOld)
            {
                SelectObject(m_hDC, m_hFontOld);
            }

            SetTextColor(m_hDC, m_clrOldText);

            DeleteObject(m_hbrWhite);
            ReleaseDC(m_hDisplayWnd, m_hDC);
            m_hDC = NULL;
        }

        void DispNumber(int num)
        {
           TCHAR szText[32];
           _stprintf_s(szText, _countof(szText), _T("%d"), num);

            //RECT rcWnd;
            //GetClientRect(m_hDisplayWnd, &rcWnd);
            
            //FillRect(m_hDC, &rcWnd, m_hbrWhite);
           FillRect(m_hDC, &m_rcMonitor, m_hbrWhite);
            SetBkMode(m_hDC, TRANSPARENT);

            //��ʾ������Ϣ
            //DrawText(m_hDC, szText, _tcsclen(szText), &rcWnd, DT_CENTER|DT_VCENTER| DT_SINGLELINE );
            DrawText(m_hDC, szText, _tcsclen(szText), &m_rcMonitor, DT_CENTER|DT_VCENTER| DT_SINGLELINE );
        }

protected:
    CImageFrame  m_oStaticMaskFrame;//��̬����ͼ, ����������ֵΪ0
    BYTE         m_cMaskThreshold  ;//��������, ���ڸ�ֵ������
    HWND         m_hDisplayWnd     ;//��ʾͼ���Ĵ���
    RECT         m_rcMonitor       ;//��Ļ����������У���������Ͻ�Ϊ����ԭ��
    int          m_nCountDown      ;//����������

   

    //��ʾ���GDI����
    HFONT m_hFont                  ;//������ʾ��������
    HFONT m_hFontOld               ;//
    HDC   m_hDC                    ;//
    HBRUSH m_hbrWhite              ;//��ɫ��ˢ
    COLORREF m_clrOldText          ;
    static const int MAX_COUNT_DOWN = 45;  //45
};

inline void AccumulateImageFrame(const CImageFrame& srcFrame, CWordFrame& destFrame)
{
    const BYTE* pBytePixel = srcFrame.GetData();
    WORD* pWordPixel = destFrame.GetData();

    int nLoopCount = destFrame.GetPixelCount();

     while(nLoopCount)
    {
        *pWordPixel += *pBytePixel;
        pWordPixel++;
        pBytePixel++;
        nLoopCount --;
    }                
}

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
    void Init(DWORD dwTimeMagnification, DWORD dwVideoDisplayDelay, BOOL bUseAbsTime=FALSE)
    {
         m_dwTimeBegin = GetTickCount();

        if (dwTimeMagnification <= 0 )
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

            return (dwTimeElapsed >= dwWaitTimeInMs*m_dwTimeMagnification + m_dwVideoDisplayDelay) ? TRUE : FALSE;
        }
        else
        {
            const int MILLI_SECOND_PER_FRAME = 16;
            DWORD dwNeeedWaitFrames = (dwWaitTimeInMs + (MILLI_SECOND_PER_FRAME  >> 1))/ MILLI_SECOND_PER_FRAME;
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
    DWORD m_dwTimeBegin        ;//������ʼʱ��
    DWORD m_dwTimeMagnification;//ʱ��Ŵ���
    DWORD m_dwFrameCount;
    DWORD m_bUseAsbTime;//ʹ�þ���ʱ��
    DWORD m_dwVideoDisplayDelay;//��Ƶ��ʾ�ӳ٣���λ:ms

};


//��Ļ����λ��
class CMonitorAreaLocator
{
public:
    CMonitorAreaLocator()
        :
    m_nSubAreaId(0),
    m_bSuccess(FALSE),
    m_hDisplayWnd(FALSE),
    m_eDebugLevel()
    {

    }

    ~CMonitorAreaLocator()
    {

    }

    //@����:��λ��λ��״̬
    //@����:nImageWidth, ͼ����
    //      nImageHeight,ͼ��߶�
    //      hDisplay, ������ʾ����
    //     rcMonitor, ��ǰ��Ļ���ľ�����
    void Reset(int nImageWidth, int nImageHeight, HWND hDisplayWnd, const RECT& rcMonitor, ECalibDebugLevel eDebugLevel = E_CALIB_DEBUG_LEVEL_CONCISE);


    //@����:����ÿһ֡�Ҷ�ͼƬ
    //@����:srcFrame, ͼƬ֡����
    //      bSimulate, ģ��У����־
    //@����ֵ:TRUE, ��������
    //        FALSE,�������
     BOOL Process(const CImageFrame& srcFrame, BOOL bSimulate = FALSE);

     //@����:��ȡ����ͼ
     const CImageFrame& GetMaskFrame(){return m_oMaskFrame;}

     //@����:�Ƿ�λ�ɹ���־
     BOOL IsSuccess()const{return m_bSuccess;}

     static const int SUB_AREA_COUNT = 4;
     POINT (&GetSubAreaCentroids())[SUB_AREA_COUNT]
     {
        return  m_SubAreaCentroids;
     }

    CWaitTimer& GetWaiterTimer() 
    {
        return m_oWaitTimer;
    }
protected:
    
    CImageFrame m_oMaskFrame;//����֡

    HWND        m_hDisplayWnd;//��ʾ����

    RECT        m_rcMonitor  ;//��ǰ��Ļ�ľ�������

    //����������, Ŀ����Ϊ�˼����ڹ��ղ����ȵ�������, ȷ����Ļ����ͼ��ȱ�ǡ�
    
    CImageFrame m_SubAreaMask[SUB_AREA_COUNT];//
    RECT        m_SubAreaRect[SUB_AREA_COUNT];//
    int         m_nSubAreaId;//������Id

    POINT       m_SubAreaCentroids[SUB_AREA_COUNT];//����������ģ�
    //�����������״̬:
    BOOL        m_bSuccess;//
    
    enum E_RUN_STAGE
    {
        E_RUN_STAGE_BEGIN            ,//��ʼ�׶�
        E_RUN_STAGE_SAMPLE_BLACKBOARD,//�����ڰ�
        E_RUN_STATE_SAMPLE_SUBAREA   ,//����������
        E_RUN_STAGE_SAMPLE_WHITEBOARD,//�����װ�
        E_RUN_STAGE_END               //�����׶�
    }m_eRunStage;

    CWordFrame  m_oBlackBoardAccFrame ;//����ͼ��֡�ۼ�ͼ
    CWordFrame  m_oWhiteBoardAccFrame ;//����ͼ��֡�ۼ�ͼ
    //int         m_nStageWaitCount     ;//�׶μ���

    //static const int STAGE_MINIMUM_WAIT_COUNT = 5;//��С�ȴ�����
    CWaitTimer m_oWaitTimer;
    static const DWORD STAGE_MINIMUM_WAIT_TIME = 100;//�ȴ�ʱ��100ms


    //static const int WHITE_BOARD_SAMPLE_START_COUNT = STAGE_MINIMUM_WAIT_COUNT*2;//��ʼ�����װ�ʱ��֡����ֵ
    //static const int WHITE_BOARD_SAMPLE_END_COUNT   = STAGE_MINIMUM_WAIT_COUNT*3;//�����װ����ʱ��֡����ֵ

    //static const int BLACK_BOARD_SAMPLE_START_COUNT = STAGE_MINIMUM_WAIT_COUNT*2;//��������ʱ��֡����ֵ
    //static const int BLACK_BOARD_SAMPLE_END_COUNT   = STAGE_MINIMUM_WAIT_COUNT*3;//��������ʱ��֡����ֵ


    static const DWORD  WHITE_BOARD_SAMPLE_START_TIME = STAGE_MINIMUM_WAIT_TIME * 2;//��ʼ�����װ�ʱ��ȴ���ʱ��
    static const DWORD  WHITE_BOARD_SAMPLE_END_TIME   = STAGE_MINIMUM_WAIT_TIME * 3;//���������װ�ʱ��ȴ���ʱ��

    static const DWORD  BLACK_BOARD_SAMPLE_START_TIME = STAGE_MINIMUM_WAIT_TIME * 2;//��ʼ�����ڰ�ʱ��ȴ���ʱ��
    static const DWORD  BLACK_BOARD_SAMPLE_END_TIME   = STAGE_MINIMUM_WAIT_TIME * 3;//���������ڰ�ʱ��ȴ���ʱ��

    ECalibDebugLevel m_eDebugLevel;//���Լ���
    
};


inline void AccumulateImageFrame(const CImageFrame& srcFrame, const CImageFrame& maskFrame, CWordFrame& destFrame)
{
    const BYTE* pSrcPixel  = srcFrame.GetData();
    const BYTE* pMaskPixel = maskFrame.GetData();
    WORD* pDestPixel       = destFrame.GetData();

    int nLoopCount = destFrame.GetPixelCount();

     while(nLoopCount)
    {
        if(*pMaskPixel)
        {
            *pDestPixel += *pSrcPixel;
        }

        pMaskPixel ++;
        pSrcPixel  ++;
        pDestPixel ++;
        nLoopCount --;
    }//while
}
//==========================
    struct TBorderMarker//��Ƶͼ���еı߽���λ����Ϣ
    {
        POINT ptCentroid;//����
        RECT  rcBound   ;//��Χ�߽�Ŀ��ľ�������
    };

//==========================
//����:��Ļ�߽������
class CMonitorBoundaryFinder
{
public:
     CMonitorBoundaryFinder();
    ~CMonitorBoundaryFinder();

    //@����:��λ״̬��
    //@����:frameInitalScreenMask,��ʼ����Ļ����ͼ
    //      hDispWnd, ��ʾ����
    //      ECalibDebugLevel, ���Լ���
    //      bSimulate, ģ��У����־
    void Reset(const CImageFrame& frameInitalScreenMask, HWND hDispWnd, const RECT& rcMonitor, ECalibDebugLevel eDebugLevel= E_CALIB_DEBUG_LEVEL_CONCISE, BOOL bSimulate = FALSE);

    //@��  ��:������Ҷ�ͼƬ
    //@��  ��:grayImage, ����ĻҶ�ͼƬ
    //        bSimulate, ģ��У����־
    //@����ֵ:TRUE,����ɹ�
    //        FALSE, ����ʧ��
    BOOL Process(const CImageFrame& grayImage, BOOL bSimulate = FALSE);

    BOOL IsDone()const{return m_bDone;}

    //const POINT& GetUpperPartCentroid()const{return m_ptUpperCentroid;}
    //const POINT& GetLowerPartCentroid()const{return m_ptLowerCentroid;}

    const POINT& GetScreenCentroid()const{return m_ptCentroid;}


    //const std::vector<TBorderMarker>& GetUpperBorderCalibratePts() const {return m_vecUpperBorderCalibratePts;}
    //const std::vector<TBorderMarker>& GetLowerBorderCalibratePts() const {return m_vecLowerBorderCalibratePts;}

    const std::vector<TBorderMarker>& GetBorderCalibratePts() const {return m_vecBorderCalibratePts;}

    //const CImageFrame& GetUpperHalfMaskFrame()const { return m_oUpperHalfMaskFrame;}
    //const CImageFrame& GetLowerHalfMaskFrame()const { return m_oLowerHalfMaskFrame;}

    const std::vector<RECT>& GetBorderMarkerPositions()const {return m_vecBorderMarkerPositions;}

    void SetSubAreaPolygons(const PolygonVertices* pPolygons, int nCount)
    {
        m_vecSubAreaPolygons.resize(nCount);
        for(int i = 0; i < nCount; i++)
        {
            m_vecSubAreaPolygons[i] = pPolygons[i];
        }
    }

    int GetMarkerCount()const;

    CWaitTimer& GetWaiterTimer()
    {
        return m_oWaitTimer;
    }
protected:
    //@����:��ʼ���߽���λ������
    void InitBoundaryMarkerPositions(int nHorzSideSquareNumber, int nVertSideSquareNumber, int nSquareWidth);


    BOOL SearchCircleCentroids(const CWordFrame&  srcImage, TBorderMarker* pPtBorderMarker, int nExpectedNumber, POINT* pGlobalCentroid);

    //@����:������ͼƬ
    //@����:diffImage, ���ͼ
    //      eScreenPart, ��Ļ����ö��ֵ
    //BOOL ProcessDiffImage(const CWordFrame& diffImage, ESearchScreenPart eScreenPart);
    BOOL ProcessDiffImage(const CWordFrame& diffImage);


     //int m_nRunTimes                ;//���д�������
     static const int MAX_FLASH_TIMES = 3;

     int m_nFlashTimes                ;//��˸����
     int m_bShowMarker                ;//�Ƿ���ʾ�߽���(ʵ��Բ)��־
     CWordFrame m_frameBackground     ;//��֡
     CWordFrame m_frameForeground     ;//��֡
     CImageFrame m_oInitialScreenMask ;//��Ļ��������ͼ

     CImageFrame m_oScreenMaskFrame   ;//����������ͼ
    
    POINT  m_ptCentroid               ;//����

    //����Ļ������ʾ�ĸ�����ɫʵ��Բ��ֱ�� , ��λ���ء�
    static const int INITIAL_MARKER_DIAMETER = 45;
    int m_MarkerDiameter;


    //����ʵ��СԲ��������������ĻӰ���ڻ����еı߽�
    std::vector<RECT> m_vecBorderMarkerPositions       ;//ȫ���߽�Բ��λ��(�������Ļ����)
    std::vector<double> m_vecDisplayIntensity          ;//ÿ��У��ʵ��Բ����ʾǿ��,��Χ0~1.0, ��Ļ�ĸ����Ǵ���ʵ��Բ��ʾǿ�����

    std::vector<TBorderMarker> m_vecBorderCalibratePts    ;//������ͷ��Ƶ�еı߽�������,���Ľ��Ӧ�ð��մ����Ͻǿ�ʼ,˳ʱ�뷽������(��Ƶ����)
    std::vector<PolygonVertices> m_vecSubAreaPolygons;//ͼ�񻮷ֵĶ����������, ���������趨��ֵ������

    RECT                   m_rcMonitor       ;//��Ļ��������
    ECalibDebugLevel       m_eDebugLevel     ;//���Լ���

    HWND                   m_hDispWnd        ;//��ʾ����
    BOOL                   m_bDone           ;//����������־


    CWaitTimer m_oWaitTimer;
    const int WAIT_STEDAY_SAMPLE_TIME = 160;//�ȴ��ȶ�����ʱ��ʱ��,��λ:ms
    const int WAIT_SAMPLE_END_TIME    = 330;//�׶ν�����ʱ��,��λ:ms

    //ˮƽ����ı߽�����
    int m_nHorzSideMarkerNumber;

    //��ֱ����ı߽�����
    int m_nVertSideMarkerNumber;

    static const int MINIMUM_SIDE_MARKER_NUMBER = 5;
    
};


///////////////////////////////////////////////////////////////////////////////////////////////////
//@�������Զ�У����
class CAutoCalibratorImpl2:public IAutoCalibratorInterface
{
public:
    //�ǵ�����
    struct TImageCalibPoint
    {
        TPoint2D  pt2DImageCoord ;//�ǵ���ͼ���е�����
        POINT     ptRotatedCoord ;//������ת�������ͼ������
       
        //POINT ptHDImgCoord ;//�߾���ͼ������ֵ,��������2^INT_SCALE_SIZE��, ������������������ľ������
    };



public:
    CAutoCalibratorImpl2(/*int nCalibratePtNumber = 36*/)
        :
        m_bIsWorking(FALSE),
        m_AutoCalibrateWnd(this),
        m_eCalibrateStage(E_AUTO_CALIBRATE_UNDEF),
        m_bIsSimulatedCalibrating(FALSE),
        m_eCalibrateError(E_AUTO_CALIBRATOR_OK),
        //m_nMaskAreaPixelCount(0),
        //m_eMonochromizeAlog(E_MONOCHROMIZE_ALOG_Y),
        //m_eSubareaCalibrateStage(E_SUB_AREA_SEARCH_CALIBRATE_UNDEF),
        m_eDebugLevel(E_CALIB_DEBUG_LEVEL_CONCISE),
        //m_bDisplayPatternAgainWhenFailed(TRUE),
        //m_bCalibrateSequenceComplete(FALSE),
        m_nTryTimes(0),
        m_fpChangeCameraParams(NULL),
        m_lpCtx(NULL),
        m_bTestAutoBrightnessCtrlMode(FALSE),
        m_AutoBrightnessRegulator(100,BrightnessCtrlCallback, (LPVOID)this),
        //SQUARE_SIZE(25),
        m_nCalibratePtNumber(0),
        m_hNotifyWnd(NULL),
        m_nMaxTryTimes(MAX_TRY_TIMES),
        m_bRestoreLanguageBar(FALSE),
        m_bUseOldMethod(FALSE),
        m_bBorderCalibratrPtsIsValid(FALSE),
        m_bEnableOnlineScreenArea(FALSE),
        m_nDbgFrameCount(0),
        m_nDbgPatternRadius(58)
    {
        
        m_uMonitorWidth  = GetSystemMetrics(SM_CXSCREEN);
        m_uMonitorHeight = GetSystemMetrics(SM_CYSCREEN);

        //m_vecCalibrateSequence.resize(E_SEQUENCE_FRAME_NUM);

    }


    ~CAutoCalibratorImpl2()
    {

        m_AutoCalibrateWnd.DestroyWindow();
    }



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
    //virtual BOOL StartCalibrating(HWND hNotifyWnd, E_AutoCalibratePattern eGridPattern=E_CALIBRATE_PATTERN_11_COLS, BYTE cBrightness = 50, CHANGECAMERAPARAMSPROC ChangeCameraParamsProc = NULL, LPVOID lpCtx = NULL, COLORREF clrGridHighlight=RGB(255,255,255),BOOL bSaveInermediatFile = FALSE ,BOOL bRecordVideo = FALSE);


    //@����:��ʼУ��, ���ⲿģ�����
    //@����:autoCalibrateParams, �Զ�У������
    //      staticMaskingParams, ��̬���β���
     virtual BOOL StartCalibrating(const TAutoCalibrateParams& autoCalibrateParams, const TStaticMaskingParams& staticMaskingParams);


    //@����:��������
    virtual void Finish();

    //@����:ֹͣУ��
    void EndCalibrating();

    //@����:�����Զ���Ļ������������
    //void EndSearchingCurrentMonitorArea();

    //@����:�����Զ���Ļ������������
    //void EndSearchingAllMonitorArea();

   

    //@����:���Զ�У����ͶιͼƬ��������֮
    //@����ֵ:�쳣����FALSE
    virtual BOOL FeedImage(const CImageFrame* pGrayFrame);


    //@����:���Զ�У����ͶιͼƬ��������֮
    //@����ֵ:�쳣����FALSE
    BOOL FeedImage_AutoCalibrate(const CImageFrame* pGrayFrame);


    //@����:������Ļ����
    virtual int GetMonitorCount()const;


    //@����:��ȡָ����Ļ��У������
    //@����:nMonitorID�� ��ʾ��Id
    //@˵��:
    //virtual  TMonitorCalibData GetMonitorCalibData(int nMonitorID) const;


     //@����:��ȡ������Ļ��У������
     virtual  const TCalibData*  GetCalibrateData()const;

     void OnDeviceMissing();

    //@����:������У��
    //@����:
    //     nExpectedMarkerNumber,�����ҵ���У�������Ŀ
    //     maskFrame, ����ͼƬ
    //     srcFrame, У��ͼƬ
    //     roateMatrix, ��У��ͼ����ת��ˮƽ��ֱʱʹ�õ���ת����ϵ��
    //     ptCenter, ��ת���ĵ�ͼ������
    //     nDetectRadius, У����������İ뾶
    //    nAreaNo,������
    //@˵��:�ҵ��Ľǵ��ŵ�����m_vecInteriorCalibPoint��
    //BOOL SubAreaCornerDetect(
    //    int nExpectedMarkerNumber,
    //    const CImageFrame& maskFrame,
    //    const CImageFrame& srcFrame,
    //    const double m[2][2],
    //    const POINT& ptCenter,
    //    int nDetectRadius,
    //    int nAreaNo);



    //@����:�ռ�У��������ĺ���
    void CollectCalibrateMarkers(
        const TBlobObject* pObjs,
        int nBlobCount,
        const double rotationMatrix[2][2],//������ת����
        const POINT& ptCenter
        );


    //@����:����У��������
    virtual HWND GetCalibrateHWnd()const;

    //@���ܣ������Զ�У����ʧ�ܵ��м���ͼƬ·��
    virtual CAtlString  GetCalibrateIntermediataDirName();

    
    //@����:ģ��У��
    BOOL DoSimulateCalibrate(LPCTSTR lpszAVIFilePath, HWND hNotifyWnd,  UINT uCx, UINT uCy, BYTE cBrightness, E_AutoCalibratePattern eGridsNumber=E_CALIBRATE_PATTERN_11_COLS);
    
	CalibrateAlgo m_oCalibrateAlgo;

	struct TNeighbourVector
    {
        int nQaudrant;//��λ�������ޱ��
        LONG lDx ;
        LONG lDy ;
        //POINT pt ;
        int nVertexIndex;//ʸ��������У�����±�
    };

    //static const int MAX_NEIBHBOUR = 16;
    static const int MAX_NEIBHBOUR = 16;

    struct TVertex
    {
        POINT ptCoord ;//ͼ������
        unsigned nAryIndex ;//�������е�������
        //BOOL bValid ;//������Ч��־
        unsigned int nNeighbourNumber ;//�ڽӵ���Ŀ
        BOOL bIsOuterLayer ;//����Ȧ��־
        BOOL bIsTurnPoint ;//�ǹսǴ��ǵ�ı�־

        struct
        {
            bool bValid;
            int nIndex;
            double distance;

        }aryNeighbours[MAX_NEIBHBOUR];//�ڽӵ�������

        TVertex()
        {
            ptCoord.x = 0;
            ptCoord.y = 0;
            nNeighbourNumber = 0;
            bIsOuterLayer = FALSE;
            bIsTurnPoint = FALSE;
            nAryIndex = 0;
            memset(aryNeighbours, 0, sizeof(aryNeighbours));
        }

    };

     struct TurnPoint
    {
        POINT ptCoord      ;//ͼ������
        UINT nLayerPtIndex ;//��Ȧ�е�������
    };


    //@�� ��:�ڽǵ�������, �������Ͻǵ�ƥ���ѡ�㡣
    //@�� ��:turnPoints, �ǵ�����
    // ptG, ��������
    // nTurnPoints, �ǵ����
    //@����ֵ:�������ϽǵĽǵ������������
    // -1, ��δ�ҵ����ϽǵĽǵ㡣
    //@˵ ��: �ú���������DoSubAreaCornerMatch���á�
    //
    //����Ȧ������GΪԭ��, ��ֱ����ΪY������ϵ��,
    // ��y
    // |
    //�D+�D��x
    // |0
    //����λ�����ϽǵĽǵ�, �ýǵ�������Ǵ���2,3����, ����X������н�
    //��С
    //int MatchLeftTopCorner(const TurnPoint* turnPoints, POINT& ptG, int nTurnPointNumber);
     int MatchLeftTopCorner(const TurnPoint* turnPoints, int nTurnPointNumber);


   //@����:��һȦ���в��ҹյ�
   //@����:pPts, Ȧ�ϵ������,�Ѱ��շ�����������
   //      nPtNumber, Ȧ�ϵ����Ŀ
   //     pTurnpoint,����յ������
   //     nExpectedNum, �յ������Ԫ�ظ���
   //@����:�յ���Ŀ
   int SearchTurnPoints(const POINT* pPts, int nPtNumber, TurnPoint* pTurnpoint, int nExpectedNum);


   //@����:��ֵ��ͼƬ
   //@����:srcImage, ԭʼͼƬ
   //      bitImage, ����Ķ�ֵ��λͼ
   void BinarizeImage(const CImageFrame& srcImage, const CImageFrame& maskImage, CBitFrame& bitImage);

   //@����:ͼ���нǵ�����Ļ��У�����ƥ��
   //@����:
   // corners, ��֪��Ƶͼ���еĵĽǵ���������
   // vecImage2ScreenMap, ÿ����Ƶͼ���еĽǵ㵽��ĻУ�����ŵ�ӳ�����顣
   BOOL DoCornerMatch(const std::vector<TImageCalibPoint>& corners, std::vector<int>& vecImage2ScreenMap);

   //@����:���˲��ϸ�ı߽��
   //@����:�ϸ�ı߽�����
   int FilteroutUnqualifiedOutlayerVertex(TVertex* pVertex, int nVertexCount);

    //@����:YUV�����ۼӺڰ�ͼ��
    void Covert2MonochromeFrame_YUV(const CImageFrame& YUVFrame, CImageFrame& monoFrame)
    {
        UINT nImageWidth = YUVFrame.Width();
        UINT nImageHeight = YUVFrame.Height();
        monoFrame.SetSize(nImageWidth, nImageHeight, YUVFrame.BytesPerPixel());

        const BYTE* pYUVData = YUVFrame.GetData();
        BYTE* pMonoData = monoFrame.GetData();
        for(UINT row = 0; row < nImageHeight; row++)
        {
            for(UINT col = 0; col < nImageWidth; col++)
            {

                if(col % 2 ==1)
                {
                    //
                    int nSum = (int)pYUVData[0] + (int)pYUVData[1] + (int)pYUVData[3];

                    *pMonoData = (BYTE)(((int)pYUVData[0] + (int)pYUVData[1] + (int)pYUVData[3]) /3);
                }
                else
                {
                    *pMonoData = (BYTE)(((int)pYUVData[1] + (int)pYUVData[2] + (int)pYUVData[3]) /3);
                }

                pMonoData++;

                if(col % 2 == 0)
                {
                    pYUVData += 4;
                }

            }//foreach col
        }//foreach row
    }

    //@����:Ԥ������ͼ������,ʹ�û����еĺڰ׶Աȸ���ǿ��
    //@����:patternFrame, Դͼ
    // maskFrame, ����ͼ
    // bkgrndFrame,����ͼ
    // outFrame, �����ͼƬ����
    void PreProcessPatternImage(const CImageFrame& patternFrame, const CImageFrame& maskFrame, const CImageFrame& bkgndFrame, CImageFrame& outFrame)
    {

        CImageFrame srcImage = patternFrame;
        int nWidth =  srcImage.Width();
        int nHeight = srcImage.Height();
        outFrame.SetSize(nWidth, nHeight, 1);
        outFrame.Clear();

        BYTE* pDestData = outFrame.GetData();
        const BYTE* pSrcData   = srcImage.GetData();
        const BYTE* pBkgndData = bkgndFrame.GetData();
        const BYTE* pMaskData  = maskFrame.GetData();

        int nLoop = outFrame.Size();

        //��ִ���
        while(nLoop--)
        {
            if(*pMaskData == 0xFF)
            {

                BYTE srcData = *pSrcData;
                BYTE bkData  = *pBkgndData;
                BYTE destData;

                BYTE subtractBkData = (bkData >> 2) * 3;

                if(srcData < subtractBkData)
                {
                    destData = 0x00;
                }
                else
                {
                    destData = srcData - subtractBkData;
                    //if(destData < 5)//���ֵС��10������Ϊ������
                    //{
                    //    destData = 0;
                    //}
                }

                *pDestData = destData;
            }

            pDestData++;
            pSrcData ++;
            pBkgndData ++;
            pMaskData++;

        }//while()

        //�Ӵ���ͼ�ĶԱȶ�
        MaximizeContrastness(outFrame);

    }

    //���Ժ���
    //@����:���������Զ����ڹ���
    virtual void TestAutoBrightnessControl(CHANGECAMERAPARAMSPROC ChangeCameraParamsProc = NULL, LPVOID lpCtx = NULL);

    //@����:�鿴У��ͼ��    
    virtual void ViewCalibratePattern();


    class CDebugWindow:public CWindowImpl<CDebugWindow>
    {
    public:

        BEGIN_MSG_MAP(CDebugWindow)
            MESSAGE_HANDLER(WM_PAINT, OnPaint)
            /*MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
            MESSAGE_HANDLER(WM_CREATE, OnCreate)
            MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblClk)
            MESSAGE_HANDLER(WM_FILE_END,OnPlayFileEnd)
            MESSAGE_HANDLER(WM_CONTEXTMENU, OnContextMenu)
            COMMAND_ID_HANDLER(ID_FULL_SCREEN,OnFullScreen)
            COMMAND_ID_HANDLER(ID_OPEN_CFG_FILE,OnOpenCfgFile)
            MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
            MESSAGE_HANDLER(WM_NCDESTROY, OnNcDestroy)*/
        END_MSG_MAP()

        LRESULT PrintFrame(const CImageFrame& frame, BOOL bExpand = FALSE/*�Ҷ���ǿ��ʾ*/)
        {
            if(m_hWnd == NULL) return E_FAIL;
            UINT nImageWidth = frame.Width();
            UINT nImageHeight = frame.Height();

            if(m_oDib.GetWidth() != nImageWidth
                ||
                m_oDib.GetHeight() != nImageHeight)
            {
                if(!m_oDib.Create(nImageWidth, nImageHeight))
                {

                    return -1L;
                }

            }

            BYTE BytePerPixel = m_oDib.GetBiBitCount()>>3;
            BYTE* pDestData = (BYTE*)m_oDib.GetBits() + (nImageHeight - 1)*nImageWidth*BytePerPixel ;
            const BYTE* pData = frame.GetData();
            int nLineBytes = nImageWidth*BytePerPixel;


            BYTE min=0, max=0;
            BYTE range =0;
            if(bExpand)
            {
                min = frame.Min();
                max = frame.Max();

                if(min == max) bExpand = FALSE;
                range = max - min;
            }

            for(UINT i=0; i<nImageHeight; i++)
            {
                for(UINT j=0; j<nImageWidth; j++)
                {
                    int val = pData[0];
                    if(bExpand)
                    {
                        val = (val-min)*255/range;
                    }

                    pDestData[0] = (BYTE)val;//B
                    pDestData[1] = (BYTE)val;//G
                    pDestData[2] = (BYTE)val;//R

                    pDestData += BytePerPixel;
                    pData ++;
                }

                pDestData -= nLineBytes * 2;
            }


            HDC hDC = GetDC();
            RECT rcClient;
            GetClientRect(&rcClient);

            m_oDib.Draw(hDC,
                rcClient.right - rcClient.left,
                rcClient.bottom - rcClient.top
                );

            ReleaseDC(hDC);

            return 0;

        }

        //@˵��:���ΪvecNo[i]��У�����Ӧ����Ƶ����ΪvecPts[i]��
        LRESULT PrintPtNo(const std::vector<POINT>& vecPts, const std::vector<int>& vecNo)
        {
            HDC hDC = GetDC();
            LOGFONT lf;
            memset(&lf, 0, sizeof(LOGFONT)); // Zero out the structure.
            lf.lfHeight = 12; // Request a 12-pixel-height font.
            lstrcpy(lf.lfFaceName, _T( "Arial ")); // Request a face name "Arial ".
            HFONT font = ::CreateFontIndirect(&lf) ; // Create the font.
            HFONT fontOld = (HFONT)::SelectObject(hDC, font);
            ::SetBkMode (hDC, TRANSPARENT);
            ::SetTextColor(hDC, RGB(255,0,0));

            for(unsigned int i=0; i < vecPts.size(); i++)
            {
                CAtlString strText;
                strText.Format(_T("%d"), vecNo[i]);

                ::TextOut(hDC,vecPts[i].x, vecPts[i].y, strText, strText.GetLength());
                ::SetPixel(hDC, vecPts[i].x, vecPts[i].y,RGB(0,255,0));
            }

            ::SelectObject(hDC, fontOld);
            ::DeleteObject(font);
            ReleaseDC(hDC);
            return 0;
        }



    protected:

        LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
        {
            // TODO: Add your message handler code here and/or call default
            PAINTSTRUCT ps;

            HDC hDC = ::BeginPaint(m_hWnd,&ps);
            RECT rcClient;
            GetClientRect(&rcClient);

            EndPaint(&ps);
            bHandled = TRUE;
            return 0;
        }


        //DIB����
        CDib m_oDib;
    };

    static ULONG _stdcall SimulatedCalibrateProc(LPVOID lpCtx);
    //{

    //    CAutoCalibratorImpl2* pCalibrator = reinterpret_cast<CAutoCalibratorImpl2*>(lpCtx);

    //    //���ɵ��Դ���
    //    int nCaptionHeight = GetSystemMetrics(SM_CYCAPTION);
    //    int nFrameCx = GetSystemMetrics(SM_CXFRAME);
    //    int nFrameCy = GetSystemMetrics(SM_CYFRAME);

    //    RECT rcPosition;
    //    rcPosition.left = 0;
    //    rcPosition.top = 0;

    //    rcPosition.right = 640;
    //    rcPosition.bottom = 480;

    //    rcPosition.right += nFrameCx*2;
    //    rcPosition.bottom += nCaptionHeight + nFrameCy*2;


    //    pCalibrator->m_oDebugWindow.Create(NULL, rcPosition, _T("Dbg Window"), WS_POPUP | WS_OVERLAPPEDWINDOW | WS_VISIBLE);


    //    pCalibrator->m_oDebugWindow.SetWindowPos(
    //        HWND_TOPMOST,
    //        rcPosition.left,
    //        rcPosition.top,
    //        rcPosition.right - rcPosition.left,
    //        rcPosition.bottom - rcPosition.top,
    //        SWP_SHOWWINDOW);

    //    const BITMAPINFOHEADER* bmpinfoHeader = pCalibrator->m_oAVIInput.GetAVIFormat();
    //    if(bmpinfoHeader == NULL)
    //    {
    //        return -1;
    //    }

    //    CImageFrame mjpgFrame;
    //    CImageFrame grayFrame;

    //    mjpgFrame.SetSize(bmpinfoHeader->biWidth, bmpinfoHeader->biHeight, bmpinfoHeader->biBitCount >> 3);
    //    grayFrame.SetSize(bmpinfoHeader->biWidth, bmpinfoHeader->biHeight, 1);

    //    CImageFrame onlineScreenArea;
    //    BYTE initValue = 0x00;
    //    onlineScreenArea.SetSize(bmpinfoHeader->biWidth, bmpinfoHeader->biHeight, 1, &initValue);
    //    FillPolygon(
    //        onlineScreenArea.GetData(),
    //        bmpinfoHeader->biWidth,
    //        bmpinfoHeader->biHeight,
    //        &pCalibrator->m_vecDbgOnlineScreenVertices[0],
    //        pCalibrator->m_vecDbgOnlineScreenVertices.size(),
    //        255,
    //        TRUE);

    //    Debug_SaveImageFrame(onlineScreenArea, _T("OnlineScreenArea.jpg"));


    //     CMJPG_Decoder m_oMJPGDecoder;//MJPG������

    //    ::SetTimer(NULL, 1, 10, NULL);

    //    MSG msg;
    //    while(::GetMessage(&msg, NULL, 0, 0 ))
    //    {

    //        switch(msg.message)
    //        {
    //        case WM_TIMER:
    //            {
    //                LONG lBytes = 0;
    //                LONG lSamples = 0;
    //                BOOL bRet = FALSE;

    //                bRet = pCalibrator->m_oAVIInput.Read((BYTE*)mjpgFrame.GetData(), mjpgFrame.Size(), &lBytes, & lSamples);
    //                if(!bRet)
    //                {

    //                    PostThreadMessage(::GetCurrentThreadId(), WM_QUIT, 0 ,0);
    //                    break;
    //                }

    //                int nMJPGDataLength = 0;
    //                m_oMJPGDecoder.ProcessData(mjpgFrame.GetData(), lBytes,(BYTE *)grayFrame.GetData(), &nMJPGDataLength);
    //                
    //                //�����ߵ�����ͼ��������
    //                grayFrame &= onlineScreenArea;

    //                bRet = pCalibrator->FeedImage(&grayFrame);
    //                if(!bRet)
    //                {
    //                    //pCalibrator->m_oDebugWindow.DestroyWindow();
    //                    PostThreadMessage(::GetCurrentThreadId(), WM_QUIT, 0 ,0);
    //                }

    //            }

    //            break;


    //        default:
    //            TranslateMessage(&msg);
    //            DispatchMessage(&msg);

    //        }//switch

    //    }//while


    //    pCalibrator->m_oDebugWindow.DestroyWindow();
    //    pCalibrator->m_bIsSimulatedCalibrating = FALSE;
    //    pCalibrator->m_bIsWorking = FALSE;
    //    pCalibrator->m_oAVIInput.Close();

    //    return 0;

    //}

    //@����:����8λ��ʽ������λͼ
    virtual const CImageFrame& GetMaskFrame() const
    {
        //return this->m_oScreenMaskFrame;
        //����ģʽ�£�����������Ļ������ͼ�ϳɵ�������ͼ
        return this->m_oAllMonitorMaskFrame;
    }

	//@����:����8λ��ʽ�ľ�̬����ͼ
	virtual  const CImageFrame& GetMaskFinderFrame(CImageFrame& maskframe) ;

    //@����:���ع��λ�õ���Ļ��ŵ�ӳ������
    virtual const CImageFrame& GetScreenMap() const
    {
        return this->m_oScreenMap;
    }


    virtual void SetDebugLevel(ECalibDebugLevel eDebugLevel)
    {
        m_eDebugLevel = eDebugLevel;
    }

    virtual SIZE GetScreenSize()const
    {

        SIZE szScreen;
        szScreen.cx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        szScreen.cy = GetSystemMetrics(SM_CYVIRTUALSCREEN);
        return szScreen;
    }


    virtual SIZE GetImageSize()const
    {
        SIZE szImage;
        szImage.cx = m_oAllMonitorMaskFrame.Width() ;
        szImage.cy = m_oAllMonitorMaskFrame.Height();
        return szImage;
    }

    //@����:���ص�����¼���ļ�����·��
    virtual const TCHAR* GetDebugVideoFilePath()const
    {
        return m_strDebugVideoFullPath;
    }

protected:
    BOOL              m_bIsWorking     ;//����У�������б�־
    CAutoCalibrateWnd m_AutoCalibrateWnd   ;//�Զ�У������
    BOOL              m_nCalibratePtNumber ;//У�������Ŀ��

    CHANGECAMERAPARAMSPROC m_fpChangeCameraParams;//�ı�����ͷ�����Ļص�����ָ��
    LPVOID                 m_lpCtx               ;///�ı�����ͷ�����Ļص���������������


    //@����:����Ļ���򻮷�Ϊ������������
    void PartitionScreenArea();


    //@����:��Ļ��������������ĺ���������Ҫ�м�����Ļ����������ͼ��������ת����
    //@����:nImageWidth, ��Ƶͼ����
    //      nImageHeight,��Ƶͼ��߶�
    BOOL OnPostSearchScreenBoundary(int nImageWidth, int nImageHeight);


    //@����:����"��Ļ��������"��״̬��, ʹ֮���С�
    //@����:grayImage, �Ҷ�ͼƬ
    //ESubStateMachineStatus RunSubStateMachine_SearchScreenArea(const CImageFrame& grayImage, ESearchScreenPart eScreenPart);



    //@����: ��ǰ��Ļ�ռ�У�����ݳɹ�
    //@˵��: ������һ����Ļ��У��������Ѿ������һ����Ļ�ˣ������У����֪ͨ�����塣
    void OnMonitorCalibrateDataReady();

    //@����: ��ǰ��Ļ�ɼ�У������ʧ��
    //@˵��: ������Դ���С���趨������Դ���,��������ԣ����Ѿ��������������������У����֪ͨ������У��ʧ�ܡ�
    void OnMonitorCollectDataFail();

    //@����:�ϲ��߽�У�������Ļ�ڲ���У����, 
       void OnPostCalibrate();


    //@����:׼����ǰ��ʾ��Ļ��У������
    void CollectMonitorCalibrateData();


    std::vector<TBorderMarker> m_vecBorderCalibratePts      ;//������ͷ��Ƶ�еı߽�������,���Ľ��Ӧ�ð��մ����Ͻǿ�ʼ,˳ʱ�뷽������(��Ƶ����)
    //std::vector<TBorderMarker> m_vecUpperBorderCalibratePts ;//������ͷ��Ƶ���ϰ벿�߽�������,���Ľ��Ӧ�ð��մ��ϲ����Ͻǿ�ʼ,˳ʱ�뷽������(��Ƶ����)
    //std::vector<TBorderMarker> m_vecLowerBorderCalibratePts ;//������ͷ��Ƶ���°벿�߽�������,���Ľ��Ӧ�ð��մ��²����Ͻǿ�ʼ,˳ʱ�뷽������(��Ƶ����)
    BOOL m_bBorderCalibratrPtsIsValid                       ;//�߽�У����������ȷ��־


    //POINT  m_ptUpperCentroid                           ;//�ϲ�ȫ������
    //POINT  m_ptLowerCentroid                           ;//�²�ȫ������
    //POINT  m_ptScreenCentroid                          ;//ȫ��������

    POINT  m_ptRoationCenter                           ;//ͼ����ת����
    double m_dbRotationMatrix[2][2]                    ;//��У��ͼ������Ϊˮƽ�ʹ�ֱ�������ת����


    CWordFrame  m_oBlackBoardAccFrame                  ;//ȫ������֡�ۼ�ͼ
    CWordFrame  m_oWhiteBoardAccFrame                  ;//ȫ������֡��ͼ
    CImageFrame m_oInitialScreenMask                   ;//��ʼ��Ļ��������ͼ

    CWordFrame  m_oUpperBorderFrame                    ;//�ϲ��߽�����ͼ
    CWordFrame  m_oLowerBorderFrame                    ;//�²��߽�����ͼ


    CImageFrame m_oUpperHalfMaskFrame                  ;//�ϲ�����ͼ
    CImageFrame m_oLowerHalfMaskFrame                  ;//�²�����ͼ
    CImageFrame m_oScreenMaskFrame                     ;//��Ļ����ͼ


    //    CImageFrame m_oLowerSrcFrame                       ;//�²�У��ͼ��
    CImageFrame   m_oPatternFrame                         ;//У��ͼ���ڰ�ͼƬ
    CImageFrame   m_oRawPatternFrame                      ;//ԭʼУ��ͼ���ڰ�ͼƬ����
     
    __int64     m_liImageGrdientEnergy                 ;//ͼƬ�������ݶ�


    std::vector<TImageCalibPoint> m_vecInteriorCalibPoint ;//����Ƶ�п������ڲ�У��������
    int m_nInteriorCalibPtNumber                          ;//�ڲ�У������Ŀ



    //�Զ�У��������
    std::vector<TCalibCoordPair> m_vecBorderCalibMap      ;//��Ļ�߽��ϵ�У����,����Ļ���굽��Ƶ�����ӳ�䡣
    std::vector<TCalibCoordPair> m_vecInteriorCalibMap    ;//��Ļ�ڲ���У����,����Ļ���굽��Ƶ�����ӳ�䡣

    std::vector<TCalibCoordPair> m_aryAllCalibMap         ;//����У���㰴����Ļ������ϵ���, �����ҵ�˳��Ƭ��

    CImageFrame m_oBackgroundFrame                       ;//����ͼƬ֡, 2012/03/08, ��Ļ����ʱ, ����ͷ�п�����ͼ��

    //<<���Ա���
   // CImageFrame m_oDebugBorderSquareFrame;
    //>>���Ա���



    static const int MAX_STAGE_TRY_TIMES = 2;
    static const int DISPLAY_AREA_MININUM_LUMA = 20;//��ʾ������С����

    //<<2019/11/11, ���ڲ�ͬ֡�ʵ�����ͷ���Լ�����Ʒ��ͶӰ����ʾ�ӳ�����Ĵ��ڡ�
    //���ӳٵȴ���֡����Ϊ����ʱ��
    CWaitTimer m_oWaitTimer;
    static const DWORD MAX_NEED_WAIT_TIME = 250;//У���׶�֮����Ҫ�ȴ���ʱ����, ��λ:ms
    static const DWORD IR_LED_FLASH_TIME   = 3000;//����ͨ�ŵ���˸ʱ��

    void InitWaitTimer(DWORD dwTimeMagnification, DWORD dwVideoDisplayDelay);



    //У���׶�ö��ֵ
    enum E_CalibrateStage
    {
        E_AUTO_CALIBRATE_START = 0,//�Զ�У����ʼ
        E_AUTO_CHANGE_BRIGHTNESS_1 = 1,//�Զ���������
        //E_SEARCH_FULL_SCREEN_MASK_WHITEBOARD = 2,//������Ļ����, ��ӡ����
        //E_SEARCH_FULL_SCREEN_MASK_BLACKBOARD = 3,//������Ļ����, ��ӡ����

        E_SEARCH_SCREEN_AREA = 2, //��λ��Ļ����

        E_AUTO_CHANGE_BRIGHTNESS_2 = 4,//�Զ���������

        //E_SEARCH_SCREEN_UPPER_MASK_AREA      = 5,//���ô�ӡ���ϰ벿�߽�׵�ķ�ʽ�������ϰ벿������
        //E_SEARCH_SCREEN_LOWER_MASK_AREA      = 6,//���ô�ӡ���°벿�߽�׵�ķ�ʽ�������°벿������
        E_SEARCH_SCREEN_IMAGE_BOUNDARY = 5,//������Ļͼ��ı߽�

        E_SEARCH_BACKGROUND = 7,//��������

        E_DETECT_ROTATION_1 = 8,//���ò��ô���ϲ��׿�ķ�ʽ, ��Ѱ�ϰ벿������
        E_DETECT_ROTATION_2 = 9,//���ò��ô���²��׿�ķ�ʽ, ��Ѱ�°벿������

        E_PRINT_PATTERN   = 10,//��ӡ��ʾͼ��
        E_WAIT_PATTERN    = 11,//�ȴ�ͼ��
        E_PROCESS_PATTERN                  = 12,//����Уͼ��

        E_START_MASKING                    = 13,//��ʼ��̬����
        E_WAIT_MASKING                     = 14,//�ȴ���̬����
        E_END_MASKING                      = 15,//������̬����

        E_AUTO_CALIBRATE_END               = 16,//�Զ�У������
        E_AUTO_CALIBRATE_STOPPED           = 17,//�Զ�У�����
        E_AUTO_CALIBRATE_UNDEF             = -1,
    };


    E_CalibrateStage m_eCalibrateStage ;//�Զ�У���׶β���
    //E_SubAreaCalibrateStage m_eSubareaCalibrateStage;//������У���׶β���

    //CAviWriter m_oAVIRecorder        ;//AVI¼�ƶ���
    //CAviReader m_oAVIInput              ;//AVI����Դ
    CAVIFile m_oAVIInput;  //AVI����

    BOOL m_bIsSimulatedCalibrating      ;//����ģ��У����
    HANDLE m_hSimulatedCalibrateThread  ;//ģ��У���߳�
    DWORD m_dwSimulatedCalibrateThreadId;//ģ��У���߳�id
    CDebugWindow m_oDebugWindow         ;//�����������


    //int m_nMaskAreaPixelCount ;//�����������سߴ�
    //BOOL m_bCalibrateSuccess ;
    HWND m_hNotifyWnd                  ;//֪ͨ������


    EAutoCalibrateError m_eCalibrateError;

    struct Vec
    {
        POINT ptFrom;
        POINT ptTo;
    };


    UINT m_nCurrentSubAreaIndex;

    ECalibDebugLevel m_eDebugLevel;

    UINT m_uMonitorWidth;//��Ļ���
    UINT m_uMonitorHeight;//��Ļ�߶�



    BOOL m_bDisplayPatternAgainWhenFailed;//����У��ʧ�ܺ�, �´��Ƿ��ٴ���ʾУ��ͼ��,


    //<<2013/03/07, У��ͼ������
    //У������ö��ֵ
    E_AutoCalibratePattern m_ePattern;

    //У��ͼ������
    //CArithSeriesRowPattern m_oCalibratePattern;
    CCirclePattern m_oCalibratePattern;

    BOOL m_bSaveInermediatFile;//�Ƿ񱣴��Զ�У���м��ļ���־
    BOOL m_bRecordVideo;

    BOOL m_bEnableOnlineScreenArea;
    //>>

    //<<2013/08/19
    static const BYTE MINIMUM_GRAY_VALUE = 64;
    //>>
 
    CAtlString m_strImageOutputBaseDir   ;//����ͼƬ�����Ŀ¼
    CAtlString m_strCurrentImageOutputDir;//����ͼƬ���Ŀ¼����
    CAtlString m_strDebugVideoFullPath   ;//��������Ƶ�洢λ��


    //����:���ȿ��ƻص�����
    //@����:bInc, TRUE:�������ȣ�FALSE:��������
    //      absDiff, ���Ȳ�ֵ����ֵ
    //      lpCtxData,����������
    static bool BrightnessCtrlCallback(bool bInc, BYTE absDiff, BYTE nIndex, LPVOID lpCtxData);

    CAutoBrightnessRegulator<BYTE> m_AutoBrightnessRegulator;//�Զ����ȵ�����

    BOOL m_bTestAutoBrightnessCtrlMode  ;//�����Զ����ȵ���ģʽ

    //@����:����������Ļ������ɵ�������ͼ
    void ProduceAllMonitorMaskFrame();

    //@����:���ɹ�㵽��Ļ���ŵ�ӳ������
    void ProduceScreenMap();

    COLORREF m_clrGridHighlight;//�ڰ׷����У��׿����ʾ��ɫ

    //<<2014/04/09
    BOOL  m_bUseOldMethod;//ʹ��ͨ����ʾ���°׿�ķ�ʽ��������ת����ı�־��
    Vec m_vRotation      ;//��Ļͼ����ͼ���е�ƫתλ��ʸ��

    //CDispMonitorFinder m_oMonitorFinder;

    //ÿ����ʾ����Ӧһ��У�����
    //У�����
    struct TCalibResults
    {
        std::vector<TCalibCoordPair> calibMap ;
        CImageFrame                 maskFrame ;
        int                        circleRadius;
    };
    
    //ӳ����λ���ĸ���Ļ�����ӳ�����飬�ûҶ�ͼƬ���ݽṹ���洢��
    //��λ��(x,y)��������ֵv��ÿλ1��ʾ��������Ļ����
    //����,
    //00000001,��ʾ������Ļ0
    //00000011,��ʾͬʱ������Ļ0��1
    //����Ӧ8����
    CImageFrame m_oScreenMap;
    
    std::vector<TCalibResults> m_vecMonitorCalibResults;//У�����

    int m_nDispMonitorId; //��ʾ��Id��
    std::vector<TScreenInfo> m_vecScreenInfos;
    TScreenInfo m_CurrentMonitorInfo;//��ǰ��ʾ����Ϣ
   
    CImageFrame m_oAllMonitorMaskFrame;//���м�����������ͼ���ɸ���������������ͼ��϶���

    TCalibData m_tCalibData;//�����У������

    //<<added by toxuke@gmail.com, 2014/12/09
    TStaticMaskingParams m_tStaticMaskingParams;//��̬���β���
    CStaticMaskFinder    m_oStaticMaskFinder   ;//��̬������
    //>>
    //<<added by toxuke@gmail.com, 2014/12/16


//    AutoCalibrateImageParamsList m_oImageParamsList        ;//�Զ���λ�����б�

    AutoCalibrateParamsList      m_oautocalibrateparamslist; //�Զ�У�������б�

    CMonitorAreaLocator          m_oMonitorAreaLocator     ;//��Ļ����λ��
    CMonitorBoundaryFinder       m_oMonitorBoundaryFinder  ;//��Ļ�߽������
    CAreaSplitter                m_oAreaSplitter           ;//�������򻮷�����
    int m_nTryTimes   ;//У������,��1��ʼ����
    int m_nMaxTryTimes;//����Դ���
    static const int MAX_TRY_TIMES = 4;

     std::vector<PolygonVertices> m_vecPolygons;

public:
    //����ģʽ
    enum EWorkMode
    {
        E_WORK_MODE_AUTO_CALIBRATE,//�Զ�У��ģʽ
        E_WORK_MODEL_AUTO_MASKING ,//�Զ�����ģʽ
    };
    EWorkMode    m_eWorkMode;


    //�Զ����ν׶�
    enum EAutoMaskStage
    {
        E_AUTO_MASK_START                        = 1,//�Զ����ο�ʼ
        E_AUTO_MASK_ADJUST_BRIGHTNESS            = 2,//�Զ�����"��������"
        E_AUTO_MASK_SEARCH_SCREEN_AREA           = 3,//�Զ�����"������Ļ����"
        E_AUTO_MASK_SEARCH_SCREEN_IMAGE_BOUNDARY = 4,//�Զ�����"������Ļ�߽�"
        E_AUTO_MASK_SEARCH_BACKGROUND            = 5,//�Զ�����"������Ļ����"
        E_AUTO_MASK_SERACH_UPPER_HALF_MASK       = 6,//�Զ�����"�ϰ벿����ͼ"
        E_AUTO_MASK_SERACH_LOWER_HALF_MASK       = 7,//�Զ�����"�°벿����ͼ"
        E_AUTO_MASK_STATIC_START                 = 8,//"��̬����"��ʼ
        E_AUTO_MASK_STATIC_MASKING               = 9,//�Զ�����"��̬����"
        E_AUTO_MASK_END                          = 10, //�Զ�����"����"�׶�
        E_AUTO_MASK_STOPPED                      = 11 //�Զ�����"ֹͣ"�׶�
    }m_eAutoMaskStage;


    //@����:��ʼ�Զ�����,���ⲿģ�����
    virtual BOOL StartMasking(const TAutoMaskingParams& autoMaskingParams, const TStaticMaskingParams& staticMaskingParams, int nSrcImageWidth, int nSrcImageHeight);

    //@����:���Զ�����״̬��ͶιͼƬ��
    //@����ֵ:�쳣����FALSE
    BOOL FeedImage_AutoMask(const CImageFrame* pYUVFrame);

    //@����:�Զ����ν����¼�
    void EndAutoMasking();

   EAutoMaskingError m_eAutoMaskingError;//�Զ����ν��״̬ 

   BOOL m_bRestoreLanguageBar;//�Ƿ�ָ���������־

   BOOL m_bRestoreTaskbar;//�Ƿ�ָ�״̬����־



   //���Թ��ߣ�2017/04/11
   void  DebugTool_LoadCalibrateData(
        const char* szCalirateDataFileName,//У�������ļ�
        const double rotationMatrix[2][2],//������ת����
        const POINT& ptCenter);


   //���Թ���,��¼ģ�����
   //<<dbg
   void DbgDumpSimulateParams(const TAutoCalibrateParams& autoCalibrateParams);
   void DbgDumpPatternInfo(int radius, const RECT& rcArea);
   void DbgLoadSimulateParams(LPCTSTR lpszFileName);
   int  m_nDbgFrameCount;

   std::vector<POINT> m_vecDbgOnlineScreenVertices;
   int m_nDbgPatternRadius;
   RECT m_rcDbgMonitor;
   //dbg>
};