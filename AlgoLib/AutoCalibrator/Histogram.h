#pragma once

//N, �и���.
//H, ͼƬ�߶�
template<class T=BYTE, int H = 400, int N=256>
class CHistogram
{

public:

	//@����:min, ���ݵ���Сֵ
	//      max, ���ݵ����ֵ
	CHistogram(T min, T max)
	{
		m_nMinVal = min;
		m_nMaxVal = max;
		Reset();
	}

	void Reset()
	{
		for(int i=0; i<N; i++)
		{
			m_bins[i].lowerVal = m_nMinVal +     i*(m_nMaxVal - m_nMinVal)/N;
			m_bins[i].upperVal = m_nMinVal + (i+1)*(m_nMaxVal - m_nMinVal)/N;
			m_bins[i].nCount   = 0;
		}

		m_nPixelCount = 0;
	}

    void SetHistData(const UINT hist[N])
    {
        Reset();

        for(int i=0; i<N; i++)
        {
            m_bins[i].nCount = hist[i];
        }

    }


	//@����:ͳ�ƻҶ�ͼ�еĻҶ�ֵ��ֱ��ͼ
	void DoStatistic(const CImageFrameT<T>& image)
	{
		const T*  pData = image.GetData();
		UINT nWidth  = image.Width();
		UINT nHeight = image.Height();
		UINT nCount  = nWidth * nHeight;


		for(UINT i=0; i < nCount; i++)
		{
			T value = *pData++;
			m_bins[value].nCount ++;

		}

		m_nPixelCount = nCount;
	}


	//@����:ͳ�ƻҶ�ͼ�еĻҶ�ֵ��ֱ��ͼ�ֲ�
	//@����:srcImage, ����ԴͼƬ
	//      maskFrame, ����ͼƬ, ����ֵ����0������Ϊ��������
	BOOL DoStatistic(const CImageFrameT<T>& srcImage,  const CImageFrameT<T>& maskFrame)
	{
		const T* pSrcData   = srcImage.GetData();
		const T* pMaskData  = maskFrame.GetData();
		UINT nWidth  = srcImage.Width();
		UINT nHeight = srcImage.Height();
		UINT nCount  = nWidth * nHeight;

		if(nWidth != maskFrame.Width() || nHeight != maskFrame.Height()) 
		{
			return FALSE;
		}

		//����ֱ��ͼͳ��ֵ��
		Reset();

		for(UINT i=0; i < nCount; i++)
		{
			T value = *pSrcData;
			T mask  = *pMaskData;

			if(T(0) != mask)
			{
				m_bins[value].nCount ++;
				m_nPixelCount ++;
			}

			pSrcData  ++;
			pMaskData ++;

		}

		return TRUE;
	}

	int operator[] (int k)const
	{
		return m_bins[k].nCount;
	}


	T GetLowerVal(int k)const
	{
		return m_bins[k].lowerVal;
	}


	T GetUpperVal(int k)const
	{
		return m_bins[k].upperVal;
	}

	void Statistic(T val)
	{
		for(int i=0; i<N; i++)
		{
			if( m_bins[i].lowerVal <= val && val <= m_bins[i].upperVal)
			{
				m_bins[i].nCount++;
			}
		}
	}

	//@����:��ֱ��ͼ��ֵ�˲�
	void AvgFilter(int M)
	{
		for(int i=0; i<N; i++)
		{
			int nColumn = 0;
			int nCount  = 0;
			for(int j = i - M/2; j <= i+ M/2; j++)
			{
				if(j < 0)continue;

				if(j >= N) continue;

				nCount += m_bins[j].nCount;
				nColumn++;
			}//for-each(j)

			m_bins[i].nCount = nCount/nColumn;

		}//for-each(i)

	}

	//@����:��ֱ��ͼ�����CImageFrame��
	//
	void SaveToImage(CImageFrameT<T> & histoImage)
	{
		//����ͼƬ�ĳߴ�
		histoImage.SetSize(N, H, 1);
		histoImage.Clear();

		T* pImageData =  histoImage.GetData();


		//������Ŀ����Bin
		//
		UINT uMaxCount = 0;
		for(UINT i=0; i<N; i++)
		{
			if(m_bins[i].nCount > uMaxCount)
			{
				uMaxCount = m_bins[i].nCount;
			}
		}

		//��Ŀ����bin�е���ĿΪ0��
		if(uMaxCount == 0) return;

		for(UINT i=0; i<N; i++)
		{

			UINT nHistoHeight = m_bins[i].nCount * H /uMaxCount;

			if(nHistoHeight == H) nHistoHeight = H;

			T* pRowData = pImageData  + (H-1)*N + i;

			while(nHistoHeight)
			{
				*pRowData = 0xFF;
				pRowData -= N;
				nHistoHeight--;
			}



		}

	}




protected:
	struct BinCell
	{
	public:
		BinCell()
		{
			upperVal = T(0);
			lowerVal = T(0);
			nCount = 0;
		}

		T upperVal;
		T lowerVal;
		UINT nCount;
	};
	//BinCell m_statictics[N];

	BinCell m_bins[N];
	UINT    m_nPixelCount;
	T       m_nMinVal;
	T       m_nMaxVal;

};
