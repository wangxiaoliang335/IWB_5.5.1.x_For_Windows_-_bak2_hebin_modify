#pragma once

//@Author  :toxuke@gmail.com
//@Date    :April/1st/2013
//@Revision:April/1st/2013
// 
//ģ�����KΪƽ���˲�����, KԽ����Խƽ��, ���������ʹ���������ƽ����Բ����

//�㷨����:����SP(i-1)Ϊ��һ���ƽ��λ�ã� ��ǰ��P(i)��Ӧ�Ķ�Ӧ��ƽ��λ��SP(i)
//    SP(i) = W0*SP(i-1) + W1*P(i);
//    W0, W1,Ϊƽ��ϵ���� �����ϵW0 + W1 = 1;
//    Ϊ�˱��⸡��Ԫ�㣬��ȡ��ϵ���Ŵ�2^W_SCALE_SHIFT_SIZE
//
template<int K>
class CStrokeSmoother
{
public:
    CStrokeSmoother(void)
        :
      m_nTimes(0)
    {

        //��ʼ��Ȩ������

        //�µ���ƽ�������е�Ȩ��
        int WEIGHT_DENOMINATOR = 1 << W_SCALE_SHIFT_SIZE;

        int  Wmin = WEIGHT_DENOMINATOR*1/10;
        int  Wmax = WEIGHT_DENOMINATOR;
        int  Xmax = WEIGHT_NUM;
        int  Xmin = 0;


        //�������ƽ��X��ƽ���˲�ϵ��W1�Ĺ�ϵ����
        //X=a*W1^2 + b, W1ΪY�ᡣ
        //W1 = sqrt((X-b)/a)
        //W1�ķ�Χ��[0, WEIGHT_DENOMINATOR]֮��
        double a = (double)(Xmax - Xmin)/(double)(Wmax*Wmax - Wmin*Wmin);
        double b = double(Xmin - a*Wmin*Wmin);
        for(int X=Xmin; X <= Xmax; X++)
        {
            m_Weights[X][1]  =(int)sqrt((X-b)/a);
            m_Weights[X][0 ] = WEIGHT_DENOMINATOR - m_Weights[X][1];

            assert(m_Weights[X][1]  >= 0);
            assert(m_Weights[X][0]  >= 0);
        }

    }

    ~CStrokeSmoother(void)
    {


    }

    POINT Reset()
    {
        m_nTimes = 0;

        POINT ptResult;
         ptResult.x = m_aryPt[0].x >> SCALE_SHIFT_SIZE;
         ptResult.y = m_aryPt[0].y >> SCALE_SHIFT_SIZE;
         return ptResult;
    }

    BOOL IsReset()
    {
        return 0 == m_nTimes;
    }
    


    //˵��:������Ǽ������Ļ����, Ϊ�˼���ȡ����ɵ���
    //     ���Ƚ���Ļ������гߴ�Ŵ󣬽����������ٵ�ԭ����
    //     1/(2^SCALE_SHIFT_SIZE)
    //
    POINT Smooth(const POINT& ptInput)
    {
        //return ptInput;
        
        if( 0 == m_nTimes )
        {
            for(int i=0; i <= K; i++)
            {
                m_aryPt[i].x = ptInput.x << SCALE_SHIFT_SIZE;
                m_aryPt[i].y = ptInput.y << SCALE_SHIFT_SIZE;
            }
        }
        else
        {
             int nDx = ptInput.x - (m_aryPt[K].x >> SCALE_SHIFT_SIZE);
             int nDy = ptInput.y - (m_aryPt[K].y >> SCALE_SHIFT_SIZE);
             int weightIndex =( nDx*nDx + nDy*nDy);

             if(weightIndex >= WEIGHT_NUM) weightIndex = WEIGHT_NUM - 1;
             if(weightIndex < 0) weightIndex = 0;
            
             //�Ծ����ƽ����Ϊ�˲�ϵͳ��������
             int W0 = m_Weights[weightIndex][0];
             int W1 = m_Weights[weightIndex][1];


            m_aryPt[0].x = ptInput.x << SCALE_SHIFT_SIZE;
            m_aryPt[0].y = ptInput.y << SCALE_SHIFT_SIZE;

            for(int i=1; i <= K; i++)
            {
                
                m_aryPt[i].x = (m_aryPt[i-1].x * W1 + m_aryPt[i].x *  W0) >> W_SCALE_SHIFT_SIZE;
                m_aryPt[i].y = (m_aryPt[i-1].y * W1 + m_aryPt[i].y *  W0) >> W_SCALE_SHIFT_SIZE;
                
            }
        }


        m_nTimes ++;

         POINT ptResult;
         ptResult.x = m_aryPt[K].x >> SCALE_SHIFT_SIZE;
         ptResult.y = m_aryPt[K].y >> SCALE_SHIFT_SIZE;

         return ptResult;

    }

protected:

    int m_nTimes       ;//�˲�����
    POINT m_aryPt[K+1] ;//K���˲��м�����顣
                        //��m_aryPt[0]��Ϊ��һ�������; m_aryPt[1]Ϊ��һ��1��ƽ��λ��; m_aryPt[2]Ϊ��һ��2��ƽ��λ��
                    
    POINT m_ptLastPos;//��һ��λ��





    static const int MAX_DIFF_RANGE  = 7;//ǰ���С�ڸþ��� �����ƽ��; ����

    static const int WEIGHT_NUM = MAX_DIFF_RANGE*MAX_DIFF_RANGE;
    int m_Weights[WEIGHT_NUM+1][2];


    //�ߴ�Ŵ�����,��С�˲����������ڼ���ȡ�����µ����
    static const int SCALE_SHIFT_SIZE  = 8;

    static const int SCALE_FACTOR = 2<<SCALE_SHIFT_SIZE;//�ߴ�Ŵ�����

	static const int W_SCALE_SHIFT_SIZE = 8;
	
    

};
