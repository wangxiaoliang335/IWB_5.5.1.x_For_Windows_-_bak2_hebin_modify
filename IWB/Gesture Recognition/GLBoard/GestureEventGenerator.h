#pragma once

//#include "assert.h"
//#include <vector>
//
////#include "headers.h"
//#include "WGestureRecPublic.h"
#include <GlobalDefine.h>
#include "GLBoardPublic.h"

/////��ʱ���ͣ����һ�����ϵ���Ĵ���
class CLongTimeClickTrigger
{
public:
	CLongTimeClickTrigger()
	{
		m_ptRangeCenter.x = (std::numeric_limits<long>::min)();
		m_ptRangeCenter.y = (std::numeric_limits<long>::min)();
		m_dwRangeUpdateTime = GetTickCount();
	}
	void Reset()
	{
		m_ptRangeCenter.x = (std::numeric_limits<long>::min)();
		m_ptRangeCenter.y = (std::numeric_limits<long>::min)();
		m_dwRangeUpdateTime = GetTickCount();

	}

	///@���ܣ�������λ���Ƿ���һ�����ڳ�ʱ���ͣ��
	///@����ֵ��TRUE ��������ˣ�˵���Ǿ۹�Ƶ���Ӧ
	////////////FALUSE,δ������˵���ǰ������Ӧ
	BOOL DetectLongTimeStay(const POINT& ptNow)
	{
		LONG dx = abs(ptNow.x - m_ptRangeCenter.x) ;
		LONG dy = abs(ptNow.y - m_ptRangeCenter.y) ;

		///˵���������λ���ڷ�Χ֮��
		if (dx > LONGTIME_CLICK_RANGE || dy > LONGTIME_CLICK_RANGE)
		{
			m_ptRangeCenter = ptNow ;
			m_dwRangeUpdateTime = GetTickCount();
		}
		else
		{
			DWORD dwNow = GetTickCount();
			if ( (dwNow - m_dwRangeUpdateTime) > LONGTIME_EVENT_TRIGGER_TIMER)
			{
				return TRUE;
			}
		}
		return FALSE;
	}

protected:
	POINT m_ptRangeCenter;
	static const DWORD LONGTIME_CLICK_RANGE  = 15;

	DWORD  m_dwRangeUpdateTime;
	static const DWORD LONGTIME_EVENT_TRIGGER_TIMER = 1500u;
};


////////////////////����˫�����ʵ��
class CHandDoubleClickChecker
{
public:
	CHandDoubleClickChecker()
	{
		m_ptLastPos.x = (std::numeric_limits<long>::min)();
		m_ptLastPos.y = (std::numeric_limits<long>::min)();
		m_pLastPosTime = 0;
		m_StartHandState = TRUE  ;
	}

	void SetHandDoubleClickStartState(BOOL start)
	{
		m_StartHandState = start ;

	}


	BOOL HandDoubleClick(const POINT& ptPos/*,const POINT& preptPos*/)
	{
	//	AtlTrace(_T("ptPos.x = %d , ptPos.y  = %d \r\n") ,ptPos.x,ptPos.y );
	//    AtlTrace(_T("m_ptLastPos.x = %d , m_ptLastPos.y = %d \r\n") ,m_ptLastPos.x,m_ptLastPos.y );
		if (m_StartHandState)
		{
			DWORD dwNow = GetTickCount();

			m_StartHandState = FALSE    ;

			if ((dwNow - m_pLastPosTime) < DOUBLE_CLICK_MAX_INTERVAL)
			{
				LONG dx = abs(ptPos.x - m_ptLastPos.x) ;
				LONG dy = abs(ptPos.y - m_ptLastPos.y) ;

				if (dx < DWORD_HAND_STATIC_MAX_SHIFT && dy <DWORD_HAND_STATIC_MAX_SHIFT)
				{
					return TRUE;
				}
			}

			m_ptLastPos = ptPos;
			m_pLastPosTime = dwNow;
		}


		return FALSE;

	}
protected:

	POINT  m_ptLastPos     ;
	DWORD  m_pLastPosTime  ;
	BOOL   m_StartHandState    ;
	static const UINT DOUBLE_CLICK_MAX_INTERVAL = 1200u;
	static const DWORD DWORD_HAND_STATIC_MAX_SHIFT  = 20;

};


//��ָ˫�����ʵ��
class CFingerDoubleClickCHecker
{
public:
	CFingerDoubleClickCHecker()
	{
		m_ptLastPos.x = (std::numeric_limits<long>::min)() ;
		m_ptLastPos.y = (std::numeric_limits<long>::min)() ;
		m_pLastPosTime = 0 ;
		m_StartState = TRUE ;
	}

	void SetDoubleClickStartState(BOOL start)
	{
		m_StartState = start ;

	}

	BOOL FingerDoubleClick(const POINT& ptPos)
	{
		if (m_StartState)
		{
			DWORD dwNow = GetTickCount();
			m_StartState = FALSE ;

			if ((dwNow - m_pLastPosTime) <DOUBLE_CLICK_MAX_INTERVAL)
			{
				LONG dx = abs(ptPos.x - m_ptLastPos.x ) ;
				LONG dy = abs(ptPos.y - m_ptLastPos.y ) ;

				if (dx < DWORD_HAND_STATIC_MAX_SHIFT && dy < DWORD_HAND_STATIC_MAX_SHIFT)
				{
					return TRUE ;
				}
			}

			m_ptLastPos = ptPos ;
			m_pLastPosTime = dwNow ;
		}

		return FALSE ;

	}

protected:
	POINT m_ptLastPos;
	DWORD m_pLastPosTime ;
	BOOL  m_StartState   ;
	static const UINT DOUBLE_CLICK_MAX_INTERVAL = 350u ;
	static const DWORD DWORD_HAND_STATIC_MAX_SHIFT = 10 ;

};

////////////////////////////////////////////////////////////////////////////
////��������ƶ���ʽ
//typedef enum
//{
//    E_2TOUCHS_MOVES_FALSE       , //������
//    E_2TOUCHS_MOVES_NONE        , //δ�ƶ�
//    E_2TOUCHS_MOVES_SAME        , //ͬ���ƶ�������ʱ���ƶ���ʽ
//    E_2TOUCHS_MOVES_ROAM        ,
//    E_2TOUCHS_MOVES_SELECTION   , 
//    //E_2TOUCHS_MOVES_BACKWARD    , //�����ƶ����Ŵ�ʱ���ƶ���ʽ
//    //E_2TOUCHS_MOVES_OPPOSITE    , //�����ƶ�����Сʱ���˶���ʽ
//    E_2TOUCHS_MOVES_ZOOM        , //�����ƶ�
//    E_2TOUCHS_MOVES_RORATE      , //��ת�ƶ�
//}E_2TOUCHS_MOVES;

class CGestureEventGenerator
{
public:
	CGestureEventGenerator(void);

	~CGestureEventGenerator(void);
	//@���ܣ��õ���߱����ĺ���
	void  SetSpotProportion(int nSpot);
	//@���ܣ����ð�������ĺ���
	void  SetMultiEraser(int nEraser);
    int  GetMultiEraser();
	//@���ܣ����ְװ崰��ĺ���
	BOOL  FindTboardWnd(const TLightSpot * aryLightSpots,  const int &nCount);

    //
    bool DoDetection(const TLightSpot * aryLightSpots,  const int &nCount/*, POINT* pptMouse = NULL*/);   

    //ʹ��GLBoard����ʶ��
    void EnableGestureRecognition(bool bEnable);
    bool IsGREnabled();

    //������־�������
    void SetLogPermit(bool bPermit) {m_isLogPermit = bPermit;}

    //
    bool IsInputInGLBorad() {return m_isInuptInGLBoard;}

    //
    void SetMultiPenPermission(bool bPermission) {m_isMultiPenPermited = bPermission;}
    bool IsMultiPenPermitted() {return m_isMultiPenPermited;}

    //
    void SetIsTouchPadMode(bool bTouchPadMode) {m_isTouchPadMode = bTouchPadMode;}
    bool IsTouchPadMode() {return m_isTouchPadMode;}

    void SetIsPenTouchDevice(bool bPenTouchDevice) {m_isPenTouchDevice = bPenTouchDevice;}

    bool IsMultiPenMode() {GetGLboardButtonState(); m_isMultiPenMode = (StrStatus ==_T("multipen")); return m_isMultiPenMode;}

    void ResetSmartMathch();

    //@����:���ô����ĳߴ��¼���Ӧ����
    //@����:nPhysicalDiagonalLength, ��Ļ�ߴ�,�Խ��߳���, ��λ:����
    //      szLogicalDimension, ��Ļ�߼��ߴ磬��λ:����
    void OnSetTouchScreenDimension(int  nPhysicalDiagonalLength, SIZE szLogicalDimension);

public:
	int      m_nSpotProportion    ; //�����Ӧʱ����С������
	int      m_nMultiEraser       ;//��߱���Ϊ�ǰ��ʱ, ��ߵ������������޵����֮��
    int      m_CollectCount       ;//�ڽ�������֮����л�ʱ����Ҫ�Ȳɼ���֡���ж��Ƿ������Ҫ�л��������ǲ����ĸ��ŵ㡣

	BOOL     m_bGetstureDone      ;//TRUE:�����ж����,����ģ������¼�; FALSE�������ж���, �����ܹ�ģ������¼���

    bool     m_bHadResetAllGRState; //�Ƿ��Ѿ�ͨ������0�������������е�GLBoard����

	POINT    m_ptInputCoord;       //�ڽ��вɼ���ʱ��������¼ǰһ�ε�����

	POINT     m_Start_MousePoint[10];   //�ڽ�����껬�ֵ��ж�ʱ��������¼�տ�ʼʱ����λ�á�'0'x����С������ֵ��    '1'//x����������ֵ 
	POINT     m_Start_MidPoint     ;   //��¼��ʼʱ��������֮����м�����ֵ
	POINT     m_Last_Multipoint[10] ;   //�ڽ�����껬�ֵ��ж��У���¼��һ�ε������ֵ
	POINT     m_MultiPoint[10]     ;    //�м���ʱ�洢���������� ��
	int       m_Pre_CollectCount    ;    //Ԥ�ɼ�����Ŀ��ǰ�漸�εĲɼ������ã��ȵ������ȶ��������ڲ��á�

	BOOL   m_bCollectWheelState;       //�ڽ�����껬�ֵ��ж��С����ڲɼ���״̬��
	int    m_nCollectTotileCount;      //�ڽ�����껬�ֵ��ж��С��ɼ����ܵ���Ŀ��

	BOOL  m_bMouseWheelState;          //������������״̬��
	int   m_nOneMissCount   ;          //һ��������������
	int   m_nOnePointCount  ;          //��¼һ����Ĳɼ��ĸ������������һ��������ʱ��˵����һ����ָ�ڲ������������2������2�����ϵĵ�Ļ���˵���Ǹ��Ź�ߡ� 

    LONG  m_lFirstDistance;            //�ʼ������֮��ľ��롣
	LONG  m_lSecondDistance;           //�ڶ��μ�������֮��ľ��룻
	int   m_nDistanceCount_Big;        //����ǵ�֮��ľ�����������ۼ�ֵ
	int   m_nDistanceCount_Small;      //����ǵ�֮��ľ����С�������ۼ�ֵ��
	int   m_nMobileDistance;           //�ڽ��л��ֵļ����У����ǻ����ƶ��ľ��롣

	HWND        m_hwnd;                           //�װ�Ĵ�����
    bool        m_isGLBoardProcExsit;                //�װ�����Ƿ����
	CString     StrStatus;                        //��¼�װ尴ť�ĵ�ǰ״̬
	CString     PreStrStatus;                     //��¼�װ尴ťǰһ��״̬
	POINT       ptErase;
	int  m_nCollectMissCount;                           //��¼�ɼ����Ĳ���ȷ���ݵĴ�����

	CLongTimeClickTrigger    m_oLongTimeClickTrigger;
	CHandDoubleClickChecker  m_oHandDoubleClickChecker;
	CFingerDoubleClickCHecker m_oFingerDoubleClickChecker ;
	BOOL  IsMouseLeftEdge  ;     //���ִ��ߵ�ʱ������λ���Ƿ�����Ļ�����Ե��
	int m_nScreenX;
	int m_nScreenY;

	int   m_Vsumx[2] ;          //��¼x����ƶ����롣
	int   m_Vsumy[2] ;          //��¼Y���ƶ��ľ��롣

    //
    bool m_isGREnabled;      //�Ƿ���������ʶ��
    bool m_isMultiPenPermited; //�Ƿ�������
    bool m_isTouchPadMode;      //��ǰ�Ƿ��Ǵ���ģʽ����ҪһЩ�����ڴ�����ֱ��ʹ��΢��ģ�����ͼƬ����

    bool m_isPenTouchDevice; //�ʴ�ģʽ����֧������ʶ��

    bool m_isMultiPenMode;

    bool m_isMouseUp;
    POINT   m_ptResponse;      //ʶ�����Ҫ�����Ӧ��
    bool m_isVMouseSelfCotrol;

    //<added reason:���ҵ�GLBoard����GLBoard���ƽ������ڵ���ģʽʱ���ͻ�Ѳ����豸������SpotListProcessor��
    //����SpotListProcessor�����豸֮ǰ�ֻ����Windows���ƣ������ͻ���GLBoard���󴥷�windows����
    bool m_isInuptInGLBoard;      //�Ƿ��ҵ���GLBoard

    bool m_isLogPermit;

    //���ܹ��ƥ��
    static const int MAXMATCH_NUM = MAX_SUPPORT_TOUCH_COUNT;
    static const int MAX_CAMERA_NUMBER = 6;
    CSmartMatch<MAXMATCH_NUM> m_oSmartMatch;//
    CStrokeFilter<MAXMATCH_NUM>  m_oStrokFilter;//�ʼ�������
    CMultiPenGestureRecognition m_GRMultiPen; //���ģʽ������ʶ��

    bool m_isSmartMatchReset;

    //bool m_isContinueWhenNoInput ;  //���ж��Ƿ���GLBoard���룬�������Ϊ0�����˱�־Ϊfalse���򷵻�δ��GLBoard�����룬���򣬷��أ����������ǰ�����룻

    int m_conNonOneTouch_when_penstate;

	enum E_ERASERTOPEN_STATE               //��¼���ǱʺͰ��ת��ʱ�ɼ���״̬�ı仯��Ҳ�ǲɼ����β���ȷ���Ƿ�����Ҫ�ĵ㡣
	{
		E_START,           // ��¼��ʼ
		E_IS_FINSHED,      //��¼�Ƿ����
	}m_eRecordState;

	enum E_COLLECT_STATE    //�ɼ�
	{
		E_COLLECT_START ,
		E_COLLECT_CONTINUE ,
		E_COLLECT_END ,
	}m_eCollectState;

	enum  E_MOUSEWHEEL_STATE       //�ɼ�ʱ��״̬�仯���ɼ����β���ȷ���Ƿ�����Ҫ�ĵ㡣
	{
		E_GESTUREEVENT_START_COLLECT,
		E_GESTUREEVENT_CONTINUE_COLLECT,
		E_GESTUREEVENT_DATA_CALCULATE,
	}m_eMouseWheelState;

  //  //added by toxuke@gmail.com,  2014/01/08
  //  //�װ��������״̬��ö��ֵ
  //  typedef enum 
  //  {
		//E_WBS_IDLE             ,    //��״̬
		//E_WBS_ERASER           ,    //���״̬
		//E_WBS_PEN              ,    //��״̬
		//E_WBS_ROAM             ,    //����״̬
		//E_WBS_SPOTLIGHT        ,    //̽�յ�״̬
		//E_WBS_RETURNTODISKTOP  ,    //��������״̬
		//E_WBS_NEXTPAGE         ,    //��һҳ״̬
		//E_WBS_PROPAGE          ,    //��һҳ״̬
		//E_WBS_ZOOM             ,    //����
		//E_WBS_RORATE           ,    //��ת
		//E_WBS_MASK             ,    //Ļ��
		//E_WBS_SELECTION        ,    //ѡ��ʻ�����

  //  }E_WBS;    

    E_WBS   m_eWBS;
    E_WBS   m_ePreWBS;
    //    

private:
       //@����:�ڰװ�����У��ʺͰ��֮���ת������
       //@����:aryLightSpots,���մ�С�Ӵ�С���еĹ������
       //      nCount, ��߸���
       //      pptMouse, ָ�򱣴�"�Ƽ����"λ�õ��ڴ�ռ��ָ��
       BOOL     EraserTransPen(const TLightSpot * aryLightSpots,  int nCount/*, POINT* pptMouse = NULL*/);  
       //�õ��װ����ڲ��������Ǹ���ť��
       void     GetGLboardButtonState();
       //@���ܣ������д�ĺ���
       BOOL     MultiplePenExecute(const  TLightSpot *aryLightSpots, int &nCount);	
       //@���ܣ���ϵͳ�����£����ֹ���ʱ��������Ӧ����
       BOOL     DetectMouseWheelAction(TLightSpot * aryLightSpots, int nCount);
       //@���ܣ���ȡ�����ڴ��е����ݵĺ���
       CString  ReadShareMemory();
       //@���ܣ��õ���ָ����ʱ�ľ���ĺ���
       int      GetMouseWheelLength();


private:
        /*
        @���ܣ������������ʱ�������ƶ�����
        @������matchResult, Match2Touchs���ؽ��
        @������pptMouse, ָ�򱣴�"�Ƽ����"λ�õ��ڴ�ռ��ָ��
        @����ֵ����E_2TOUCHS_MOVES����
        */
        E_2TOUCHS_MOVES Calc2TouchsMoveDirection(bool matchResult = false, POINT* pptMouse = NULL);
        
        /*
        @���ܣ���������ƶ�ƥ��
        @��������EraserTransPen
        @����ֵ��true,���ƥ�䣻false��ƥ��ʧ��
        */
        bool Match2Touchs(const TLightSpot * aryLightSpots,  int nCount);

        bool LeastDistanceMatch(const TLightSpot * aryLightSpots, int nCount, POINT &pt);

        void Restore2OldPenState(); 

        void CancelMouseButtonUp(const TLightSpot * aryLightSpots,  int nCount);
        void SimulHIDReset(const POINT &pt);
        void SimulHIDDown(const POINT &pt);

        bool IsReponseByMyself(E_WBS eState)
        {            
             return !(eState == E_WBS_IDLE || eState == E_WBS_PEN || eState == E_WBS_ERASER ||eState == E_WBS_SPOTLIGHT || eState == E_WBS_MASK);
        }

        void Input2Device(const POINT *ptArry, const int &nActualCount, bool needMatch = true);        
};