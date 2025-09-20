#pragma once
#include <limits>
#include "ctmf.h"
#include "Histogram.h"
#include <assert.h>
#include "CornerDetect.h"
    
//#include "Profile.h"
//��ֵ�˲�
template<int R = 2>
class CMeidanFilter
{
public:
	
	CMeidanFilter()
		:
	m_oSrcBitmap()
	{

	}

	void SetSize(int nWidth, int nHeight)
	{
		m_oSrcBitmap.SetSize(nWidth, nHeight, 1);
	}


	void operator()(CImageFrame& srcBitmap)
	{

		m_oSrcBitmap = srcBitmap;

		const BYTE* pSrc  = m_oSrcBitmap.GetData();
		      BYTE* pDest = srcBitmap.GetData();

        int nImageWidth  = srcBitmap.Width();
		int nImageHeight = srcBitmap.Height();

		ctmf(
			pSrc,
			pDest,
			nImageWidth,
			nImageHeight,
			nImageWidth,
			nImageWidth,
			R,
			1,//gray scale image has only one channel
			512*1024);

	}
protected:

	CImageFrame m_oSrcBitmap;

};


//@����:��ÿһ��������(2R+1)��(2R+1)��������ȡ��ֵ��
template<int R=1>
class CBlurOperator
{
public:
    void operator ()(size_t w, size_t h, const uint8_t* srcImage, uint8_t* dstImage)
    {
        size_t y,x;
        int j,i;
        const unsigned pixel_count = (2*R+1)*(2*R+1);
        for(y = R; y < h - R; y++)
            for( x = R; x < w - R; x++)
            {
                const unsigned offset= x + y * w;
                uint16_t sum = 0u;
                for(j = -R; j <= R; j++)
                    for( i = -R; i <= R; i++)
                    {
                        sum += srcImage[offset + j*w + i];
                    }

               dstImage[offset] = sum/pixel_count;
                
            }
    }


};

//@����:��ֵ��������
class CBinarizeOperator 
{
public:
    template<class T>
    void operator () (uint8_t threshold, size_t w, size_t h, T* image)
    {
        size_t y,x;
        for(y = 0; y < h ; y++)
            for( x = 0; x < w; x++)
            {
                const unsigned offset=  x + y * w;
                image[offset] = image[offset] > threshold?0xFF:0x00;
            }
    }



};


//��������
//��Robust Checkerboard Recognition for Efficient Nonplaner Geometry Registration In Project-camera Systems��
//ǿ³���Խǵ����㷨
//

/*
template<int  LAYER_NO = 1>
class Layer
{

//ÿ���������������ĵ�λ��ƫ������ɵ�����
int layer[LAYER_NO * 8];
};*/


#include "../inc/Algorithm/HeapSort.h"
class CLayer
{

public:
	CLayer()
		:
	m_nLayerIndex(0)
	{

	}

	//@����:���ò��������
	//@����:nLayerIndex, ��0��ʼ�Ĳ�������
	void SetLayerIndex(int nLayerIndex)
	{
		unsigned int nSize = (nLayerIndex +1) * 8;
		m_vecOffset.resize(nSize);

		//����ò���������ĵ�ƫ����
		int nMaxOffset = nLayerIndex + 1;
		int iPhase     = 0;

		int nOffsetX = - nMaxOffset;
		int nOffsetY = - nMaxOffset;

		for(unsigned int i= 0 ; i < nSize; i++)
		{
			//�����Ͻǿ�ʼ,˳ʱ�������
			m_vecOffset[i].x = nOffsetX;
			m_vecOffset[i].y = nOffsetY;

			switch(iPhase)
			{
			case 0://�ϲ��

				if(nOffsetX == nMaxOffset)
				{
					iPhase = 1;
					nOffsetY ++;
				}
				else
				{
					nOffsetX ++;
				}
				break;

			case 1://�Ҳ��

				if(nOffsetY == nMaxOffset)
				{
					iPhase = 2;
					nOffsetX--;
				}
				else
				{
					nOffsetY++;
				}


				break;

			case 2:

				if(nOffsetX == - nMaxOffset)
				{
					iPhase = 3;
					nOffsetY --;
				}
				else
				{
					nOffsetX -- ;
				}

				break;


			case 3:

				if(nOffsetY == -nMaxOffset)
				{

				}
				else
				{
					nOffsetY--;
				}

				break;

			default:
				assert(FALSE);
			}//switch(iPhase)

		}//for-each(i)

		m_nLayerIndex = nLayerIndex;
	}

	const POINT* GetOffsetList()const
	{
		return &m_vecOffset[0];
	}

	UINT GetLayerSize()const
	{
		return m_vecOffset.size();
	}


protected:
	std::vector<POINT> m_vecOffset;
	int                m_nLayerIndex;
};



//����̽�ⴰ
class RectScanWindow
{

public:
	//����:���ο�Ŀ����2w+1, ��wȦ����
	//       
	RectScanWindow(unsigned int  w)
	{
		m_vecLayers.resize(w);

		for(unsigned int i = 0; i < w; i++)
		{
			m_vecLayers[i].SetLayerIndex(i);
		}

	}


	const CLayer* GetLayer(unsigned int iLayerIndex)const
	{
		if(iLayerIndex >= m_vecLayers.size()) return NULL;

		return &m_vecLayers[iLayerIndex];

	};
protected:
	std::vector<CLayer> m_vecLayers;
};

//���ػ�
//static const int MAX_LAYER = 21;
static const int MAX_LAYER = 50;//modified by toxuke@gmail.com, 2011/12/29
static const int MIN_LAYER = 5;
struct TLineSegment
{
	UINT nStartPos;
	UINT nEndPos  ;
	UINT nLength  ;
	UINT type     ;//0:Ϊ'0'��;1:Ϊ'1'��
};
class CBitRing
{
public:
	CBitRing():
	  m_uBitLength(1)
	  {
		  Clear();
	  }
	  void SetLength(UINT  uBitLength)
	  {
		  UINT uNeedDword = uBitLength >> 5;
		  if(uNeedDword > _countof(m_aryData))
		  {
			  m_uBitLength = _countof(m_aryData)<<5;
		  }
		  else
		  {
			  m_uBitLength = uBitLength;
		  }
	  }

	  void Clear()
	  {
		  memset(m_aryData, 0, sizeof(m_aryData));
		  m_uBitLength = 0;

		  memset(m_aryLineSegments, 0, sizeof(m_aryLineSegments));
		  m_uLineSegmentCount = 0;

	  }

	  void SetBit(UINT uBitNo)
	  {
		  UINT uWordOffset  = uBitNo>>5;
		  UINT uBitOffset   = uBitNo % 32;

		  m_aryData[uWordOffset] |= 1 << uBitOffset;
	  }

	  void ClearBit(UINT uBitNo)
	  {

		  UINT uWordOffset  = uBitNo>>5;
		  UINT uBitOffset = uBitNo % 32;

		  m_aryData[uWordOffset] &= ~(1 << uBitOffset);	
	  }

	  UINT operator[](UINT uBitNo)
	  {
		  UINT uDwordIndex  = uBitNo>>5;
		  UINT uDwordOffset = uBitNo % 32;

		  return (m_aryData[uDwordIndex] & (1 << uDwordOffset))?1:0;	
	  }


	  UINT GetBit(UINT uBitNo)
	  {
		  UINT uDwordIndex  = uBitNo>>5;
		  UINT uDwordOffset = uBitNo % 32;

		  return (m_aryData[uDwordIndex] & (1 << uDwordOffset))?1:0;	
	  }

	  DWORD* GetData()
	  {
		  return m_aryData;
	  }
	  const DWORD* GetData()const
	  {
		  return m_aryData;
	  }

	  //�����ֲ�Ϊ0�ĸ���
	  UINT GetPartCount()
	  {
		  UINT uPartCout = 0;

		  UINT uPrevData = GetBit(m_uBitLength - 1);
		  UINT uCurData  = m_aryData[0] & 0x01;

		  UINT count = 0;

		  int nBitOffset = 0;
		  DWORD* pdwData = m_aryData;
		  DWORD dwData   = *pdwData;
		  do
		  {

			  uCurData = (dwData & (1<<nBitOffset))?1:0;

			  if(uCurData != uPrevData)
			  {
				  uPartCout++;
			  }

			  uPrevData = uCurData;

			  nBitOffset++;
			  if(nBitOffset == 32)
			  {
				  nBitOffset = 0;
				  pdwData++;
				  dwData  = *pdwData;

			  }
			  count++;

		  }while(count < m_uBitLength);

		  return uPartCout;
	  };


	  //@����:�Ի�״�ṹ���и�ʴ����
	  //@����:SEL, �ṹԪ�����鳤��
	  //      type, 0,ɾ����������С��SEL��0��
	  //            1,ɾ����������С��SEL��1��
	  //
	  //˵��:����ʵ�ʹ��Ϊ,�ڻ�״�ṹ���޳�����1����С��SEL���߶Ρ�
	  //����
	  //SEL = 2ʱ��
	  //����:00000100000000110000000000000001111111111111100000000
	  //          ���޳�   ���޳�
	  //
	  //����:  111111111110111111111000000000000000001111
	  //                 ���޳�
	  //�������'1', '0'�����±갴��˳ʱ�뷽�����ӡ�
	  //
	  //�����ƻ���
	  //����
	  //0x007b1400 0xffff0000 0x0000000f
	  //
	  //�����ƴ�(�ӵ�λ����λ)
	  //0000 0000 0010 1000 1101 1110 0000 0000 0000 0000 0000 0000 1111 1111 1111 1111 1111 0000
	  //
	  //�ȶ�'1'����ʴ��Ľ��Ϊ
	  //0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 1111 1111 1111 1111 1111 0000
	  //
	  //�ȶ�'0'����ʴ��Ľ��Ϊ
	  //0000 0000 0011 1111 1111 1110 0000 0000 0000 0000 0000 0000 1111 1111 1111 1111 1111 0000
	  //
	  //�������ĸ�ʴ���Ϊ
	  //0000 0000 0000 0000 1111 1110 0000 0000 0000 0000 0000 0000 1111 1111 1111 1111 1111 0000
	  //
	  //����С��SEL��'0'����'1'��, ������ǰ���'1'����'0'���ĳ����������Ƿ���и�ʴ������
	  //
	  void Erosion(UINT SEL, UINT type)
	  {
		  UINT uLength   = 0;//�߶γ���
		  UINT uFirst    = m_aryData[0] & 0x01;//��һ��Ԫ��
		  BOOL bInc      = TRUE;//�±����ѭ��
		  UINT uEndPos   = 0;
		  UINT uStartPos = 0;
		  if(uFirst == type)
		  {
			  uLength  = 0;

			  //˳ʱ����Ҷ�����'0'�����߶�����'1'���Ľ���λ��
			  while(GetBit(uEndPos) == type && uEndPos < m_uBitLength)
			  {
				  uEndPos++;
				  uLength++;
			  }

			  if(uEndPos == m_uBitLength)//������ȫΪ1
			  {
				  return;
			  }

			  bInc = FALSE;//ʣ�²��ְ���ʱ��˳����
			  uStartPos =  m_uBitLength - 1;

		  }
		  else
		  {
			  uStartPos = 1;
			  uEndPos   = m_uBitLength - 1;
		  }


		  UINT pos = uStartPos;


		  if(bInc)//��˳ʱ��˳��(�±����)����
		  {
			  UINT uBitOffset = uStartPos%32;
			  DWORD* pdwData  = &m_aryData[uStartPos>>5];
			  DWORD dwData    = *pdwData;


			  while(pos != uEndPos)
			  {

				  if((dwData & (1<<uBitOffset)) == (type << uBitOffset))
				  {
					  uLength ++;
				  }
				  else
				  {
					  if(uLength > 0  && uLength < SEL)
					  {
						  UINT uChangePos = pos - 1;

						  do
						  {

							  if(type)
							  {
								  ClearBit(uChangePos);
							  }
							  else
							  {
								  SetBit(uChangePos);
							  }

							  uChangePos--;
							  uLength --;
						  }while(uLength > 0);
					  }

					  uLength = 0;
				  }

				  uBitOffset++;

				  if(uBitOffset == 32)
				  {
					  pdwData ++;
					  dwData  = *pdwData;
					  uBitOffset = 0;
				  }

				  pos++;
			  }//while



			  //���һ���߶�
			  if(uLength > 0  && uLength < SEL)
			  {
				  UINT uChangePos = pos - 1;

				  do
				  {

					  if(type)
					  {
						  ClearBit(uChangePos);
					  }
					  else
					  {
						  SetBit(uChangePos);
					  }

					  uChangePos--;
					  uLength --;
				  }while(uLength > 0);
			  }


		  }

		  else//����ʱ��˳��(�±�ݼ�)����
		  {
			  UINT uBitOffset = uStartPos%32;
			  DWORD* pdwData  = &m_aryData[uStartPos>>5];
			  DWORD dwData    = *pdwData;

			  while(pos != uEndPos)
			  {
				  if((dwData & (1<<uBitOffset)) == (type << uBitOffset) )
				  {
					  uLength ++;
				  }
				  else
				  {
					  if(uLength > 0 && uLength < SEL)
					  {
						  UINT uChangePos = pos + 1;

						  do
						  {
							  if(uChangePos == m_uBitLength)
							  {
								  uChangePos = 0;

							  }
							  if(type)
							  {
								  ClearBit(uChangePos);
							  }
							  else
							  {
								  SetBit(uChangePos);
							  }

							  uChangePos++;

							  uLength --;
						  }while(uLength > 0);

					  }

					  uLength = 0;
				  }


				  if(uBitOffset == 0)
				  {

					  pdwData --;
					  dwData  = *pdwData;
					  uBitOffset = 31;
				  }
				  else
				  {
					uBitOffset -- ;
				  }

				  pos--;
			  }//while


			  if(uLength > 0 && uLength < SEL)
			  {
				  UINT uChangePos = pos + 1;

				  do
				  {
					  if(uChangePos == m_uBitLength)
					  {
						  uChangePos = 0;

					  }
					  if(type)
					  {
						  ClearBit(uChangePos);
					  }
					  else
					  {
						  SetBit(uChangePos);
					  }

					  uChangePos++;

					  uLength --;
				  }while(uLength > 0);
			  }//if

		  }//else
	  }


	  //�����ƴ��е��߶κϲ���
	  //����������ַ���
	  //�ӵ�λ----------------->��λ
	  // 0000 0111  1111 1011 1001 1111 1111 0111 0111 11 00
	  //��10���߶�
	  //start   end  length type
	  //0x26    0x04  7      0
	  //0x05    0x0B  8      1
	  //0x0D    0x0D  1      0
	  //0x0E    0x10  3      1
	  //0x11    0x12  2      0 
	  //0x13    0x1B  9      1
	  //0x1C    0x1C  1      0
	  //0x1D    0x1F  3      1
	  //0x20    0x20  1      0
	  //0x21    0x25  5      1
	  //
	  //�����ϲ�������,�����ַ�������Ϊ�����߶�
	  //0000 0111  1111 1111 1111 1111 1111 1111 1111 11 00
	  //  
	  // (��λ)        (��λ)
	  // Left    Line  Right
	  //
	  //���ڵ�һ���߶εĺϲ���Line 0
	  // Line(N-1)    Line0 Line 1
	  //
	  //�������һ���߶εĺϲ������Ǻϲ���0���߶���ȥ��
	  //Line(N-2) Line(N-1) 0
	  //
	  void Merge(UINT SEL)
	  {
		  //ͳ�������߶�
		  StatisticLineSemgent();

		  UINT iLineIndex  = 0;
		  

		  //1.�����������߶γ��ȶ�С��SEL,���������ϲ���ĳ��ȴ���SEL,������������,���Ƚ��кϲ�
		  if(m_uLineSegmentCount >2 )
		  {
			 
			  while(iLineIndex < m_uLineSegmentCount)
			  {
				  UINT uRightIndex = 0;
				  UINT uLeftIndex  = 0;

				  if(iLineIndex == 0)
				  {
						uLeftIndex = m_uLineSegmentCount - 1;
				  }
				  else
				  {
						uLeftIndex = iLineIndex - 1;
				  }

				  if(iLineIndex == m_uLineSegmentCount-1)
				  {

					  uRightIndex = 0;
				  }
				  else
				  {
					  uRightIndex = iLineIndex + 1;
				  }


				  UINT a = m_aryLineSegments[uLeftIndex].nLength;
				  UINT b = m_aryLineSegments[iLineIndex].nLength;
				  UINT c = m_aryLineSegments[uRightIndex].nLength;

				  
				  //if(a < SEL && b < SEL && c < SEL && (a + b +c) >= SEL && (a+c) > 2*b)
				  if(b < SEL  && (a + b +c) >= SEL && (a+c) > 2*b)
				  {

					  //�ϲ������߶Ρ�
					  //�ϲ�����ǰǰ�߶���
					 if(iLineIndex == 0)
					 {
						 m_aryLineSegments[iLineIndex].nLength   = a + b +c;
						 m_aryLineSegments[iLineIndex].nStartPos = m_aryLineSegments[uLeftIndex].nStartPos;
						 m_aryLineSegments[iLineIndex].nEndPos   = m_aryLineSegments[uRightIndex].nEndPos;
						 m_aryLineSegments[iLineIndex].type      = m_aryLineSegments[uRightIndex].type;

						 //��2...m_uLineSegmentCount-1��ǰŲ��һ����Ԫ
						 for(UINT iLoop = 1; iLoop < m_uLineSegmentCount - 2; iLoop ++)
						 { 
							m_aryLineSegments[iLoop] = m_aryLineSegments[iLoop + 1];
						 }

					 }
					 else if(iLineIndex == m_uLineSegmentCount - 1)
					 {

						 m_aryLineSegments[0].nLength   = a + b +c;
						 m_aryLineSegments[0].nStartPos = m_aryLineSegments[uLeftIndex].nStartPos;
						 m_aryLineSegments[0].nEndPos   = m_aryLineSegments[uRightIndex].nEndPos;
						 m_aryLineSegments[0].type      = m_aryLineSegments[uRightIndex].type;

					 }
					 else//�ϲ������߶���
					 {
						
					 	 m_aryLineSegments[uLeftIndex].nLength   = a + b + c;
						 //m_aryLineSegments[uLeftIndex].nStartPos = m_aryLineSegments[uLeftIndex].nStartPos;
						 m_aryLineSegments[uLeftIndex].nEndPos   = m_aryLineSegments[uRightIndex].nEndPos;
						//m_aryLineSegments[uLeftIndex].type     = m_aryLineSegments[uRightIndex].type;

						 //��uRightIndex+1...m_uLineSegmentCount-1��ǰŲ��������Ԫ
						 for(UINT iLoop = uRightIndex + 1; iLoop <= m_uLineSegmentCount-1; iLoop ++)
						 {
							m_aryLineSegments[iLoop-2] = m_aryLineSegments[iLoop];
						 }

					 }
					  

					 m_uLineSegmentCount -= 2;

					 if(m_uLineSegmentCount <=2 )
					 {
						break;
					 }
					  
					 //�����˺ϲ�����,��ǰ�߶�λ�����������µ�����,�����ǵ�һ���߶ε�����
					 //�߶������Ų�����
					 if(iLineIndex == 0)
					 {
						iLineIndex++;
					 }

				  }
				  else
				  {
					  iLineIndex++;
				  }


			  }//while()
		  }//if(segmengLine >= 4)


		  //2.�����ߺϲ��ɳ��ߡ�
		  //iLineIndex = 0;
		  //if(m_uLineSegmentCount >=4 )
		  //{
			 //
			 // while(iLineIndex < m_uLineSegmentCount)
			 // {
				//  UINT uRightIndex = 0;
				//  UINT uLeftIndex  = 0;

				//  if(iLineIndex == 0)
				//  {
				//		uLeftIndex = m_uLineSegmentCount - 1;
				//  }
				//  else
				//  {
				//		uLeftIndex = iLineIndex - 1;
				//  }

				//  if(iLineIndex == m_uLineSegmentCount-1)
				//  {

				//	  uRightIndex = 0;
				//  }
				//  else
				//  {
				//	  uRightIndex = iLineIndex + 1;
				//  }

				//  


				//  UINT a = m_aryLineSegments[uLeftIndex].nLength;
				//  UINT b = m_aryLineSegments[iLineIndex].nLength;
				//  UINT c = m_aryLineSegments[uRightIndex].nLength;
				//  if(b < SEL && (a+c) > b )
				//  {

				//	  //�ϲ������߶Ρ�
				//	  //�ϲ�����ǰ�߶���
				//	 if(iLineIndex == 0)
				//	 {
				//		 m_aryLineSegments[iLineIndex].nLength   = a + b +c;
				//		 m_aryLineSegments[iLineIndex].nStartPos = m_aryLineSegments[uLeftIndex].nStartPos;
				//		 m_aryLineSegments[iLineIndex].nEndPos   = m_aryLineSegments[uRightIndex].nEndPos;
				//		 m_aryLineSegments[iLineIndex].type      = m_aryLineSegments[uRightIndex].type;

				//		 //��2...m_uLineSegmentCount-1��ǰŲ��һ����Ԫ
				//		 for(UINT iLoop = 1; iLoop < m_uLineSegmentCount - 2; iLoop ++)
				//		 { 
				//			m_aryLineSegments[iLoop] = m_aryLineSegments[iLoop + 1];							
				//		 }

				//	 }
				//	 else//�ϲ������߶���
				//	 {
				//		
				//	 	 m_aryLineSegments[uLeftIndex].nLength   = a + b + c;
				//		 //m_aryLineSegments[uLeftIndex].nStartPos = m_aryLineSegments[uLeftIndex].nStartPos;
				//		 m_aryLineSegments[uLeftIndex].nEndPos   = m_aryLineSegments[uRightIndex].nEndPos;
				//		//m_aryLineSegments[uLeftIndex].type     = m_aryLineSegments[uRightIndex].type;

				//		 //��uRightIndex+1...m_uLineSegmentCount-1��ǰŲ��������Ԫ
				//		 for(UINT iLoop = uRightIndex + 1; iLoop <= m_uLineSegmentCount-1; iLoop ++)
				//		 {
				//			m_aryLineSegments[iLoop-2] = m_aryLineSegments[iLoop];
				//		 }

				//	 }
				//	  

				//	 m_uLineSegmentCount -= 2;

				//	 if(m_uLineSegmentCount <=4 )
				//	 {
				//		break;
				//	 }

				//  }

				//  iLineIndex++;

			 // }//while()
		  //}//if(segmengLine >= 4)


	  }


	  UINT GetLineSegmentCount()const
	  {
		return m_uLineSegmentCount;
	  }

	    UINT GetLineSegmentCount(unsigned int SEL)const
	  {

			UINT iLineIndex  = 0;
			int nLineCount = 0;
		  	 while(iLineIndex < m_uLineSegmentCount)
			 {
				if(m_aryLineSegments[iLineIndex].nLength  >= SEL)
				{
					nLineCount++;
				}
				iLineIndex++;
			 }

		return nLineCount;
	  }

   //methods
	//ͳ��'0', '1'�����ƴ����е��߶�������
	int StatisticLineSemgent()
	{

		m_uLineSegmentCount = 0;

		UINT uLineStartPos = 0;
		UINT uLoopEndPos   = 0;
		UINT uPos          = 0;
		UINT uLength       = 0;
		UINT uLineType     = 0;

		if(m_uBitLength == 0)
		{

			return 0;
		}

		if(m_uBitLength == 1)
		{
			m_aryLineSegments[0].nStartPos = 0;
			m_aryLineSegments[0].nEndPos   = 0;
			m_aryLineSegments[0].nLength   = 1;
			m_aryLineSegments[0].type      = GetBit(0);
			m_uLineSegmentCount = 1;
			return m_uLineSegmentCount;

		}

		//1.������һ���߶εĳ���
		UINT uFirstBit = GetBit(0);
		UINT uLastBit  = GetBit(m_uBitLength - 1);

		//�߶ο�Խͷβ
		if(uFirstBit ==  uLastBit)
		{
			uLength = 1;
			uLoopEndPos = m_uBitLength - 1;


			while(uLoopEndPos >0 &&   GetBit(uLoopEndPos) == uLastBit)
			{
				uLoopEndPos --;
				uLength++;
			}

			if(uLoopEndPos == 0)//ֻ��һ���߶�
			{
				m_aryLineSegments[0].nStartPos = 0;
				m_aryLineSegments[0].nEndPos   = m_uBitLength - 1;
				m_aryLineSegments[0].nLength   = m_uBitLength;
				m_aryLineSegments[0].type      = uLastBit;

				m_uLineSegmentCount = 1;
				return m_uLineSegmentCount;
			}

			uLineStartPos = uLoopEndPos + 1;
			uPos          = 1;
			uLineType     = uLastBit;


		}
		else
		{ 
			uPos         = uLineStartPos = 1;
			uLength      = 1;
			uLineType    = GetBit(0);
			uLoopEndPos  = m_uBitLength - 1;
		}


		UINT uBitOffset = uPos % 32;
		DWORD* pdwData  = &m_aryData[uPos>>5];
		DWORD dwData    = *pdwData;


		while(uPos <= uLoopEndPos)
		{
			UINT val = (dwData & (1<<uBitOffset))?1:0;
			if(val == uLineType)
			{
				uLength ++;
			}
			else
			{

				m_aryLineSegments[m_uLineSegmentCount].nStartPos = uLineStartPos;
				m_aryLineSegments[m_uLineSegmentCount].nEndPos   = uPos - 1;
				m_aryLineSegments[m_uLineSegmentCount].nLength   = uLength;
				m_aryLineSegments[m_uLineSegmentCount].type      = uLineType;

				m_uLineSegmentCount++;


				uLineStartPos = uPos;
				uLineType = val;
				//if(uPos != uLoopEndPos)
				//{
				//	uLength  = 1;
				//}
				//else
				//{
					uLength  = 1;
				//}

			}

			uPos++;
			uBitOffset++;
			if(uBitOffset == 32)
			{
				uBitOffset = 0;
				dwData    = *(++pdwData);	

			}

		}//while(1);


		//���һ���߶�
		if(uLength > 0)
		{
			m_aryLineSegments[m_uLineSegmentCount].nStartPos = uLineStartPos;
			m_aryLineSegments[m_uLineSegmentCount].nEndPos   = uPos - 1;
			m_aryLineSegments[m_uLineSegmentCount].nLength   = uLength;
			m_aryLineSegments[m_uLineSegmentCount].type      = uLineType;
			m_uLineSegmentCount++;
		}


		return m_uLineSegmentCount;
	}
protected://members
	DWORD m_aryData[((MAX_LAYER<<3) + 16) / 32];
	UINT  m_uBitLength;

	TLineSegment m_aryLineSegments[MAX_LAYER<<3];//�߶�����,��˵�������1λ��һ���߶�
	UINT m_uLineSegmentCount;
};



/* Type declarations */
#ifdef _MSC_VER
#include <basetsd.h>
typedef UINT8  uint8_t;
typedef UINT16 uint16_t;
typedef INT16  int16_t;
typedef UINT32 uint32_t;
#pragma warning( disable: 4799 )
#else
#include <stdint.h>
#endif

class CRobustCornerDetector
{
public:
	
	BOOL operator ()(const CImageFrame& refSrcFrame, const CImageFrame& refMaskFrame, UINT uExpectedCornerNumber, int nDetectRadius, ECalibDebugLevel eDebugLevel=E_CALIB_DEBUG_LEVEL_CONCISE, int nDebugTimes =1)
	{
		//window size range[WSmin, WSmax] to limit the window size
		// for extreme large or small grid.

		//Debug_SaveImageFrame(frame, _T("Source.bmp"));


		CImageFrame frame = refSrcFrame;
        
        frame = frame & refMaskFrame;

		if(eDebugLevel == E_CALIB_DEBUG_LEVEL_DEBUG)
		{
			Debug_SaveImageFrame(frame, _T("CheckerDetector_Raw.jpg"));
			
		}

        CImageFrame frameBlurred = frame;


        CBlurOperator<2> blur;
        blur(frame.Width(), frame.Height(), frame.GetData(), frameBlurred.GetData());
        frame = frameBlurred;

        


		UINT nWidth  = frame.Width();
		UINT nHeight = frame.Height();
		CImageFrame response;
		response.SetSize(nWidth, nHeight, 2);
        response.Clear();

		
       cross_detect5(nWidth, nHeight, frame.GetData(), response.GetData());
		if(eDebugLevel == E_CALIB_DEBUG_LEVEL_DEBUG)
		{
			Debug_SaveImageFrame(response, _T("Corner_Response.jpg"));
			
		}
        //corner_detect5����Ŀ����Ӧͼ��Ϊ����ͼ
        CImageFrame candidateMaskFrame = refMaskFrame;
        
        BYTE*    pCandidateMaskFrame = candidateMaskFrame.GetData();
        uint8_t* pResponse  = response.GetData();

        uint8_t response_Threshold = frame.Avg();
        response_Threshold >>= 1;
        for(UINT row=0; row<nHeight; row++)
		{
            for(UINT col=0; col<nWidth; col++)
			{
                if(*pCandidateMaskFrame == 0xFF)
                {
                    if(*pResponse< response_Threshold)
                    {
                      *pCandidateMaskFrame = 0x00;
                    }
                }

                pResponse++;
                pCandidateMaskFrame++;
            }
        }
		if(eDebugLevel == E_CALIB_DEBUG_LEVEL_DEBUG)
		{
			Debug_SaveImageFrame(candidateMaskFrame, _T("CheckerDetector_Mask.jpg"));
		}



		
		//CMeidanFilter<> medianFilter;

		//medianFilter(frame);
		//medianFilter(frame);
		//Debug_SaveImageFrame(frame, _T("source_after_median_filter.bmp"));
		//if(nDetectRadius <= MIN_LAYER)
		{
			//medianFilter(frame);
			//Debug_SaveImageFrame(frame, _T("source_after_median_filter.bmp"));
		}
		


		//CHistogram<> histo(0,255);
		//CImageFrame histoImage;

		//histo.DoStatistic(frame, maskFrame);
		//histo.AvgFilter(3);		
		//histo.SaveToImage(histoImage);
		//Debug_SaveImageFrame(histoImage, _T("histo_calibrate_pattern.bmp"));

		const int WSmin = MIN_LAYER;//
		int WSmax = MAX_LAYER;// PROFILE::GetAutoCalibrateMaxDetectLayer();//MAX_LAYER;//

		//if(WSmax > MAX_LAYER)
		//{
		//	WSmax = MAX_LAYER;
		//}

		int nW = nDetectRadius;
		if(nW < WSmin)
		{
			nW = WSmin;
		}

		if(nW > WSmax)
		{
			nW = WSmax;
		}


		BOOL bRet = TRUE;

		RectScanWindow scanWindow(nW);

		std::vector<int> scanResults;
		scanResults.resize(nW);



		UINT nImageWidth  = frame.Width();
		UINT nImageHeight = frame.Height();

		//��������Ҷ�ͼ
		//<<
		CImageFrame debugFrame;
		debugFrame.SetSize(nImageWidth, nImageHeight, 1);
		BYTE* pDebug  = debugFrame.GetData();
		//>>

		//λͼ����
		CBitFrame bitFrame;
		bitFrame.SetSize(nImageWidth, nImageHeight);
		bitFrame.Clear();
		//ָ���ָ��λͼ���׵�ַ��ָ��
		DWORD* p32BitData   = bitFrame.GetData();
		UINT uBitOffset     = 0;

		//
		const BYTE* pSrcData = frame.GetData();		
      

		const BYTE* pMaskData            = refMaskFrame.GetData();
		const BYTE* pMaskDataCursor      = pMaskData;
        const BYTE* pCandidateData       = candidateMaskFrame.GetData();
        const BYTE* pCandidateDataCursor = pCandidateData;

		//std::vector<BYTE> layerData;
		//std::vector<BYTE> diffData;
		BYTE layerData[MAX_LAYER * 8];

		//���
		//layerData.resize(nW * 8 * 2 );
		//diffData.resize (nGridWidth * 8 * 2);

		//std::vector<BYTE> layerData;

		//int n4RegionThreshold = nW * 3/5;
		//int n4RegionThreshold = nW * 7/10;

        //2012/08/13,�����Ѿ�ͨ��corner_detect5�������������ͼ��
        //���ŵ��Ѿ���С��
        //<<
		int n4RegionThreshold = nW * 7/10;
        //>>

		//int n4RegionThreshold = nW * 4/5;//modified by toxuke@gmail.com, 2011/12/26
		//int n4RegionThreshold = nW * 1/2;

		CBitRing ring;


		UINT SEL = 0;
		//��ÿ��ѭ��,
		//���ڼ������޴�, �����Ҫ�����㷨���Ż�
		for(UINT row=0; row<nImageHeight; row++)
		{
			
			//��ÿ��ѭ��
			for(UINT col=0; col<nImageWidth; col++)
			{

				if( *pCandidateDataCursor == 0)
				{
					goto NEXT;

				}

				int nDebug = 0;
				//if(col == 95 && row == 257) 
				//{
				//	nDebug = 1;
				//}
				//else if(row ==401 && col == 314)
				//{
				//	 nDebug = 2;

				//}

				//if(col == 176 && row == 302)
				//{
				//	int nDebug = 0;	
				//}

				for(int layer=0; layer<nW; layer++)
				{

					//if(row == 477 && col == 527 && layer == 9)
					//{
					//	AtlTrace(_T("col=%d\r\n"), col);
					//}

					ring.Clear();
					ring.SetLength((layer+1)<<3);

					const CLayer* pLayer = scanWindow.GetLayer(layer);
					//assert(pLayer);

					int layerSize  = pLayer->GetLayerSize();
					const POINT* pOffset = pLayer->GetOffsetList();


					int nSum = 0;
					int nCount = 0;
					BOOL bLayerIsNotComplete = false;//Ȧ�㲻����
					for(int i=0;  i <layerSize; i++)
					{

						int nX = col + pOffset[i].x;
						int nY = row + pOffset[i].y;

						//Ȧ�㲿����ͼ������
						if(nX < 0 || nX >= int(nImageWidth) )
						{
							bLayerIsNotComplete = TRUE;
							break;
							//continue;
						}
						//Ȧ�㲿����ͼ������
						if(nY < 0 || nY >= int(nImageHeight))  
						{
							
							bLayerIsNotComplete = TRUE;
							break;
							//continue;
						}


						//Ȧ����������������
						if(pMaskData[nImageWidth*nY + nX] == 0x00)
						{
							bLayerIsNotComplete = TRUE;
							break;
						}


						layerData[i] = pSrcData[nImageWidth*nY + nX];
						nSum += pSrcData[nImageWidth*nY + nX];
						nCount ++;

					}//for-each(i);



					if(bLayerIsNotComplete)//Ȧ����������������
					{
						scanResults[layer] = 0;
					}
					else//Ȧ��������������
					{

						//ͳ�����ؾ�ֵ	
						BYTE avgIntensity = 0;

						if(nCount != 0)
						{
							avgIntensity = (nSum + (nCount>>1))/nCount;
						}

						//avgIntensity = (avgIntensity + middleIntensity)/2;

						//if(avgIntensity < 250)
						//{
						//	avgIntensity += 2;
						//}
						
						//�ǵ�����������ƽ��ֵ�����ƫ������
						//static const int MIN_OFFSET_FROM_AVG_VAL = 0;
						static const int MIN_OFFSET_FROM_AVG_VAL = 4;

						//��һάLayer���ж�ֵ������
						for(int i=0;  i <layerSize; i++)
						{
							if(layerData[i] >= avgIntensity + MIN_OFFSET_FROM_AVG_VAL)
							{
								ring.SetBit(i);
								//layerData[i]  = 1;
							}
							//else
							//{
							//	layerData[i]  = 0;
							//}

						}//for-each(i)


						int nDifCount = 0;

						if(layer > 1)
						{
						
							
								SEL = ((layer+1)<<3)/10;


                                ring.Merge(SEL>3?3:SEL);
								//ring.Erosion(SEL, 1);
								//ring.Erosion(SEL, 0);
								//ring.Dilation(SEL);
								scanResults[layer] = ring.GetLineSegmentCount(SEL);
	

						}
						else
						{
							ring.StatisticLineSemgent();
							scanResults[layer] = ring.GetLineSegmentCount();
						}

					}
					//else
					//{
					   //scanResults[layer] = ring.GetPartCount();
					//}

				}//for-each(layer)


				int n4RegionLayerCount = 0;
				for(int layer=0; layer<nW; layer++)
				{
					if(scanResults[layer] == 4)
					{
						n4RegionLayerCount ++;
					}
				}

				if(n4RegionLayerCount >= n4RegionThreshold)
				{
					pDebug[row * nImageWidth + col] = 0xFF;

					*p32BitData |= (1<<uBitOffset);
				}

NEXT:
				uBitOffset++;
				if(uBitOffset == 32)
				{
					uBitOffset = 0;
					p32BitData++;

				}
                pCandidateDataCursor ++;
                pMaskDataCursor++;

			}//for-each(col);
		}//for-each(row)

		//
		//���
		if(eDebugLevel == E_CALIB_DEBUG_LEVEL_DEBUG)
		{
			Debug_SaveImageFrame(debugFrame, _T("Checker_Candidates.jpg"));
		}


		//<<debug
		if(eDebugLevel == E_CALIB_DEBUG_LEVEL_DEBUG)
		{
			CImageFrame overlapFrame;
			overlapFrame = frame;
			OverlapImage(overlapFrame,debugFrame);
			Debug_SaveImageFrame(overlapFrame, _T("Checker_Overlap_Source.jpg"));
		}
		//debug>>


		//
		CBitFrame resultFrame;


		//��������
        //����������Ŀ���
		Erosion(bitFrame, 1, 0, resultFrame);

		
		//<<debug
		if(eDebugLevel == E_CALIB_DEBUG_LEVEL_DEBUG)
		{
			pDebug      = debugFrame.GetData();
			p32BitData  = resultFrame.GetData();
			uBitOffset  = 0;


			for(UINT row=0; row<nImageHeight; row++)
			{
				//��ÿ��ѭ��
				for(UINT col=0; col<nImageWidth; col++)
				{


					*pDebug = (*p32BitData & (1<< uBitOffset))?0xFF:0x00;

					pDebug ++;

					uBitOffset ++;

					if(uBitOffset == 32)
					{
						uBitOffset = 0;
						p32BitData++;

					}
				}
			}



			Debug_SaveImageFrame(debugFrame, _T("Checkers_NoiseReduction.jpg"));
		}



		bitFrame = resultFrame;


        /*
		//���Ͳ���
		//Dilate(bitFrame, 1, 1, resultFrame);


		//<<debug
		if(eDebugLevel == E_CALIB_DEBUG_LEVEL_DEBUG)
		{
			pDebug      = debugFrame.GetData();
			p32BitData  = resultFrame.GetData();
			uBitOffset  = 0;


			for(UINT row=0; row<nImageHeight; row++)
			{
				//��ÿ��ѭ��
				for(UINT col=0; col<nImageWidth; col++)
				{


					*pDebug = (*p32BitData & (1<< uBitOffset))?0xFF:0x00;

					pDebug ++;

					uBitOffset ++;

					if(uBitOffset == 32)
					{
						uBitOffset = 0;
						p32BitData++;

					}
				}
			}

			Debug_SaveImageFrame(debugFrame, _T("Checkers_Dilation.bmp"));
		}*/



		if(eDebugLevel == E_CALIB_DEBUG_LEVEL_DEBUG)
		{
			CImageFrame overlapFrame;
			overlapFrame = frame;
			OverlapImage(overlapFrame,debugFrame);
			Debug_SaveImageFrame(overlapFrame, _T("Checker_Overlap_Source_After_Noise_Reduction.jpg"));
		}


		CConnectedComponentScanlineAlogrithm<> blobDetect;
		blobDetect.ProcessImage((const BYTE*)resultFrame.GetData(), resultFrame.Width(), resultFrame.Height());


		//
		CBitFrame edgeFrame;
		GetEdge(resultFrame, edgeFrame);


		//<<debug
		if(eDebugLevel == E_CALIB_DEBUG_LEVEL_DEBUG)
		{
			pDebug       = debugFrame.GetData();
			p32BitData   = edgeFrame .GetData();
			uBitOffset   = 0;


			for(UINT row=0; row<nImageHeight; row++)
			{
				//��ÿ��ѭ��
				for(UINT col=0; col<nImageWidth; col++)
				{

					*pDebug = (*p32BitData & (1<< uBitOffset))? 0xFF:0x00;
					pDebug ++;

					uBitOffset ++;
					if(uBitOffset == 32)
					{
						uBitOffset = 0;
						p32BitData++;
					}
				}
			}

			Debug_SaveImageFrame(debugFrame, _T("Checker_Edge.jpg"));
		}
		//debug>>



		std::vector<TBlobObject> blobObjs;
		
		const TBlobObject* pObj = blobDetect.GetObjs();

		int nObjCount = blobDetect.GetObjCount();
		int nCandidateNum = nObjCount;
		for(int i= 0; i < nObjCount; i++)
		{
			//�޳�����֮�� > 2��Ŀ��
			int nWidth  = pObj[i].rcArea.right  - pObj[i].rcArea.left;
			int nHeight = pObj[i].rcArea.bottom - pObj[i].rcArea.top;

            
			//if(nCandidateNum > uExpectedCornerNumber)
			{
				if(nWidth == 0 || nHeight == 0)
				{
					nCandidateNum -- ;
					continue;
				}
				else if(nWidth > nHeight && nWidth*2 > nHeight*5)//W/L > 5/2
				{
					nCandidateNum -- ;
					continue;
				}
				else if(nWidth < nHeight && nHeight*2 > nWidth*5)//L/W > 5/2
				{
					nCandidateNum -- ;
					continue;
				}
			}

			blobObjs.push_back(pObj[i]);
			nCandidateNum -- ;
		}

		if(blobObjs.size() == 0)
		{
			return FALSE;
		}


		//<<
		if(eDebugLevel == E_CALIB_DEBUG_LEVEL_DEBUG)
		{
			CImageFrame debugFrame2;
			debugFrame2 = debugFrame;
			for(UINT i= 0; i < blobObjs.size(); i++)
			{
				debugFrame2.DrawRectange(blobObjs[i].rcArea, 128);
			}
			Debug_SaveImageFrame(debugFrame2, _T("Checkers_Before_Merge.jpg"));
		}
		//>>

		//Merge(blobObjs, nGridWidth/2);
        Merge(blobObjs, edgeFrame, 3);//Modified by toxuke@gmail.com, 2012/08/16

		//Merge(blobObjs, edgeFrame, nDetectRadius/4);
		//Merge(blobObjs, edgeFrame,  5);//Modified by toxuke@gmail.com, 2011/12/26

        //

        //����Ŀ��֮����໥������бȽ���������Ŀ��
        FilterFalseObject(blobObjs, nDetectRadius);

        if(blobObjs.size() == 0) return FALSE;

		CHeapSortAlgorithm<TBlobObject> heapSort;


		//��������
		heapSort(&blobObjs[0], blobObjs.size(), false);


		if(blobObjs.size() < uExpectedCornerNumber)
		{
			bRet = FALSE;


    		for(UINT i=0; i<blobObjs.size(); i++)
			{
				debugFrame.DrawRectange(blobObjs[i].rcArea, 125);
                debugFrame.SetPixel(blobObjs[i].GPos, 0xFF);
			}


		}
		else
		{

			
            //��������
            m_vecCentroids.resize(uExpectedCornerNumber);
            m_vecCenters.resize(uExpectedCornerNumber);

			for(UINT i=0; i<uExpectedCornerNumber; i++)
			{

				//Modified by toxuke@gmail.com, 2013/04/11
				/*
                m_vecCentroids[i].x = blobObjs[i].GPos.x;//blobObjs[i].my/blobObjs[i].mass;
				m_vecCentroids[i].y = blobObjs[i].GPos.y;//blobObjs[i].mx/blobObjs[i].mass;

				*/
				m_vecCentroids[i].x =  (blobObjs[i].my << INT_SCALE_SHIFT_SIZE)/blobObjs[i].mass;
				m_vecCentroids[i].y  = (blobObjs[i].mx << INT_SCALE_SHIFT_SIZE)/blobObjs[i].mass;


               m_vecCenters[i].x = (blobObjs[i].rcArea.left  + blobObjs[i].rcArea.right ) /2;
               m_vecCenters[i].y = (blobObjs[i].rcArea.top   + blobObjs[i].rcArea.bottom) /2;

               //ȡ�������ĺ����ĵļ�Ȩƽ��ֵ��ΪУ��������
               //m_vecCenters[i].x = (m_vecCenters[i].x + m_vecCentroids[i].x + 1)>>1;
               //m_vecCenters[i].y = (m_vecCenters[i].y + m_vecCentroids[i].y + 1)>>1;
               //���Խ���: ��Ȩ���Ĳ��缸�����ġ�
               

				debugFrame.DrawRectange(blobObjs[i].rcArea, 125);
				//debugFrame.SetPixel(m_vecCentroids[i], 0xFF);
				debugFrame.SetPixel(blobObjs[i].GPos, 0xFF);
			}

		}

		//<<debug
		if(eDebugLevel == E_CALIB_DEBUG_LEVEL_DEBUG)
		{
			Debug_SaveImageFrame(debugFrame, _T("Checkers_Merged_And_Filtered.jpg"));
		}
		//debug>>

		return bRet;
	}




	//@����:srcFrame,ԭʼͼƬ
	//       W, ��ⴰ�ڰ뾶
	//       nThreshold, ��ʴ��׼,�����ص��ڽӵ���ĿС�ڸ�ֵ,������Ӧ�ñ���ʴ�����ء�
	//       resultFrame, [���],��ʴ��Ľ��ͼƬ
	//@˵��:��2W+1�ߴ�Ĵ����м��ÿ�����ص��ھӵĸ���, ����С������ֵֵ����Ϊ������Ϊ�����㣬��ͼ�����޳���
	void Erosion(const CBitFrame& srcFrame, int W, int nThreshold,  CBitFrame& resultFrame)
	{
		int nWidth  = srcFrame.Width();
		int nHeight = srcFrame.Height();

		resultFrame.SetSize(nWidth, nHeight);
		resultFrame.Clear();


		const DWORD* pSrcData     = srcFrame.GetData();
		const int dwordSizePerRow = nWidth>>5;
		DWORD* pResultData        = resultFrame.GetData();
		int nSrcBitOffset         = 0;


		for(int nRow=0; nRow<nHeight; nRow++)
		{
			for(int nCol = 0; nCol<nWidth; nCol++)
			{
				int nWindowPixelCount = 0;

				//����ֵΪ1
				if(*pSrcData & (1<<nSrcBitOffset))
				{
					for(int r=-W; r <= W; r++)
					{
						int y = nRow + r;

						if(y<0 || y>=nHeight) continue;

						const DWORD* pRowCenter = pSrcData + r*dwordSizePerRow;

						for(int c=-W; c <= W; c++)
						{
							if(r == 0 && c ==0) continue;
							int x = nCol + c;
							if(x<0 || x>=nWidth) continue;

							const DWORD* pdwData = pRowCenter;
							int nWindowBitOffset = nSrcBitOffset + c;//˫����ƫ��

							if(nWindowBitOffset < 0)
							{
								nWindowBitOffset += 32;
								pdwData -- ;

							}
							else if(nWindowBitOffset >= 32)
							{
								nWindowBitOffset -=32;
								pdwData ++ ;
							}

							if(*pdwData & (1<<nWindowBitOffset))
							{
								nWindowPixelCount++;
							}

						}//for(c)
					}//for(r)

					if(nWindowPixelCount > nThreshold)
					{
						//ֻ��ԭͼ����Ϊ1��λ��, �����������ͼƬ�Ķ�Ӧλ�ò�Ϊ1��
						*pResultData |= 1<<nSrcBitOffset;

					}

				}


				nSrcBitOffset++;
				if(nSrcBitOffset == 32)
				{
					nSrcBitOffset = 0;
					pSrcData++;
					pResultData++;

				}


			}//for-each(col)
		}//for-each(row)

	}


	//@����:srcFrame,ԭʼͼƬ
	//       W, ��ⴰ�ڰ뾶
	//       nThreshold,  ���ͱ�׼,����Ŀ�����ص��ڽӵ���Ŀ���ڵ��ڸ�ֵ��������ΪĿ�ꡣ
	//       resultFrame, [���],���ͺ�Ľ��ͼƬ
	//@˵��:��2W+1�ߴ�Ĵ����м��ÿ�����ص��ھӵĸ���, ����С������ֵֵ����Ϊ������Ϊ�����㣬��ͼ�����޳���
	void Dilate(const CBitFrame& srcFrame, int W, int nThreshold,  CBitFrame& resultFrame)
	{
		int nWidth  = srcFrame.Width();
		int nHeight = srcFrame.Height();

		
		resultFrame	= srcFrame;


		const DWORD* pSrcData     = srcFrame.GetData();
		const int dwordSizePerRow = nWidth>>5;
		DWORD* pResultData        = resultFrame.GetData();
		int nSrcBitOffset         = 0;


		for(int nRow=0; nRow<nHeight; nRow++)
		{
			for(int nCol = 0; nCol<nWidth; nCol++)
			{
				int nWindowPixelCount = 0;

				//����ֵΪ0
				if((*pSrcData & (1<<nSrcBitOffset)) == 0)						
				{
					for(int r=-W; r <= W; r++)
					{
						int y = nRow + r;

						if(y<0 || y>=nHeight) continue;

						const DWORD* pRowCenter = pSrcData + r*dwordSizePerRow;

						for(int c=-W; c <= W; c++)
						{
							if(r == 0 && c ==0) continue;
							int x = nCol + c;
							if(x<0 || x>=nWidth) continue;							

							const DWORD* pdwData = pRowCenter;
							int nWindowBitOffset = nSrcBitOffset + c;//˫����ƫ��

							if(nWindowBitOffset < 0)
							{
								nWindowBitOffset += 32;
								pdwData -- ;

							}
							else if(nWindowBitOffset >= 32)
							{
								nWindowBitOffset -=32;
								pdwData ++ ;
							}

							if(*pdwData & (1<<nWindowBitOffset))
							{
								nWindowPixelCount++;
							}

						}//for(c)
					}//for(r)

					if(nWindowPixelCount >= nThreshold)
					{
						//ֻ��ԭͼ����Ϊ1��λ��, �����������ͼƬ�Ķ�Ӧλ�ò�Ϊ1��
						*pResultData |= 1<<nSrcBitOffset;

					}

				}


				nSrcBitOffset++;
				if(nSrcBitOffset == 32)
				{
					nSrcBitOffset = 0;
					pSrcData++;
					pResultData++;

				}


			}//for-each(col)
		}//for-each(row)

	}



	//
	//@����:��ȡλͼ������
	void GetEdge(const CBitFrame& srcFrame, CBitFrame& edgeFrame)
	{
		int nWidth  = srcFrame.Width();
		int nHeight = srcFrame.Height();
		edgeFrame.SetSize(nWidth, nHeight);


		const DWORD* pSrcData     = srcFrame.GetData();
		const int dwordSizePerRow = nWidth>>5;
		DWORD* pEdgeData        = edgeFrame.GetData();
		int nSrcDWordBitOffset            = 0;


		for(int nRow = 0; nRow < nHeight; nRow++)
		{
			for(int nCol = 0; nCol < nWidth; nCol++)
			{

				int nNeighbourCount  = 0;
				int x,y;
				x = nCol;
				y = nRow;


				if(x-1 < 0  || x+1 >= nWidth || y+1>= nHeight || y-1 < 0)//�޳���������ĵ�
				{

				}
				else
				{
					//8������
					for(int r = -1; r <= 1; r++)
					{
						const DWORD* pRowCenter = pSrcData + r*dwordSizePerRow;

						for(int c=-1; c <= 1; c++)
						{
							if(r == 0 && c == 0 ) continue;
							const DWORD* pdwData = pRowCenter;
							int   nEdgeDWordBitOffset = nSrcDWordBitOffset + c;//˫����ƫ��

							if(nEdgeDWordBitOffset < 0)
							{
								nEdgeDWordBitOffset += 32;
								pdwData -- ;
							}

							if(nEdgeDWordBitOffset >= 32)
							{
								nEdgeDWordBitOffset -= 32;
								pdwData++;
							}


							if(*pdwData & (1<< nEdgeDWordBitOffset))
							{
								nNeighbourCount++;
							}
						}//for-each(c)
					}//for-each(r)

				}

				if((*pSrcData & (1<<nSrcDWordBitOffset)) &&  nNeighbourCount < 8)//С��8������Ϊ�߽��
				{
					*pEdgeData |= (1<<nSrcDWordBitOffset);
				}

				nSrcDWordBitOffset++;

				if(nSrcDWordBitOffset == 32)
				{
					nSrcDWordBitOffset = 0;
					pSrcData++;
					pEdgeData++;

				}


			}//for_each(nCol)

		}//for_each(nRow)


	}



	//@����:����Ŀ������ľ�����кϲ�
	//@����:
	void Merge(std::vector<TBlobObject>& allObjs, int nMaxDist)
	{
		std::vector<TBlobObject>::iterator itCur = allObjs.begin();


		//��������
		for(;itCur != allObjs.end();itCur++)
		{
			itCur->GPos.x = itCur->my/itCur->mass;
			itCur->GPos.y = itCur->mx/itCur->mass;
			itCur->bIsValid = TRUE;
		}

		itCur = allObjs.begin();

		int nMaxDistSqure = nMaxDist*nMaxDist;
		for(;itCur != allObjs.end();itCur++)
		{		
			if(!itCur->bIsValid) continue;
			std::vector<TBlobObject>::iterator itCandidate = itCur ;
			while(1)
			{
				itCandidate++;
				if(itCandidate == allObjs.end())
				{
					break;
				}
				if(!itCandidate->bIsValid) continue;

				int nCur2CandiateDist = (itCandidate->GPos.x - itCur->GPos.x)*(itCandidate->GPos.x - itCur->GPos.x) + (itCandidate->GPos.y - itCur->GPos.y)*(itCandidate->GPos.y - itCur->GPos.y);

				if(nCur2CandiateDist < nMaxDistSqure)
				{


					BOOL bMerge = TRUE;
					//�ж��Ƿ���������ǵ�, ���ѡ�ϲ��ǵ����
					std::vector<TBlobObject>::iterator itCmp = itCur ;

					while(1)
					{
						itCmp ++;
						if(itCmp == allObjs.end()) break;
						if(itCmp == itCur) continue;

						int nCmp2CandidateDist = (itCmp->GPos.x - itCandidate->GPos.x)*(itCmp->GPos.x - itCandidate->GPos.x) + (itCmp->GPos.y - itCandidate->GPos.y)*(itCmp->GPos.y - itCandidate->GPos.y);
						int nCmp2CurDist       = (itCmp->GPos.x - itCur->GPos.x)*(itCmp->GPos.x - itCur->GPos.x) + (itCmp->GPos.y - itCur->GPos.y)*(itCmp->GPos.y - itCur->GPos.y);

						if(nCmp2CurDist < nMaxDistSqure)
						{
							continue;
						}
						else
						{

							if(nCmp2CandidateDist < nCur2CandiateDist)
							{
								bMerge = FALSE;
								break;
							}
						}

					}



					if(bMerge)
					{

						itCur->mass += itCandidate->mass;
						itCur->mx   += itCandidate->mx;
						itCur->my   += itCandidate->my;

						if(itCur->rcArea.left > itCandidate->rcArea.left)
						{
							itCur->rcArea.left = itCandidate->rcArea.left;
						}

						if(itCur->rcArea.top > itCandidate->rcArea.top)
						{
							itCur->rcArea.top = itCandidate->rcArea.top;
						}


						if(itCur->rcArea.right < itCandidate->rcArea.right)
						{
							itCur->rcArea.right = itCandidate->rcArea.right;
						}


						if(itCur->rcArea.bottom < itCandidate->rcArea.bottom)
						{
							itCur->rcArea.bottom = itCandidate->rcArea.bottom;
						}

						//itCmp = allObjs.erase(itCmp);
						itCandidate->bIsValid = FALSE;
					}

				}//if			

			}//while
		}

		while(itCur != allObjs.end())
		{
			if(!itCur->bIsValid)
			{
				itCur = allObjs.erase(itCur);
			}
			else
			{
				itCur++;
			}
		}




		//���¼�������
		itCur = allObjs.begin();
		for(;itCur != allObjs.end();itCur++)
		{
			itCur->GPos.x = itCur->my/itCur->mass;
			itCur->GPos.y = itCur->mx/itCur->mass;
		}

	}





	//@����:����Ŀ��߽�֮�����С�������ϲ�Ŀ��
	void Merge(std::vector<TBlobObject>& allObjs, const CBitFrame& edgeFrame, int nMaxDist)
	{
		int nMaxDistSqure = nMaxDist*nMaxDist;
		const int MERGE_MASS_THRESHOLD = 50;

		UINT nObjCount = allObjs.size();
		
		std::vector<int> vecEdgeNeighours;//��¼ÿ��Ŀ������ڽӵ������

		vecEdgeNeighours.resize(nObjCount);

		for(std::vector<int>::size_type iObj = 0; iObj < nObjCount; iObj++)
		{
			vecEdgeNeighours[iObj] = -1;
			allObjs[iObj].bIsValid = TRUE;
		}//for


		//����ÿ��Ŀ������߽���� < nMaxDist������ڽ�Ŀ�ꡣ
		for(std::vector<int>::size_type iObj = 0; iObj < nObjCount; iObj++)
		{
			int nMinEdgeDist2 = nMaxDistSqure;

			RECT& rcObj = allObjs[iObj].rcArea;
			for(std::vector<int>::size_type jCmpObj = 0; jCmpObj < vecEdgeNeighours.size(); jCmpObj++)
			{

				if(iObj == jCmpObj) continue;

				//��������߽����Ƚ�
				int nDist2 = GetEdgeDist2(edgeFrame, allObjs[iObj].rcArea, allObjs[jCmpObj].rcArea);
				if(nDist2 < nMinEdgeDist2)
				{
					vecEdgeNeighours[iObj] = jCmpObj;
					nMinEdgeDist2 = nDist2;
				}
			}

		}//for-each(obj)


		//�ϲ��ڽ�Ŀ��
		for(std::vector<int>::size_type iObj = 0; iObj < nObjCount; iObj++)
		{
			if(vecEdgeNeighours[iObj] == -1) continue;

			int nMergeObjIndex = vecEdgeNeighours[iObj];//�ϲ���ѡĿ��

			if(nMergeObjIndex == iObj)//���Լ�Ϊ�ڽӵ�, ˵��һ��"����߽��ڽ�"��ϵ������,
			{
				continue;
			}
			TBlobObject& obj1  = allObjs[iObj];
			TBlobObject& obj2  = allObjs[nMergeObjIndex];

			assert(obj1.bIsValid);
			assert(obj2.bIsValid);


			if(obj1.mass >= MERGE_MASS_THRESHOLD && obj2.mass >= MERGE_MASS_THRESHOLD)
			{
				continue;
			}


			obj2.mass += obj1.mass;
			obj2.mx   += obj1.mx;
			obj2.my   += obj1.my;

			if(obj2.rcArea.left > obj1.rcArea.left)
			{
				obj2.rcArea.left = obj1.rcArea.left;
			}

			if(obj2.rcArea.top > obj1.rcArea.top)
			{
				obj2.rcArea.top = obj1.rcArea.top;
			}


			if(obj2.rcArea.right < obj1.rcArea.right)
			{
				obj2.rcArea.right = obj1.rcArea.right;
			}


			if(obj2.rcArea.bottom < obj1.rcArea.bottom)
			{
				obj2.rcArea.bottom = obj1.rcArea.bottom;
			}

			obj1.bIsValid = FALSE;
			vecEdgeNeighours[iObj] = -1;


			//������iObjΪ�ڽӵ��Ŀ��ı��
			for(std::vector<int>::size_type jObj = iObj+1; jObj < nObjCount; jObj++)
			{
				if(vecEdgeNeighours[jObj] == iObj)
				{
					vecEdgeNeighours[jObj] = nMergeObjIndex;

					if(jObj == nMergeObjIndex)//�γ�"����߽��ڽ�"��ϵ�ջ�����Ҫ����ϵ�ջ���Ӧ�ĵ�ز�������"����߽��ڽ�"��ϵ���ڽӵ㼯
					{
						int nMinEdgeDist2 = nMaxDistSqure;
						vecEdgeNeighours[nMergeObjIndex] = -1;

						for(std::vector<int>::size_type kCmpObj = 0; kCmpObj < nObjCount; kCmpObj ++)
						{
							if(nMergeObjIndex == kCmpObj) continue;
							if(!allObjs[kCmpObj].bIsValid) continue;

							int nDist2 = GetEdgeDist2(edgeFrame, allObjs[nMergeObjIndex].rcArea, allObjs[kCmpObj].rcArea);
							if(nDist2 < nMinEdgeDist2)
							{

								vecEdgeNeighours[nMergeObjIndex] = kCmpObj;
								nMinEdgeDist2 = nDist2;
							}


						}

					}
				}

			}


		}

		std::vector<TBlobObject>::iterator itCur = allObjs.begin();

		while(itCur != allObjs.end())
		{
			if(!itCur->bIsValid)
			{
				itCur = allObjs.erase(itCur);
			}
			else
			{
				itCur++;
			}
		}




		//���¼�������
		itCur = allObjs.begin();
		for(;itCur != allObjs.end();itCur++)
		{
			itCur->GPos.x = itCur->my/itCur->mass;
			itCur->GPos.y = itCur->mx/itCur->mass;
		}
	}

    //@����:����Ŀ���֮��ľ���Ҫ���ڽǵ����뾶���ص�,�޳���Ŀ��
    void FilterFalseObject(std::vector<TBlobObject>& allObjs, int nMaxTargteDist)
    {
        
        const int DIST_MAX = nMaxTargteDist*nMaxTargteDist;

        //����ÿ��Ŀ��ľ���ƽ��С��DIST_MAX���ڽ�Ŀ�ꡣ
        std::vector<TBlobObject>::iterator itCur=allObjs.begin();

       
        while(itCur != allObjs.end())
        {

             BOOL bErase = FALSE;
            std::vector<TBlobObject>::iterator itComp = itCur + 1;
            for(;itComp != allObjs.end(); )
            {

                int nDist = (itCur->GPos.x - itComp->GPos.x)*(itCur->GPos.x - itComp->GPos.x) + (itCur->GPos.y - itComp->GPos.y)*(itCur->GPos.y- itComp->GPos.y);
                if(nDist < DIST_MAX)
                {
                    //����֮��ɾ������С��Ŀ��
                    if(itCur->mass > itComp->mass)
                    {
                        itComp = allObjs.erase(itComp);
                    }
                    else
                    {
                        
                        bErase = TRUE;
                        break;//break from inner for loop;
                    }


                    //

                }
                else
                {
                   itComp++;
                }

            }
            if(bErase)
            {
                itCur = allObjs.erase(itCur);
            }
            else
            {
                itCur ++;
            }

        }






    }






	//
	//@����:��������Ŀ��߽�֮�����������ƽ��
	int GetEdgeDist2(const CBitFrame& edgeFrame, const RECT& rc1, const RECT& rc2) const
	{

		int nWidth = edgeFrame.Width();
		const int dwordSizePerRow = nWidth>>5;
		const DWORD* pEdgeData =  edgeFrame.GetData();
		int nMinDist2 = (std::numeric_limits<int>::max)();


		int nDWordBitOffset_1 = 0;
		int nDWordBitOffset_2 = 0;



		const DWORD* pRowData_1 = pEdgeData + rc1.top * dwordSizePerRow + (rc1.left >> 5);


		for(LONG r1 = rc1.top; r1 < rc1.bottom; r1++)
		{

			const DWORD* pColData_1  = pRowData_1;
			nDWordBitOffset_1 = rc1.left % 32;

			for(LONG c1 = rc1.left; c1 < rc1.right; c1++)
			{

				//Ŀ��1������Ϊ1�Ĳ���
				if(*pColData_1 & (1<< nDWordBitOffset_1))
				{

					const DWORD * pRowData_2 = pEdgeData + rc2.top * dwordSizePerRow + (rc2.left >> 5);

					for(LONG r2 = rc2.top; r2 < rc2.bottom; r2++)
					{

						const DWORD* pColData_2 = pRowData_2;
						nDWordBitOffset_2 = rc2.left % 32;
						for(LONG c2 = rc2.left; c2 < rc2.right; c2++)
						{
							if(*pColData_2 & (1<<nDWordBitOffset_2))
							{

								int nDist2 = (r1 - r2)*(r1 - r2) + (c1 - c2)*(c1 - c2);

								if(nMinDist2 > nDist2)
								{
									nMinDist2 = nDist2;
								}

							}


							nDWordBitOffset_2 ++;
							if(nDWordBitOffset_2 == 32)
							{
								nDWordBitOffset_2 -= 32;
								pColData_2 ++;
							}

						}//for_each(c2)

						pRowData_2 += dwordSizePerRow;

					}//for_each(r2)

				}

				nDWordBitOffset_1 ++;
				if(nDWordBitOffset_1 == 32)
				{
					nDWordBitOffset_1 -= 32;
					pColData_1++;
				}



			}//for_each(c1)

			pRowData_1 += dwordSizePerRow;

		}//for_each(r1)

		return nMinDist2;
	}




	const std::vector<POINT>& GetCentroids()const
	{
		return m_vecCentroids;
	}

    const std::vector<POINT>& GetCenters()const
	{
		return m_vecCenters;
	}

protected:
	//�����
    //����У��������Ľ��
	std::vector<POINT> m_vecCenters;
    std::vector<POINT> m_vecCentroids;

};//