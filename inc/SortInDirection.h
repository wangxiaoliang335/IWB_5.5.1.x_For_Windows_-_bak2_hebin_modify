#pragma once
#include <vector>



//@����:��ָ���Ĳο���Ϊԭ�㽨��ֱ������ϵ,���շ�λ�Ǵ�С�����˳�����и����㡣
//@����:pPoints, ָ��������ָ��
//      nPtNumber, �����Ŀ
//      ptOrigin, ԭ������
void SortInDirection(POINT*  pPoints, int nPtNumber, const POINT& ptOrigin)
{
    //����ʸ��
    struct TDirectionVector
    {
        int nQuadrant;//��λ�������ޱ��(1,2,3,4)
                      //��λ��ʼ��Ϊx������, �������޷�λ��������
        LONG lDx     ;//�����ԭ���ˮƽʸ����С
        LONG lDy     ;//�����ԭ��Ĵ�ֱʸ����С

        POINT  pt    ;//ԭ����
        //int nPtIndex ;//��ԭ�����е�����
    };

    std::vector<TDirectionVector>  vecPtDirections;
    vecPtDirections.resize(nPtNumber);

    for(int i = 0;i < nPtNumber; i++)
    {

        TDirectionVector temp;
        temp.pt       = pPoints[i];
        temp.lDx      = pPoints[i].x - ptOrigin.x;
        temp.lDy      = pPoints[i].y - ptOrigin.y;
        //temp.nPtIndex = i;


        if(temp.lDx > 0 && temp.lDy>=0)
        {
             temp.nQuadrant = 1;//Quadrant���һ����
        }
        else if(temp.lDx <= 0 && temp.lDy > 0)
        {
             temp.nQuadrant = 2;//Quadrant ��ڶ�����
        }
        else if(temp.lDx < 0 && temp.lDy <= 0)
        {
             temp.nQuadrant = 3;//Quadrant ���������
        }
        else
        {
             temp.nQuadrant = 4;//Quadrant����������
        }


        //��ǰ����Ѱ�Ҳ���λ��
        int j= 0;
        for(j=0;  j < i; j++)
        {

            //��j��Ԫ�ص����� > "�����"������
            //��ζ��Ҫ����λ��j����ԭ��λ��j�����Ժ�����ݺ��ơ�
            if(vecPtDirections[j].nQuadrant > temp.nQuadrant)
            {

                break;
            }
            else if(vecPtDirections[j].nQuadrant == temp.nQuadrant)
            {
                  LONG crossProduct = temp.lDx * vecPtDirections[j].lDy - temp.lDy*vecPtDirections[j].lDx ;
                  
                  //�Ե�ǰʸ��Ϊʼ��, ���Ƚϵ�ʸ��Ϊ�ձ�, 
                  //ʸ�����>0,��ζ�ű��Ƚϵ�ʸ���ķ�λ�Ǵ��ڵ�ǰʸ���ķ�λ�ǣ��ҵ��˲���λ��
                   if(crossProduct > 0 )
                   {                    
                        break;
                   }//if
            }
        }

        for(int k = i; k > j; k --)
        {
            vecPtDirections[k] = vecPtDirections[k - 1];
        }

        vecPtDirections[j] = temp;

    }//for

    
    for(int i = 0; i < nPtNumber; i++)
    {
        pPoints[i] = vecPtDirections[i].pt;
    }

}

