#pragma once

#ifdef WINGESTURERECOGNITION_EXPORTS
#define EASI_WGR_CLASS __declspec(dllexport)
#else
#define EASI_WGR_CLASS __declspec(dllimport)
#endif

#include "IWGRUtilContext.h"
#include "../LightSpot.h"

class EASI_WGR_CLASS CWindowsGestureRecognition
{
public:
    CWindowsGestureRecognition(void);
    ~CWindowsGestureRecognition(void);

public:
    //@���ܣ����ù�߱����ĺ���
    void  SetSpotProportion(int nSpot)
    {
        m_nSpotProportion = nSpot;
    }

    //@���ܣ��õ���������ĺ���
    void  SetMultiEraser(int nEraser)
    {
        m_nMultiEraser = nEraser;
    }

    //@���ܣ�������ر���־����
    void SetLogPermitted(bool bPermit)
    {
        m_bLogPermited = bPermit;        
    }

public:
    //����ʶ����
    bool Start();

    //ֹͣʶ����
    bool Stop();

    //�������ܵ�ִ����
    void AttachExceContext(IWGRUtilContext *pContext);

    //�жϹ��ܵ�ִ����
    void DetachExceContext();

    //�ж��Ƿ����Windows Gesture Recognition
    bool DetermineWhethertoEnterWGR(const TLightSpot * aryLightSpots,  int nCount);

    //bool DeterminationShape(const TLightSpot * aryLightSpots,  int nCount);


    ////ִ��ʶ���ڵ���DoRecongition֮ǰ���мǵ���AttachExceContext����ʶ�����ִ����صĹ���
    //bool DoRecongition(const TLightSpot * aryLightSpots,  int nCount);

    //ִ��ʶ���ڵ���DoRecongition֮ǰ���мǵ���AttachExceContext����ʶ�����ִ����صĹ���
    bool DoRecongition();

    /*
        @���ܣ����ƥ���Ĵ���
        @������uId�������id
               ptPos,�����λ��
               matchState�������ƥ��״̬����0������ƥ��ʧ�ܣ�1��ƥ��ɹ���2���¼���ĵ㡣
    */
    void AddTouchAfterMatch(const UINT &uId, const POINT &ptPos, int matchState);

private:    
    IWGRUtilContext             *m_pContextImpl        ;//����ʶ����ִ����       
    int                         m_nMultiEraser       ;//��߱���Ϊ�ǰ��ʱ, ��ߵ������������޵����֮��
    int                         m_nSpotProportion    ; //�����Ӧʱ����С������

    bool                        m_bLogPermited       ; //��������־

    int              		m_curState           ;//��ǰ״̬

    //EWGR_GESTURE_SHAPE          m_curGestureShape    ;//��ǰ��״
};
