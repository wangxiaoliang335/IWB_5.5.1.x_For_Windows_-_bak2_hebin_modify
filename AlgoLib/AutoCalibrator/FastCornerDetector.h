#pragma once
#include <vector>
#include <Algolib.h>
//@˵��:���ٽǵ���
class CFastCornerDetector
{
public:

    //@����:���ټ��ǵ�;
    //@����:pSrcImage, ָ��Ҷ�ͼƬ���ݵ�ָ��
    //      nWidth, ͼƬ���
    //      nHeight, ͼƬ�߶�
    bool operator()(
    const unsigned char* pSrcImage, 
    int nWidth, 
    int nHeight, 
    UINT uExpectedCornerNumber, 
    //int nDetectRadius, 
    ECalibrationDebugLevel eDebugLevel=E_CALIB_DEBUG_LEVEL_CONCISE, 
    int nDebugTimes =1);



	const std::vector<POINT>& GetResults()const
	{
		return m_vecCorners;
	}


protected:
	//�����
	std::vector<POINT> m_vecCorners;
};
