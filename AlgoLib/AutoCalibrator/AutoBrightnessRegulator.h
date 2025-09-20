#pragma once
//����:�Զ����ȵ�����
//ԭ��:ͨ������Ҷ�ͼƬ��ƽ������ֵ������ֵ֮��Ĳ�ֵ�����÷������ƺ���,��ʵ�ֶ����ȵ��Զ�����

template<class T>
class CAutoBrightnessRegulator
{
public:

    CAutoBrightnessRegulator(int nExpectedBrightness = 100,  bool (*fpControl)(bool inc,T absDiff, T nIndex, LPVOID lpCtxData) = NULL, LPVOID lpCtxData = NULL)
        :
    m_ExptectedBrightness(nExpectedBrightness),
		m_nIndex(0),
        m_fpControl(fpControl),
        m_lpCtxData(lpCtxData),
		m_nFrameCount(0)
    {

    }

    //@����:��������������ֵ
    void SetExpectedBrightness(T nExpectedBrightness)
    {
        m_ExptectedBrightness = nExpectedBrightness;
    }

	void SetAutoCalibrateParamsIndex(T nIndex)
	{
		m_nIndex = nIndex;
	}

    //@����:��������������ֵ
    T GetExpectedBrightness()const
    {
        return m_ExptectedBrightness;
    }

    void SetFeedbackCtrlFunction(bool (*fpControl)(bool inc, T absDiff ,T nIndex, LPVOID lpCtxData), LPVOID lpCtxData = NULL)
    {
        m_fpControl = fpControl;
		m_lpCtxData = lpCtxData;
    }

    //@����:���ݵ�ǰͼƬ��������Ϣ�������ھ���
    //@����:pImage, ͼƬ�����׵�ַ
    //      nWidth, ͼƬ���
    //      nHeight,ͼƬ�߶�
    //@����ֵ:ͼƬ����������ֵ�ò�ֵ�ľ���ֵ
    T ProcessImage(const T* pImage, int nWidth, int nHeight)
    {

        T avg_brightness = 0;
        T absDiff        = 0;//���Ȳ�ֵ
        __int64 brightness_sum = 0;
        for(int y = 0; y < nHeight; y++)
        {
            for(int x = 0; x < nWidth; x++)
            {
                brightness_sum += *pImage ++;
            }
        }
        int nPixelCount = nWidth*nHeight;
        if(nPixelCount)
        {
            avg_brightness = BYTE(brightness_sum/(nWidth*nHeight));
        }


        bool bInc = true;
        if(avg_brightness > m_ExptectedBrightness)
        {
            absDiff = avg_brightness - m_ExptectedBrightness;
            bInc = false;
        }
        else
        {
            absDiff = m_ExptectedBrightness - avg_brightness;

        }

        if((m_nFrameCount % 10) == 0)//ÿ10֡�ı�һ�����Ȳ���,����ͷ�Բ����ı仯��Ҫһ��ʱ����Ӧ
        {
            if(m_fpControl)
            {
                m_fpControl(bInc, absDiff,m_nIndex,m_lpCtxData);
            }
        }

        m_nFrameCount ++;

        return absDiff;
    }

    //@����:״̬��λ
    void Reset()
    {
        m_nFrameCount = 0;
    }

protected:
    T m_ExptectedBrightness;//����������
	T m_nIndex;             //�ڼ�������ͷ����

    //�������ƺ���ָ��
    //@�� ��:bInc, true:��������; false:��������
    //       db, ���Ⱦ��Բ�ֵ
    //       lpCtxData,����������
    //@����ֵ:
    bool (*m_fpControl)(bool bInc, T absDiff, T nIndex, LPVOID lpCtxData);//

    //������������������
    LPVOID m_lpCtxData;

    int m_nFrameCount;

};