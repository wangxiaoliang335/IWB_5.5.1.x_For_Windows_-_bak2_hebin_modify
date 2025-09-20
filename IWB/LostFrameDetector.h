#pragma once
//@����:��ʧ֡�����
class CLostFrameCountDetector
{
public:
	CLostFrameCountDetector()
		:
	m_lRecvFrameCount(0),
	m_lFPS(60)
	{
		QueryPerformanceFrequency(&m_liFrequency);
		m_liLastTime.QuadPart = 0;
		m_liBeginTime.QuadPart = 0;
		
	}

	
	~CLostFrameCountDetector()
	{


	}

	void Reset(LONG lFPS)
	{
	
		QueryPerformanceCounter(&m_liBeginTime);
		QueryPerformanceCounter(&m_liLastTime);
		m_lFPS = lFPS;
		m_lRecvFrameCount = 0;

	}

	void DoDetect()
	{
		LARGE_INTEGER liNow;
		QueryPerformanceCounter(&liNow);

		LONG lExpectedFrameCount = (LONG)((liNow.QuadPart - m_liBeginTime.QuadPart) * m_lFPS / m_liFrequency.QuadPart );

		LARGE_INTEGER liDelta ;
		liDelta.QuadPart = (liNow.QuadPart - m_liLastTime.QuadPart) * 1000/ m_liFrequency.QuadPart;//��λ����


		if(liDelta.QuadPart >=  16)
		{
			m_liLastTime.QuadPart = liNow.QuadPart;

#ifdef _LOG
		//LOG_INF("Expected Frame=%li, Actual Frame=%li, missing=%li",  lExpectedFrameCount, m_lRecvFrameCount, lExpectedFrameCount - m_lRecvFrameCount) ;
#endif
		}
		m_lRecvFrameCount++;
	}

protected:
	LARGE_INTEGER m_liFrequency    ;//
	LARGE_INTEGER m_liBeginTime    ;//��ʼͳ��ʱ��
	LARGE_INTEGER m_liLastTime     ;//��һ��ͳ�����ʱ��
	LONG          m_lFPS           ; //����֡��
	LONG          m_lRecvFrameCount;//֡������
};