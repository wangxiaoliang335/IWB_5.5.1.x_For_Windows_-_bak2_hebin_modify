#pragma once
#include "CalibrationDataDef.h"
#include "../inc/AVI/AVIWriter.h"

#include <vector>
#include "../inc/DispMonitorFinder.h"
#include <queue>
/////////////////////////�õ�����ߵĽṹ��
//��߲�������
typedef struct TLightSpotSampleSize
{
	POINT ptCenter;    //���ĵ������
	LONG  lSize;       //��ߵ�����Ĵ�С
	float fPercentage ;

}LightSpotSampleSize;


//����һ���������Ĺ�����ߴ���Ϣ��һά���顣
typedef std::vector<LightSpotSampleSize> MONITOR_LIGHTSPOT_SAMPLE_SIZE;
struct  TMonitorLightSpotSampleSize
{
    RECT rcMonitor;
    MONITOR_LIGHTSPOT_SAMPLE_SIZE vecSampleSize; 
};

//

//�������м������Ĺ�����ߴ���Ϣ�Ķ�ά����
typedef std::vector<TMonitorLightSpotSampleSize> ALL_LIGHTSPOT_SAMPLE_SIZE;


typedef struct 
{
	double x;
	double y;
}DoublePoint;

//��߲���λ�ù�һ����(0,1)��Ĳ�����Ϣ
typedef struct TNormalizedLightSpotSampleSize
{
	DoublePoint dptNormalized;
	LONG lSize;
	FLOAT fPercentage;

}NormalizedLightSpotSampleSize;

typedef std::vector<NormalizedLightSpotSampleSize> MONITOR_NORMALIZED_SMAPLE_SIZE;
typedef std::vector<MONITOR_NORMALIZED_SMAPLE_SIZE> NORMALIZED_SMAPLE_SIZE;


//����������Ϣ
typedef struct 
{
    POINT ptDisplay         ;//��ʾλ������

    SIZE  size              ;       //�������ŵĳߴ�ߴ�
    COLORREF clrSampleBefore;       //����ǰʮ�ֵ���ɫ
    COLORREF clrSampleAfter ;       //�������ʮ�ֵ���ɫ
	COLORREF clrSampleAdjustment;   //����������ʮ�ֵ���ɫ
    BOOL     bSampled       ;//�Ѳ�����־
    POINT ptCenter          ;//˫���Բ�ֵ��ʹ�õ�����,Ϊ�˸�׼ȷ�ؽ���˫���Բ�ֵ��
                             //���ǽ�������Ļ�߽�Ĳ���λ��Ų����Ļ�߽紦��
	RECT    rcRect          ; //��������ڵ�����

}TSampleSymbol;


//��߲���ģʽ
//enum ESampleCollectPattern
//{
//    E_SAMPLE_COLLECT_PATTERN_9_Points, //9�����������ģʽ�Ĳ���
//    E_SAMPLE_COLLECT_PATTERN_15_Points,//15�������˫��ģʽ�Ĳ���
//};

struct ValidParamsInfo
{
	RECT Area;
	int mass;
};

struct SpotManualCollectInfo
{
	TPoint2D ptPos;
	RECT  rcArea;
	int   mass;
};
class  SpotManualCollectInfoManager
{
public:
	void Put(const SpotManualCollectInfo& data)
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csForQueue);
		m_queManualCollectInfo.push(data);
	}

	 BOOL Fetch(SpotManualCollectInfo* data)
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csForQueue);
		if(m_queManualCollectInfo.empty())
		{
			return FALSE;
		}
		if (data)
		{
		    *data = m_queManualCollectInfo.front();
		}
		m_queManualCollectInfo.pop();
		return TRUE;
	}
protected:
	CComAutoCriticalSection m_csForQueue;
	std::queue<SpotManualCollectInfo> m_queManualCollectInfo;
};

class CCollectSpotSize
{
public:
	CCollectSpotSize(E_COLLECTSPOT_MODE eCollectSpotMode = COLLECTSPOT_MODE_COLLECT, HWND hOwner = NULL);   //���캯��
	~CCollectSpotSize(void);                //��������

    //@����:��ʼ��߲���
    //@����:pMonitors, ��Ļ��������
    //      nMonitorCount, ��Ļ����
    //BOOL  StartCollectSpotSize(const RECT* pMonitorAreas, int nAreaCount, HWND hNotifyWnd, ESampleCollectPattern ePattern = E_SAMPLE_COLLECT_PATTERN_9_Points);
    BOOL  StartCollectSpotSize(const RECT* pMonitorAreas, int nAreaCount, HWND hNotifyWnd, int nSampleNumEachRow, int nSampleNumEachCol,int nSensorId, TCHAR *lpszbuf, int nSensorCount);

    

    //@����:�ж��Ƿ������߲���    
    //@����ֵ:TRUE, ��������
    //        FALSE, ��������
    BOOL AbortCollectSpotSize();//����

    const ALL_LIGHTSPOT_SAMPLE_SIZE& GetScreenSamples() const {return m_ScreenLightspotSample;}


    //@����:��Ļ�ֱ��ʱ仯�¼���Ӧ����
    //@����:nScreenWidth, �µ���Ļ���
    //      nScreenHeight,�µ���Ļ�߶�
    void OnDisplayChange(int nScreenWidth, int nScreenHeight);


    //added by toxuke@gmail.com, 201/5/03/09
    //<<�жϹ�߲��������Ƿ�ɼ�
    BOOL IsVisible() { return ::IsWindowVisible(m_hWnd);}
    //>>

    //<<added by toxuke@gmail.com, 201/5/03/09
    //@����:�豸��ʧʱ���¼���Ӧ����
    void OnDeviceMissing();
    //>>

	BOOL LoadCollectSpotPoint();
	BOOL SaveCollectSpotPoint();

protected:
	BOOL Create();                          //��������

	void FullScreen(BOOL bFull);
	BOOL IsFullScreen(){ return m_bFullScreen;}

	HWND GetOwnerWnd()const;
	void SetOwnerWnd(HWND hWnd);
	

	static LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);   //�ص�����
	LRESULT  InternalWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);       //���к���
	void  DrawCross(HDC hDC, const POINT& ptCenter, COLORREF color,  const SIZE& size); //���������
	void  DrawLine(HDC hDC, const POINT& ptStart ,const POINT& ptEnd, COLORREF color);
public:

       HWND      m_hWnd;

protected:

	HINSTANCE m_hInstacne;

	HWND      m_hOwner;                                    //�����ߴ���,���������¼�֪ͨ

	const TCHAR*   m_szClassName;
	WINDOWPLACEMENT   m_OldWndPlacement;
	
	BOOL        m_bFullScreen;

	static const int MAX_SPOT_SAMPLING_TIMES  = 30;  //һ�����ϲɼ�12����ͬһ�����ϡ�˵��������Ҫ�ɼ�������Ϣ
	static int s_nSpotSettingCount;


//int DoubleScreenMergeCount ; //˫ƴ�ͷ�˫ƴ�ɼ��ĸ�����һ����

//	CAviWriter  m_oAVIRecorder;          //����ͼ��Ķ���


protected:
    //����
    
     //��ʼ������λ��
    void InitSamplePosition(const RECT &rcMonitor);

    //����
	int                               m_nSampleTimes     ;//������Ĳ�������������

//	POINT                             m_ptSampleStartPos  ;//����������,��һ�β���ʱ��λ��

    RECT                              m_rcCurrentMonitor  ;//��ǰ��ʾ������Ļ����
    int                               m_nCxVScreen        ;//��������Ŀ��
	int                               m_nCyVScreen        ;//��������ĸ߶�
	int                               m_nSampleNumEachRow ;//ÿ�еĲ�������
	int                               m_nSampleNumEachCol ;//ÿ�еĲ�������
    int                               m_nCurrentSampleNo  ;//��ǰУ�����ű��,��0��ʼ
	std::vector<TSampleSymbol>        m_vecSampleSymbols  ;//У��������Ϣ

	//std::vector<POINT>                m_vecStandardCross;//��׼�������
	//std::vector<LightSpotSampleSize>  m_vecMaxSpot      ;//�õ����õĵ������������

	std::vector<ValidParamsInfo>         m_vecSampleData     ;//����MAX_SPOT_SAMPLING_TIMES�β��������ݵ�����

	int  m_nSensorID ;                 //�ڽ��еڼ�����sensor

    ALL_LIGHTSPOT_SAMPLE_SIZE         m_ScreenLightspotSample;//���м������Ĺ�߲�����, һ����������Ӧһ�������


	std::vector<RECT>                m_vecMonitorAreas    ;//��Ļ��ʾ����Ϣ
    int                              m_nCurMonitorAreaId  ;//��ǰ��ʾ��Id

	int                             m_nSelectDragIndex    ;//ѡ����ק�ĵ������
	std::vector<POINT>              m_vecConfigCross;//��׼�������

	enum E_SPOTSAMPLING_STATE
	{
        E_ALL_SPOT_SAMPLING_START         , //��ʼ���й�ߵĲɼ�
        E_CURRENT_SYMBOL_SAMPLING_START   , //��ǰ���Ŵ��Ĺ�߲�����ʼ
        E_CURRENT_SYMBOL_CONTINUE_SAMPLING, //��ǰ���Ŵ��Ĺ�߲ɼ���
        E_CURRENT_SYMBOL_SAMPLING_END     , //��ǰ���Ŵ��Ĺ�߲ɼ�����
        E_ALL_SPOT_SAMPLEING_END          , //���й�ߵĲɼ�����
	}m_eSpotSamplingState;

    //E_COLLECTSPOT_MODE m_eSpotSamplingMode    ;//��߲ɼ�ģʽ,�ֶ�,ģ���
    //ESampleCollectPattern m_eSpotSamplePattern;//

    int    m_nSymbolHorzInterval ;//У������ˮƽ���
    int    m_nSymbolVertInterval ;//У�����Ŵ�ֱ���
    

    int    m_nLightSpotDitherOffsetX;//ˮƽ����Ķ���ƫ�ƾ���
    int    m_nLightSpotDitherOffsetY;//��ֱ����Ķ���ƫ�ƾ���

	TCHAR     CollectSpotDragPath[MAX_PATH];
	int       m_nCollectSensorCount;                 //��Ҫ�ɼ���ߵ�sensor����
	BOOL      m_ShowFlag           ;

	public:
	  static SpotManualCollectInfoManager m_oSpotManualCollectInfoManager;
	
};


