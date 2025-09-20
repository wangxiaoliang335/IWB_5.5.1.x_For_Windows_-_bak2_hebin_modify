#pragma once
#include <vector>
#include <Algolib.h>
//@˵��:���ٽǵ���
class CCrossDetector
{
public:

    //@����:���ټ��ǵ�;
    //@����:pSrcImage, ָ��Ҷ�ͼƬ���ݵ�ָ��
    //      nWidth, ͼƬ���
    //      nHeight, ͼƬ�߶�    
    //      uExpectedCornerNumber, ������У�������
    //      nGridSize, ��������ƽ����С
    //      pMaskImage, ָ������ͼ��ָ�룬����ͼ��ָ���ͼƬ�ĸ߶ȺͿ��һ��
    //      ECalibDebugLevel, ���Լ���
    //      nDebugTimes,���Բ���
    bool operator()(
    const unsigned char* pSrcImage, 
    int nWidth, 
    int nHeight, 
    UINT uExpectedCrossNumber, 
    int nGridSize, 
    const unsigned char* pMaskImage,
    ECalibDebugLevel eDebugLevel=E_CALIB_DEBUG_LEVEL_CONCISE, 
    int nDebugTimes =1);



    

    //@����:����У������������
	const POINT* GetResults()const
	{
		return &m_vecCrosses[0];
	}
    



    //@����:���ظ߾��ȵ�У������������, ���걻�Ŵ�2^INT_SCALE_SHIFT_SIZE��
  	const POINT* GetHDResults()const
	{
		return &m_vecHDCrosses[0];
	}



protected:
	//��Ƶ�����е�,ʮ��У�������������, 
	std::vector<POINT> m_vecCrosses;

    //����2^INT_SCALE_SHIFT_SIZE����У�����������飬�����������������������������
    std::vector<POINT> m_vecHDCrosses;
};
