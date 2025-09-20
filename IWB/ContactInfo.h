#pragma once

enum EPenState
{
    E_PEN_STATE_UP   = 0,
    E_PEN_STATE_DOWN = 1,
};

struct TContactInfo
{

    UINT  uId;//��0��ʼ�Ĵ��ص���
    POINT  pt;//���ص����Ļ����    
    EPenState ePenState;//�ʵ�״̬��
    //BOOL   bIgnored    ;//���ݺ��Ա�־

    int gunId = -1; //���������ǹ֧Id
//<<debug
    DWORD dwCameraId = -1;//�������ID
    //>>

    TContactInfo()
    {
        uId = -1;
        //bIgnored = FALSE;
        gunId = -1;
        dwCameraId = -1;
    }
};