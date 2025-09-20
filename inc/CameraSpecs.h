#pragma once
//���������ݽṹ

//��ͷ���
struct TLensSpecification
{
    double backFocalLength;//�񷽽���, ��λ:mm
    double FOV_horz       ;//ˮƽ�ӽ�, ��λ:��
    double FOV_vert       ;//��ֱ�ӽ�, ��λ:��
    double FOV_diagonal   ;//�Խ����ӽ�, ��λ:��
    double throwRatio     ;//Ͷ���

};

//CMOSоƬ���
struct TCMOSChipSpecification
{
    double width_in_mm    ;//����������, ��λ:mm
    double height_in_mm   ;//��������߶�,��λ:mm
    int    width_in_pixel ;//���ؿ��
    int    height_in_pixel;//���ظ߶�
    double pixel_size     ;//���ؾ�������ߴ�, ��λ:mm, eg, 0.006mm=6um

};

//�Զ�У������ϵ��
struct TAutoCalibCompCoefs
{
    TAutoCalibCompCoefs()
    {
        u0 = 0.0;
        v0 = 0.0;
        k[0] = 0.0;
        k[1] = 0.0;
        k[2] = 0.0;

    }

    double u0;//���Ĵ�ֱ����,��һ����(0,1)��Χ,�Զ����ھ���ͼ��ֱ���
    double v0;//���Ĵ�ֱ����,��һ����(0,1)��Χ,�Զ����ھ���ͼ��ֱ���
    double k[3];//


};

//��ͷ�ڲ�����
struct TInternalAndSymmetricDistortParams
{
	double mu;
	double mv;
	double u0;
	double v0;
	double radialDistrt[5];//�������ϵ��
	//UINT   dwFlags;//�жϸ����ֶ���Ч�ı�־
};

//typedef enum
//{
// LENS_INTERNAL_PARAMS_FALG_MU_MV_VALID = 0x01,
// LENS_INTERNAL_PARAMS_FLAG_U0_M0_VALID = 0x02,
// LENS_INTERNAL_PARAMS_FLAG_DISTORTION_VALID = 0x04
//}LEN_INTERNAL_PARAMS_FLAG;

//
struct TCameraDebugData
{
	TPoint2D pt2DContactInImage;//��ʴ�����ͼ���е����ꡣ
	TPoint2D pt2DCentroid;
};