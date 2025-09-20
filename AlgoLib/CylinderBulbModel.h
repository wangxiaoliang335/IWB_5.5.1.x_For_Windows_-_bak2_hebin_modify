#pragma once


//Բ���ε����������ģ��
class CGenericCameraModel;
class CylinderBulbModel
{
public:
    CylinderBulbModel();

    //Copy Constructor
    //����Ҫ���忽�����캯��, ��Ϊ���Ա�����кܶ������á�
     CylinderBulbModel(const CylinderBulbModel& clone);

    //@����:���������ÿ�����ص��Ӧ����������λ��Pi��,ֱ��z=0ƽ�����һ���뾶ΪR,����ΪL��
    //      Բ���ε���, ����������񣬼����������Ƭ�е���������Gi, ��=Pi-Gi��ֵ�������������ʴ����������ꡣ
    //@����:pCameraModel, ���ģ��ָ��
    //      pMaskImage, ����ͼָ��
    //      nImageWidth, ���ͼƬ���
    //      nImageHeight, ���ͼƬ�߶�
    //void CalcProjectData(const CGenericCameraModel* pCameraModel, const BYTE* pMaskImage, int nImageWidth, int nImageHeight);
     void CalcProjectData(const CGenericCameraModel* pCameraModel, const RECT& rcMonitor, int nImageWidth, int nImageHeight);


    //@����:���ݼ���ʹ�ߵ����ĺ��������󴥵������ĵ�ƫ������
    //@����:pCentroid,������ĵ����ꡣ
    //      pContourPts,������������顣
    //      nContourPointsCount, �����������
    //TVector2D  GetOffset(const TPoint2D& ptCentroid, const TPoint2D* pContourPts, int nContourPointsCount, TPoint2D* pContactPoint);
    TVector2D  GetOffset(const TPoint2D& ptCentroid);


	TVector2D  GetRealOffset(const TPoint2D& ptCentroid);
protected:
    //@����:����Բ�������ϵĵ�
    //@����:ptPlacePosition������������ϵZ=0ƽ���ϵİڷ�λ��
    //      nRadius, Բ����뾶
    //      nLenght, Բ���峤��
    //void SampleCylinderSurface(const TPoint3D& ptPlacePosition, double Radius, double Length);

    const CGenericCameraModel* m_pGenericCameraModel  ;//ͨ�����ģ��
    struct TContactPointOffsetInformation
    {
        TPoint2D  vOffset;
        TPoint2D  pt2dCentroidPos;
        
        //int       nLightSpotMass;//ģ��ģ�����ɵĹ������
        //RECT      rcBound;
        BOOL      bValid;
    };
    std::vector<TContactPointOffsetInformation>    m_vecContatcPointOffset;//������������ĵ�ƫ��ʸ�����顣
    //std::vector<TPoint3D>      m_vecSurfacePoints     ;//Բ�������ĵ�����ꡣ
    int                        m_nImageWidth;
    int                        m_nImageHeight;
};