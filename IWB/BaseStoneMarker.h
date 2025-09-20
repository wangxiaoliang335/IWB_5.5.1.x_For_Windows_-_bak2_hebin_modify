#pragma once
//@����:����궨��
//@˵��:�궨����ʱ������û������ͼ,Ϊ�˼����ⲿ���ţ�Ҫ��ͨ�����ʵ�˫��
//      �����ÿ������ı궨��
class CBaseStoneMarker
{
public:
    CBaseStoneMarker();
    ~CBaseStoneMarker();

    void Reset();

    //@����:У�����c
    //@����ֵ:ȫ�����c�궨��ϣ��򷵻�TRUE, ���򷵻�FALSE
    BOOL Process(const TPoint2D* pLightSpots, int nlightSpotsCount);

    const TPoint2D* GetBasePoints(UINT* pCount = NULL)const;
    
    void LoadBasePoints(const TPoint2D* pBasePoints, UINT nCount);

    //@����:��õ�ǰ���c��������
    int GetCurrentBaseStoneIndex() const;


    BOOL IsDataValid()const;

    enum EMachinState
    {
        E_MACHINE_STATE_READY,//�����׶�
        E_MACHINE_STATE_RUNNING,//���н׶�
        E_MACHINE_STATE_END,  //�����׶�

    };

    EMachinState GetMachineState() const
    {
        return m_eMachineState;
    }

    

    static const int BASE_STONE_NUMBER = 4;

protected:

    //�궨��״̬
    enum ELightSpotState
    {
        E_MARK_STATE_READY          ,//����״̬
        E_MARK_STATE_PEN_FIRST_DOWN ,//��ʵ�һ�ΰ���״̬
        E_MARK_STATE_PEN_FIRST_UP   ,//��ʵ�һ�ε���״̬  
        E_MARK_STATE_PEN_SECOND_DOWN,//��ʵ�һ�ΰ���״̬
        E_MARK_STATE_PEN_SECOND_UP  ,//��ʵڶ��ε���״̬
        E_MARK_STATE_TIMEOUT        ,//�궨��ʱ
    };


    struct MarkLightspot//�궨���
    {
        ELightSpotState m_eState         ;//�궨״̬
        TPoint2D        m_pt2dPos        ;//�궨������һʱ�̵�λ�á�
        DWORD           m_dwLastTickcount;//�������һʱ�̵�ʱ�Ӽ���,��λ:ms
        BOOL            m_bProcessed     ;//�Ѵ����־
        //BOOL       m_bValid            ;//������Ч��־
    };
    
    BOOL InternalProcess(const TPoint2D* pNewLightSpots, int nlightSpotsCount);

    void ProcessLightSpot(MarkLightspot* lightSpot, const TPoint2D* pPtNewPos);
    
    

    int m_nCurrentMarkIndex;//��ǰ�궨�Ļ���������š�

    TPoint2D m_BaseStones[BASE_STONE_NUMBER];
    BOOL m_bDataIsValid;
    

    static const int MAX_LIGHTSPOT = 8;
    MarkLightspot m_MarkLightSpots[MAX_LIGHTSPOT];//�������
    int m_nActiveLightSpotCount;//��Ĺ�߸���

    EMachinState m_eMachineState;
};