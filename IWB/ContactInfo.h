#pragma once

enum EPenState
{
    E_PEN_STATE_UP   = 0,
    E_PEN_STATE_DOWN = 1,
};

struct TContactInfo
{

    UINT  uId;//从0开始的触控点编号
    POINT  pt;//触控点的屏幕坐标    
    EPenState ePenState;//笔的状态。
    //BOOL   bIgnored    ;//数据忽略标志

    int gunId = -1; //所属射击的枪支Id
//<<debug
    DWORD dwCameraId = -1;//所属相机ID
    //>>

    TContactInfo()
    {
        uId = -1;
        //bIgnored = FALSE;
        gunId = -1;
        dwCameraId = -1;
    }
};