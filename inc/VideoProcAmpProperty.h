#pragma once
//video propertise on a video capture device.
struct TVideoProcAmpProperty
{
public:

	long Prop_VideoProcAmp_Brightness           ;//����
	long Prop_VideoProcAmp_Contrast             ;//�Աȶ�
	long Prop_VideoProcAmp_Hue                  ;//ɫ��
	long Prop_VideoProcAmp_Satuation            ;//���Ͷ�
	long Prop_VideoProcAmp_Sharpness            ;//������
	long Prop_VideoProcAmp_Gamma                ;//Gammaֵ
	long Prop_VideoProcAmp_ColorEnable          ;//��ɫ����
	long Prop_VideoProcAmp_WhiteBalance         ;//��ƽ��
	long Prop_VideoProcMap_BacklightCompensation;//���ⲹ��
	long Prop_VideoProcMap_Gain                 ;//����

	long Prop_CameraControl_Exposure            ;//�ع�ʱ��
	
	//TVideoProcAmpProperty()
	//{
	//	Prop_VideoProcAmp_Brightness            = 125;
	//	Prop_VideoProcAmp_Contrast              = 50;
	//	Prop_VideoProcAmp_Hue                   = 0;
	//	Prop_VideoProcAmp_Satuation             = 104;
	//	Prop_VideoProcAmp_Sharpness             = 8;
	//	Prop_VideoProcAmp_Gamma                 = 46;
	//	Prop_VideoProcAmp_ColorEnable           = 0;
	//	Prop_VideoProcAmp_WhiteBalance          = 5000;
	//	Prop_VideoProcMap_BacklightCompensation = 2;
	//	Prop_VideoProcMap_Gain                  = 0;
	//}

};