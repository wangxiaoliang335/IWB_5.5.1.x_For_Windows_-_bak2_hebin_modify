#pragma once
static const int            GLBOARDMAXPENNUM    = 30;
static const int            GLBOARDMAXERASERNUM = 30;

/*
 @���ܣ����ģʽ�µ�����ʶ�𣬶��ģʽ�����������ƣ���ʺͶ���Ƥ����������Ĺ�ߴ���һ����ֵʱ����Ϊ��Ƥ��������Ϊ�ʣ�
 @��ʵ�ʵ�ַ�ʽ����Ϊ���֣�
        1.��ǰģʽΪ����ģʽ��E_DEV_MODE_TOUCHSCREEN��ʱ��ֱ������ıʹ�㼯���豸��
        2.��ǰģʽΪ���ģʽ��E_DEV_MODE_MOUSEʱ��ʱ�����е�һ�������������豸�������ͨ����Ϣ����װ壻
 @����Ƥ����ʵ�ַ�ʽ��ֻ��һ�֣�
        1.ͨ����Ϣ����װ塣
*/
class CSmartMultiPen
{
public:
    CSmartMultiPen();
    ~CSmartMultiPen();

public:
    bool DoRecognition(POINT *points, int nPointCount);

public:
    bool IsTouchPadMode() {return m_bIsTouchPadMode;}
    void SetTouchPadMode(bool isTouchPadMode) {m_bIsTouchPadMode = isTouchPadMode;}

    void SetGLBoadHWnd(HWND hWnd) {m_hWndGLBoad = hWnd;}

    bool IsIdle() {return m_bIsIdle;}

    void ResetSmartMatch();

    void OnSetTouchScreenDimension(int  nPhysicalDiagonalLength, SIZE szLogicalDimension);

private:
    bool                        m_bIsTouchPadMode;      //��ǰ����ģʽ�Ƿ�Ϊ����ģʽ
    bool                        m_bIsIdle;

    HWND                        m_hWndGLBoad;
    
    CSmartMatch<GLBOARDMAXPENNUM>  m_oSmartPenMatch;
    CStrokeFilter<GLBOARDMAXPENNUM>  m_oStrokFilter;//��?
    bool                        m_isSmartMatchReset;
};

class CSmartMultiEraser
{
public:
    CSmartMultiEraser();
    ~CSmartMultiEraser();

public:
    bool DoRecognition(POINT *points, int nPointCount);

public:    
    void SetGLBoadHWnd(HWND hWnd) {m_hWndGLBoad = hWnd;}

    bool IsIdle() {return m_bIsIdle;}

    void ResetSmartMatch();

private:  
    bool                        m_bIsIdle;

    HWND                        m_hWndGLBoad;

    CSmartMatch<GLBOARDMAXPENNUM>  m_oSmartEraserMatch;
    CStrokeFilter<GLBOARDMAXPENNUM>  m_oStrokFilter;//��?
    bool                        m_isSmartMatchReset;
};

class CMultiPenGestureRecognition
{
public:
    CMultiPenGestureRecognition(void);
    ~CMultiPenGestureRecognition(void);

public:
    void SetGLBoadHWnd(HWND hWnd)
    {
        m_multiPen.SetGLBoadHWnd(hWnd);
        m_multiEraser.SetGLBoadHWnd(hWnd);
    }

    void SetTouchPadMode(bool isTouchPadMode)
    {
        m_multiPen.SetTouchPadMode(isTouchPadMode);
    }  

    void SetEableGR(bool isEnable) {m_isEnableGR = isEnable;}

    bool IsMultiPenGRIdle() {return m_multiPen.IsIdle() && m_multiEraser.IsIdle();}

    void ResetSmartMatch();

    void OnSetTouchScreenDimension(int  nPhysicalDiagonalLength, SIZE szLogicalDimension);

public:
    bool DoGestureRecognition(const TLightSpot * aryLightSpots,  const int &nCount, const int &nMultiEraser);

private:
    /*
    @���ܣ����ݹ�������ѹ���Ϊ���ࡪ���ʺ���Ƥ��
    @������[in]aryLightSpots��������飻
           [in]nCount����߸�����
    @��ַ�ʽ������һ���Ĺ���ѹ�߷�Ϊ���ࡪ���ʺ���Ƥ���� 
               �������aryLightSpots[i].mass > nMultiEraser * aryLightSpots[i].lStdSpotAreaInVideo����aryLightSpots[i]Ϊ��Ƥ����
               ��������Χһ����Χ�ڵĹ�ߺϲ����������ַ�ʽ���ҳ�����е�������Ƥ������������Ϊ��
    */
    bool SplitLightSpotByMass(const TLightSpot * aryLightSpots,  const int &nCount, const int &nMultiEraser);

private:    
    CSmartMultiPen          m_multiPen;
    CSmartMultiEraser       m_multiEraser;

    POINT                   m_ptSplitPens[GLBOARDMAXPENNUM];
    int                     m_nActualSplitPenCount;

    POINT                   m_ptSplitErasers[GLBOARDMAXPENNUM];
    int                     m_nActualSplitEraserCount;

    bool                    m_isEnableGR;

    POINT                   m_ptHistoryEraser[GLBOARDMAXERASERNUM];
    int                     m_nHistoryErasers;    

    //std::map<UINT>          m_lastEraserMap;

    /*static  int             m_*/
};
