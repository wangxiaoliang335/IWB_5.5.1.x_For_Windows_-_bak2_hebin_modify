#include "stdafx.h"
//#include "headers.h"
CContactPtFinder::CContactPtFinder()
:
m_dbTangentHorz(0.0),
m_dbTangentVert(0.0),
//m_dbR_D_Ratio_Vert(0.05),
//m_dbR_D_Ratio_Horz(0.015),
m_dbR_D_Ratio_Vert(0.01),
m_dbR_D_Ratio_Horz(0.01),
m_nImageCx(640<<INT_SCALE_SHIFT_SIZE),
m_nImageCy(480<<INT_SCALE_SHIFT_SIZE),
m_eLensType(E_LENS_TR_0_DOT_34)
{
    //<<2014/04/21
    //m_dbTangentVert = tan(M_PI*0.0/180.0);//LCD Һ����
    //m_dbTangentVert = tan(M_PI*15.0/180.0);//2015/02/11
     m_dbTangentVert = tan(M_PI*45.0/180.0);//2015/02/11
    //m_dbTangentVert = tan(M_PI*0.0/180.0);//2015/02/11
    //m_dbTangentVert = tan(M_PI*10.0/180.0);
    //m_dbTangentVert = tan(M_PI*45.0/180.0);
    //2014/04/21>>
    m_dbHalfCx = m_nImageCx/2.0;
    m_dbHalfCy = m_nImageCy/2.0;
    SetViewAngle(120.0);

    
    m_Model1_K[0] = 1.0;
    m_Model1_K[1] = 0.0;

    
    //SetThrowRatio(0.34);//�̽���ͷͶ���  :0.34
                          //���̽���ͷͶ���:0.28,0.19
                          //����Ͷ���:1.34
}


//@��  ��:���ݹ�ߵ���Ӿ�����Ϣ��������������е��λ��, 
//@����ֵ:
POINT CContactPtFinder::operator()(const RECT& rcBound)
{
    POINT pt;
    
    ////��ֱ����
    ////������ĵ�����
    //double y = (rcBound.bottom + rcBound.top)/2.0 - m_dbHalfCy;

    //double tangent_alpha = y/m_dbFocalLength;

    //double tangent_alpha_sub_delta   = (tangent_alpha - m_dbTangentVert)/(1 + tangent_alpha*m_dbTangentVert);
    //double tangent_espilon_sub_delta = tangent_alpha_sub_delta * (1 - m_dbR_D_Ratio);
    //double tangent_epsilon           = (m_dbTangentVert + tangent_espilon_sub_delta)/(1 - m_dbTangentVert*tangent_espilon_sub_delta);
    //       y                         = (tangent_epsilon*m_dbFocalLength + m_dbHalfCy)*(double)MAGNIFY_FACTOR;
    //pt.y                             = int(y);



    ////ˮƽ�������
    //double x = (rcBound.left + rcBound.right)/2.0 - m_dbHalfCx;

    //tangent_alpha = x/m_dbFocalLength;

    //tangent_alpha_sub_delta   = (tangent_alpha - m_dbTangentHorz)/(1 + tangent_alpha*m_dbTangentHorz);
    //tangent_espilon_sub_delta = tangent_alpha_sub_delta * (1 - m_dbR_D_Ratio);
    //tangent_epsilon           = (m_dbTangentHorz + tangent_espilon_sub_delta)/(1 - m_dbTangentHorz*tangent_espilon_sub_delta);
    //       x                  = (tangent_epsilon*m_dbFocalLength + m_dbHalfCx)*(double)MAGNIFY_FACTOR;
    //pt.x                      = int(x);


    //double dy = pt.y - ((rcBound.bottom + rcBound.top)<<INT_SCALE_SHIFT_SIZE)/2.0;
    //double dx = pt.x - ((rcBound.left + rcBound.right)<<INT_SCALE_SHIFT_SIZE)/2.0;
    //AtlTrace(_T("dx=%.2f, dy=%.2f\n"), dx/(double)MAGNIFY_FACTOR, dy/(double)MAGNIFY_FACTOR);



    //pt.x = (long)(((rcBound.left   + rcBound.right)<<INT_SCALE_SHIFT_SIZE)/2.0);
    //pt.y = (long)(((rcBound.bottom + rcBound.top  )<<INT_SCALE_SHIFT_SIZE)/2.0);


    pt.x  = (long)(((rcBound.left + rcBound.right  )<<INT_SCALE_SHIFT_SIZE)/2.0);
    pt.y = rcBound.top << INT_SCALE_SHIFT_SIZE;

    return pt;
}

//@����:���ݹ�����ĵ�λ��,�ҳ�
//      ptCentroid, �������λ��, ע��������Ѿ��Ŵ���2^INT_SCALE_SHIFT_SIZE����
POINT CContactPtFinder::operator()(const POINT& ptCentroid)
{

   // //return ptCentroid;
   // POINT pt;
   // 
   // //��ֱ����
   // //������ĵ�����
   // double y = ptCentroid.y - m_dbHalfCy;

   // //��������ļнǡ�
   // double tangent_alpha = y/m_dbFocalLength;

   // 
   // //tg(��-��)=(tg�� - tg��)/(1 + tg��*tg��)
   ////double tangent_alpha_sub_delta   = (tangent_alpha - m_dbTangentVert)/(1 + tangent_alpha*m_dbTangentVert);

   // //tg(��+��)=(tg�� + tg��)/(1 - tg��*tg��)
   // //ע��˴���Ϊ����ͷ������ˮƽ����ļнǡ�
   // //tg�ļ�Ϊm_dbTangentVert
   // double tangent_alpha_add_delta   = (tangent_alpha + m_dbTangentVert)/(1 - tangent_alpha*m_dbTangentVert);

   // //tg(��-��)=tg(��-��)*/(1 - m_dbR_D_Ratio)
   // //double tangent_espilon_sub_delta = tangent_alpha_sub_delta * (1 - m_dbR_D_Ratio);

   // //tg(��+��)=tg(��+��)*/(1 - m_dbR_D_Ratio)
   // double tangent_espilon_add_delta = tangent_alpha_add_delta * (1 - m_dbR_D_Ratio_Vert);

   // //tg(��) =  tg(��+��-��) = [tg(��+��) - tg��]/[1 + tg(��+��)*tg��]
   // //double tangent_epsilon           = (m_dbTangentVert + tangent_espilon_sub_delta)/(1 - m_dbTangentVert*tangent_espilon_sub_delta);
   // double tangent_epsilon             = (tangent_espilon_add_delta - m_dbTangentVert)/(1 + m_dbTangentVert*tangent_espilon_add_delta);

   // //
   //        y                         = tangent_epsilon*m_dbFocalLength + m_dbHalfCy;

   // pt.y                             = int(y);





   // //ˮƽ�������
   // double x = ptCentroid.x - m_dbHalfCx;

   // tangent_alpha = x/m_dbFocalLength;

   //// double tangent_alpha_sub_delta   = (tangent_alpha - m_dbTangentHorz)/(1 + tangent_alpha*m_dbTangentHorz);
   // tangent_alpha_add_delta   = (tangent_alpha + m_dbTangentHorz)/(1 - tangent_alpha*m_dbTangentHorz);
   // //double tangent_espilon_sub_delta = tangent_alpha_sub_delta * (1 - m_dbR_D_Ratio);
   // tangent_espilon_add_delta = tangent_alpha_add_delta * (1 - m_dbR_D_Ratio_Horz);

   // //tangent_epsilon           = (m_dbTangentHorz + tangent_espilon_sub_delta)/(1 - m_dbTangentHorz*tangent_espilon_sub_delta);
   //  tangent_epsilon           = (tangent_espilon_add_delta - m_dbTangentHorz)/(1 + m_dbTangentHorz*tangent_espilon_add_delta);


   //        x                  = tangent_epsilon*m_dbFocalLength + m_dbHalfCx;
   // pt.x                      = int(x);


   // double dy = pt.y - ptCentroid.y;
   // double dx = pt.x - ptCentroid.x;
   // AtlTrace(_T("x=%f, y=%f, dx=%.2f, dy=%.2f\n"), 
   //     ptCentroid.x/(double)MAGNIFY_FACTOR, 
   //     ptCentroid.y/(double)MAGNIFY_FACTOR, 
   //     dx/(double)MAGNIFY_FACTOR, 
   //     dy/(double)MAGNIFY_FACTOR);

    //pt.x = ((rcBound.left   + rcBound.right)<<INT_SCALE_SHIFT_SIZE)/2.0;
    //pt.y = ((rcBound.bottom + rcBound.top  )<<INT_SCALE_SHIFT_SIZE)/2.0;


    POINT pt = Model_1(ptCentroid);
    //POINT pt = Model_2(ptCentroid);

    return pt;
}


//����ģ��1
//���Ϊ����뾵ͷ������ļн�
//���ݹ�ߵ�u,v������Լ���õ����뻭�����ĵľ���r, �ͽ���(��λ:xiangsu)����������Ƚǡ�
//�������ģ��
//��'= k1*�� + k2*��^3
//��'Ϊ������Ĺ���뾵ͷ������ļн�
//���ݦ�'����������Ĺ������u',v'��
//ע��������Ѿ��Ŵ���2^INT_SCALE_SHIFT_SIZE��
POINT CContactPtFinder::Model_1(const POINT& ptCentroid)
{
   POINT pt;

    //����������ĵ�����
    int u0 = m_nImageCx >> 1;
    int v0 = m_nImageCy >> 1;

    //����������Ի�������Ϊԭ�������ϵ�е�ʸ�����ȡ�
    int u = ptCentroid.x  - u0;
    int v = ptCentroid.y  - v0;

    double radius = sqrt((double)(u*u + v*v));


    double theta = atan(radius/m_dbFocalLength);

    //double theta_modified =  (1 - lambda)*theta;

    double theta_modified = m_Model1_K[0]*theta + m_Model1_K[1]*theta*theta*theta;

    double radius_modified = m_dbFocalLength * tan(theta_modified);


    int u_modified = u;
    int v_modified = v;

    if(radius > 1e-6)
    {
       u_modified =  u * radius_modified / radius;
       v_modified =  v * radius_modified / radius;
    }

    //������Ĺ��λ��
    pt.x = u_modified  + u0;
    pt.y = v_modified  + v0;

    return pt;

}


//����ģ��2
//���Ϊ����뾵ͷ������ļн�
//���ݹ������(u,v), �������������ĵľ���r,��������dr
//dr = k*��
//r' = r + dr
POINT CContactPtFinder::Model_2(const POINT& ptCentroid)
{
    POINT pt;

    //����������ĵ�����
    int u0 = m_nImageCx >> 1;
    int v0 = m_nImageCy >> 1;

    //����������Ի�������Ϊԭ�������ϵ�е�ʸ�����ȡ�
    int u = ptCentroid.x  - u0;
    int v = ptCentroid.y  - v0;

    double radius = sqrt((double)(u*u + v*v));


    double theta = atan(radius/m_dbFocalLength);

    double delta_radius = m_Model2_K[0] * theta;


    double radius_modified = radius + delta_radius;


    int u_modified = u;
    int v_modified = u;

    if(radius > 1e-6)
    {
        u_modified = u*radius_modified/radius;
        v_modified = v*radius_modified/radius;
    }

    pt.x = u_modified + u0;
    pt.y = v_modified + v0;

    return pt;

}



//@����:���ݹ�����ĵ�λ��,�ҳ�
//      ptCentroid, �������λ��, ע��������Ѿ��Ŵ���2^INT_SCALE_SHIFT_SIZE����
//      lambda,ƫ����������
//@˵��:������������ĵļн�Ϊ��, ������ļн�Ϊ(1-lambda)��,�н��������
//      ���������
//      x=f*sin[(1-lambda)��]*cos��
//      y=f*sin[(1-lambda)��]*sin��
POINT CContactPtFinder::operator()(const POINT& ptCentroid, double lambda1, double lambad2)
{
    POINT pt;

    //����������ĵ�����
    int x0 = m_nImageCx >> 1;
    int y0 = m_nImageCy >> 1;

    //����������Ի�������Ϊԭ�������ϵ�е�ʸ�����ȡ�
    int x = ptCentroid.x  - x0;
    int y = ptCentroid.y  - y0;

    double radius = sqrt((double)(x*x + y*y));


    double theta = atan(radius/m_dbFocalLength);

    //double theta_modified =  (1 - lambda)*theta;

    double theta_modified = lambda1*theta + lambad2*theta*theta;

    double radius_modified = m_dbFocalLength * tan(theta_modified);


    int x_modified = x;
    int y_modified = y;

    if(radius > 1e-6)
    {
       x_modified =  x*radius_modified/radius;
       y_modified =  y*radius_modified/radius;
    }

    //������Ĺ��λ��
    pt.x = x_modified  + x0;
    pt.y = y_modified  + y0;


    return pt;
}

//@����:����"����������ͶӰĻ�͵��涼��ֱ��ƽ��"������ͷ����ļнǦյ�����
void CContactPtFinder::SetTangentHorz(const double& dbTangentHorz)
{
    m_dbTangentHorz = dbTangentHorz;
}


//@����:����"�����ġ���ֱͶӰĻ������ˮƽ���ƽ��"������ͷ����ļнǦ׵�����
void CContactPtFinder::SetTangentVert(const double& dbTangentVert)
{
    m_dbTangentVert = dbTangentVert;
}


//@����:����
void CContactPtFinder::SetViewAngle(const double& dbDegree)
{
    m_dbMaxViewAngle = dbDegree;


    //����Խ��߳���
    double d = sqrt(double(m_nImageCx * m_nImageCx + m_nImageCy * m_nImageCy));

    double tang_half_view_angle = tan((double)dbDegree*M_PI/360);

    //�����Ч���೤��
    m_dbFocalLength = d/tang_half_view_angle;


    //����Ͷ���
    m_dbThrowRatio = m_dbFocalLength/m_nImageCx;
}


//@����:����Ͷ���
//      
void CContactPtFinder::SetThrowRatio(const double& dbThrowRatio)
{
    m_dbThrowRatio = dbThrowRatio;
    m_dbFocalLength = m_nImageCx*m_dbThrowRatio;//���ص�Ч���࣬��λ:����


    //����Խ��߳���
    double d = sqrt(double(m_nImageCx * m_nImageCx + m_nImageCy * m_nImageCy));

    m_dbMaxViewAngle = 2*atan(d/2/m_dbFocalLength)*180.0/M_PI;

}


//@����:���þ�ͷ����
void CContactPtFinder::SetLensType(ELensType eLensType)
{
    m_eLensType = eLensType;


    switch(m_eLensType)
    {
    case E_LENS_TR_1_DOT_34:
        SetThrowRatio(1.34);
        //m_Model1_K[0] = 0.9915;////matlab������;
        m_Model1_K[0] = 0.9920;////matlabʵ����
        m_Model1_K[1] = 0.0;

        m_Model2_K[0] =-8.8285* double(1<<INT_SCALE_SHIFT_SIZE);
        m_Model2_K[1] = 0.0;
        break;


    case E_LENS_TR_0_DOT_34://Ͷ���0.34
        SetThrowRatio(0.34);
        m_Model1_K[0] =  0.9891;//matlab������
        m_Model1_K[0] = 0.9891 ;//ʵ������
        m_Model1_K[1] = 0.0;

        m_Model2_K[0] = -4.285 * double(1<<INT_SCALE_SHIFT_SIZE);
        m_Model2_K[1] = 0.0;
        break;

    case E_LENS_TR_0_DOT_21://Ͷ���0.28
        /*
      //  SetThrowRatio(0.28);        
      //m_Coefs[0]   = 0.98988;//Matlab��������
        m_Model1_K[0]   = 0.99225;//��������
        m_Model1_K[1] = 0.0;
        */

        //ʵ�鷢��, ����Ͷ���0.28��,ƫ��ϴ�
        //����ȡ�ӽ�120��,�������ȡ0.98569ʱУ����Ϊ��ȷ��ԭ����顣
        //
        SetViewAngle(120.0);
        m_Model1_K[0] = 0.98560;
        m_Model1_K[1] = 0.0;


        m_Model2_K[0] = -3.459 * double(1<<INT_SCALE_SHIFT_SIZE);
        m_Model2_K[1] = 0.0;
        break;

    case E_LENS_TR_0_DOT_15:
        /*
        SetThrowRatio(0.19);
        //m_Model1_K[0] = 0.9921;//Matlab������
        //m_Model1_K[0] = 0.9936  ;//��������
        m_Model1_K[0] = 0.9930    ;//��������
        m_Model1_K[1] = 0.0;
        */
        //ʵ�鷢��, ����Ͷ���0.28��,ƫ��ϴ�
        //����ȡ�ӽ�120��,�������ȡ0.98569ʱУ����Ϊ��ȷ��ԭ����顣
        //
        SetViewAngle(120.0);
        m_Model1_K[0] = 0.98560;
        m_Model1_K[1] = 0.0;

        m_Model2_K[0] = -2.7821 * double(1<<INT_SCALE_SHIFT_SIZE);
        m_Model2_K[1] = 0.0;
        break;

    }

}