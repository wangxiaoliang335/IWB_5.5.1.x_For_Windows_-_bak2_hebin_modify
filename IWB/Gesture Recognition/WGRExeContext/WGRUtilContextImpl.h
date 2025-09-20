#pragma once

class CWGRUtilContextImpl
    : public IWGRUtilContext
{
public:
    CWGRUtilContextImpl(void);
    ~CWGRUtilContextImpl(void);

public:

    virtual void TriggerHIDDown(const POINT *pt);
    virtual void TriggerHIDUp(const POINT *pt);

    virtual void TriggerHMouseWheelEvent(POINT ptCur, int dir, int rollDistance = 120);
    virtual void TriggerVMouseWheelEvent(POINT ptCur, int dir, int rollDistance = 120);   

    virtual void TriggerAltLArrowEvent(const POINT &ptCur);
    virtual void TriggerAltRArrowEvent(const POINT &ptCur);
    virtual void TriggerMiniCurWindow(const POINT &ptCur);
    virtual void TriggerMaxiCurWindow(const POINT &ptCur);
    virtual void TriggerDisplayDesktop(const POINT &ptCur);

    virtual void TriggerShowWndSBS(const POINT &ptCur);
    virtual void TriggerRefreshWnd(const POINT &ptCur);

    virtual void TriggerSwitchWindowEvent(const POINT &ptCur, E_SWHWND_MODE eSWMode = E_SWHWND_NONEMODE);
    virtual void TriggerSwitchWindowHintEvent(POINT ptCur, bool bHint);

    virtual void TriggerDocSaveEvent(const POINT &ptCur);

    virtual void TriggerCloseCurrentWindowEvent(const POINT &ptCur);

    virtual void TriggerMoveWindowHintEvent(POINT ptCur, bool bHint);
    virtual void TriggerMoveWindowEvent(POINT ptCur, POINT ptLast, E_MOVWND_MODE eMode);

public:
    virtual bool Get_TriggerEvent_Permission(E_TRIGGEREVENT eEvent, const POINT &pt);

    /*
    @���ܣ��Ƿ�ر�����������
    */
    virtual bool IsAllGestureClosed();

    /*
    @���ܣ��Ƿ�ر������л������Ƶ�����
    */
    virtual bool IsAllPalmBasedGestureClosed();

    /*
    @���ܣ��Ƿ�ر������л�����ָ������
    */
    virtual bool IsAllFingerBasedGestureClosed();

    virtual bool IsLogPermited();

    virtual int GetMultiOfPalm2Std();

    /*
    //@����:��Ļ�ֱ��ʱ仯�¼���Ӧ����
    //@����:nScreenWidth, �µ���Ļ���
    //      nScreenHeight,�µ���Ļ�߶�
    */
    virtual void OnDisplayChange(int nScreenWidth, int nScreenHeight);

    /*
    //@����:����ߴ�仯ʱ����Ӧ����
    //@����:����ߴ�
    */
    virtual void OnPhysicalLengthChange(float fPhysicalLength);

    /*
    @���ܣ�ӳ��x����ı�׼���뵽��������ʵ�ʾ���
    @����˵�����ñ�׼Ϊ����80Ӣ��װ塢1024 * 768�ֱ���
    */
    virtual int MapStandX2Actual(int xStandard);

    /*
    @���ܣ�ӳ��y����ı�׼���뵽��������ʵ�ʾ���
    @����˵�����ñ�׼Ϊ����80Ӣ��װ塢1024 * 768�ֱ���
    */
    virtual int MapStandY2Actual(int yStandard);

    virtual int GetWeightedSpotMass(const int &nActualMass, const int &nExtRectArea, const int &nStdAreaMulti);

private:
    /*
    @���ܣ�ģ����̰���
    @������wVkArry������������飻
           vkCount, ���������Ŀ
    */
    void SimulateKeyDown(WORD wVk);
    void SimulateKeyUp(WORD wVk);

    //@���ܣ���ȡ���㸸����
    //@������ptCur��λ��            
    HWND GetTopParent(const POINT &ptCur); 
    //@���ܣ���ȡ��һ�����б������ĸ�����
    HWND GetFirstParentWithCaption(const POINT &ptCur);

    void Input2Device(const POINT *ptArry, const int &nActualCount); 

private:
    CWinShellUtility        m_shellUtility;  

    static const int MAXMATCH_NUM = 10;
    static const int MAX_CAMERA_NUMBER = 6;
    //CSmartMatch<MAXMATCH_NUM, 50> m_oSmartMatch;//
    CSmartMatch<MAXMATCH_NUM> m_oSmartMatch;//

    static POINT m_ptsReset[MAXMATCH_NUM];

    bool m_switchWndFail_by_shell;
};
