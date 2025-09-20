#pragma once
//@����:�ں����Ĺ�ߺϲ���
class CSpotMerger
{
public:
     CSpotMerger();
    ~CSpotMerger();

    //@����:ִ����Ļ�ں�����ߺϲ�����
    //@����:pLightSpots������/�������, �������
    //      pLightSpotCount, ����/���������ָ���߸�����ָ��
    //@˵��:����߷ֱ�����Ļ�ָ�����2��
    void DoMerge(TLightSpot* pLightSpots, int* pLightSpotCount);


    //@����:�����ں��������Ϣ
    //@����:nSeperateX, ����Ļƴ�ӱߵ�X����
    //      nMergeAreaWidth, ��Ļ�ں����Ŀ��
    void SetMergeAreaInfo(int nSeperateX, int nMergeAreaWidth);

    //int GetMergeAreaRightBorder()const {return m_nMergeAreaRightBorder;}
    //int GetMergeAreaLeftBorder()const  {return m_nMergeAreaLeftBorder ;}
    int GetMergeDistThreshold() const  {return m_nMergeDistThreshold;  }

    
    //@����:��Ļ�ֱ��ʱ仯�¼���Ӧ����
    //@����:nScreenWidth, �µ���Ļ���
    //      nScreenHeight,�µ���Ļ�߶�
    void OnDisplayChange(int nScreenWidth, int nScreenHeight);

    
    
    //@����:�����ں�����
    //@����:mergeAreas, �ں�������
    //      nCount, �ں�����Ŀ
    void SetMergeAreas(const RECT  mergeAreas[], int nAreaCount);


    //@����:����ָ�����ں���
    const RECT* GetMergeArea(UINT nIndex) const;
protected:
    //int m_nLeftBorder ;//��Ļ�ں��������߽�
    //int m_nRightBorder;//��Ļ�ں�������ұ߽�
    //static const int MERGE_THRESHOLD = 20*20;
    int m_nSeperateX             ;//����Ļƴ�ӱߵ�X����
    //int m_nMergeAreaLeftBorder   ;//�ں�����߽�
    //int m_nMergeAreaRightBorder  ;//�ں����ұ߽�
    int m_nMergeDistThreshold    ;//�ںϾ������ޣ�С�ڸ�ֵ���ں�



    std::vector<RECT> m_vecMergeAreas;//���غϲ���



};