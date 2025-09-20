#include "stdafx.h"
//#include "../inc/DispMonitorFinder.h"
//#include "CornerDetect.h"
//#include "CrossDetector.h"
//#include "ImageFrame.h"
//#include "ImageDib.h"
//#include "BlobDetectorImpl.h"
//#include "SIMD.h"
//#include "ImageDebug.h"
//#include "../MorphologyAlgo/inc/MorphologyAlgo.h"
#include "../headers.h"

struct RatioFilterElement
{
    UINT nObjIndex;
    double dbRatio;

	bool operator < (RatioFilterElement& right)
	{
		return (dbRatio < right.dbRatio);
	}

	bool operator <= (RatioFilterElement& right)
	{
		return (dbRatio <= right.dbRatio);
	}


	bool operator > (RatioFilterElement& right)
	{
		return (dbRatio > right.dbRatio);
	}

	bool operator >= (RatioFilterElement& right)
	{
		return (dbRatio >= right.dbRatio);
	}

};

//@����:����һ����׼�����Ŀ��
//@����:Ŀ��֮�����С����
//      pSrcObjs, ԭʼĿ������
//      n,ԭʼĿ��ĸ���
//      pOutObjs, ���������Ŀ��Ļ�����
//      nOutSize, �����������ʱָ���������ߴ�, �����������ʱָ�����˺��Ŀ�����
void FilterObject(int nMinDist, const TBlobObject* pSrcObjs, unsigned int n, TBlobObject* pOutObjs,unsigned int &nOutObjs)
{

    std::vector<bool> vecClutterFlags;//Ŀ��Ϊ������־

    unsigned int nMaxObjs = nOutObjs;
        nOutObjs = 0;

    vecClutterFlags.resize(n,false);
 

    int nMinDist2 = nMinDist * nMinDist;
    //����׼��
    //1.����ÿ��Ŀ������֮��ľ���, �������С���趨�����ޣ�������С��Ŀ�걻�޳�
    for(UINT i=0; i < n; i++)
    {
        if(vecClutterFlags[i])continue;//����Ŀ��
        const POINT& pt1   = pSrcObjs[i].GPos;
        const int&   mass1 = pSrcObjs[i].mass;

        for(UINT j = i+1; j < n; j++)
        {
            const POINT& pt2   = pSrcObjs[j].GPos;
            const int&   mass2 = pSrcObjs[j].mass;

            int nDx   = (pt2.x  - pt1.x);
            int nDy   = (pt2.y  - pt1.y);
            int nDist2 = nDx*nDx + nDy*nDy;
            if(nDist2 < nMinDist2)
            {
                if(mass1 <= mass2)
                {
                    vecClutterFlags[i] = true;
                    break;
                }
                else
                {
                     vecClutterFlags[j] = true;
                }
                
            }

        }//for(j)

        if(!vecClutterFlags[i])
        {
            if(nOutObjs >= nMaxObjs) break;
            pOutObjs[nOutObjs] =  pSrcObjs[i];
            nOutObjs++;
        }


    }//for(i)

}


//@����:���ؾ��������ڵ�Ŀ��ĳ���,����
//@����:image  ,�Ҷ�ͼƬ
//      rcBound,��������  
//@˵��:����PCA(Principal Component Analyis)��������ȡĿ��ĳ���Ͷ��᳤��
//      http://en.wikipedia.org/wiki/Principal_component_analysis
struct DoublePrecisionSize
{
    double cx;
    double cy;
};

DoublePrecisionSize GetObjectAxisLength(const CImageFrame& image, const RECT& rcBound)
{
    DoublePrecisionSize sz;
    sz.cx  = 0.0;
    sz.cy  = 0.0;

    int W = image.Width();
    int H = image.Height();
    const BYTE* pImageData = image.GetData();
    //1.��������
    POINT ptG;
    long lMx = 0, lMy = 0, lMass = 0;

    for(int y = rcBound.top ; y < rcBound.bottom; y ++)
    {
        const BYTE* pData = pImageData + y * W + rcBound.left;
       
        for(int x = rcBound.left; x < rcBound.right; x ++)
        {
            int val = *pData;
            if(val)
            {
                lMass += val;
                lMx   += val * x;
                lMy   += val * y;
            }

            pData ++;
        }
        
    }

    if(lMass == 0) return sz;

    ptG.x = lMx / lMass;
    ptG.y = lMy / lMass;


    long Dxx = 0, Dyy = 0, Dxy = 0;

    //����Э�������
    //          ��Dx^2   DxDy ��
    //cov(x,y) =��            ��
    //          ��DxDy   Dy^2 ��
    //          ��            ��
    //
    long n = 0;
    for(int y = rcBound.top ; y < rcBound.bottom; y ++)
    {
        const BYTE* pData = pImageData + y * W + rcBound.left;
       
        for(int x = rcBound.left; x < rcBound.right; x ++)
        {
            int val = *pData;
            if(val)
            {
              int dx = x - ptG.x;
              int dy = y - ptG.y;

              Dxx += dx*dx;
              Dyy += dy*dy;
              Dxy += dx*dy;
              n ++ ;
            }
            pData ++;
        }
        
    }

    

    Dxx = Dxx/n;
    Dyy = Dyy/n;
    Dxy = Dxy/n;

    //���Э������������������
    //����������ֵ��ƽ��������ӦĿ��ĳ��뾶���ȺͶ̰뾶����
    //��������Ϊ
    //��^2 - (Dxx + Dyy) + (DxxDyy - Dxy^2) = 0;

    double eigen_value1, eigen_value2;

    double b = -(Dxx + Dyy);
    double c = Dxx * Dyy - Dxy*Dxy;

    double d = sqrt(b*b - 4*c);//ע��b*b - 4*c�����0

    eigen_value1 = (-b  + d)/2.0;//��ֵ
    eigen_value2 = (-b  - d)/2.0;//Сֵ


    sz.cx = sqrt(eigen_value1) * 2.0;//����2,����Ϊ����ֵΪ�뾶����,���Ƿ��ص���ֱ��
    sz.cy = sqrt(eigen_value2) * 2.0;



    //���ֻ��һ����,�����õ�������ֵΪ0

    if(0.0 == sz.cx)sz.cx = 1.0;
    
    if(0.0 == sz.cy)sz.cy = 1.0;

    return sz;
}
           


//@����:pSrcImage, ָ��Ҷ�ͼƬ���ݵ�ָ��
//      nWidth, ͼƬ���
//      nHeight, ͼƬ�߶�
//      uExpectedCornerNumber, ������У�������
//      nGridSize, ��������ƽ����С
//      pMaskImage, ָ������ͼ��ָ�룬����ͼ��ָ���ͼƬ�ĸ߶ȺͿ��һ��
//      ECalibDebugLevel, ���Լ���
//      nDebugTimes,���Բ���
bool CCrossDetector::operator()(
    const unsigned char* pSrcImage, 
    int nWidth, 
    int nHeight, 
    UINT uExpectedCornerNumber, 
    int nGridSize, 
    const unsigned char* pMaskImage,
    ECalibDebugLevel eDebugLevel, 
    int nDebugTimes )
{
    //ģʽͼƬ
    static CImageFrame s_src;
    s_src.SetSize(nWidth, nHeight, 1);
    memcpy(s_src.GetData(), pSrcImage, s_src.Size());


    //ģʽͼƬ
    static CImageFrame s_pattern;
    s_pattern.SetSize(nWidth, nHeight, 1);
    //memcpy(s_pattern.GetData(), pSrcImage, s_pattern.Size());

    //����ͼƬ
    static CImageFrame s_mask;
    s_mask.SetSize(nWidth, nHeight, 1);
    if(pMaskImage)
    {
        //�ⲿ����ָ��������ͼ��ʹ�ø�����ͼ
        memcpy(s_mask.GetData(), pMaskImage, s_mask.Size());
    }
    else
    {
        //�ⲿδָ��������ͼ����ȡ���ͼƬ
        s_mask.Set(0xFF);
    }


    static CImageFrame s_debug;
    s_debug.SetSize(nWidth, nHeight, 1);


    if(nGridSize > 15)
    {
        //5*5�ļ�ⴰ��
        //cross_detect5_sse2(nWidth, nHeight, pSrcImage, s_pattern.GetData());
        cross_detect5_improve(nWidth, nHeight, pSrcImage, s_pattern.GetData());
        
        //��Ļ�������ʴ5������
        for(int i=0;i<5; i++)
        {
            Morph_Erode8(s_mask.GetData(),s_mask.GetData(),s_mask.Width(), s_mask.Height());
        }
        
        //Debug_SaveImageFrame(s_mask, _T("FastCrossDetect_mask.bmp"));

    }
    else
    {
        //3*3�ļ�ⴰ��
        cross_detect3_sse2(nWidth, nHeight, pSrcImage, s_pattern.GetData());
        
        
        //��Ļ�������ʴ3������
        for(int i=0;i<3; i++)
        {
            Morph_Erode8(s_mask.GetData(),s_mask.GetData(),s_mask.Width(), s_mask.Height());
        }
        
    }
    
    //�ø�ʴ�������ͼ,���α߽紦�ĸ��ŵ�
    s_pattern &= s_mask;


    CConnectedComponentScanlineAlogrithm<> blobDetect;


    CBitFrame bitFrame;
    bitFrame.SetSize(nWidth, nHeight);

    BYTE cThreshold = 0x08;

    GrayToBitFrame_SSE2((const BYTE*)s_pattern.GetData(), (BYTE*)bitFrame.GetData(), cThreshold, nWidth*nHeight);


    //if(eDebugLevel == E_CALIB_DEBUG_LEVEL_DEBUG)
    {

        //��ֵ����1λλͼתΪ8λ�Ҷ�λͼ
        BitToGrayFrame_MMX(
            (const BYTE*)bitFrame.GetData(),
            s_debug.GetData(),
            bitFrame.GetPixelCount());

        s_debug.Merge(s_src, 50);
        
    }

  

    blobDetect.ProcessImage((const BYTE*)bitFrame.GetData(), nWidth, nHeight);


    const TBlobObject* pObj = blobDetect.GetObjs();

    int nObjCount = blobDetect.GetObjCount();
    
    
    const int MAX_OBJS_COUNT  = 256;
    TBlobObject objs[MAX_OBJS_COUNT];
    unsigned int nObjNumber = _countof(objs);

    //1.У��ͼ���α��Сʱ��Ŀ����С����
    //int nMinDist   =  nGridSize/2 + 2;

    //2.У��ͼ��Ͱ�α�ϴ�ʱ��Ŀ����С����
    int nMinDist   =  nGridSize/3;
    

    //���ݾ���Ŀ��֮��ľ����ϵ, ��������Ŀ��
    FilterObject(nMinDist, pObj, nObjCount, objs, nObjNumber);
    

    
    if(nObjNumber < uExpectedCornerNumber)
    {
        //�������������ͼƬ,�Ա��ҳ�ʧ��ԭ��
        Debug_SaveImageFrame(s_debug, _T("Missing_Corner.jpg"));
        return FALSE;
    }

    if(nObjNumber > uExpectedCornerNumber)
    {

         CImageFrame binaryImage;
         binaryImage.SetSize(nWidth, nHeight,1);

        //��ֵ����1λλͼתΪ8λ�Ҷ�λͼ
        BitToGrayFrame_MMX(
            (const BYTE*)bitFrame.GetData(),
            binaryImage.GetData(),
            bitFrame.GetPixelCount());


        std::vector<RatioFilterElement> vecCandidates;
        
        double ratio_upper_limit = 4.0;
        //�޳�����ȺͿ��ȴ���4�б�ֵ����Ŀ��        
        for(UINT i=0; i< nObjNumber; i++)
        {
      
          //����Ŀ��ĳ���Ͷ���
          DoublePrecisionSize size = GetObjectAxisLength(binaryImage, objs[i].rcArea);

          double ratio = max(size.cx/size.cy, size.cy/size.cx);

          if(ratio > ratio_upper_limit)
          {
              RatioFilterElement e;
              e.nObjIndex = i;
              e.dbRatio   = ratio;
              vecCandidates.push_back(e);
          }
        }

        if(vecCandidates.size())
        {

            CHeapSortAlgorithm<RatioFilterElement> heapSort;
            heapSort(&vecCandidates[0], vecCandidates.size(),FALSE);


            UINT nDelNumber = nObjNumber - uExpectedCornerNumber;
            if(vecCandidates.size() < nDelNumber)
            {
                nDelNumber = vecCandidates.size();
            }

            for(UINT i=0; i < nDelNumber; i++)
            {
                objs[vecCandidates[i].nObjIndex].bIsValid = FALSE;
            }

            //�Ƴ�������bIsValid�����Ϊfalse�Ķ���
            int  j = 0;
            for(UINT i=0; i < nObjNumber; i++)
            {
                if(objs[i].bIsValid)
                {
                    objs[j] = objs[i];
                    j ++ ;
                }
            }

        

            nObjNumber -= nDelNumber;
        }
        

    

    }
   
    
	CHeapSortAlgorithm<TBlobObject> heapSort;


    //����Ŀ��������������и���Ŀ��, 
    //�Ա㰴��������С, ����ѡȡĿ��
    heapSort(objs, nObjNumber, false);


    //<<����
    CDib dibDebug;
        
    //if(eDebugLevel == E_CALIB_DEBUG_LEVEL_DEBUG)
    {
        //����ͼ��У��ͼ���ϲ�, ����ͼ͸����Ϊ10%
        //s_debug.Merge(s_mask, 5);
        LoadToDib(s_debug, dibDebug);
     }

    //����>>
    m_vecCrosses.resize  (uExpectedCornerNumber);

    m_vecHDCrosses.resize(uExpectedCornerNumber);

    const BYTE*  pPattern = s_pattern.GetData();

    for(UINT i=0; i<uExpectedCornerNumber; i++)
    {

        m_vecCrosses[i].x   = (objs[i].my + (objs[i].mass>>1))/objs[i].mass;
        m_vecCrosses[i].y   = (objs[i].mx + (objs[i].mass>>1))/objs[i].mass;

        //�߾�����ֵ,
        m_vecHDCrosses[i].x = ((objs[i].my << INT_SCALE_SHIFT_SIZE) + (objs[i].mass>>1))/objs[i].mass;
        m_vecHDCrosses[i].y = ((objs[i].mx << INT_SCALE_SHIFT_SIZE) + (objs[i].mass>>1))/objs[i].mass;


        //����Ŀ�����Ӿ���,����ӦΪͼ�м��㾫ȷ����Ŀ����Ӧ�����ĵ�λ��
        //2014/03/25,����ʮ�ַ��ŵ��α��ͼ�������, ����Ӿ���������ʮ�ַ��ŵľ�ȷ����,�ǲ����ʵġ�
        //RECT& rcObj = objs[i].rcArea;
        //long t = rcObj.top;
        //long b = rcObj.bottom;
        //long l = rcObj.left;
        //long r = rcObj.right;

        //unsigned int nMx   = 0;//����
        //unsigned int nMy   = 0;//����
        //unsigned int nMass = 0;//����

        ////��������
        //for(long y = t; y < b; y++)
        //{
        //    for(int x = l; x < r; x++)
        //    {
        //        BYTE cResponse = pPattern[y * nWidth + x];
        //        nMx   += cResponse * x;
        //        nMy   += cResponse * y;
        //        nMass += cResponse;
        //    }
        //}

        //if(nMass == 0 )
        //{
        //    return FALSE;//������   
        //}

         //m_vecHDCrosses[i].x = ((nMx << INT_SCALE_SHIFT_SIZE) + (nMass>>1))/nMass;
         //m_vecHDCrosses[i].y = ((nMy << INT_SCALE_SHIFT_SIZE) + (nMass>>1))/nMass;
                


        if(eDebugLevel == E_CALIB_DEBUG_LEVEL_DEBUG)
        {
            //<<����
            //��Ӿ���
            dibDebug.Rectangle(objs[i].rcArea, RGB(128,0,0));


            //ʮ�ַ��ŵ����ĵ�
            dibDebug.SetPixel(m_vecCrosses[i].x, m_vecCrosses[i].y, RGB(255,0,0));

            //ʮ�ַ��ŵĸ߾���λ�õ�
            dibDebug.SetPixel(
                (m_vecHDCrosses[i].x +  2^(INT_SCALE_SHIFT_SIZE -1)) >> INT_SCALE_SHIFT_SIZE, 
                (m_vecHDCrosses[i].y +  2^(INT_SCALE_SHIFT_SIZE -1)) >> INT_SCALE_SHIFT_SIZE, 
                RGB(0,255,0));
            //����>>
        }
    }


     if(eDebugLevel == E_CALIB_DEBUG_LEVEL_DEBUG)
     {
        //ʣ�µĵ�������Ŀ��
        for(UINT i = uExpectedCornerNumber; i < nObjNumber; i++)
        {
            //<<����
            //dibDebug.Rectangle(objs[i].rcArea, RGB(0,255,255));
             dibDebug.SetPixel(objs[i].GPos.x, objs[i].GPos.y, RGB(255,0,255));
             //����>>
        }




        Debug_SaveDibToJPEG(dibDebug, _T("FastDetectCross_Result.jpg"));
    }
    return TRUE;

}
