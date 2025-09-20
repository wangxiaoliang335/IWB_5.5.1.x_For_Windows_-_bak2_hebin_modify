#pragma once
#include "ImageFrame.h"

////@�� ��:����Otsu�����չ֮�����ĤΤ֤椭���㷨,����ǰ���ͱ���������ֵ
////@�� ��:pSrcData��ָ��ڰ��������ݵ��׵�ַ
////      nDataLength,���ݵĳ���
////@����ֵ:���ؼ���ó�������ֵ
////@˵ ��:������䷽������ԭ��������
////       between-class variance:
////      ��B = ��0�� ��1��(��1-��0)^2    ---(1)
////       ��0 + ��1 = 1
////             
////       ��0 = (��Ni)/N,  i��[1,k]
////       ��1, ǰ���ľ�ֵ
////       ��0, �����ľ�ֵ
////��  ��:
////		��ʽ(1)���˶�����N^2��õ��µĹ�ʽΪ
////      N^2 �� ��B  = N0 �� N1 ��(��1-��0)^2 --(2)
////      ���� 
////      N0 = (��Ni), i��[1,k]
////      N1= (��Nj) = N - N0;j��[k+1,L)
////
////      NΪ�������صĸ���, L����L���Ҷ�
////
////                  
////                    
////       ��0 = 1/N0 ����(i*Ni), i��[1,k]
////                    
////
////                         
////      ��U0 = N*��0   =  ��(i*Ni), i��[1,k]
////                        
////                     L
////        ��1 = 1/N1* ��(j*Nj), j��[k+1,L]
////                    j= k+1
////
////                        L
////      ��U1 = N1*��1  = ��(j*Nj),j��[k+1,L]
////                       j=k+1
////
////
////       U0 + U1 = U
////
////��(2)�����ٳ���(N1��N0)^2�õ�
////
////   Sigma = N^2 �� ��B �� (N0��N1)^2 =  N0 �� N1 ��(��1*N0*N1-��0*N0*N1)^2 = N0��N1 ��(U1*N0 - U0*N1)^2 --(3)
////
////  ֻ���ҳ�(3)�Ҷ����ʱ��Ӧ��kֵ��Ϊ���������ֵ???
////
//inline BYTE FindThreshold_Ostu(const INT (&histogram)[256])
//{
//    int L = 256;
//
//    BYTE cThreshold = 0;
//
//    int N = 0;
//    for(int i=0; i<L; i++)
//    {
//        N += histogram[i];
//    }
//    int N0 = 0;
//	int N1 = 0;
//
//	//  k
//	int U0 = 0;//U=��(i*Ni)
//	//  i=1
//
//
//	//  L
//	int U1 = 0;//U=��(i*Ni)
//	//  i=k+1
//
//
//	//  L
//	int U = 0; //U=��(i*Ni)
//	//  i=1
//	//__int64 SigmaMax = 0;
//	//__int64 Sigma = 0;
//
//
//	for(int i=0; i<L; i++)
//	{
//		U += i*histogram[i];
//	}
//
//    __int64  varMax = 0;
//
//	for(int i=0; i< L; i++)
//	{
//
//        int gray_count = histogram[i];
//        if(gray_count)
//        {
//            N0 += gray_count;//Weighted Backgound
//        }
//        else
//        {
//            continue;
//        }
//
//
//		N1  = N - N0;//Weighted Foreground
//        if(N1 == 0)
//        {
//            break;
//        }
//
//
//        U0 += i*histogram[i];
//
//		U1  = U - U0;
//
//
//        int mB = (U0 + (N0>>2)) /N0;//Mean Background;
//        int mF = (U1 + (N1>>2)) /N1;//Mean Foreground
//
//        // Calculate Between Class Variance
//         __int64 varBetween = (__int64)N0 * (__int64)N1 * (mB - mF) * (mB - mF);
//
//         // Check if new maximum found
//         if (varBetween > varMax) {
//              varMax = varBetween;
//              cThreshold = i;
//          }
//
//	}//for
//
//
//	return cThreshold;
//}
//



    //@����:��YUY2��ʽ�Ĳ�ɫλͼת��Ϊ�Ҷ�λͼ
    //@����:YUVFrame,����, YUY2��ʽ��ԭʼͼƬ֡
    //      monoFrame,���,�Ҷ�λͼ
	inline void Covert2MonochromeFrame_Y(const CYUY2Frame& YUVFrame, CImageFrame& monoFrame)
	{
		UINT nImageWidth  = YUVFrame.Width();
		UINT nImageHeight = YUVFrame.Height();
		monoFrame.SetSize(nImageWidth, nImageHeight, 1);

		const WORD* pYUVData = YUVFrame.GetData();
		BYTE* pMonoData      = monoFrame.GetData();
		for(UINT row = 0; row < nImageHeight; row++)
		{
			for(UINT col = 0; col < nImageWidth; col++)
			{

				*pMonoData = (*pYUVData)& 0x00FF;

				pMonoData++;

				pYUVData ++;

			}//foreach col
		}//foreach row
	}

	//@����:��YUY2��ʽ��ͼƬ�е�������ת��ΪRGB��ʽ, ����ȡR����,����Ϊ�Ҷ�
	inline void Covert2MonochromeFrame_R(const CYUY2Frame& YUVFrame, CImageFrame& monoFrame)
	{
		UINT nImageWidth  = YUVFrame.Width();
		UINT nImageHeight = YUVFrame.Height();
		monoFrame.SetSize(nImageWidth, nImageHeight, 1);

		const BYTE* pYUVData = (const BYTE*)YUVFrame.GetData();
		BYTE* pMonoData      = monoFrame.GetData();
		for(UINT row = 0; row < nImageHeight; row++)
		{
			for(UINT col = 0; col < nImageWidth; col+=2)
			{

				//BYTE Y, Cr, Cb;

				short Y, Cr, Cb;
				short R = 0;
				Cb = pYUVData[1];
				Cr = pYUVData[3];

				Cb = Cb - 128;
				Cr = Cr - 128;

				//˫���еĵ�һ������
				Y = pYUVData[0];

				R = Y + Cr + (Cr >> 2) + (Cr >> 3) + (Cr >> 5);

				if(R>255) R = 255;
				if(R<0) R = 0;

				*pMonoData =  (BYTE)R;
				pMonoData++;


				//˫���еĵڶ�������
				Y = pYUVData[2];
				R = Y + Cr + (Cr >> 2) + (Cr >> 3) + (Cr >> 5);
				if(R>255) R = 255;
				if(R<0) R = 0;
				*pMonoData = (BYTE)R;
				pMonoData++;



				pYUVData += 4;

			}//foreach col
		}//foreach row
	}


   //@����:�ڶ�ֵ�����Ҷ�ͼƬ����ȡ�������Ŀ��
	//@����:monoFrame, �ڰ׶�ֵ��ͼƬ,������Ϻ�ͼƬ��ֻ�����������Ŀ��
	//      pBoundary, ָ�򱣴������Ӿ��γߴ�Ļ������׵�ַ
	//      pPixelCount, ָ�򱣴����Ŀ�����ظ����Ļ������׵�ַ��ָ��
	//      pPtCenter, ָ�򱣴����Ŀ�����ĵĻ������׵�ַ��ָ��
	//@˵��:
	//
	inline BOOL ExtractTheLargestObject(CImageFrame& monoFrame, RECT* pBoundary = NULL, int* pPixelCount = NULL, POINT* pPtCenter = NULL)
	{

		BOOL bRet = FALSE;


		int nImageWidth  = (int)monoFrame.Width ();
		int nImageHeight = (int)monoFrame.Height();

		int nMaxObjCount = (nImageWidth/2 + 1)* (nImageHeight /2 + 1);//�����ܵ�Ŀ����Ŀ


		int* pLinkSet    = new int[nMaxObjCount];
		for(int i=0; i< nMaxObjCount; i++)
		{
			pLinkSet[i] = i;//ÿ����Ǽ���ֻ�б���Լ���
		}//for(i)


		int nLabelImageSize = nImageWidth * nImageHeight;
		int* pLabelImage = new int[nImageWidth * nImageHeight];
		memset(pLabelImage, 0, sizeof(int)*nImageWidth * nImageHeight);

		const BYTE* pImageDataCursor = monoFrame.GetData();

		int nNorthWest  ;//= -1 - m_nImageWidth;
		int nNorth      ;//= - m_nImageWidth;
		int nNorthEast  ;//= 1 - m_nImageWidth;
		int nWest       ;//= -1;


		int* pLabelCursor = pLabelImage;
		int nLabelSeed = 0;//��ǩ����
		//��ͼ����б��


		nNorthWest  = -1 - int(nImageWidth);
		nNorth      = - int(nImageWidth);
		nNorthEast  = 1 - int(nImageWidth);
		nWest       = -1;


		//��һ�е�һ��
		if(pImageDataCursor[0] != 0)
		{
			*pLabelCursor = ++nLabelSeed;
		}

		pLabelCursor++;
		pImageDataCursor++;
		//��һ��
		for(int col=1; col < nImageWidth; col++)
		{

			if(*pImageDataCursor != 0 )
			{
				//����Ԫ��
				if(pLabelCursor[nWest] != 0)
				{
					*pLabelCursor = pLabelCursor[nWest];
				}
				else
				{
					*pLabelCursor = ++nLabelSeed;
				}
			}

			pLabelCursor++;
			pImageDataCursor++;
		}

		//�ڶ��е����һ��
		for(int row = 1; row < nImageHeight; row++)
		{

			//��1��,û��West��Northwest�ھ�
			if(*pImageDataCursor != 0)
			{

				if(pLabelCursor[nNorth] != 0)
				{
					*pLabelCursor = pLabelCursor[nNorth];
				}
				else if(pLabelCursor[nNorthEast] != 0)
				{
					*pLabelCursor = pLabelCursor[nNorthEast];
				}
				else
				{
					*pLabelCursor = ++nLabelSeed;
				}

			}
			pImageDataCursor ++;
			pLabelCursor ++;

			//�ӵ�2�е�������2��
			for(int col=1; col < nImageWidth - 1; col++)
			{
				if(*pImageDataCursor !=0 )
				{

					if(pLabelCursor[nNorthWest] != 0)
					{
						*pLabelCursor = pLabelCursor[nNorthWest];
					}
					else if(pLabelCursor[nNorth] != 0)
					{
						*pLabelCursor = pLabelCursor[nNorth];
					}
					else if(pLabelCursor[nWest] != 0 )
					{
						*pLabelCursor = pLabelCursor[nWest];
					}

					//�Զ����ھ����⴦��, ���ܷ������鼯�ϵĺϲ�
					if(pLabelCursor[nNorthEast] !=0)
					{
						if(!*pLabelCursor)
						{
							*pLabelCursor = pLabelCursor[nNorthEast];
						}
						else//��ǰ�����붫�����ص�Label���ܲ�ͬ,��ͬ��Ҫ�����ȼ���Ļ���
						{


							int nCurLabel       = pLabelCursor[0];
							int nNorthEastLabel = pLabelCursor[nNorthEast];



							//
							if(nCurLabel != nNorthEastLabel//Label��ͬ
								&&
								pLinkSet[nCurLabel] != pLinkSet[nNorthEastLabel] //������ͬһ�ȼ���
							)
							{


								if(pLinkSet[nCurLabel] == nCurLabel)
								{
									pLinkSet[nCurLabel] = pLinkSet[nNorthEastLabel];

								}
								else if(pLinkSet[nNorthEastLabel] == nNorthEastLabel)
								{
									pLinkSet[nNorthEastLabel] = pLinkSet[nCurLabel];
								}
								else
								{
									//�������
									int nEqivalanceNo = pLinkSet[nCurLabel];
									//ע�⵽��������ĳ���ʼ�ղ�����2
									pLinkSet[nEqivalanceNo] = pLinkSet[nNorthEastLabel];

									//pLinkSet[nCurLabel]  = pLinkSet[nNorthEastLabel];


								}


								//������������ĳ��ȣ���֤�䳤��ʼ�ղ�����2
								for(int n=1; n<=nLabelSeed; n++)
								{
									if(pLinkSet[n] != n)
									{
										int nNext =  pLinkSet[n] ;
										while(nNext != pLinkSet[nNext])
										{
											nNext = pLinkSet[nNext];
										}
										pLinkSet[n] = nNext;
									}

								}//for each label  equivalenc set


							}
						}

					}//������Label��Ϊ0


					//�µı��
					if(!*pLabelCursor )
					{
						*pLabelCursor = ++nLabelSeed;
					}

				}//�������ز�Ϊ0


				pImageDataCursor ++;
				pLabelCursor ++;

			}//for each col

			//���е����һ��,û�ж������ھ�
			if(pImageDataCursor[0] != 0)
			{

				if(pLabelCursor[nWest] != 0)
				{
					pLabelCursor[0] = pLabelCursor[nWest];
				}
				else if(pLabelCursor[nNorth] != 0)
				{
					pLabelCursor[0] = pLabelCursor[nNorth];
				}
				else
				{
					pLabelCursor[0] = ++nLabelSeed;
				}

			}

			pImageDataCursor ++;
			pLabelCursor ++;

		}//for each row


		int nObjectCount = 0;

		//���·���Ŀ����
		for(int i = 1; i <= nLabelSeed; i++)
		{
			if(pLinkSet[i] == i)
			{
				int nNewObjectNo = ++ nObjectCount;

				int j = 1;
				while(j <= nLabelSeed)
				{
					if(pLinkSet[j] == i)
					{
						pLinkSet[j] = nNewObjectNo;

					}

					j++;
				}

			}
		}

		if(nObjectCount > 0 )
		{

			//������Labelͼ�����µ�Label���
			for(int i = 0; i < nLabelImageSize; i++)
			{
				pLabelImage[i] = pLinkSet[pLabelImage[i]];
			}

			//ͳ����ռÿ����������صĸ���
			int* pPixelStatistic = new int[nObjectCount+1];


			memset(pPixelStatistic, 0, sizeof(int)*(nObjectCount+1));
			for(int i=0; i<nLabelImageSize; i++)
			{
				pPixelStatistic[pLabelImage[i]]++;
			}


			//���ұ�ǵ����ı��ֵ
			int nMaxPixelCountLabel = 1;
			for(int i=2; i<= nObjectCount; i++)
			{
				if(pPixelStatistic[i] > pPixelStatistic[nMaxPixelCountLabel])
				{
					nMaxPixelCountLabel = i;
				}

			}


			int nPixelCount = 0;
			RECT rcBoundary;
			rcBoundary.left  = nImageWidth - 1;
			rcBoundary.right = 0;
			rcBoundary.top   = nImageHeight - 1;
			rcBoundary.bottom = 0;

			int nMx = 0;
			int nMy = 0;
			POINT ptCenter;

			BYTE* pImageDataCusor = monoFrame.GetData();
			for(int row = 0; row < nImageHeight; row++)
			{
				for(int  col = 0; col < nImageWidth; col++)
				{
					int nIndex = row*nImageWidth + col;
					if(pLabelImage[nIndex] == nMaxPixelCountLabel)
					{
						pImageDataCusor[nIndex] = 0xFF;

						if(rcBoundary.left   > col) rcBoundary.left   = col;
						if(rcBoundary.right  < col) rcBoundary.right  = col;
						if(rcBoundary.top    > row) rcBoundary.top    = row;
						if(rcBoundary.bottom < row) rcBoundary.bottom = row;

						nMx += row;
						nMy += col;

						nPixelCount++;
					}
					else
					{
						pImageDataCusor[nIndex] = 0x00;
					}

				}//for-each col

			}//for-each row


			ptCenter.x = nMy/nPixelCount;
			ptCenter.y = nMx/nPixelCount;

			bRet = TRUE;

			if(pBoundary)
				*pBoundary = rcBoundary;

			if(pPixelCount)
				*pPixelCount = nPixelCount;

			if(pPtCenter)
				*pPtCenter = ptCenter;
			delete [] pPixelStatistic;


		}
		else
		{

			bRet = FALSE;
		}


		delete[]  pLabelImage;
		delete[] pLinkSet;


		return bRet;
	}
	

	//@����:frame, �Ҷ�ͼƬ
	//@����:�ԻҶ�ͼƬ���в�������
	inline void FillHole(CImageFrame& frame)
	{
		int nImageWidth  = frame.Width();
		int nImageHeight = frame.Height();

		BYTE* pData       = frame.GetData();
		BYTE* pDataCursor = pData;

		//�Ƚ�����ɨ,���в���
		for(int nRow = 0; nRow < nImageHeight; nRow++)
		{
			int nLeftMost   = nImageWidth-1;
			int nRightMost  = 0;

			for(int nCol = 0; nCol < nImageWidth; nCol++)
			{
				if(*pDataCursor > 0)
				{
					if(nCol < nLeftMost ) nLeftMost  = nCol;
					if(nCol > nRightMost) nRightMost = nCol;
				}

				pDataCursor++;
			}

			BYTE* pFillDataCursor = pData + nRow*nImageWidth + nLeftMost;

			for(int nCol = nLeftMost; nCol <= nRightMost;  nCol++)
			{
				*pFillDataCursor = 0xFF;
				pFillDataCursor++;			
			}
		}


		//�ٽ�����ɨ,���в���
		for(int nCol = 0; nCol < nImageWidth; nCol++)
		{
			int nTopMost     = nImageHeight-1;
			int nBottomMost  = 0;

			for(int nRow = 0; nRow < nImageHeight; nRow++)
			{
				if(*(pData + nRow*nImageWidth + nCol) > 0)
				{
					if(nRow < nTopMost   ) nTopMost    = nRow;
					if(nRow > nBottomMost) nBottomMost = nRow;
				}

			}

			BYTE* pFillDataCursor = pData + nTopMost*nImageWidth + nCol;

			for(int nRow = nTopMost; nRow <= nBottomMost;  nRow++)
			{
				*pFillDataCursor = 0xFF;
				pFillDataCursor += nImageWidth;			
			}
		}

	}

    //����:��󻯶Աȶ�
    inline void MaximizeContrastness(CImageFrame& srcImage)
    {
        BYTE cMinVal = srcImage.Min();
        BYTE cMaxVal = srcImage.Max();

        BYTE map[256];

        BYTE val = 0;


        BYTE range = cMaxVal - cMinVal;

        if(range == 0) return;//��ֹ�������

        for(int i=cMinVal;  i <= cMaxVal; i++)
        {
            map[i]= 255*(i - cMinVal)/range;
        }//for

        BYTE* pSrcData = srcImage.GetData();
        int nLoop = srcImage.Size();
        while(nLoop--)
        {
            *pSrcData = map[*pSrcData];
            pSrcData++;
        }

    }



	//@����:���ò��ԭ������ȡ��Ļ����������
	inline void CalcMaskFrame(const CImageFrame& whiteFrame, const CImageFrame& blackFrame,  CImageFrame& maskFrame)
	{

		UINT nImageWidth  = whiteFrame.Width();
		UINT nImageHeight = whiteFrame.Height();
		maskFrame.SetSize(nImageWidth, nImageHeight, 1);

		const BYTE* pWhiteDataCursor = whiteFrame.GetData();
		const BYTE* pBlackDataCursor = blackFrame.GetData();
		BYTE* pMaskDataCursor        = maskFrame.GetData();

		int nPixelCount = maskFrame.Size();

		//int activePixelCount   = 0;
		//int ActiveIntensitySum = 0;


		UINT hist[256];//ֱ��ͼͳ��
		memset(hist, 0, sizeof(hist));

		for(int l=0; l < nPixelCount; l++)
		{

			BYTE cWhiteFrameVal = *pWhiteDataCursor;
			BYTE cBlackFrameVal = *pBlackDataCursor;
			BYTE cDifVal = 0;


			if(cWhiteFrameVal > cBlackFrameVal)
			{
				cDifVal = cWhiteFrameVal - cBlackFrameVal;
			}
			else
			{
				cDifVal = 0x00;//cBlackFrameVal - cWhiteFrameVal;
			}

			hist[cDifVal] ++;

			*pMaskDataCursor = cDifVal;

			pWhiteDataCursor ++;
			pBlackDataCursor ++;

			pMaskDataCursor  ++;
		}//for-each(l)



        //��󻯶Աȶ�
        //MaximizeContrastness(maskFrame);


		UINT hist2[256];//�˲����ֱ��ͼͳ��
		memset(hist2, 0, sizeof(hist2));

		//��ֱ��ͼ���о�ֵ�˲�
		for(int i=0; i< _countof(hist); i++)
		{
			int nColumn = 0;
			int nCount  = 0;
			int M = 5;
			for(int j = i - M/2; j <= i+ M/2; j++)
			{
				if(j < 0)continue;

				if(j >= 256) continue;

				nCount += hist[j];
				nColumn++;
			}//for-each(j)

			hist2[i] = nCount/nColumn;

		}//for-each(i)




		//�������ػ����ڵ��������͸�������������, ���ܹ�ʹ�ú���FindOptimumThresholdȷ����������
		//BYTE threshold = FindOptimumThreshold(hist2);
		const UINT FilterThreshold  = nPixelCount*1/256;
		WORD wMaxSearchRange = 0;
		//��255��ʼ���Ҹ���>FivePercentCount��ֱ��ͼ��Ԫ
		for(short c=255; c>=0; c--)
		{
			if(hist2[c] >= FilterThreshold)
			{
				wMaxSearchRange = (WORD)c;
				break;
			}
		}



		short  wPeak1, wPeak2;
        const int MIN_THRESHOLD = 5;
		//��hist2[nMinThreshold, wMaxSearchRange/2]�в��ҵ�һ��ֱ��ͼ���
		wPeak1 = 0;
		UINT nPeak1Count = 0;
		for(WORD  k=MIN_THRESHOLD; k < (wMaxSearchRange/2); k++)
		{
			if(hist2[k] > nPeak1Count)
			{
				wPeak1 = k;
				nPeak1Count = hist2[k];
			}
		};


		//��hist2[wMaxSearchRange/2, wMaxSearchRange]�в��ҵڶ���ֱ��ͼ���
		wPeak2 = 0;
		UINT nPeak2Count = 0;
		for(WORD k= wMaxSearchRange/2; k < wMaxSearchRange; k++)
		{
			if(hist2[k] > nPeak2Count)
			{
				wPeak2 = k;
				nPeak2Count = hist2[k];
			}

		}


		//��hist2[cPeak1, cPeak2]�в���ֱ��ͼ�ȵ�
		BYTE threshold = 5;//��������
		UINT nValleyCount  = (std::numeric_limits<int>::max)();
		for(WORD k = wPeak1; k < wPeak2; k++)
		{
			if(hist2[k] < nValleyCount)
			{
				nValleyCount = hist2[k];
				threshold = (BYTE)k;
			}

		}

        //threshold -=2;//��Ϊ��������,��ȷ��Ŀ�꾡��������


       //threshold = FindThreshold_Ostu(hist);

		pMaskDataCursor  = maskFrame.GetData();
		for(int l = 0; l < nPixelCount; l++)
		{
			if(*pMaskDataCursor >= threshold)
			{
				*pMaskDataCursor = 0xFF;
			}
			else
			{
				*pMaskDataCursor = 0x00;
			}
			pMaskDataCursor++;

		}

	}

    inline void WordFrame2ByteFrame(const CWordFrame& wordFrame, CImageFrame& byteFrame)
   {
        byteFrame.SetSize(wordFrame.Width(), wordFrame.Height(), 1);
        WORD max = wordFrame.Max();
        WORD min = wordFrame.Min();
        WORD range = max -min;
        const WORD* pWordPixel = wordFrame.GetData();

        BYTE* pPixel = byteFrame.GetData();
        int nLoopCount  = wordFrame.GetPixelCount();
        //16λ�Ҷ�λͼתΪ8λ�Ҷ�λͼ
        while(nLoopCount)
        {
            if(range == 0)
            {
                *pPixel = (BYTE)0xFF;
            }
            else
            {
                *pPixel = BYTE( (UINT)255 * (UINT)(*pWordPixel - min)/(UINT)range);
            }
            pWordPixel++;
            pPixel ++;
            nLoopCount --;
        }

    }

