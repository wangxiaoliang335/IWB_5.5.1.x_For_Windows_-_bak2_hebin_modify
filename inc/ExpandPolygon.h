#pragma once
#include <limits>
#include <vector>
//@����:���˵����������������Ĺ��ߵĵ�
inline  int FilterOutColinearPoints(const POINT* pOrgVertices, int nVertexNum, POINT* pNewVertices)
{
    //ʣ��Ĳ����ߵĶ������
	int nTurnVertexCount = 0;
    const POINT* pLastTurnVertex = NULL;//�ϴ��������ı߽�յ�

	//�����о�:��ֱ�߼н���(0~5)����(175~179)֮��,�������Ϊ����
	double sin5 = sin(5.0*M_PI / 180.0);
	double sin5_square = sin5*sin5;

	const POINT* pPrev, *pCurrent, *pNext;
	for (int i = 0; i < nVertexNum; i++)
	{
		pCurrent = pOrgVertices + i;
		pPrev = (i == 0) ? pOrgVertices + (nVertexNum - 1) : pCurrent - 1;
		pNext = (i == nVertexNum - 1) ? pOrgVertices : pCurrent + 1;
		
        //�ǰ���ǰһ�ڽӵ�����ֱ��ΪA
		//va = (va_x, va_y, 0)
		long va_x = pPrev->x - pCurrent->x;
		long va_y = pPrev->y - pCurrent->y;
		
		double norm_va_square = va_x*va_x + va_y*va_y;

        //�ǰ�����һ�ڽӵ�����ֱ��ΪB
		//vb = (vb_x, vb_y, 0)
		long vb_x = pNext->x - pCurrent->x;
		long vb_y = pNext->y - pCurrent->y;
        double norm_vb_square = vb_x*vb_x + vb_y*vb_y;

        double cross_a_b = va_x*vb_y - va_y*vb_x;//Cross(A,B)

		//|Cross(A,B)|=|A||B|sin(theta)
		double cross_square = cross_a_b*cross_a_b;
		
		
		double threshold = norm_va_square*norm_vb_square*sin5_square;

		//(|A||B|sin(theta))^2 < (|A||B|*sin5)^2
		if (cross_square < threshold)
		{
            if (nTurnVertexCount > 0)
            {
                if (pLastTurnVertex != pPrev)
                {//��һ�յ㲻�ǵ�ǰ����ǰһ�ڽӵ㣬 ��ǰ�����һ�߽�յ����ڵ�ֱ��C��ֱ��B�Ƚ�
                    long vc_x = pLastTurnVertex->x - pCurrent->x;
                    long vc_y = pLastTurnVertex->y - pCurrent->y;
                    double norm_vc_square = vc_x * vc_x + vc_y*vc_y;

                    double cross_c_b = vc_x*vb_y - vc_y*vb_x;//Cross(C,B);
                    cross_square = cross_c_b*cross_c_b;

                    if (cross_square < threshold)
                    {//ǰ������ֱ��C��B���ߣ���ǰ��㲻�ǹյ�
                        continue;
                    }
                }
                else
                {//ǰ������ֱ��A,B���ߣ���ǰ��㲻�ǹյ�
                    continue;
                }

            }
		}
		
		
        pLastTurnVertex = pCurrent;
        pNewVertices[nTurnVertexCount++] = *pCurrent;
		

	}//for

	return nTurnVertexCount;
}

//@����:�����������������������
//@����:pOrgVertices, ָ��ԭʼ���������ָ��
//      nVertexNum, ������Ŀ
//      nExpandSize, ���ͳߴ�
inline BOOL ExpandPolygon(const POINT* pOrgVertices, int nVertexNum,  int nExpandSize, POINT* pNewVertices)
{
	POINT centroid;
	long sum_x = 0, sum_y = 0;

	if (nVertexNum == 0) return FALSE;

	for (int i = 0; i < nVertexNum; i++)
	{
		sum_x += pOrgVertices[i].x;
		sum_y += pOrgVertices[i].y;
	}
	centroid.x = sum_x / nVertexNum;
	centroid.y = sum_y / nVertexNum;

	//ֱ��ax+by+c=0
	struct Line
	{
		long a;
		long b;
		long c;
	};
	std::vector<Line> lines;
	lines.resize(nVertexNum);

	POINT ptStart = pOrgVertices[0];
	ptStart.x -= centroid.x;
	ptStart.y -= centroid.y;

	for (int i = 0; i < nVertexNum; i++)
	{
		POINT ptEnd;
		
		if (i == nVertexNum - 1)
		{
			ptEnd = pOrgVertices[0];
		}
		else
		{
			ptEnd = pOrgVertices[i + 1];
		}
		ptEnd.x -= centroid.x;
		ptEnd.y -= centroid.y;

		//����ͨ����������ptStart, ptEnd��ֱ��
		//L = cross(ptStart,ptEnd)
		lines[i].a = ptStart.y - ptEnd.y;
		lines[i].b = -(ptStart.x - ptEnd.x);
		lines[i].c = ptStart.x*ptEnd.y - ptStart.y*ptEnd.x;

		//a*x/sqrt(a^2+b^2) + b*y/sqrt(a^2+b^2) = -c/sqrt(a^2+b^2)
		//����abs(-c/sqrt(a^2+b^2))��Ϊԭ�㵽ֱ�ߵľ���
		LONG sign_of_c = (lines[i].c >= 0) ? 1 : -1;
		
		//ԭ�㵽ֱ�ߵľ�������expandSize
		//a*x + b*y= - sign(c)*[abs(c) + expandSize*sqrt(a^2+b^2))
		double dbNorm = sqrt(double(lines[i].a*lines[i].a + lines[i].b*lines[i].b));
		LONG norm = (int)(dbNorm + 0.5);
		lines[i].c = sign_of_c * (abs(lines[i].c) + nExpandSize*norm);

		//Next Loop
		ptStart = ptEnd;
    }//for


	
	Line prevLine = lines[nVertexNum - 1];
	for (int i = 0; i < nVertexNum; i++)
	{
		Line currentLine = lines[i];

		//��������ֱ�ߵĽ���
		//cross_point = cross(L1,L2);
		//
		long X =   prevLine.b*currentLine.c - prevLine.c*currentLine.b;
		long Y = -(prevLine.a*currentLine.c - prevLine.c*currentLine.a);
		long Z =   prevLine.a*currentLine.b - prevLine.b*currentLine.a;
		
		if (Z)  //Z>=1
		{
			X = X / Z;
			Y = Y / Z;
			pNewVertices[i].x = X + centroid.x;
			pNewVertices[i].y = Y + centroid.y;
		}
		else
		{
			//����ֱ��ƽ��,
			//����ֱ��û�н��㣬Z=0, ʱ
			//���������������, ��ֱ���غϴ�ʱX=0,Y=0, Z=0.
			double norm = sqrt(double(currentLine.a*currentLine.a + currentLine.b*currentLine.b));
			if (norm < std::numeric_limits<double>::epsilon())
			{
				return FALSE;
			}
			
			double vx = nExpandSize*currentLine.a / norm;
			double vy = nExpandSize*currentLine.b / norm;
			pNewVertices[i].x = pOrgVertices[i].x + long(vx + .5);
			pNewVertices[i].y = pOrgVertices[i].y + long(vy + .5);

		}

		prevLine = currentLine;
	}//for
	

	return TRUE;
}
