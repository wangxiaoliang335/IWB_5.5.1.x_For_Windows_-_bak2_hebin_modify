#include "stdafx.h"
#include "headers.h"
#include <vector>
#include "../inc/CameraModel.h"
#include "../inc/MultiDimensionPoint.h"
#include "../inc/ImageFrame.h"
#include "../inc/SolidGeometry.h"
#include "CylinderBulbModel.h"


//@����:�����Բ����һ��P,��Բ�����е�����ƽ����Բ������н�ֱ��L1,L2��
//
//@����:P, Բ��������һ������
//      cy, Բ�������
//      line1, ��һ����ƽ����Բ������н�ֱ��L1
//      line2, �ڶ�����ƽ����Բ������н�ֱ��L2
//@����:��P����ƽ����Բ�����ߵ�ֱ��, ����ֱ��������Բ���������������ƽ�档
void PointTangentCylinder(const TPoint3D& P, const Cylinder& cyl, Line3D* line1, Line3D* line2)
{

    //
    //���P����Բ������ƽ�е�ֱ��L������Cyliner.w֮��ľ���d
    TPoint3D BP = P - cyl.base;

    TVector3D vBP = BP/norm(BP);

    //Dʸ��, ��Բ������Ϊԭ��, ����P��ƽ��ֱ�ߵľ���ʸ��
    TVector3D vD = BP - dot(cyl.axis, BP)*cyl.axis;
    
    //����
    double d = norm(vD);

    //��һ��Ϊ��λʸ��
    vD = vD/d;

   //E��λʸ��, ���ߺ͵�P����ƽ��еķ��ߵ�λʸ��
    TVector3D vE = cross(cyl.axis, vBP);
    
    //��һ��Ϊ��λʸ��
    vE = vE/norm(vE);

    //Բ�������ϵ����е���ɵ��ҵĳ��ȵ�һ��
    //����ֱ�������ε����S=1/2*��ֱ�Ǳߵĳ˻���һ��=1/2*б�ߵĳ���*б���ϵĸ�
    double e = cyl.radius * sqrt(d*d - cyl.radius*cyl.radius)/d;


    //Բ�ĵ��ҵľ���
    double c = (cyl.radius * cyl.radius)/d;

    //Բ�����������ϵ�ͶӰ��Proj_B
    TPoint3D proj_B = cyl.base + c*vD;

    if(line1)
    {
        line1->base = proj_B + e*vE;
        line1->w    = cyl.axis;
    }

    if(line2)
    {
        line2->base = proj_B - e*vE;
        line2->w    = cyl.axis;
    }


}


//@����:��center��t1, t2�������ڵ�ƽ����, ��centerΪ���ģ� radiusΪ�뾶��Բ
//      t1,t2��Բ�ϵ�������, ��Բ�Ľǲ���t1,t2֮���Բ���ϵ�M���㡣

void SampleCircleArc(
    const TPoint3D& center,
    const double& radius,
    const TPoint3D& t1, 
    const TPoint3D& t2, 
    int M,  
    TPoint3D* pSampleData)
{

    //������Բ��Ϊ����ʸ��
     TVector3D v1 = t1 - center;
     TVector3D v2 = t2 - center;

     //v1��v2�Ĳ��
     TVector3D cross_v1v2 = cross(v1,v2);

     if(cross_v1v2.d[2] < 0)
     {
        TVector3D tmp = v1;
        v1 = v2;
        v2 = tmp;
     }

     double norm_v1 = norm(v1);
     double norm_v2 = norm(v2);

     //��vx��ΪԲ�������x��ĵ�λʸ��
     TVector3D vx = v1/norm_v1;

     //��vx��ΪԲ�������y��ĵ�λʸ��
     double proj_v2_vx = dot(v2,vx);
     TVector3D vy = v2 - proj_v2_vx * vx;
     vy = vy/norm(vy);

     //����ʸ��v1,v2֮��ļн�
     double angle = asin(norm(cross_v1v2)/(norm_v1*norm_v2));

     if(proj_v2_vx < 0 )//�нǴ��ڦ�/2
         angle = M_PI - angle;


     //�����ĽǶȼ��
     double sample_angle_interval = angle/(M - 1);

     double sample_angle = 0.0;
     for(int i = 0; i < M; i++)
     {

         //Բƽ������תΪ3D����ľ���R
         //
         //|x|      |x'|                       |x'|
         //|y| = R *|y'| = [vx vy cross(vx,vy)]|y'| = vx*x' + vy* y'
         //|z|      |0 |                       |0 |
         //

         double x = radius * cos(sample_angle );
         double y = radius * sin(sample_angle);


        pSampleData[i] = vx*x + vy*y + center;


        sample_angle += sample_angle_interval;

     }//for

}

//@����:��Բ����һ��P������Բ�����е�������������������������Բ��Բ�ܲ���M��, ���������N��,
//      ���M*N��������
//@�������:
//     P, Բ��������һ������
//    cyl, Բ�������
//    M,����������Բ������Բ�ܵĲ�������
//    N,����������Բ������Ĳ�������
//
//@�������:
//    pSampleData���������������飬�ߴ��СΪM*N
//
 void PointTangentCylinderGrid(const TPoint3D& P, const Cylinder& cyl, int M, int N, TPoint3D* pSampleData)
 {

     Line3D line1, line2;

     //�������P��Բ����ƽ����Բ����
     PointTangentCylinder(P, cyl, &line1, &line2);

     //�ڹ����㴹ֱ��Բ�����ߵĺ������, line1.b��line2.b��������ƽ���ڸú���������µ��е㡣
     //�����е�֮���Բ������M��,
     SampleCircleArc(cyl.base, cyl.radius, line1.base, line2.base, M, pSampleData);

     double length_interval = cyl.length / (N-1);

     pSampleData += M;

     //��Բ�����߷�����չ������
     for(int i = 1; i < N; i++)
     {
            TPoint3D vInc = cyl.axis * length_interval;
            for(int j = 0; j < M; j++)
            {
                pSampleData[j] = pSampleData[j-M] + vInc;//����һ��M����Ļ����ϼ�����µ�
            }

            pSampleData += M;
     }



 }



CylinderBulbModel::CylinderBulbModel()
:
m_pGenericCameraModel(NULL)
{

}


//Copy Constructor
//����Ҫ���忽�����캯��, ��Ϊ���Ա�����кܶ������á�
CylinderBulbModel::CylinderBulbModel(const CylinderBulbModel& clone)
{
    m_pGenericCameraModel   = clone.m_pGenericCameraModel;
    m_vecContatcPointOffset = clone.m_vecContatcPointOffset;

}


//@����:���������ÿ�����ص��Ӧ����������λ��Pi��,ֱ��z=0ƽ�����һ���뾶ΪR,����ΪL��
//      Բ���ε���, ����������񣬼����������Ƭ�е���������Gi, ��=Pi-Gi��ֵ�������������ʴ����������ꡣ
//@����:pCameraModel, ���ģ��ָ��
//      pMaskImage, ����ͼָ��
//      nImageWidth, ���ͼƬ���
//      nImageHeight, ���ͼƬ�߶�
void CylinderBulbModel::CalcProjectData(const CGenericCameraModel* pCameraModel, const RECT& rcMonitor, int nImageWidth, int nImageHeight)
{
    // CImageFrame grayImage;

    //<<debug
    //CImageFrame debugFrame;//����֡
    //debugFrame.SetSize(nImageWidth, nImageHeight);
    //debugFrame.Clear();
    //debug>>

    m_nImageWidth  = nImageWidth;
    m_nImageHeight = nImageHeight;

    m_pGenericCameraModel = pCameraModel;

    //grayImage.SetSize(nImageWidth, nImageHeight);

    m_vecContatcPointOffset.resize((nImageWidth>> 2)*(nImageHeight>>2));

    TPoint3D p3dOrg;
    p3dOrg.d[0] = 0.0;
    p3dOrg.d[1] = 0.0;
    p3dOrg.d[2] = 0.0;


    //Բ����ߴ���Ϣ
    //radius =1.5, height=6��ʹ��Ͷ���Ϊ0.15�ľ�ͷ��,��Ļ�ĸ�����Ĵ��ؾ�������Ҫ��
    //double Cylinder_Radius = 1.5;//��λ:��Ļ����
    //ouble Cylinder_Height = 6  ;//��λ:��Ļ����

	/*
    double Cylinder_Radius = 4  ;//��λ:��Ļ����
    double Cylinder_Height = 10 ;//��λ:��Ļ����
	*/
	double Cylinder_Radius = 1.5;//��λ:��Ļ����
	double Cylinder_Height = 5;//��λ:��Ļ����

    double Cylinder_Offset_From_Board = 0.0;//Բ���������ľ���(��Z�Ḻ��)

    int M = 13;
    int N = 13;



    //Բ�������������3D���ꡣ
    std::vector<TPoint3D> vecSurface3DPoints;
    vecSurface3DPoints.resize(M*N);

    //Բ�������������ͼ������
    std::vector<TPoint2D> vecSurfaceImagePoints;
    vecSurfaceImagePoints.resize(M*N);


    TContactPointOffsetInformation* pOffsetInformation = &m_vecContatcPointOffset[0];


    RECT rcMaskArea = rcMonitor;
    long Margin = 20;
    rcMaskArea.left   -= Margin;
    rcMaskArea.right  += Margin;
    rcMaskArea.top    -= Margin;
    rcMaskArea.bottom += Margin;

    //ÿ16����������һ��������
    for(int r = 0; r < nImageHeight; r+=4)
    {

        for(int c = 0; c < nImageWidth; c+=4)
        {
            //if(pMaskImage[r*nImageWidth + c] == 0x00)
            //{
            //    pOffsetInformation->vOffset.d[0] = 0.0;
            //    pOffsetInformation->vOffset.d[1] = 0.0;
            //    pOffsetInformation->pt2dCentroidPos.d[0] = -1.0;
            //    pOffsetInformation->pt2dCentroidPos.d[1] = -1.0;
            //    pOffsetInformation->bValid               = FALSE;
            //    pOffsetInformation ++;

            //    continue;
            //}
            //else
            //{
            //ͼ������ӳ��Ϊ��������, 
            //�����꼴ΪԲ���ε����봹ֱ����ĽӴ��㡣
            TPoint2D pt2DImgContact;
            pt2DImgContact.d[0] =  (double)c;
            pt2DImgContact.d[1] =  (double)r;

            TPoint2D ptWorldContactPos;

            m_pGenericCameraModel->FromImage2World(&pt2DImgContact, 1, &ptWorldContactPos);

            if(ptWorldContactPos.d[0]  < rcMaskArea.left 
                || 
                ptWorldContactPos.d[0] > rcMaskArea.right
                ||
                ptWorldContactPos.d[1] < rcMaskArea.top
                ||
                ptWorldContactPos.d[1] > rcMaskArea.bottom)
            {

                pOffsetInformation->bValid = FALSE;

            }
            else
            {

                //Բ������
                Cylinder cyl;

                //Բ���Ļ�������
                cyl.base.d[0] = ptWorldContactPos.d[0];//x����
                cyl.base.d[1] = ptWorldContactPos.d[1];//y����
                cyl.base.d[2] = -Cylinder_Offset_From_Board;//��Z�Ḻ��


                //Բ������ʸ��
                cyl.axis.d[0] = 0;
                cyl.axis.d[1] = 0;
                cyl.axis.d[2] = -1;


                //Բ���İ뾶
                cyl.radius = Cylinder_Radius;

                //Բ���ĳ���
                cyl.length = Cylinder_Height;


                //������������ĵ��������������ڲ��ĵ㡣
                PointTangentCylinderGrid(pCameraModel->GetCameraWorldPosition(), cyl, M, N, &vecSurface3DPoints[0]);


                //for(size_t i=0; i < m_vecSurfacePoints.size(); i++)
                //{
                //    vecSurface3DPoints[i].d[0] = m_vecSurfacePoints[i].d[0] + pt2DWorld.d[0];
                //    vecSurface3DPoints[i].d[1] = m_vecSurfacePoints[i].d[1] + pt2DWorld.d[1];
                //    vecSurface3DPoints[i].d[2] = m_vecSurfacePoints[i].d[2];
                //}

                //����������ϵĵ��3D��������ӳ�䵽2Dͼ�����ꡣ
                m_pGenericCameraModel->MapWorldToImage(
                    &vecSurface3DPoints[0],
                    vecSurface3DPoints.size(),
                    &vecSurfaceImagePoints[0]);

                //���ػ�2Dͼ��
                //grayImage.Clear();

                RECT rcSpotArea;
                rcSpotArea.left    = (std::numeric_limits<LONG>::max)();
                rcSpotArea.top     = (std::numeric_limits<LONG>::max)();
                rcSpotArea.right   = (std::numeric_limits<LONG>::min)();
                rcSpotArea.bottom  = (std::numeric_limits<LONG>::min)();


                double dbMx = 0.0; double dbMy = 0.0;
                double dbMass = (double)vecSurfaceImagePoints.size();
                for(size_t i=0; i < vecSurfaceImagePoints.size(); i++)
                {
                    const TPoint2D& ptImage = vecSurfaceImagePoints[i];

                    dbMx += ptImage.d[0];
                    dbMy += ptImage.d[1];

                    /*
                    long x = long(ptImage.d[0] + .5);
                    long y = long(ptImage.d[1] + .5);

                    if(x <0 || x >= nImageWidth) continue;
                    if(y <0 || y >= nImageHeight) continue;

                    grayImage.SetPixel(x, y, 0x80);

                    if(x < rcSpotArea.left)
                    {
                    rcSpotArea.left = x;
                    }

                    if(x > rcSpotArea.right)
                    {
                    rcSpotArea.right = x + 1;
                    }

                    if(y < rcSpotArea.top)
                    {
                    rcSpotArea.top = y;
                    }

                    if(y > rcSpotArea.bottom)
                    {
                    rcSpotArea.bottom = y + 1;
                    }*/

                }//for-each(i)

                //grayImage.SetPixel(c, r, 0xFF);//�������ɵ���
                //
                ////��������ͼ���е�����
                //int mass = 0;
                //int mx   = 0;
                //int my   = 0;

                //BYTE* pRowData = grayImage.GetData() + rcSpotArea.top*nImageWidth + rcSpotArea.left;
                //for(long y = rcSpotArea.top; y < rcSpotArea.bottom; y++)
                //{
                //    BYTE* pImageData = pRowData;

                //    for(long x = rcSpotArea.left; x < rcSpotArea.right; x++)
                //    {
                //        if(*pImageData++ > 0)
                //        {
                //            mass ++;
                //            mx += x;
                //            my += y;
                //        }
                //    }

                //    pRowData += nImageWidth;
                //}

                //
                //���������
                //TPoint2D pt2DCentroid;
                //pt2DCentroid.d[0] = (double)mx / (double)mass;
                //pt2DCentroid.d[1] = (double)my / (double)mass;
                TPoint2D pt2DCentroid;
                pt2DCentroid.d[0] = (double)dbMx / (double)dbMass;
                pt2DCentroid.d[1] = (double)dbMy / (double)dbMass;
                //
                //grayImage.SetPixel(int(pt2DCentroid[0] + .5), int(pt2DCentroid[1] + .5), 0xC0);//����

                //���㴥��������ĵ�ƫ����
                pOffsetInformation->vOffset.d[0] =  pt2DCentroid.d[0] - pt2DImgContact.d[0];
                pOffsetInformation->vOffset.d[1] =  pt2DCentroid.d[1] - pt2DImgContact.d[1];
                pOffsetInformation->pt2dCentroidPos.d[0] = pt2DCentroid.d[0];
                pOffsetInformation->pt2dCentroidPos.d[1] = pt2DCentroid.d[1];
                //pOffsetInformation->nLightSpotMass = (int)dbMass;
                //pOffsetInformation->rcBound        = rcSpotArea;
                pOffsetInformation->bValid         = TRUE;

                //}//else


                //���Դ���
                //if(0 == r % 32  && 0 == c % 64)
                //{

                //    debugFrame |= grayImage;

                //}
                //
            }

            pOffsetInformation ++;
        }//for-each(c)


    }//for-each(r)

    /*
    {
    SYSTEMTIME st;
    GetLocalTime(&st);
    TCHAR szFilePath[MAX_PATH];

    _stprintf_s(
    szFilePath,
    _countof(szFilePath),
    _T(".\\Intermediate\\Debug\\%4d-%2d-%2d\\%02d-%02d-%02d-%3d.png"),
    st.wYear,
    st.wMonth,
    st.wDay,
    st.wHour,
    st.wMinute,
    st.wSecond,
    st.wMilliseconds
    );

    CAtlString strFullPath  = CheckAndCreatePath(szFilePath);

    SaveGrayAsPng(CT2CA(strFullPath), debugFrame.GetData(), debugFrame.Width(), debugFrame.Height());
    }

    */

}


//@����:���ݼ���ʹ�ߵ����ĺ��������󴥵������ĵ�ƫ������
//@����:pCentroid,������ĵ����ꡣ
//      pContourPts,������������顣
//      nContourPointsCount, �����������

//TVector2D  CylinderBulbModel::GetOffset(const TPoint2D& ptCentroid, const TPoint2D* pContourPts, int nContourPointsCount, TPoint2D* pContactPoint)
TVector2D  CylinderBulbModel::GetOffset(const TPoint2D& ptCentroid)
{

    TVector2D offset;
    offset.d[0] = 0.0;
    offset.d[1] = 0.0;

    long x = long(ptCentroid.d[0]);
    long y = long(ptCentroid.d[1]);


    size_t nIndex  = (y>>2)*(m_nImageWidth>>2) + (x >> 2);

    if(nIndex <0 || nIndex >= m_vecContatcPointOffset.size()) return offset;

    if(x < 0) x = 0;
    if(x >= m_nImageWidth) x = m_nImageWidth - 1;

    if(y < 0) y =0;
    if(y >= m_nImageHeight) y = m_nImageHeight - 1;

    TContactPointOffsetInformation& offsetInformation = m_vecContatcPointOffset[nIndex];


    static const double scale = 1.0;


    offset.d[0] = (offsetInformation.bValid?offsetInformation.vOffset.d[0]*scale:0.0);
    offset.d[1] = (offsetInformation.bValid?offsetInformation.vOffset.d[1]*scale:0.0);



    return offset;
}



TVector2D  CylinderBulbModel::GetRealOffset(const TPoint2D& ptImagePoint)
{

	TPoint2D ptWorldContactPos;
	m_pGenericCameraModel->FromImage2World(&ptImagePoint, 1, &ptWorldContactPos);

	double Cylinder_Offset_From_Board = 0.0;//Բ���������ľ���(��Z�Ḻ��)

	//Բ������
	Cylinder cyl;

	//Բ���Ļ�������
	cyl.base.d[0] = ptWorldContactPos.d[0];//x����
	cyl.base.d[1] = ptWorldContactPos.d[1];//y����
	cyl.base.d[2] = -Cylinder_Offset_From_Board;//��Z�Ḻ��


												//Բ������ʸ��
	cyl.axis.d[0] = 0;
	cyl.axis.d[1] = 0;
	cyl.axis.d[2] = -1;


	double Cylinder_Radius = 1.5;//��λ:��Ļ����
	double Cylinder_Height = 5;//��λ:��Ļ����

	//Բ���İ뾶
	cyl.radius = Cylinder_Radius;

	//Բ���ĳ���
	cyl.length = Cylinder_Height;

	int M = 6;
	int N = 6;

	std::vector<TPoint3D> vecSurface3DPoints;
	vecSurface3DPoints.resize(M*N);

	//������������ĵ��������������ڲ��ĵ㡣
	PointTangentCylinderGrid(m_pGenericCameraModel->GetCameraWorldPosition(), cyl, M, N, &vecSurface3DPoints[0]);


	//Բ�������������ͼ������
	std::vector<TPoint2D> vecSurfaceImagePoints;
	vecSurfaceImagePoints.resize(M*N);

	
	//����������ϵĵ��3D��������ӳ�䵽2Dͼ�����ꡣ
	m_pGenericCameraModel->MapWorldToImage(
		&vecSurface3DPoints[0],
		vecSurface3DPoints.size(),
		&vecSurfaceImagePoints[0]);


	double dbMx = 0.0; double dbMy = 0.0;
	double dbMass = (double)vecSurfaceImagePoints.size();
	for (size_t i = 0; i < vecSurfaceImagePoints.size(); i++)
	{
		const TPoint2D& ptImage = vecSurfaceImagePoints[i];

		dbMx += ptImage.d[0];
		dbMy += ptImage.d[1];

	}


	TPoint2D pt2DCentroid;
	pt2DCentroid.d[0] = (double)dbMx / (double)dbMass;
	pt2DCentroid.d[1] = (double)dbMy / (double)dbMass;
	

	TVector2D offset;
	offset = pt2DCentroid - ptImagePoint;

	return offset;

}
