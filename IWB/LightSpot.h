#pragma once

//������λ����Ϣ
struct TLightSpot
{
    int mass                  ;//����
	//int mx                    ;//��x�������, �ϲ����ʱ��Ҫ�õ�
    //int my                    ;//��y�������,,�ϲ����ʱ��Ҫ�õ�

	//POINT ptPosInVideo         ;//��ʴ�������(��Ƶ����),�Ŵ�2^INT_SCALE_SHIFT_SIZ��
	//RECT rcPosInVideo          ;//����λ��(��Ƶ����), ԭʼ����δ�Ŵ�
  
	//LONG AreaInVideo         ;//��ߵ�ʵ�����(��Ƶ����)
	LONG lStdSpotAreaInVideo   ;//�������λ�õı�׼���(��Ƶ����)
    POINT ptPosInScreen        ;//��������(��Ļ����)


    
	bool operator < (TLightSpot& right)
	{
		return (mass < right.mass);
	}

	bool operator <= (TLightSpot& right)
	{
		return (mass <= right.mass);
	}


	bool operator > (TLightSpot& right)
	{
		return (mass > right.mass);
	}

	bool operator >= (TLightSpot& right)
	{
		return (mass >= right.mass);
	}

};