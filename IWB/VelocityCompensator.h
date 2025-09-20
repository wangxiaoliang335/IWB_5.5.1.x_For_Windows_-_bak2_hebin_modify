#pragma once
//�ٶȲ���
//���ݼ�⵽�Ĺ��λ��,ʵʱ��̬�������λ��
//
class CVelocityCompensator
{
public:
	CVelocityCompensator()
	{
		Reset();
	}

	//@����:�˶�������λ
	void Reset()
	{
		m_Vx = 0;
		m_Vy = 0;

		m_nRunTimes = 0;
	}

	POINT Compensate(POINT& ptNow)
	{
		POINT ptResult;

		if(0 == m_nRunTimes)
		{

			ptResult =  ptNow;
		}
		else
		{	
			long Vx = (ptNow.x - m_ptLastPos.x) << SCALE_SHIFT_SIZE;
            long Vy = (ptNow.y - m_ptLastPos.y) << SCALE_SHIFT_SIZE;

            

			long SmoothFactor1 = 20;
			long SmoothFactor2 = 80;
			m_Vx = (Vx * SmoothFactor1  + m_Vx*SmoothFactor2)/100;
			m_Vy = (Vy * SmoothFactor1  + m_Vy*SmoothFactor2)/100;

            ptResult =  ptNow;


			const int N_FRAME = 3;

            //����ٶȹ�С����������
            if(Vx >= (5<<SCALE_SHIFT_SIZE) || Vy >= (5<<SCALE_SHIFT_SIZE))
            {
			    ptResult.x = (m_Vx >> SCALE_SHIFT_SIZE)*N_FRAME + ptNow.x;//��ǰ����N֡���ƶ�����
                ptResult.y = (m_Vy >> SCALE_SHIFT_SIZE)*N_FRAME + ptNow.y;//��ǰ����N֡���ƶ�����
            }
			

			AtlTrace(_T("Vx = %d, Vy= %d\n"), m_Vx >> SCALE_SHIFT_SIZE , m_Vy>>SCALE_SHIFT_SIZE);

		}
		
		m_ptLastPos = ptNow;

		m_nRunTimes ++;

		return ptResult;
	}

protected:
	POINT m_ptLastPos;
	long  m_Vx;//ˮƽ�ٶ�,����64��
	long  m_Vy;//��ֱ�ٶ�,,����64��
	static const int SCALE_SHIFT_SIZE = 6;
	int  m_nRunTimes;//���д���


};