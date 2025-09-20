//MaxWeightBipartiteMatch.h
#pragma once
#include <limits>
#include <string.h>
//����ͼ���Ȩ��ƥ���㷨(Max-Weight Matching Algorithm)
template<int N>
class CMaxWeightBipartiteMatch
{
public:
        //����:pWeights, ��`ά�����ʾ��n��nά��Ȩ�ؾ���ǰn��Ԫ�ض�Ӧ����ĵ�һ�С�
        //            n, Ȩ�ؾ����ά��
        //bMininumValue,����С���۱�־
        //����ֵ: ���Ȩ��ƥ���Ȩ�غ�
        //
        //˵��:n����С�� N
        bool operator()(const int* pWeights, int n)
        {
            if(n > N)
            {
                return false;
            }

            int x, y;

            m_n = n;
            for(x = 0; x < m_n; x++)
                for(y = 0 ; y < m_n; y++)
                    m_Weights[x][y] = *pWeights++;

            Hungarian();

            
            return true;
        }

        const int* GetMatchResult()
        {
            return &this->m_Xy[0];
        }
private:

   void InitLabels()
   {
        int x,y;
        memset(m_Lx, 0, sizeof(m_Lx));
        memset(m_Ly, 0, sizeof(m_Ly));
        for(x = 0; x < m_n; x++)
            for(y = 0; y < m_n; y++)
                m_Lx[x] = (m_Lx[x]> m_Weights[x][y]) ? m_Lx[x]:m_Weights[x][y];
   }

   void UpdateLabels()
   {
       int x,y, delta = (std::numeric_limits<int>::max)();//init delta s infinity

       //serarch minimum slack  value in elements of Y set  which are not belong to set T.
       for (y = 0; y < m_n; y ++)
            if( !m_T[y])
                delta = (delta < m_slack[y])? delta:m_slack[y];

       //update X labels
       for(x = 0; x < m_n; x ++)
           if(m_S[x]) m_Lx[x] -= delta;

       //update Y labels
       for(y = 0; y < m_n; y ++)
            if(m_T[y]) m_Ly[y] += delta;

       //update slack array
       for(y = 0; y < m_n; y++)
           if(!m_T[y])
           {   //��֪m_slack[y]�Ķ�������:
               //m_slack[y]= min{m_Lx[x] + m_Ly[y] - m_weights[x][y]}, x��S
               //������x��S, m_Lx[x]����ȥ��delta,
               //������y������T, m_Ly[y]���ֲ���
                //����slack[y]��ҲҪ��ȥdelta.
                m_slack[y] -= delta;
           }
   }


   void AddToTree(int x, int prevx)
   //x, current vertex
   //prevx, vertex from set X before x in the alternating path,
   //so we add edges <prevX, m_Xy[x]>, <m_Xy[x], X>
   {
        m_S[x] = true;//add x to set S
        m_prev[x] = prevx;//

        //
        //From the definiton of m_slakc[y], m_slakc[y] is the minimum of a set of values.
        //m_slack[y]= min{m_Lx[x] + m_Ly[y] - m_weights[x][y]}, x��S
        //Because we add a new vertex to S, so we must update slack
        for(int y = 0; y < m_n; y++)
            if(m_Lx[x] + m_Ly[y] - m_Weights[x][y] < m_slack[y])
            {//slack[y] can be smaller
                m_slack [y] = m_Lx[x] + m_Ly[y] - m_Weights[x][y];

                //the vertex in set X whick makes the smallest slack[y]
                m_slackX[y] = x;
            }


   }

   //@����:ͨ����������·������ƥ��
   //@˵��:�ú���ִ��һ�Σ����ҵ�һ��ƥ��ߡ�
   void Augment()
   {
        
       if(m_nMaxMatch == m_n) return;//check whether matching is alreay perfect, 
                                     //the if condition is also the condition to finish the recursive calling of this function
        int x, y, root;
        int q[N], wr = 0, rd = 0    ;//�����������ʹ�õĶ��У� wr, rd�Ǳ�Ϊ���еĵ�ǰ��ȡ��д��λ�á�

        //
       memset(m_S, false, sizeof(m_S));
       memset(m_T, false, sizeof(m_T));
       memset(m_prev, -1, sizeof(m_prev));

       //Ϊ��Ѱ������·��������X�㼯��δƥ��ĵ�һ�������Ϊ�������ĸ�(Alternating Tree)
       for( x = 0; x < m_n; x ++)
           if(m_Xy[x] == -1)//when x is not a matched vertex
           {
                q[wr++] = root = x;
                m_prev[x] = -2;//���·����㣬ǰ�治����x��㡣
                m_S[x] = true ;//
                break;
           }

       //init set slack
       for(y = 0; y < m_n; y++)
       {
            m_slack[y] = m_Lx[root] + m_Ly[y] - m_Weights[root][y];
            m_slackX[y] = root;
       }

       while(true)//main cycle
       {
           while(rd < wr)
           {
               x = q[rd++];//current vertex form X part

               for(y = 0; y < m_n; y++)
                    if(m_Weights[x][y] == m_Lx[x] + m_Ly[y] && !m_T[y])
                    {//iterats through all edges in equality graph

                        if(m_Yx[y] == -1) break;//an exposed vertex in Y found, so
                                                //augmenting path exists!

                        m_T[y] = true;

                        //˵��:��ʱm_Yx[y]���������ڼ���S��
                        //     ��Ϊ����ִ�е��Ĵ�ǰ��ʱ�̣�ÿ��S�е�Ԫ�ض���һ��ƥ���TԪ��yk
                        //     ����ʱ������һ��Ԫ��y��m_Yx[y]ƥ�䣬��ζ��yk,yͬʱ��ͬһ�����
                        //     m_Yx[y]ƥ��
                        q[wr++] = m_Yx[y];//add vertex yx[y], which is matched
                                              //with y, to the queue
                         AddToTree(m_Yx[y], x);//add edges(x,y) and (y, yx[y]) to the tree

 
                    }
           }//while

        if(y < m_n) break; //augmenting path found!
        
        UpdateLabels();//augmenting path not found, so improve labeling
        
        //clear queue
        wr = rd = 0;

       for(y = 0; y < m_n; y++)
        //in this cycle we add edges that were added to the equality graph as a
        //result of improving the labeling, we add edge(slackx[y], y) to the tree if
        //and only if !T[y] && slack[y] == 0, also with this edge we add another one
        //(y, yx[y]) or augment the matching, if y was exposed
        if(!m_T[y] && m_slack[y] == 0 )
        {
            if(m_Yx[y] == -1)//vertex y is unmatched
            {
                x = m_slackX[y];
                break;
            }
            else
            {//vertex y has been matched
                m_T[y] = true;
                //
                //�������µ�equality edge, 
                if(!m_S[m_Yx[y]])//
                {
                    q[wr++] = m_Yx[y];
                    AddToTree(m_Yx[y], m_slackX[y]);
                }

            }
        }

        if(y < m_n) break;                          //augment path found!
       }//while(true) main cycle

       if(y < m_n)//augment path found
       {
           //����ƥ�����
            m_nMaxMatch ++;


            for(int cx = x, cy = y, ty; cx != -2; cx = m_prev[cx], cy = ty)
            {//in this cycle, inverse edges along augmenting path
                ty = m_Xy[cx];
                m_Yx[cy]  = cx;
                m_Xy[cx ] = cy;
            }

            //recall function.
            Augment();
       }


    }


   void Hungarian()
   {
       m_nMaxMatch = 0;
       memset(m_Xy, -1, sizeof(m_Xy));
       memset(m_Yx, -1, sizeof(m_Yx));
       InitLabels();
       Augment();
   }

protected:
    int m_Weights[N][N]   ;//Ȩ�ؾ���

    int m_n               ;//ʵ�ʵ�Ȩ�ؾ����ά��
    int m_nMaxMatch       ;//
    int m_Xy[N]           ;//�����е�ÿһ����Ԫ���ÿ��"X�㼯�еĽ��"ƥ���"Y�㼯�еĽ��"�ı��, '-1'��ʾδƥ��
    int m_Yx[N]           ;//�����е�ÿһ����Ԫ���ÿ��"Y�㼯�еĽ��"ƥ���"X�㼯�еĽ��"�ı��, '-1'��ʾδƥ��
    int m_Lx[N]           ;//"X�㼯�и����"�ı��ֵ
    int m_Ly[N]           ;//"Y�㼯�и����"�ı��ֵ

    //========����·��������ر���=======
    bool m_S[N], m_T[N]    ;//S:����·��������X�ĵ㼯;T:����·��������Y�ĵ㼯
    int  m_slack[N]        ;//��Y�㼯�еĵ�y�����, m_slack[y]= min{m_Lx[x] + m_Ly[y] - m_weights[x][y]}, x��S
    int  m_slackX[N]       ;//m_slack[y]ȡ��Сֵʱ����Ӧ��X�㼯�еĽ����
    int  m_prev[N]         ;//��¼����·��������X�㼯�Ľ��ı�����飬m_prev[i]�ĺ����ǽ���·���б��Ϊi�Ľ���ڽ���
                            //·����ǰ������Ȼ����X�㼯�Ľ���š�
                            //-2,��ʾ���ڵ㣬��ʾ�����˽���·������㡣
                            //-1,��ʾδ��ֵ
                            //һ������·����m_Xy�����m_prev���鹲ͬ������
};
