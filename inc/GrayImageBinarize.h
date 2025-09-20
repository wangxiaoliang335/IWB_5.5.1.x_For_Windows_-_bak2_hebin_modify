//GrayImageBinarize.h
//Ŀ��: ���Һ��ʵ���ֵ��׼ȷ�طָ�ڰ�ͼƬ��Ŀ��ͱ�����
//�ο��ĵ�<<ֱ�Ӵ�˫��ֱ��ͼȷ����ֵ����ֵ 2001>>
//
//һ�������뱳���Ա����Ե�ͼ��һ����а���˫��ĻҶ�ֱ��ͼ��
//�ҳ�����˫��λ��x1,x2, ��ֵ������ΪXt= (X1 + X2)/2
//ʵ��������������Ѱ������֮��Ĺȵ׸��ӿɿ���
//����Ĺؼ������Ѱ�ҳ�˫��
//��������
//1.���ͼƬ�ı�׼��ϴ�, �������Ŀ����ͼ������ռ����������൱��
//  �ҶȾ�ֵ��������֮�䣬�������ûҶȾ�ֵ��������·ֿ�������
//  �ֱ��ڽϸ߻ҶȲ��ֺͽϵͻҶȲ��ֲ���˫�塣���������λ�õ��м�
//  ֵ��Ϊ��ֵ��



//2.����ͱ�����ͼ������ռ�����������⣬��ô��ֱ��ͼ������ӳ���Ľ���һ��һС
//  �����塣���ػҶ�ƽ��ֵ���ڽϴ���Ǹ�ɽ��ķ�Χ�ڣ� ���һҶȵ�ƽ��ֵ���ڽϴ�
//  ���Ǹ�ɽ��ķ�Χ�ڡ����һҶȵı�׼ƫ���С�������������£� �Ҷ�ƽ��ֵ���ֿܷ�
//  ����ɽ�塣�ҵ�������ɽ������߷�������Ҫ�ҵĴ�壬���ڵ���������β���С�塣
//  ͨ������֪���Ҷ�ֵ�е�ͻҶ�ƽ��ֵ�Ĺ�ϵ��֪���Ҷ�ƽ��ֵ�������Զ��С�壬��
//  �Ҷ�ֵ�е�mid�ȻҶ�ƽ��ֵavg���ӽӽ�С�塣�ԻҶ�ֵ�е�Ϊ������С���λ�á�
//  ��mid > avg, ���ԻҶ�֮�е�Ϊ���ڸ߻Ҷ���������С��
//  ��mid < avg, ���ԻҶ�֮�е�Ϊ���ڵͻҶ���������С��
//
//
//�㷨���㲽������
//1.�������ػҶ�ƽ��ֵ(abg),��׼��std.
//2.������ƽ��ֵΪ�ֽ�㣬�ֱ�������Ҳ��ֵ����ֵ��λ��
//3.������λ�����Ͻ�(�ڱ�׼�Χ��), ˵��ֵ��ͼ��˫�����и���ܵ�,
//  �����Ҫ��Ѱ�ͷ��λ�ã���������(7)����
//4.������ػҶ���ֵ��λ��(midpos);
//5.���midpos > avg, ����С���ڴ����࣬�������С���ڴ���Ҳ�.
//6.�ԻҶ���ֵ��Ϊ�磬�ҳ��ͷ��λ�á�
//7.������λ�õ��е���Ϊ��Ҫ��ȡ����ֵ��

//
//@����:����ֱ��ͼ���Ҷ�ֵ������
//@����:hist[N], ����N��bin��ֱ��ͼ
//

//template<typename T, int N>
//inline int GetBinarizeThreshold(T hist[N])
//could not deduce template argument for 'T [N]' from 'int [256]'

//http://stackoverflow.com/questions/5173494/pass-reference-to-array-in-c
//C++ standard
//14.8.2.1 Deducing template arguments from a function call [temp.deduct.call]
//Template argument deduction is done by comparing each function template parameter type (call it P) with the type of the corresponding argument of the call (call it A) as described below.
//If P is not a reference type:
//-- If A is an array type, the pointer type produced by the array-to-pointer standard conversion (4.2) is used in place of A for type deduction; otherwise,
//-- If A is a function type, the pointer type produced by the function-to-pointer standard conversion (4.3) is used in place of A for type deduction; otherwise,
//-- If A is a cv-qualified type, the top level cv-qualifiers of A's type are ignored for type deduction.
//-- If P is a cv-qualified type, the top level cv-qualifiers of P's type are ignored for type deduction. If P is a reference type, the type referred to by P is used for type deduction


//http://www.mombu.com/programming/c/t-whats-is-cv-qualified-type-801613.html
//"cv-qualified" is an abbreviation for "const- and/or volatile-
//qualified". A cv-qualified type is a type that has const and/or
//volatile at its top level, e.g. const int.
//
//ȷ�����������޵ķ���
enum EThresholdMethod
{
    TM_MIDDLE_POINT           ,//��ֱ��ͼ�����ҷ���е���Ϊ����ֵ
    TM_VALLEY_FROM_RIGHT      ,//��ֱ��ͼ�ҷ忪ʼ�����ȵף���Ϊ����ֵ, ��֤�ڹȵ�ƽ̹��������, ѡȡ�ϴ�����
    TM_VALLEY_FROM_LEFT       ,//��ֱ��ͼ��忪ʼ�����ȵף���Ϊ����ֵ, ��֤�ڹȵ�ƽ̹��������, ѡȡ��С����
};

template<typename T, int N >
inline int GetBinarizeThreshold(T (&hist)[N], EThresholdMethod eMethod=TM_MIDDLE_POINT)

{
    BYTE nThreshold  = 0;
    int i           = 0;
    int nGraySum    = 0;
    int nPixelCount = 0;
    int nAvgVal     = 0;

    static const int MINIMUM_GRAY_VALUE = 5;//��С�Ҷ�ֵ

    //1.�������ػҶ�ƽ��ֵ
    for(i = MINIMUM_GRAY_VALUE; i < N ; i++)
    {
        nPixelCount  += hist[i];
        nGraySum     += i*hist[i];
    }

    if(nPixelCount  == 0 ) return  MINIMUM_GRAY_VALUE;

    nAvgVal = nGraySum/nPixelCount;


    //2.�������ػҶȱ�׼��
    int nSumOfSquaredError = 0;


    for(i = MINIMUM_GRAY_VALUE; i < N ; i++)
    {
        nSumOfSquaredError += hist[i]*(i - nAvgVal) * (i - nAvgVal);
    }

    int std_square = nSumOfSquaredError/nPixelCount;


    const int NOISY_PIXEL_COUNT_THRESHOLD = 10;//���ظ�������������

    //������ ���ظ��� < MIMINUM_PIXEL_COUNT�ĻҶ�ֵ����, �Ա�
    //ȷ����ȷ�����Ҷ�ֵ

    //3.�������طֲ��ĻҶ����ֵ
    int nMaxGrayValue = MINIMUM_GRAY_VALUE;//���Ҷ�ֵ
    for(i=N-1; i> MINIMUM_GRAY_VALUE; i--)
    {
        if(hist[i] > NOISY_PIXEL_COUNT_THRESHOLD)
        {
            nMaxGrayValue = i;
            break;
        }
    }



    //4.��ƽ��ֵΪ��, ����С�Ҷ�ֵ�����Ҷ�ֵ֮���������Ҳ��ֵķ�ֵ��λ��
    int nPeakLeft = 0, nPeakRight = nAvgVal;

    //�����󲿷ַ�ֵ(�ͻҶ�ֵ����)
    for(i = MINIMUM_GRAY_VALUE; i<= nAvgVal; i++)
    {
        if(hist[i] > hist[nPeakLeft])
        {
            nPeakLeft = i;
        }
    }

    //�����Ҳ��ַ�ֵ(�߻Ҷ�ֵ����)
    for(i= nAvgVal + 1; i < N; i++)
    {
        if(hist[i] > hist[nPeakRight])
        {
            nPeakRight = i;
        }
    }

    //5.����ľ���С�ڱ�׼��, ֤�������쳣����, ��ҪѰ��С��
    int nPeakError = (nPeakLeft - nPeakRight)*(nPeakLeft - nPeakRight);
    if(nPeakError < std_square)
    {

        int nMidValue = (nMaxGrayValue + MINIMUM_GRAY_VALUE) >> 1;//����ֵ�е�

        if(nMidValue > nAvgVal)
        {
            //����ֵ�м�ֵ���ڻҶ�ƽ��ֵ����С���ڴ����Ҳࡣ
            //���е㿪ʼ����Ѱ��С��
            nPeakRight = nMidValue;
            for(i= nMidValue + 1; i<nMaxGrayValue; i++)
            {
                if(hist[i] > hist[nPeakRight])
                {
                    nPeakRight = i;
                }

            }


        }
        else
        {
            //����ֵ�м�ֵС���ڻҶ�ƽ��ֵ����С���ڴ����Ҳࡣ
            nPeakLeft = nMidValue;
            for(i= MINIMUM_GRAY_VALUE ; i< nMidValue - 1; i++)
            {
                if(hist[i] > hist[nPeakLeft])
                {
                    nPeakLeft = i;
                }

            }
        }
    }

    //6.����������м�ֵ��Ϊ����
    switch(eMethod)
    {
    case TM_VALLEY_FROM_RIGHT://���ҷ忪ʼ�����ȵף���Ϊ����ֵ, 
                                             
            nThreshold = nPeakRight;
            for(i=nPeakRight; i>=nPeakLeft; i--)
            {
                if(hist[i]  + NOISY_PIXEL_COUNT_THRESHOLD < hist[nThreshold])
                {
                    nThreshold = i;
                }
            }
            break;
    
        case TM_VALLEY_FROM_LEFT://����忪ʼ�����ȵף���Ϊ����ֵ
            nThreshold = nPeakLeft;
            for(i=nPeakLeft; i <= nPeakRight; i++)
            {
                if(hist[i] + NOISY_PIXEL_COUNT_THRESHOLD < hist[nThreshold])
                {
                    nThreshold = i;
                }
            }

            break;
        
        default:nThreshold = (nPeakLeft + nPeakRight) >> 1;

    }

    return nThreshold;
}



//@�� ��:����Otsu�����չ֮�����ĤΤ֤椭���㷨,����ǰ���ͱ���������ֵ
//@�� ��:pSrcData��ָ��ڰ��������ݵ��׵�ַ
//      nDataLength,���ݵĳ���
//@����ֵ:���ؼ���ó�������ֵ
//@˵ ��:������䷽������ԭ��������
//       between-class variance:
//      ��B = ��0�� ��1��(��1-��0)^2    ---(1)
//       ��0 + ��1 = 1
//             
//       ��0 = (��Ni)/N,  i��[1,k]
//       ��1, ǰ���ľ�ֵ
//       ��0, �����ľ�ֵ
//��  ��:
//		��ʽ(1)���˶�����N^2��õ��µĹ�ʽΪ
//      N^2 �� ��B  = N0 �� N1 ��(��1-��0)^2 --(2)
//      ���� 
//      N0 = (��Ni), i��[1,k]
//      N1= (��Nj) = N - N0;j��[k+1,L)
//
//      NΪ�������صĸ���, L����L���Ҷ�
//
//                  
//                    
//       ��0 = 1/N0 ����(i*Ni), i��[1,k]
//                    
//
//                         
//      ��U0 = N*��0   =  ��(i*Ni), i��[1,k]
//                        
//                     L
//        ��1 = 1/N1* ��(j*Nj), j��[k+1,L]
//                    j= k+1
//
//                        L
//      ��U1 = N1*��1  = ��(j*Nj),j��[k+1,L]
//                       j=k+1
//
//
//       U0 + U1 = U
//
//��(2)�����ٳ���(N1��N0)^2�õ�
//
//   Sigma = N^2 �� ��B �� (N0��N1)^2 =  N0 �� N1 ��(��1*N0*N1-��0*N0*N1)^2 = N0��N1 ��(U1*N0 - U0*N1)^2 --(3)
//
//  ֻ���ҳ�(3)�Ҷ����ʱ��Ӧ��kֵ��Ϊ���������ֵ???
//
inline BYTE FindThreshold_Ostu(const INT (&histogram)[256])
{
    int L = 256;

    BYTE cThreshold = 0;

    int N = 0;
    for(int i=0; i<L; i++)
    {
        N += histogram[i];
    }
    int N0 = 0;
    int N1 = 0;

    //  k
    int U0 = 0;//U=��(i*Ni)
    //  i=1


    //  L
    int U1 = 0;//U=��(i*Ni)
    //  i=k+1


    //  L
    int U = 0; //U=��(i*Ni)
    //  i=1
    //__int64 SigmaMax = 0;
    //__int64 Sigma = 0;


    for(int i=0; i<L; i++)
    {
        U += i*histogram[i];
    }

    __int64  varMax = 0;

    for(int i=0; i< L; i++)
    {

        int gray_count = histogram[i];
        if(gray_count)
        {
            N0 += gray_count;//Weighted Backgound
        }
        else
        {
            continue;
        }


        N1  = N - N0;//Weighted Foreground
        if(N1 == 0)
        {
            break;
        }


        U0 += i*histogram[i];

        U1  = U - U0;


        int mB = (U0 + (N0>>2)) /N0;//Mean Background;
        int mF = (U1 + (N1>>2)) /N1;//Mean Foreground

        // Calculate Between Class Variance
        __int64 varBetween = (__int64)N0 * (__int64)N1 * (mB - mF) * (mB - mF);

        // Check if new maximum found
        if (varBetween > varMax) {
            varMax = varBetween;
            cThreshold = i;
        }

    }//for


    return cThreshold;
}
