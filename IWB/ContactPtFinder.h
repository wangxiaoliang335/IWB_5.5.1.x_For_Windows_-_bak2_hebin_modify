#pragma once

//@����:��������ĽӴ�λ�ü����
//      ���ݹ���ڻ����е�λ���Լ�����ͷ��ˮƽ�ǶȺʹ�ֱ�Ƕ�, ���������������е����ɵ���
//       
class CContactPtFinder
{
public:
    CContactPtFinder();

    //@����:���ݹ�ߵ���Ӿ�����Ϣ��������������е��λ��
    POINT operator()(const RECT& rcBound);
    

    //@����:���ݹ�����ĵ�λ��,�ҳ�
    //      ptCentroid, �������λ��, ע��������Ѿ��Ŵ���2^INT_SCALE_SHIFT_SIZE����
    POINT operator()(const POINT& ptCentroid);


    POINT operator()(const POINT& ptCentroid, double lambda1, double lambad2);

    

    //@����:����"����������ͶӰĻ�͵��涼��ֱ��ƽ��"������ͷ����ļнǦյ�����
    void SetTangentHorz(const double& dbTangentHorz);

    //@����:����"�����ġ���ֱͶӰĻ������ˮƽ���ƽ��"������ͷ����ļнǦ׵�����
    void SetTangentVert(const double& dbTangentVert);


    //@����:��������ӽ�,��λ:��
    void SetViewAngle(const double& nDegree);


    //@����:����Ͷ���
    void SetThrowRatio(const double& dbThrowRatio);

    //@����:���þ�ͷ����
    void SetLensType(ELensType eLensType);
protected:
    //����ģ��1
    //
    POINT Model_1(const POINT& ptCentroid);

    //����ģ��2
    POINT Model_2(const POINT& ptCentroid);

protected:
    double m_dbTangentHorz;//"����������ͶӰĻ�͵��洹ֱ��ƽ��"������ͷ����ļнǦյ�����
    double m_dbTangentVert;//"�����ġ���ֱͶӰĻ�������ƽ�е�ƽ��"������ͷ����ļнǦ׵�����
    double m_dbR_D_Ratio_Vert ;//��ʵ���ֱ����"��ͷ���ĵ�ͶӰĻ����"֮��, һ��Ϊ0.01���ҡ�
    double m_dbR_D_Ratio_Horz ;//��ʵ���ֱ����"��ͷ���ĵ�ͶӰĻ����"֮��, һ��Ϊ0.01���ҡ�
    int    m_nImageCx     ;//�����ȣ���λ:���أ�ע��Ŵ���(2^INT_SCALE_SHIFT_SIZE)��
    int    m_nImageCy     ;//����߶ȣ���λ:���أ�ע��Ŵ���(2^INT_SCALE_SHIFT_SIZE)��
    double m_dbFocalLength;//��Ч���࣬��λ:���أ�ע��Ŵ���(2^INT_SCALE_SHIFT_SIZE)��
    double m_dbMaxViewAngle;//�Խ�������ӽǣ���λ:��,����Խǵ�����ĵ����ߵļнǡ�

    double m_dbThrowRatio  ;//Ͷ���

    //

    ELensType m_eLensType ;//���þ�ͷ����

    double    m_Model1_K[2] ;//��_modified = m_Coefs[0]*�� + m_Coefs[1]*��^3;
    double    m_Model2_K[2] ;//��_modified = m_Coefs[0]*�� + m_Coefs[1]*��^3;
private:
    double m_dbHalfCx;
    double m_dbHalfCy;
};