#pragma once
extern "C"
{
//@����:ʹ��MMXָ���������YUY2ͼƬ֡
//@����:pYUY2Src, ָ��YUY2Դ���ݵ�ָ��
//      pYUY2Mask�� ָ��YUY2����ͼ���ݵ�ָ��
//      nPixelCount����������

void  YUY2FrameMask_MMX(
    unsigned short* pYUY2Src,
    const unsigned short* pYUY2Mask,
    unsigned int nPixelCount);



//@����:ʹ��SSEָ�YUY2ͼƬ֡ת��Ϊ1λλͼ

void  YUY2ToBitFrame_SSE(
    const unsigned short* pYUY2,
    unsigned char* pBits,
    unsigned char cThreshold,
    unsigned int nPixelCount);


//@������:DynamicMaskStatistic_MMX
//@��  ��:ͳ��ÿ�����ش���ĳһ���޵Ĵ�����������ش������������ض�Ӧ�ļ�����+1, ���С�����ޣ��������-1���������ķ�Χ��0~255
//
//ʵ�ֵ��߼���
//if(*pImageData >= cCmpThreshold��
//{
//  if(* pStatistic<0xFF) *pStatistic + = 0x01;
//}
//else
//{
//	 if(* pStatistic>0) *pStatistic -= 0x01;
//}

void  DynamicMaskStatistic_YUY2_MMX(
    const unsigned short* pImageData,//YUY2��ʽ��λͼ
    unsigned char* pStatistic       ,//ͳ��λͼ
    unsigned char cCmpTheshold      ,//�Ƚ�����
    int nPixelCount                  //��������
    );



void  DynamicMaskStatistic_Gray_MMX(
    const unsigned char* pImageData,//8Bit�Ҷȸ�ʽ��λͼ
    unsigned char* pStatistic       ,//ͳ��λͼ
    unsigned char cCmpTheshold      ,//�Ƚ�����
    int nPixelCount                  //��������
    );



void  GrayFrameMask_MMX(
    unsigned char* pGraySrc,
    const unsigned char* pGrayMask,
    unsigned int nPixelCount);


//@����:ʹ��SSE2ָ�8λ�Ҷ�ͼƬ֡ת��Ϊ1λλͼ
extern "C"
void  GrayToBitFrame_SSE2(
    const unsigned char* pGray,
    unsigned char* pBits,
    unsigned char cThreshold,
    unsigned int nPixelCount);

//@����:ʹ��SSE2ָ�8λ�Ҷ�ͼƬ֡��ֵ��,Ŀ������ֵΪ0xFF, ��������ֵΪ0x00, ת��Ϊ1λλͼ
//@����:pGray, ����/�������,����ָ��8λ�Ҷ�ͼƬ���׵�ַ,��������ֵ����Ľ��
//      pBits, �������,1λλͼ
//     cThreshold, ��ֵ������
//    nPixelCount, ��������
extern "C"
void BinarizeGrayToBitFrame_SSE2(
    unsigned char* pGray,
    unsigned char* pBits,
    unsigned char cThreshold,
    unsigned int nPixelCount);



//@����:ʹ��SSE2ָ�1λλͼ֡ת��Ϊ8λ�Ҷ�ͼƬ
//@����:pGray, �������,ָ��8λ�Ҷ�ͼƬ���׵�ַ
//      pBits, �������,
//      nPixelCount, ��������
extern "C"
void  BitToGrayFrame_MMX(
    const unsigned char* pBits,
    unsigned char* pGray,
    unsigned int nPixelCount);
}

//@����:��8λ�Ҷ�ͼƬת��ΪARGB��ʽ
//@����:
//     p8BitGrayData, �Ҷ�ͼƬ����
//      pARGBData, ARGB����
//      pixelCount, ���ظ���
//      
extern "C"
void  GrayToARGB_SSE2(
        const unsigned char* p8BitGrayData, 
        unsigned long* pARGBData, 
        unsigned long pixelCount);