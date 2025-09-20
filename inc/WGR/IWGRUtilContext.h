#pragma once

#include "WGRPermissionPublic.h"

//�����л����ö����
typedef enum
{
    E_SWHWND_NONEMODE,   //δ���巽ʽ
    E_SWHWND_STARTMODE, //��ʼ�����л�
    E_SWHWND_EXECMODE,  //ִ�д����л�
    E_SWHWND_EXELMODE,  //�����л�����
    E_SWHWND_EXERMODE,  //�����л�����
    E_SWHWND_OKMODE,    //ȷ��
    E_SWHWND_CANCLEMODE,//ȡ���л�
    E_SWHWND_ENDMODE    //���������л�
}E_SWHWND_MODE;

//�����ƶ����ö����
typedef enum
{
    E_MOVWND_NONMODE,
    E_MOVWND_STARTMODE,
    E_MOVWND_EXECMODE,
    E_MOVWND_ENDMODE,
}E_MOVWND_MODE;


//����ʶ�������ִ����ع��ܵĽӿ�
//��Ҫ��һЩ�ص�����
class IWGRUtilContext
{
public:
    /*
    @���ܣ����������豸
    @����������λ��
    */
    virtual void TriggerHIDDown(const POINT *pt) = 0;
    /*
    @���ܣ����������豸
    @����������λ��
    */
    virtual void TriggerHIDUp(const POINT *pt) = 0;


    /*
    @���ܣ��������ˮƽ�����¼�
    @������rollDistance���������룬Ĭ��Ϊ120;
           dir����������-1����1���ң��������ƶ�
    */
    virtual void TriggerHMouseWheelEvent(POINT ptCur, int dir, int rollDistance = 120) = 0;
    /*{
        return;
    }*/

    /*
    @���ܣ����������ֱ�����¼�
    @������rollDistance���������룬Ĭ��Ϊ120;
    dir����������-1���ϣ�1���£��������ƶ�
    */
    virtual void TriggerVMouseWheelEvent(POINT ptCur, int dir, int rollDistance = 120) = 0;
    /*{
        return;
    }*/

    /*
    @���ܣ�����Alt + Left Arrow�¼�
    @������ptCur�����λ��
    */
    virtual void TriggerAltLArrowEvent(const POINT &ptCur) = 0;

    /*
    @���ܣ�����Alt + Right Arrow�¼�
    @������ptCur�����λ��
    */
    virtual void TriggerAltRArrowEvent(const POINT &ptCur) = 0;

    /*
    @���ܣ�������С����ǰ�����¼�
    @������ptCur�����λ��
    */
    virtual void TriggerMiniCurWindow(const POINT &ptCur) = 0;

    /*
    @���ܣ�������󻯵�ǰ�����¼�
    @������ptCur�����λ��
    */
    virtual void TriggerMaxiCurWindow(const POINT &ptCur) = 0;

    /*
    @���ܣ�������ʾ�����¼�
    @������ptCur�����λ��
    */
    virtual void TriggerDisplayDesktop(const POINT &ptCur) = 0;

    //////////////////////////////////////////////////////////////////////////
    /*
    @���ܣ�����������ʾ�����¼�
    @������ptCur�����λ��
    */
    virtual void TriggerShowWndSBS(const POINT &ptCur) = 0;

    /*
    @���ܣ�����ˢ�´����¼�
    @������ptCur�����λ��
    */
    virtual void TriggerRefreshWnd(const POINT &ptCur) = 0;

    /*
       @���ܣ��������������л��¼�
       @������ptCur�����λ��
              eSWMode�������л���ģʽ�����ǿ���������ִ�У�����������
    */
    virtual void TriggerSwitchWindowEvent(const POINT &ptCur, E_SWHWND_MODE eSWMode = E_SWHWND_NONEMODE) = 0;

    /*
       @���ܣ�������ʾ���Խ��д����л���Ŀǰ�Ǹı������״��
       @������ptCur�����λ��
              bHint��true��������ʾ��false���ر���ʾ
    */
    virtual void TriggerSwitchWindowHintEvent(POINT ptCur, bool bHint) = 0;

    /*
        @���ܣ������ĵ������¼�
        @������ptCur
    */
    virtual void TriggerDocSaveEvent(const POINT &ptCur) = 0;

    /*
        @���ܣ������رյ�ǰ�����¼�
        @������ptCur
    */
    virtual void TriggerCloseCurrentWindowEvent(const POINT &ptCur) = 0;

    /*
        @���ܣ�������ʾ���Խ��д����ƶ���Ŀǰ�Ǹı������״��
        @������ptCur�����λ��
               bHint��true��������ʾ��false���ر���ʾ
    */
    virtual void TriggerMoveWindowHintEvent(POINT ptCur, bool bHint) = 0;

    /*
        @���ܣ����������ƶ��¼�
        @������ptCur����ǰ���λ��
               ptLast,��ǰ���λ��
               eMode�������ƶ���ʽ
    */
    virtual void TriggerMoveWindowEvent(POINT ptCur, POINT ptLast, E_MOVWND_MODE eMode) = 0;

public:    
    /*
        @���ܣ���ȡ���Ƿ�������**�¼�������
    */
    virtual bool Get_TriggerEvent_Permission(E_TRIGGEREVENT eEvent, const POINT &pt) = 0;

    /*
        @���ܣ��Ƿ�ر�����������
    */
    virtual bool IsAllGestureClosed() = 0;

    /*
        @���ܣ��Ƿ�ر������л������Ƶ�����
    */
    virtual bool IsAllPalmBasedGestureClosed() = 0;

    /*
        @���ܣ��Ƿ�ر������л�����ָ������
    */
    virtual bool IsAllFingerBasedGestureClosed() = 0;

    /*
        @���ܣ��Ƿ���������־
    */
    virtual bool IsLogPermited() = 0;

    /*
        @���ܣ���ȡ�������׼������ߵı���
    */
    virtual int GetMultiOfPalm2Std() = 0;

    /*
    //@����:��Ļ�ֱ��ʱ仯�¼���Ӧ����
    //@����:nScreenWidth, �µ���Ļ���
    //      nScreenHeight,�µ���Ļ�߶�
    */
    virtual void OnDisplayChange(int nScreenWidth, int nScreenHeight) = 0;

    /*
    //@����:����ߴ�仯ʱ����Ӧ����
    //@����:����ߴ�
    */
    virtual void OnPhysicalLengthChange(float fPhysicalLength) = 0;

    /*
        @���ܣ�ӳ��x����ı�׼���뵽��������ʵ�ʾ���
        @����˵�����ñ�׼Ϊ����80Ӣ��װ塢1024 * 768�ֱ���
    */
    virtual int MapStandX2Actual(int xStandard) = 0;

    /*
        @���ܣ�ӳ��y����ı�׼���뵽��������ʵ�ʾ���
        @����˵�����ñ�׼Ϊ����80Ӣ��װ塢1024 * 768�ֱ���
    */
    virtual int MapStandY2Actual(int yStandard) = 0;

    /*
        @���ܣ���ȡ��Ȩ�������
    */
    virtual int GetWeightedSpotMass(const int &nActualMass, const int &nExtRectArea, const int &nStdAreaMulti) = 0;
};