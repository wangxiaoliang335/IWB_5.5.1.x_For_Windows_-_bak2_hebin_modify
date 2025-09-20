#pragma once
#include "../inc/MultiDimensionPoint.h"

struct TLightSpotParam
{
	POINT ptPosInScreen;//��������(��Ļ����)
	int gunId; //���������ǹ֧Id
	//<<debug
	DWORD dwCameraId = -1;//�������ID
	//debug>>
};

//������λ����Ϣ
struct TLightSpot
{
    int mass                    ;//����
	int mx                    ;//��x�������, �ϲ����ʱ��Ҫ�õ�
    int my                    ;//��y�������,,�ϲ����ʱ��Ҫ�õ�

	TPoint2D pt2dPosInVideo   ;//�������Ƶ�е�����
	LONG lAreaInVideo          ;//��ߵ���Ӿ������(��Ƶ����)
	LONG lStdSpotAreaInVideo   ;//�������λ�õı�׼���(��Ƶ����)

    int  nMonitorId = -1;

	TLightSpotParam spotParam;

    //�������ݽṹ
    struct  TAux
    {
		TAux()
		{
			uMergeAreaIndex  = -1;
			//bBeyondMergeArea = false;
            bOutsideOwnedArea = false;
		}

        UINT uMergeAreaIndex      ;//������ڵ��ں��������š�
                                    //-1, �����κ��ں���
                                    //>=0, �ں���������
        //bool bBeyondMergeArea      ;//λ�ó�Խ�ں�����־

        bool bOutsideOwnedArea     ;//�������Ͻ����Ļ���������־
    }aux;

    
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