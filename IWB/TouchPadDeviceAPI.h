#pragma once
#include "..\inc\EASI_TOUCHSCREEN_INTERFACE.h"

//
enum EPenState
{
    E_PEN_STATE_UP   = 0,
    E_PEN_STATE_DOWN = 1,
};

struct PenInfo
{
    int  nId;//��0��ʼ�Ĵ��ص���
    POINT pt;//���ص����Ļ����
    
    EPenState ePenState;//�ʵ�״̬��
};

class CVirtualTouchPad
{
public:

    CVirtualTouchPad();

    ~CVirtualTouchPad();

    BOOL OpenDevice();

    BOOL CloseDevice();

    BOOL InputTouchPoints(const PenInfo* penInfos, int nPenCount);

protected:
    HANDLE m_hDev;


    static const int MAX_TOUCH_POINT_COUNT  = 10;
    EASI_TouchPoint m_TouchPoints[MAX_TOUCH_POINT_COUNT];
};