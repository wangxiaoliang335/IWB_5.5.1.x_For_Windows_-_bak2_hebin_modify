#pragma once
#include "MultiDimensionPoint.h"
#include <limits>

//
//
//

//@����:�ж������Ƿ����߶��ཻ
inline BOOL IsRayIntersectLine(
                               const TPoint2D&  ptRayOrigin,
                               const TVector2D& RayDirection,
                               const TPoint2D (&LineSegment)[2],
                               TPoint2D* pIntersects = NULL
                               )
{
    //�߶�AB�ķ��̿���дΪ
    //Xp = Xa + (Xb - Xa)*��1, ��1��[0,1]
    //Yp = Ya + (Yb - Ya)*��1
    //(Xp,Yp)Ϊ�����ϵ�����һ��
    //(Xa,Ya)Ϊ�߶�AB��A�˵�����
    //(Xb,Yb)Ϊ�߶�AB��B�˵�����
    //���ߵķ���дΪ
    //Xp = Xo + Xd*��2, ��2>1
    //Yp = Yo + Yd*��2
    //(Xp,Yp)Ϊ�����ϵ�����һ��
    //(Xo,Yo)Ϊ������������
    //(Xd,Yd)Ϊ���ߵķ���ʸ��
    //���ߺ��߶�AB�Ľ���ͬʱ������������, 
    //
    //Xa + (Xb - Xa)*��1 =  Xo + Xd*��2
    //Ya + (Yb - Ya)*��1 =  Yo + Yd*��2
    //д�ɾ�����ʽ
    //
    //��          ����    �� ��     ��
    //��Xb-Xa  -Xd������1 ��=��Xo-Xa��
    //��Yb-Ya  -Yd������2 �� ��Yo-Ya��
    //��          ����    �� ��     ��
    //����Cram��������1�ͦ�2
    //��
    //��Xb-Xa  -Xd��
    //��          �� = 0, ���߶�������ƽ�У�����������������޽��㡣
    //��Yb-Ya  -Yd��
    //
    //ͨ���жϦ�1, ��2�Ƿ����ڦ�1��[0,1]�� ��2>=0 ���ж������Ƿ����߶��ཻ,
    //
    //

    //Enpoints of line segment
    double Xa = LineSegment[0][0] ;
    double Ya = LineSegment[0][1];
    double Xb = LineSegment[1][0];
    double Yb = LineSegment[1][1];;

    //Direction of the ray
    double Xd = RayDirection[0];
    double Yd = RayDirection[1];

    //Origin of the ray
    double X0 = ptRayOrigin[0];
    double Y0 = ptRayOrigin[1];

    //����ϵ������
    double A[2][2];
    A[0][0] = Xb - Xa;
    A[0][1] = -Xd ;
    A[1][0] = Yb -Ya ;
    A[1][1] = -Yd ;

    //
    double b[2];
    b[0] = X0 - Xa;
    b[1] = Y0 - Ya;

    double detA = A[0][0]*A[1][1] - A[0][1]*A[1][0];
    if( detA < std::numeric_limits<double>::epsilon())
    {
        return FALSE;
    }

    double lambda1 = (b[0]*A[1][1] - b[1]*A[0][1])/detA;
    double lambda2 = (b[1]*A[0][0] - b[0]*A[1][0])/detA;

    if(0.0 <= lambda1 && lambda1 <= 1.0 && lambda2 >= 0.0)
    {
        if(pIntersects)
        {
            pIntersects->d[0] = X0 + lambda2*Xd;
            pIntersects->d[1] = Y0 + lambda2*Yd;
        }

        return TRUE;
    }//if


    return FALSE;
}


//@����:ָ����������A�ڵ�m*n���㣬����Щ���ų�m��n�е�����ʽ������ʽ�ڲ����ڵ��ĸ������һ���ı��Σ�
//      ��������Ȧ�е��߶��ش���������ķ������죬��Щ�߶α����������A���������ཻ���õ����ɸ����㡣
//      ����Щ����;�������Ȧ�ϵĵ��Լ����ε��ĸ����㣬�������ɸ�������, �ı���,���ޱ��Ρ�
//      ���ھ���Ҫ�ҳ���Щλ�ھ��α߽紦�Ķ���Ρ�
//

class CAreaSplitter
{
public:
    BOOL SplitBorderArea(
        const RECT& rect,//ָ���ľ�������
        const TPoint2D* pInteriorPoints,//���������ڲ��ĵ���ɵľ���
        int m,//m�� 
        int n //n ��
        )
    {
        if(n <= 1 || m <= 1 ) return FALSE;


        int nRayCount = 2*(m + n);

        //�������Ҳ��2*(m+n)
        int nBorderAreaCount = nRayCount;


        //����������ĸ��ǵ�
        TPoint2D cornerPoints[4];

        cornerPoints[0].d[0] = rect.left ;
        cornerPoints[0].d[1] = rect.top;

        cornerPoints[1].d[0] = rect.right;
        cornerPoints[1].d[1] = rect.top;

        cornerPoints[2].d[0] = rect.right;
        cornerPoints[2].d[1] = rect.bottom;

        cornerPoints[3].d[0] = rect.left;
        cornerPoints[3].d[1] = rect.bottom;



        //�߽��ϵ���������2*(m + n)

        //======��ʼ������ʸ��=======
        m_rays.resize(nRayCount);

        int rayIndex  = 0;

        //1.top-most
        for(int c = 0; c < n; c ++)
        {
            TPoint2D p1,p2;
            p1 = pInteriorPoints[c];//��һ�еĵ�
            p2 = pInteriorPoints[n + c];//�ڶ��еĵ�

            m_rays[rayIndex].ptDirection.d[0] = p1.d[0] - p2.d[0];
            m_rays[rayIndex].ptDirection.d[1] = p1.d[1] - p2.d[1];
            m_rays[rayIndex].ptOrigin         = p1;
            rayIndex ++;
        }


        //2.right-most
        for(int r = 0; r < m; r++)
        {
            TPoint2D p1,p2;
            p1 = pInteriorPoints[n - 1 + r*n];//��n-1�еĵ�
            p2 = pInteriorPoints[n - 2 + r*n];//��n-2�еĵ�

            m_rays[rayIndex].ptDirection.d[0] = p1.d[0] - p2.d[0];
            m_rays[rayIndex].ptDirection.d[1] = p1.d[1] - p2.d[1];
            m_rays[rayIndex].ptOrigin = p1;
            rayIndex ++;
        }

        //3.bottom-most
        for(int c = n-1; c >= 0; c --)
        {
            TPoint2D p1,p2;
            p1 = pInteriorPoints[(m-1)*n + c];//��m-1�еĵ�
            p2 = pInteriorPoints[(m-2)*n + c];//��m-2�еĵ�

            m_rays[rayIndex].ptDirection.d[0] = p1.d[0] - p2.d[0];
            m_rays[rayIndex].ptDirection.d[1] = p1.d[1] - p2.d[1];
            m_rays[rayIndex].ptOrigin = p1;
            rayIndex ++;
        }

        //4.left-most
        for(int r = m-1; r >= 0; r -- )
        {
            TPoint2D p1,p2;
            p1 = pInteriorPoints[r*n + 0];//��1�еĵ�
            p2 = pInteriorPoints[r*n + 1];//��2�еĵ�

            m_rays[rayIndex].ptDirection.d[0] = p1.d[0] - p2.d[0];
            m_rays[rayIndex].ptDirection.d[1] = p1.d[1] - p2.d[1];
            m_rays[rayIndex].ptOrigin = p1;
            rayIndex ++;
        }



        //�������������α߽�Ľ���
        m_intersections.resize(nRayCount);

        for(int rayIndex = 0; rayIndex < nRayCount;  rayIndex ++)
        {

            BOOL bFound = FALSE;
            //����߽��ϵ���������������ߵĽ��㡣
            for(int lineSegmentIndex = 0; lineSegmentIndex <= 3; lineSegmentIndex ++)
            {
                TPoint2D lineSegment[2];
                int nCornerIndex = lineSegmentIndex;
                lineSegment[0] = cornerPoints[nCornerIndex];

                nCornerIndex += 1;
                if(4 == nCornerIndex) nCornerIndex = 0;
                lineSegment[1] = cornerPoints[nCornerIndex];

                BOOL bIntersect =   IsRayIntersectLine(
                    m_rays[rayIndex].ptOrigin,
                    m_rays[rayIndex].ptDirection,
                    lineSegment,
                    &m_intersections[rayIndex]);

                if(bIntersect)
                {
                    bFound = TRUE;
                    break;//break out inner loop
                }

            }//for


            if(!bFound)
            {
                return FALSE;
            }

        }//for-each(rayIndex)


        //��ʼ����������
        m_areas.resize(nBorderAreaCount);
        int nAreaIndex = 0;

        //����ÿ������Ķ���ε㼯
        for(int nAreaIndex = 0; nAreaIndex < nBorderAreaCount; nAreaIndex ++)
        {
            int nRay_A_Index  = nAreaIndex - 1;
            if(nAreaIndex == 0)
            {
                nRay_A_Index = nRayCount - 1;
            }

            int nRay_B_Index = nAreaIndex;

            TPoint2D ptRayAOrigin = m_rays[nRay_A_Index].ptOrigin;
            TPoint2D ptRayBOrigin = m_rays[nRay_B_Index].ptOrigin;;

            TPoint2D ptRayAIntersection = m_intersections[nRay_A_Index];
            TPoint2D ptRayBIntersection = m_intersections[nRay_B_Index];

            TPoint2D v1 = ptRayAIntersection - ptRayAOrigin;
            TPoint2D v2 = ptRayBIntersection - ptRayAOrigin;

            double norm_v1 = norm(v1);
            double norm_v2 = norm(v2);

            //�ж�v1,v2�ļн���[0,��]֮�䡣
            //���ò�����������жϼнǵķ���(˳ʱ�������ʱ��)
            //��ֵ��ζ��ʧ��֮��ļнǷ�ΧΪ[0,��)
            //��ֵ��ζ��ʧ��֮��ļнǷ�ΧΪ[��,2��]��
            double  cross_product = (v1.d[0] * v2.d[1] - v1.d[1] * v2.d[0]);

            BOOL bReverse = FALSE;

            TPoint2D startVector = v1;
            TPoint2D endVector   = v2;
            double   norm_start_vector = norm_v1;
            if(cross_product < 0)
            {
                startVector = v2;
                endVector   = v1;
                norm_start_vector = norm_v2;
                bReverse = TRUE;
            }

            double costheta_AB = (v1.d[0] * v2.d[0] + v1.d[1] * v2.d[1])/(norm_v1 * norm_v2);

            //�жϾ��ε��ĸ��ǵ��Ƿ�����A,Bʸ��֮��
            TPoint2D ptCornerCandidate;
            BOOL bFoundCorner = FALSE;
            for(int i = 0; i < 4 ; i++)
            {
                TPoint2D v3 = cornerPoints[i] - ptRayAOrigin;
                double norm_v3 = norm(v3);

                //ͨ������ж�ʸ����ʼ��ʸ�����⡣
                //startVector��v3
                double crossproduct_v3  = startVector.d[0]*v3.d[1] - startVector.d[1] * v3.d[0] ;
                if(crossproduct_v3 < 0)
                {//ʸ��v3��A,Bʸ��֮��
                    continue;
                }

                //ͨ������ж�ʸ���Ƿ����ձ����⡣
                double dotproduct_v3 = v3.d[0] * startVector.d[0] + v3.d[1] * startVector.d[1];
                double costheta_v3   = dotproduct_v3 /(norm(v3)*norm(v1));

                if(costheta_v3 < costheta_AB)
                {//�Ƕȴ��ڼн�AB, ����.
                    continue;
                }
                else
                {
                    bFoundCorner = TRUE;
                    ptCornerCandidate = cornerPoints[i];
                    break;
                }

            }//for-each(corner)

            m_areas[nAreaIndex].PolygonPoints.clear();
            m_areas[nAreaIndex].PolygonPoints.push_back(ptRayAIntersection);
            if(bFoundCorner)
            {
                m_areas[nAreaIndex].PolygonPoints.push_back(ptCornerCandidate);
            }
            m_areas[nAreaIndex].PolygonPoints.push_back(ptRayBIntersection);
            m_areas[nAreaIndex].PolygonPoints.push_back(ptRayBOrigin);
            if(ptRayAOrigin != ptRayBOrigin)
            {
                m_areas[nAreaIndex].PolygonPoints.push_back(ptRayAOrigin);
            }

        }//for-each(nAreaIndex)

        return TRUE;

    }//SplitBorderArea


    int GetAreaCount()const
    {
        return m_areas.size();
    }

    BOOL GetAreaPolygonPoints(unsigned int nIndex, const TPoint2D** pPolygonPoints, int* pPointCount)
    {
        if(nIndex >= m_areas.size()) return FALSE;
        if(pPolygonPoints) *pPolygonPoints = &m_areas[nIndex].PolygonPoints[0];

        if(pPointCount)*pPointCount = m_areas[nIndex].PolygonPoints.size();
        return TRUE;

    }

protected:

    struct TPolygonArea
    {

        std::vector<TPoint2D> PolygonPoints;//������ϵĵ�
    };

    std::vector<TPolygonArea>     m_areas;


    struct TRay
    {
        TPoint2D ptOrigin;
        TPoint2D ptDirection;
    };
    std::vector<TRay> m_rays;

    std::vector<TPoint2D>  m_intersections;



};