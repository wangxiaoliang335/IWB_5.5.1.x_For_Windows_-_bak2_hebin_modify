#pragma once

//////////////////////////////////////////////////////////////////////////
//��������ƶ���ʽ
typedef enum
{
    E_2TOUCHS_MOVES_FALSE       , //������
    E_2TOUCHS_MOVES_NONE        , //δ�ƶ�
    E_2TOUCHS_MOVES_SAME        , //ͬ���ƶ�������ʱ���ƶ���ʽ
    E_2TOUCHS_MOVES_ROAM        ,
    E_2TOUCHS_MOVES_SELECTION   , 
    //E_2TOUCHS_MOVES_BACKWARD    , //�����ƶ����Ŵ�ʱ���ƶ���ʽ
    //E_2TOUCHS_MOVES_OPPOSITE    , //�����ƶ�����Сʱ���˶���ʽ
    E_2TOUCHS_MOVES_ZOOM        , //�����ƶ�
    E_2TOUCHS_MOVES_RORATE      , //��ת�ƶ�
}E_2TOUCHS_MOVES;

//added by toxuke@gmail.com,  2014/01/08
//�װ��������״̬��ö��ֵ
typedef enum 
{
    E_WBS_IDLE             ,    //��״̬
    E_WBS_ERASER           ,    //���״̬
    E_WBS_PEN              ,    //��״̬
    E_WBS_ROAM             ,    //����״̬
    E_WBS_SPOTLIGHT        ,    //̽�յ�״̬
    E_WBS_RETURNTODISKTOP  ,    //��������״̬
    E_WBS_NEXTPAGE         ,    //��һҳ״̬
    E_WBS_PROPAGE          ,    //��һҳ״̬
    E_WBS_ZOOM             ,    //����
    E_WBS_RORATE           ,    //��ת
    E_WBS_MASK             ,    //Ļ��
    E_WBS_SELECTION        ,    //ѡ��ʻ�����

}E_WBS;    

//**************************************************************
//����������״
typedef enum
{
    E_GLGR_GShape_Unknown,
    E_GLGR_GShape_Finger,
    E_GLGR_GShape_Palm
}EGLGR_GESTURE_SHAPE;
//**************************************************************

enum GLMatchState
{
    E_GLMISMATCHED  = 0,
    E_GLMATCHED     = 1,
    E_GLNEW_ELEMENT = 2,
};

struct GLMatchInfo
{
    UINT  uId              ;//��0��ʼ��ƥ����
    POINT ptPos            ;//���ꡣ
    GLMatchState eMatchState;//ƥ��״̬
};