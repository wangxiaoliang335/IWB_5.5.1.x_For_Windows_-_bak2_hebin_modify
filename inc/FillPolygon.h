#pragma once

//����:N��������ɵĶ����,��N���߶Ρ�ÿ��ɨ���߶����N���߶��ཻ��
//     ��Ҫ��¼ÿ��ˮƽɨ����������߶εĽ��㡣
//
inline void DebugHeap2()
{
	std::vector<int> vecTemp;
	vecTemp.resize(10000);

	DWORD dwFlags = 0x0;

	BOOL bRet = HeapValidate(GetProcessHeap(), dwFlags, NULL);

	vecTemp[0] = 2;

	if (!bRet)
	{
		::DebugBreak();
	}

}
class CPolygonScanLineInfo
{
public:
    CPolygonScanLineInfo()
        :
    m_LineStatistic(NULL),
        m_pHorzPos(NULL),
        m_nMaxPtPerScanLine(0),
        m_nHeight(0)
    {

    }

    ~CPolygonScanLineInfo()
    {
        if(m_LineStatistic)
        {
            delete [] m_LineStatistic;
        }

        if(m_pHorzPos)
        {
            delete[] m_pHorzPos;
        }
    }


    //����:���û���ߴ�
    void SetSize(int nImageHeight, int nPolygonVetexNum)
    {
        if(nImageHeight != m_nHeight)
        {
            if(m_LineStatistic)
            {
                delete[] m_LineStatistic;
                m_LineStatistic = NULL;
            }

            m_LineStatistic = new int[nImageHeight];

        }

        //��ʼ��
        memset(m_LineStatistic, 0, sizeof(int)*nImageHeight);

        if(nImageHeight != m_nHeight || nPolygonVetexNum != m_nMaxPtPerScanLine)
        {
            if(m_pHorzPos)
            {   
                delete[] m_pHorzPos;
                m_pHorzPos = NULL;
            }
            m_pHorzPos = new int[nImageHeight*nPolygonVetexNum];
         
			m_nMaxPtPerScanLine = nPolygonVetexNum;
			m_nHeight           = nImageHeight;

        }
		
        memset(m_pHorzPos, 0, sizeof(int)*nImageHeight*nPolygonVetexNum);
    }


    //@����:��¼��������
    void AddPoint(const POINT& ptNew)
    {
        if(ptNew.y < 0 ) return;
        if(ptNew.y >=  m_nHeight) return;

        int& rPtNum = m_LineStatistic[ptNew.y];

        int* pHorzPos = &m_pHorzPos[ptNew.y * m_nMaxPtPerScanLine];

        //����ˮƽ����Ĵ�Сʹ�ò��뷨����
        int i=0;
        for(i= 0; i < rPtNum; i++)
        {
            if(pHorzPos[i]  < ptNew.x)
            {
                continue;
            }
            else
            {

                break;
            }

        }

        //iλ���Ժ��Ԫ������ƶ�һ����Ԫ,�ճ�һ��λ�á�
        for(int j= rPtNum; j>i; j--)
        {
            pHorzPos[j] = pHorzPos[j-1];
        }

        pHorzPos[i] = ptNew.x;

        //�߽����Ŀ����1
        rPtNum ++;

    }



    //@����:��ȡÿ��ɨ���еı߽�����Ŀ
    int GetScanLinePtNumber(int nScanLineNo)const
    {
        if(nScanLineNo < 0) return 0;
        if(nScanLineNo > m_nHeight) return 0;
        return m_LineStatistic[nScanLineNo];
    }


    const INT* GetIntersectList(int nScanLineNo)const
    {
        if(nScanLineNo < 0) return 0;
        if(nScanLineNo > m_nHeight) return 0;
        return &m_pHorzPos[nScanLineNo * m_nMaxPtPerScanLine];
    }

protected:

    int*   m_LineStatistic   ;//ÿ��ˮƽɨ���������εĽ����ͳ�Ƽ�����
    int*   m_pHorzPos        ;//�߽��ˮƽ��������
    int   m_nMaxPtPerScanLine;//ÿ��ˮƽɨ���������ν���������Ŀ
    int   m_nHeight          ;//ͼƬ�߶�

};



//@����:�ڻҶ�ͼƬ���������
//@����:pCanvas, ָ�򻭲���ָ��
//      nWidth, �������
//      nHeight, �����߶�
//      pVertices, ����ζ�����������
//      nVertexNum, �������
//      gray, ���ĻҶ�ֵ
//      bDrawBorder, ���Ʊ߽��־
inline void FillPolygon(BYTE* pCanvas, int nWidth, int nHeight, const POINT* pVertices, int nVertexNum, BYTE gray, BOOL bDrawBorder)
{
     //static CPolygonScanLineInfo edgePointInfo;//ɨ������Ϣ����̬���������ظ���ʼ��������ٶȡ�
	 CPolygonScanLineInfo edgePointInfo;//ɨ������Ϣ, ��ջ���������ڶ��߳�ʹ�á�
	 static HANDLE s_oldHeap = (HANDLE)-1;

	 if (s_oldHeap != GetProcessHeap())
	 {
		 if (s_oldHeap != (HANDLE)-1)
		 {
			 ::DebugBreak();
		 }
		 s_oldHeap = GetProcessHeap();
	 } 


    edgePointInfo.SetSize(nHeight, nVertexNum);

    if(nVertexNum < 3) return;//������Ŀ<3, ��������


    POINT  nextSegmentEndVertex,curSegmentStartVertex, curSegmentEndVertex;

    curSegmentStartVertex = pVertices[0];
    curSegmentEndVertex   = pVertices[1];
    nextSegmentEndVertex  = pVertices[2];

    int nNextSegmentEndVerextIndex = 2;

    int nSegmentEndX = 0;
    BYTE borderColor = gray;
    for(int iLine = 0; iLine< nVertexNum; iLine ++)
    {
        POINT start = curSegmentStartVertex;
        POINT end   = curSegmentEndVertex;

        if(start.y == end.y)//ˮƽֱ��
        {
            if(bDrawBorder)
            {
                
                int x0,x1;
                if(start.x > end.x)
                {
                    x0 = end.x;
                    x1 = start.x;
                }
                else
                {
                   x0 = start.x;
                   x1 = end.x;
                   
                }
                for(int x = x0; x <= x1; x++)
                {
                    if(0<= start.y &&  start.y < nHeight && 0 <= x && x < nWidth)
                    {
                        pCanvas[start.y * nWidth + x] = borderColor;
                    }
                }
            }
        }
        else//
        {

            edgePointInfo.AddPoint(start);

            LONG dx =  (end.x - start.x > 0)?end.x - start.x : start.x - end.x ;
            LONG dy  = (end.y - start.y > 0)?end.y - start.y :start.y  - end.y;
            LONG sx, sy, err, e2;

            sx = (start.x < end.x)?1:-1;
            sy = (start.y < end.y)?1:-1;
            err = dx -dy;

            do
            {
                if(bDrawBorder)
                {
                    if(0<= start.y &&  start.y < nHeight && 0 <= start.x && start.x < nWidth)
                    {
                        pCanvas[start.y * nWidth + start.x] = borderColor;
                    }
                }

                if(start.x == end.x && start.y == end.y) 
                {//�����߶��յ�,�ж����������߶��Ƿ���ͬ��

                    //������һ����ˮƽ���߶ε��յ�
                    POINT nextNonHorizontalSegmentEndVertex = nextSegmentEndVertex;
                    int nSearchIndex = nNextSegmentEndVerextIndex;
                    while(nextNonHorizontalSegmentEndVertex.y == end.y)
                    {
                        nSearchIndex ++;
                        if(nSearchIndex == nVertexNum)
                        {
                            nSearchIndex = 0;
                        }

                        nextNonHorizontalSegmentEndVertex = pVertices[nSearchIndex];
                    }


                    //�жϵ�ǰ�߶ε��������һ����ˮƽ���߶ε��յ��Y�ı仯��ϵ
                    //��ͬ�����߶��յ���Ϊ�߽�����, ����һ��
                    //�߶ε����Ϊͬһ�㡣��������Ŀ����ȷ����ȷ��䡣
                    if(nextNonHorizontalSegmentEndVertex.y > end.y  && curSegmentStartVertex.y > end.y
                        ||
                        nextNonHorizontalSegmentEndVertex.y < end.y  && curSegmentStartVertex.y < end.y)
                    {
                        start.x = nSegmentEndX;
                        edgePointInfo.AddPoint(start);
                    }

                    break;
                }


                e2 = err*2;
                if(e2 > -dy)
                {
                    err -= dy;
                    start.x += sx; 
                }
                if(e2 < dx)
                {
                    err += dx;
                    start.y  += sy;

                    if(start.y != end.y)//y�仯�˼���ɨ����Ϣ
                    {
                        edgePointInfo.AddPoint(start);
                    }
                    else
                    {
                        nSegmentEndX = start.x;
                    }
               }
            }while(1);

        }//else

        curSegmentStartVertex = curSegmentEndVertex;
        curSegmentEndVertex   = nextSegmentEndVertex;

        nNextSegmentEndVerextIndex ++;

        if(nNextSegmentEndVerextIndex == nVertexNum)
        {
            nextSegmentEndVertex = pVertices[0];
            nNextSegmentEndVerextIndex = 0;
        }
        else
        {
            nextSegmentEndVertex = pVertices[nNextSegmentEndVerextIndex];
        }


    }//for each segments

    //���ݼ�¼�Ķ���α߽���Ϣ, ����һ��������
    for(int y=0; y <nHeight; y++)
    {
        int nPtNumber = edgePointInfo.GetScanLinePtNumber(y);

        //if(nPtNumber % 2 != 0)
        //{
        //    //assert(false);
        //}
        const INT* pIntersectList = edgePointInfo.GetIntersectList(y);

        for(int i = 0 ;i < nPtNumber; i+=2)
        {
            int x1 = pIntersectList[i];
            int x2 = pIntersectList[i+1];
            for(int x = x1;x <= x2;x++)
            {
                
                if(0<= y &&  y < nHeight && 0 <= x && x < nWidth)
                {
                    pCanvas[y * nWidth + x] = gray;
                }
            }
        }
    }//for



}